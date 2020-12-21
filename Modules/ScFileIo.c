/*@file
  Provides functions to handle file I/O.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <ScFileIo.h>

//
// We may perform bitwise operations on the file data, e.g. to normalise or
// ignore letter case. To avoid unnecessary risk for Undefined or
// Implementation-define Behaviour, force char to be unsigned.
// With GCC and Clang, this is possible by passing the -funsigned-char flag, for
// MSVC you need to pass /J. For obvious reasons, anything but 8-bit values
// are not supported at all.
//
_Static_assert(
  CHAR_MIN == 0 && CHAR_MAX == 0xFF,
  "For code safety reasons, please ensure char is an unsigned 8-bit value."
  );

char *ScReadFile(
  size_t     *FileSize,
  const char *FileName,
  size_t     MaxFileSize
  )
{
  assert(FileSize != NULL);
  assert(FileName != NULL);
  //
  // Open files in binary mode to avoid unexpected text translation effects.
  //
  FILE *FileHandle = fopen(FileName, "rb");
  if (FileHandle == NULL) {
    return NULL;
  }
  //
  // Set the current position to the end of the file to retrieve the file size.
  // Due to binary open mode, this is technically Implementation-defined
  // Behaviour, however all common implementations support this fine.
  //
  int SeekResult = fseek(FileHandle, 0, SEEK_END);
  if (SeekResult != 0) {
    return NULL;
  }
  //
  // Retrieve and sanitize the file size in bytes.
  // The cast in the second condition is safe due to the first condition.
  //
  long BinFileTell = ftell(FileHandle);
  if (BinFileTell < 0 || (unsigned long) BinFileTell > MaxFileSize) {
    return NULL;
  }

  size_t BinFileSize = (size_t) BinFileTell;
  //
  // Rewind the file to its start position to account for size retrieval.
  //
  rewind(FileHandle);

  char *FileBuffer = malloc(BinFileSize);
  if (FileBuffer == NULL) {
    return NULL;
  }
  //
  // Read the file's entire content into FileBuffer up to the actual data size.
  //
  size_t ReadSize = fread(
    FileBuffer,
    1,
    BinFileSize,
    FileHandle
    );
  if (ReadSize != BinFileSize) {
    free(FileBuffer);
    return NULL;
  }
  //
  // Close the file handle as it is no longer required.
  // While an error-exit due to closing failure may be unintuitive, we must not
  // allow dangling resources to pile up.
  //
  int CloseResult = fclose(FileHandle);
  if (CloseResult != 0) {
    free(FileBuffer);
    return NULL;
  }
  //
  // Return the read file data.
  //
  *FileSize = BinFileSize;
  return FileBuffer;
}

bool ScWriteFile(
  const char *FileName,
  size_t     FileSize,
  const char *Buffer
  )
{
  assert(FileName != NULL);
  assert(Buffer != NULL || FileSize == 0);

  //
  // Open files in binary mode to avoid unexpected text translation effects.
  //
  FILE *FileHandle = fopen(FileName, "wb");
  if (FileHandle == NULL) {
    return false;
  }

  size_t WrittenSize = fwrite(Buffer, 1, FileSize, FileHandle);
  if (WrittenSize != FileSize) {
    return false;
  }
  //
  // Close the file handle as it is no longer required.
  // While an error-exit due to closing failure may be unintuitive, we must not
  // allow dangling resources to pile up.
  //
  int CloseResult = fclose(FileHandle);
  if (CloseResult != 0) {
    return false;
  }

  return true;
}

const char *
ScGetFileExtension (
  const char *FileName
  )
{
  const char *Extension;

  Extension = FileName;
  for (size_t CharIndex = 0; FileName[CharIndex] != '\0'; ++CharIndex) {
    if (FileName[CharIndex] == '.') {
      Extension = &FileName[CharIndex + 1];
    }
  }

  return Extension;
}
