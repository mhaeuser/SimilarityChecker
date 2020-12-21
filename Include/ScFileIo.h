/*@file
  Provides functions to handle file I/O.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#ifndef SC_FILE_IO_H_
#define SC_FILE_IO_H_

#include <stdbool.h>
#include <stddef.h>

/*
  Reads the file from path FileName.

  @param[out] FileSize     A pointer into which the file buffer's size is
                           returned iff it is not NULL.
  @param[in]  FileName     The path of the file to read.
  @param[in]  MaxFileSize  The maximum number of bytes to read.

  @retval NULL   An unexpected error has occured, no memory has been allocated.
  @retval other  A buffer containing the file's content. It is allocated by
                 malloc and caller-owned.
*/
char *ScReadFile(
  size_t     *FileSize,
  const char *FileName,
  size_t     MaxFileSize
  );

/*
  Writes a file to path FileName.

  @param[in]  FileData  The data to write to the file. It does not need to be
                        terminated.
  @param[out] FileSize  The size of FileData, in bytes, to write.
  @param[in]  FileName  The path of the file to write. It needs to be properly
                        terminated.

  @returns  Whether the file has been written successfully.
*/
bool ScWriteFile(
  const char *FileData,
  size_t     FileSize,
  const char *FileName
  );

/*
  Retrieves the file extension of path FileName.

  @param[in] FileName  The path of the file to inspect. It needs to be properly
                       terminated.

  @returns  A pointer into FileName containing the terminated file extension.
*/
const char *
ScGetFileExtension (
  const char *FileName
  );

#endif // SC_FILE_IO_H_
