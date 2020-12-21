/*@file
  Provides APIs to cleanse text snippets.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#ifndef SC_CLEANSE_INPUT_H_
#define SC_CLEANSE_INPUT_H_

#include <ScStringMisc.h>

/*
  Forms a sc_lenghted_string_t structure instance from a string literal

  @param[in] A  The string to create a sc_lenghted_string_t instance out of.
*/
#define SC_LENGTHED_STRING(A) { (A), sizeof(A) - 1 }

///
/// Provides information on how to cleanse a generaliser.
///
typedef struct {
  ///
  /// The generaliser for this group.
  ///
  const sc_lenghted_string_t Generaliser;
  ///
  /// An array of generalisees which are to be generalised to Generaliser.
  /// Every element must be of a length as least as long as Generaliser.
  ///
  const sc_lenghted_string_t *Generalisees;
  ///
  /// The number of elements in Generalisees.
  ///
  size_t                     NumGeneralisees;
} sc_cleanse_generalise_t;

///
/// Provides configuration for the cleansing process.
///
typedef struct {
  ///
  /// Describes the file extensions to match with this config.
  ///
  const char                    **FileExts;
  ///
  /// The number of elements in FileExts.
  ///
  size_t                        NumFileExt;
  ///
  /// Describes the sequence to start a multi line comment.
  ///
  const sc_lenghted_string_t    MultiCommentStart;
  ///
  /// Describes the sequence to end a multi line comment.
  ///
  const sc_lenghted_string_t    MultiCommentEnd;
  ///
  /// An array of prefixes to drop including the rest of the line.
  ///
  const sc_lenghted_string_t    *LineDropPrefixes;
  ///
  /// The number of elements in LineDropPrefixes.
  ///
  size_t                        NumLineDropPrefixes;
  ///
  /// An array of characters that are treated as new lines.
  ///
  const char                    *NewLineChars;
  ///
  /// The number of elements in NewLineChars.
  ///
  size_t                        NumNewLineChars;
  ///
  /// An array of generalises defining generalisation operatations.
  ///
  const sc_cleanse_generalise_t *Generalises;
  ///
  /// The number of elements in Generalises.
  ///
  size_t                        NumGeneralises;
} sc_cleanse_config_t;

/*
  Cleanse Buffer of comments and line fragments with prefixes to drop.
  
  @param[in,out] Buffer        The text buffer to cleanse.
  @param[in]     BufferLength  The length, in characters, of Buffer.
  @param[in]     Config        Configuration for the cleansing process.
*/
void ScCleanseLines(
  char                      *Buffer,
  size_t                    BufferLength,
  const sc_cleanse_config_t *Config
  );

/*
  Cleanse Buffer of whitespace characters while preserving new line characters.

  @param[in,out] Buffer        The text buffer to cleanse.
  @param[in]     BufferLength  The length, in characters, of Buffer.
  @param[in]     Config        Configuration for the cleansing process.
*/
void ScCleanseWhitespacesInLines(
  char                      *Buffer,
  size_t                    BufferLength,
  const sc_cleanse_config_t *Config
  );

/*
  Generalise Generalisees to Generaliser in Buffer. If a generalisee is longer
  than the generaliser, its trailer is set to all spaces.

  @param[in,out] Buffer          The buffer to generalise substrings in.
  @param[in]     BufferLength    The length, in characters, of Buffer.
  @param[in]     Config        Configuration for the cleansing process.
*/
void ScCleanseGeneralisees(
  char                      *Buffer,
  size_t                    BufferLength,
  const sc_cleanse_config_t *Config
  );

/*
  Remove space characters in Buffer.

  @param[in,out] Buffer        The text buffer to cleanse.
  @param[in,out] BufferLength  On input, the length, in characters, of Buffer.
                               On output, the cleansed length, in characters, of
                               Buffer.
*/
void ScCleanseRemoveSpaces(
  char   *Buffer,
  size_t *BufferLength
  );

/*
  Cleanse Buffer according to Config. For details regarding the operations,
  please refer to the other functions within this header.

  @param[in,out] Buffer        The text buffer to cleanse.
  @param[in,out] BufferLength  On input, the length, in characters, of Buffer.
                               It must be bigger than 0.
                               On output, the cleansed length, in characters, of
                               Buffer.
  @param[in]     Config        Configuration for the cleansing process.
*/
void ScCleanseInput(
  char                      *Buffer,
  size_t                    *BufferLength,
  const sc_cleanse_config_t *Config
  );

#endif // SC_CLEANSE_INPUT_H_
