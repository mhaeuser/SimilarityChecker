/*@file
  Provides similarity checker APIs for given code inputs.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#ifndef SC_COMMON_H_
#define SC_COMMON_H_

#include <limits.h>

#include <ScCleanseConfigs.h>
#include <ScCleanseInput.h>

///
/// Defines the maximum file size supported by this tool.
///
#ifndef SC_MAX_FILE_SIZE
  #define SC_MAX_FILE_SIZE  (1U * 1024U * 1024U)
#endif

///
/// Defines the maximum line length supported by this tool.
///
#ifndef SC_MAX_LINE_LENGTH
  #define SC_MAX_LINE_LENGTH  512U
#endif

///
/// Defines the number of lines prior to and past the current line of file 1 to
/// compare with in file 2.
///
#ifndef SC_NUM_LINES_SWAP
  #define SC_NUM_LINES_SWAP  3U
#endif

//
// As per ScStrGetLineInfo() precondition, the maximum file size value must
// be smaller than SIZE_MAX.
//
_Static_assert(
  SC_MAX_FILE_SIZE < SIZE_MAX,
  "The maximum file size is not smaller than the maximum buffer size."
  );

typedef struct {
  ///
  /// The buffer holding the cleansed file's contents.
  ///
  char                *Buffer;
  ///
  /// The length, in characters, of Buffer.
  ///
  size_t              Length;
  ///
  /// The lines information for Buffer.
  ///
  sc_str_lines_info_t *LinesInfo;
  ///
  /// This field is reserved for usage by the consumer.
  ///
  unsigned int        Reserved;
} sc_cleanse_file_t;

/*
  Initialises the environment to allow the execution of ScLevenshteinSwap().
*/
void ScLevenshteinSwapInitialise(void);

/*
  Calculates the Levenshtein distance from File1 to File2 on per-line basis.
  ScLevenshteinSwapInitialise() must be called before calling this one.

  @param[in] File1         The first file to compare.
  @param[in] File2         The second file compare.
  @param[in] NumLinesSwap  The radius to pick lines in file 2 from to compare to
                           lines of file 1.

  @retval INFINITY  An error occured while comparing File1 and File2.
  @retval other     The Levenshtein distance between File1 and File2.
*/
double ScLevenshteinSwap(
  const sc_cleanse_file_t *File1,
  const sc_cleanse_file_t *File2,
  size_t                  NumLinesSwap
  );

/*
  Reads the file from path FileName and cleases it by internal configuration for
  FileType.

  @param[out] File      A pointer to return the cleansed file information into.
  @param[in]  FileName  The path of the file to read. It needs to be correctly
                        terminated.
  @param[in]  FileType  The cleansing type of the file.
                        If ScCleanseConfigTypeMax is passed, the type will be
                        automatically detected based on the file extension of
                        FileName.

  @returns  Whether the file has been read and cleansed successfully.
*/
bool ScReadCleansedFile(
  sc_cleanse_file_t        *File,
  const char               *FileName,
  sc_cleanse_config_type_t FileType
  );

/*
  Initialise File based on File->Buffer, File->Length and FileType.
  Ownership of the resources is temporarily transfered to thos function.
  On failure, File->Buffer is freed.

  @param[in,out] File      On input, contains the file buffer to cleanse and its
                           length.
                           On output, File is a valid cleansed file instance.
  @param[in]     FileType  The cleansing type of the file.

  @returns  Whether the file has been cleansed successfully.
*/
bool ScInitialiseCleanseFile(
  sc_cleanse_file_t        *File,
  sc_cleanse_config_type_t FileType
  );

/*
  Frees a cleansed file information structure.

  @param[in] File  The cleansed file information to free. it must have been
                   successfully constructed by ScReadCleansedFile().
*/
void ScFreeCleansedFile(
  sc_cleanse_file_t *File
  );

extern size_t mScLevenshteinMatrixInit[SC_MAX_LINE_LENGTH];

#endif // SC_COMMON_H_
