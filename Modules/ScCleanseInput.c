/*@file
  Provides functions to cleanse text snippets.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
*/

#include <assert.h>
#include <string.h>
#include <stddef.h>

#include <ScCleanseInput.h>
#include <ScStringMisc.h>

/*
  Cleanse the current line from Buffer.

  @param[in,out] Buffer        The buffer to cleanse. It does not need to be
                               terminated.
  @param[in,out] BufferLength  On input, the length of the string in Buffer.
                               On output, the input value minus the distance
                               from the returned pointer to Buffer.

  @returns  A pointer to the new line character in Buffer.
*/
static char *ScCleanseDropLine(
  char   *Buffer,
  size_t *BufferLength
  )
{
  assert(BufferLength != NULL);
  assert(Buffer != NULL || *BufferLength == 0);
  //
  // When a single line comment is encountered, cleanse all characters till the
  // end of the line.
  //
  for (; *BufferLength > 0; --*BufferLength, ++Buffer) {
    //
    // The comment is terminated by a new line.
    //
    if (*Buffer == '\n') {
      break;
    }

    *Buffer = ' ';
  }
  //
  // *BufferLength is kept consistent for all operations above.
  //
  return Buffer;
}

/*
  Cleanse a multi line comment from Buffer.

  @param[in,out] Buffer        The buffer to cleanse. It does need to be
                               terminated. Must not overlap with CommentEnd.
  @param[in,out] BufferLength  On input, the length of the string in Buffer.
                               On output, the input value minus the distance
                               from the returned pointer to Buffer.
  @param[in]     CommentEnd    The sequence that ends a multi line comment.
                               Must not overlap with Buffer.

  @returns  A pointer to the character in Buffer after the end of the comment.
*/
static char *ScCleanseMultiComment(
  char                       * restrict Buffer,
  size_t                     *BufferLength,
  const sc_lenghted_string_t *CommentEnd
  )
{
  assert(BufferLength != NULL);
  assert(Buffer != NULL || *BufferLength == 0);
  assert(CommentEnd != NULL);
  //
  // When a multi line comment is encountered, cleanse all characters till the
  // end of the comment while omiting new line characters.
  //
  for (; *BufferLength > 0; --*BufferLength, ++Buffer) {
    //
    // If the multi line comment terminator is encountered, cleanse it and exit.
    //
    const int Prefixed = ScStrnPrefix(
      Buffer,
      CommentEnd->String,
      *BufferLength,
      CommentEnd->Length
      );
    if (Prefixed == 0) {
      for (size_t Index = CommentEnd->Length; Index > 0; --Index) {
        *Buffer = ' ';
        ++Buffer;
      }

      *BufferLength -= CommentEnd->Length;

      break;
    }

    *Buffer = ' ';
  }
  //
  // *BufferLength is kept consistent for all operations above.
  //
  return Buffer;
}

void ScCleanseWhitespacesInLines(
  char                       *Buffer,
  size_t                     BufferLength,
  const sc_cleanse_config_t  *Config
  )
{
  assert(Buffer != NULL || BufferLength == 0);
  //
  // Use a temporary buffer for PreceedingNewLine when not present to improve
  // performance by avoiding NULL checks. 
  //
  char PreceedingNewLineTmp;
  char *PreceedingNewLine = &PreceedingNewLineTmp;
  const char *const BufferTop = Buffer + BufferLength;
  for (; Buffer < BufferTop; ++Buffer, --BufferLength) {
    //
    // Cleanse whitespaces that do not denote a new line to space.
    // Cleanse line feed to new line to account for Windows-style and similar
    // line endings. Remove new lines that are only superseeded only by
    // whitespaces before another new line.
    //
    switch(*Buffer) {
      case '\v':
      case '\t':
      {
        *Buffer = ' ';
        //
        // Fall-through to space handling.
        //
      }
      
      case ' ':
      {
        //
        // Do not alter previous character information for whitespaces, pretend
        // they do not exist.
        //
        break;
      }

      default:
      {
        //
        // Check whether the unmatched character is to be treated as a new line.
        //
        size_t Index;
        for (Index = 0; Index < Config->NumNewLineChars; ++Index) {
          if (*Buffer == Config->NewLineChars[Index]) {
            break;
          }
        }

        if (Index == Config->NumNewLineChars) {
          //
          // A non-whitespace has been encountered, clear previous new line.
          //
          PreceedingNewLine = &PreceedingNewLineTmp;
          break;
        }
        //
        // A character that is to be treated as a new line as been encountered,
        // fall-through to normalisation and new line handling.
        //
      }

      case '\r':
      {
        *Buffer = '\n';
        //
        // Fall-through to new line handling.
        //
      }

      case '\n':
      {
        //
        // Remove the previous new line (only serparated by arbitrarily many
        // whitespaces from the current one) and remember the current one.
        //
        *PreceedingNewLine = ' ';
        PreceedingNewLine  = Buffer;
        break;
      }
    }
  }
  //
  // Clear the trailing new line character if existent.
  //
  *PreceedingNewLine = ' ';
}

void ScCleanseLines(
  char                      *Buffer,
  size_t                    BufferLength,
  const sc_cleanse_config_t *Config
  )
{
  assert(Buffer != NULL || BufferLength == 0);
  assert(Config != NULL);

  const char *const BufferTop = Buffer + BufferLength;
  while (Buffer < BufferTop) {
    //
    // Cleanse constructs like comments according to Config.
    // First, check for prefixes to drop the rest of the line.
    //
    size_t DropIndex;
    for (DropIndex = 0; DropIndex < Config->NumLineDropPrefixes; ++DropIndex) {
      const int Prefixed = ScStrnPrefix(
        Buffer,
        Config->LineDropPrefixes[DropIndex].String,
        BufferLength,
        Config->LineDropPrefixes[DropIndex].Length
        );
      if (Prefixed == 0) {
        Buffer = ScCleanseDropLine(Buffer, &BufferLength);
        break;
      }
    }
    //
    // If no prefix to drop the rest of the line has been encountered, check
    // whether this is a multi line comment.
    //
    if (DropIndex == Config->NumLineDropPrefixes) {
      const int Prefixed = ScStrnPrefix(
        Buffer,
        Config->MultiCommentStart.String,
        BufferLength,
        Config->MultiCommentStart.Length
        );
      //
      // The check for length is to account for the unknown configuration not
      // providing information on multi line comments.
      //
      if (Prefixed == 0 && Config->MultiCommentStart.Length > 0) {
        //
        // Delete comment start first so constructs like /*/ do not cause early
        // aborts.
        //
        const char *const CommentTop = Buffer + Config->MultiCommentStart.Length;
        for (; Buffer < CommentTop; ++Buffer, --BufferLength) {
          *Buffer = ' ';
        }

        Buffer = ScCleanseMultiComment(
          Buffer,
          &BufferLength,
          &Config->MultiCommentEnd
          );
      } else {
        //
        // No known prefix has been encountered, advance to the next character.
        //
        ++Buffer;
        --BufferLength;
      }
    }
    //
    // All operations must ensure consistency of Buffer and BufferLength.
    //
    assert(Buffer + BufferLength == BufferTop);
  }
}

void ScCleanseGeneralisees(
  char                      *Buffer,
  size_t                    BufferLength,
  const sc_cleanse_config_t *Config
  )
{
  assert(Buffer != NULL || BufferLength == 0);
  assert(Config != NULL);
  assert(Config->Generalises != NULL || Config->NumGeneralises == 0);
  //
  // Check the precondition of generalisees needing to be at least as long as
  // generalisers.
  //
  for (size_t Index = 0; Index < Config->NumGeneralises; ++Index) {
    const sc_cleanse_generalise_t *Generalise = &Config->Generalises[Index];
    for (size_t Index2 = 0; Index2 < Generalise->NumGeneralisees; ++Index2) {
      assert(Generalise->Generaliser.Length <= Generalise->Generalisees[Index2].Length);
    }
  }

  for (; BufferLength > 0; ++Buffer, --BufferLength) {
    //
    // Check for occurences of all generalisees.
    //
    for (size_t GenIndex = 0; GenIndex < Config->NumGeneralises; ++GenIndex) {
      const sc_lenghted_string_t *Generaliser = &Config->Generalises[GenIndex].Generaliser;
      size_t Index2;
      for (Index2 = 0; Index2 < Config->Generalises[GenIndex].NumGeneralisees; ++Index2) {
        const sc_lenghted_string_t *Generalisee = &Config->Generalises[GenIndex].Generalisees[Index2];
        const int GenPrefix = ScStrnPrefix(
          Buffer,
          Generalisee->String,
          BufferLength,
          Generalisee->Length
          );
        if (GenPrefix == 0) {
          //
          // Copy the generaliser string over the generalisee occurence.
          //
          memcpy(Buffer, Generaliser->String, Generaliser->Length);
          Buffer += Generaliser->Length;
          //
          // Fill up the trailer with spaces.
          //
          #pragma omp simd
          for (
            size_t TrailIndex = 0;
            TrailIndex < Generalisee->Length - Generaliser->Length;
            ++TrailIndex
            ) {
            *Buffer = ' ';
            ++Buffer;
          }
          //
          // Account for next outside loop iteration's increment.
          //
          --Buffer;
          BufferLength -= Generalisee->Length - 1;

          break;
        }
      }

      if (Index2 < Config->Generalises[GenIndex].NumGeneralisees) {
        break;
      }
    }
  }
}

void ScCleanseRemoveSpaces(
  char   *Buffer,
  size_t *BufferLength
  )
{
  assert(Buffer != NULL);
  assert(BufferLength != NULL);
  assert(*BufferLength > 0);

  size_t SourceIndex = 0;
  size_t TargetIndex = 0;
  //
  // Skip the following logic when Buffer starts with a new line.
  //
  if (Buffer[0] != '\n') {
    //
    // Find the first space character and store it as target to strip it.
    // Any prior character is a non-whitespace character and is preserved.
    //
    for (; TargetIndex < *BufferLength; ++TargetIndex) {
      if (Buffer[TargetIndex] == ' ') {
        break;
      }
    }
    //
    // When the first character is a space character, treat it the same way as
    // leading new lines outside of this if clause.
    //
    if (TargetIndex != 0) {
      if (TargetIndex == *BufferLength) {
        return;
      }
      //
      // There is at least one non-whitespace character, the last of which at
      // TargetIndex - 1. Skip the whitespace and start looking for further such
      // at TargetIndex + 1 in the loop below.
      //
      SourceIndex = TargetIndex + 1;
    }
  }
  //
  // When TargetIndex is 0, Buffer starts with either a space or a new line.
  // Advance SourceIndex to the first character that is not one of those to
  // strip the whitespace prefix.
  //
  if (TargetIndex == 0) {
    for (SourceIndex = 1; SourceIndex < *BufferLength; ++SourceIndex) {
      if (Buffer[SourceIndex] != ' ' && Buffer[SourceIndex] != '\n') {
        break;
      }
    }
  }

  for (; SourceIndex < *BufferLength; ++SourceIndex) {
    //
    // Found a new fragment to preserve.
    //
    if (Buffer[SourceIndex] != ' ') {
      size_t SourceIndexEnd;
      //
      // Find the end of the fragment.
      //
      for (SourceIndexEnd = SourceIndex + 1; SourceIndexEnd < *BufferLength; ++SourceIndexEnd) {
        if (Buffer[SourceIndexEnd] == ' ') {
          break;
        }
      }
      //
      // Strip the whitespaces prior to the fragment.
      //
      size_t ChunkSize = SourceIndexEnd - SourceIndex;
      memmove(&Buffer[TargetIndex], &Buffer[SourceIndex], ChunkSize);
      //
      // Advance source to the end of the fragment at its origin and target to
      // right after the copied fragment.
      //
      TargetIndex += ChunkSize;
      SourceIndex  = SourceIndexEnd - 1;
    }
  }
  //
  // TargetIndex is equal to the amount of preserved characters.
  //
  *BufferLength = TargetIndex;
}

void ScCleanseInput(
  char                      *Buffer,
  size_t                    *BufferLength,
  const sc_cleanse_config_t *Config
  )
{
  assert(Buffer != NULL);
  assert(BufferLength != NULL);
  assert(*BufferLength > 0);
  assert(Config != NULL);

  ScCleanseLines(Buffer, *BufferLength, Config);
  ScCleanseGeneralisees(Buffer, *BufferLength, Config);
  ScCleanseWhitespacesInLines(Buffer, *BufferLength, Config);
  ScCleanseRemoveSpaces(Buffer, BufferLength);
  //
  // After cleansing there may not be empty lines.
  //
  for (size_t CharIndex = 1; CharIndex < *BufferLength; ++CharIndex) {
    assert(Buffer[CharIndex - 1] != '\n' || Buffer[CharIndex] != '\n');
  }
  //
  // After cleansing there may not be a trailing new line.
  //
  if (*BufferLength > 0) {
    assert(Buffer[*BufferLength - 1] != '\n');
  }
}
