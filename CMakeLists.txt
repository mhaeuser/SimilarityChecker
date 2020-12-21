## @ file
# Provides build definitions for the SimilarityChecker project.
#  
# Copyright (C) 2020 Marvin Häuser. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
##
cmake_minimum_required(VERSION 3.17)

if(CMAKE_BUILD_TYPE MATCHES "DebugFuzzTestingSan" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfoFuzzTestingSan")
  set(sc_main_file EntryPoints/ScLlvmFuzzing.c)
elseif(CMAKE_BUILD_TYPE MATCHES "DebugUnitTestingSan")
  set(sc_main_file EntryPoints/ScUnitTesting.c)
else()
  set(sc_main_file EntryPoints/ScMain.c)
endif()

project(SimilarityChecker LANGUAGES C)
add_executable(SimilarityChecker
  CleanseConfigs/ScCleanseConfigC.c
  CleanseConfigs/ScCleanseConfigFSharp.c
  CleanseConfigs/ScCleanseConfigJava.c
  CleanseConfigs/ScCleanseConfigs.c
  CleanseConfigs/ScCleanseConfigUnknown.c
  EntryPoints/ScCommon.c
  Modules/ScCleanseInput.c
  Modules/ScDistances.c
  Modules/ScFileIo.c
  Modules/ScSafeInt.c
  Modules/ScStringMisc.c
  ${sc_main_file}
  )
target_include_directories(SimilarityChecker PRIVATE Include)

#
# Project-specific configuration. Add compile-time definitions of configuration options if provided.
#
if(SC_MAX_FILE_SIZE)
  target_compile_definitions(SimilarityChecker PRIVATE SC_MAX_FILE_SIZE=${SC_MAX_FILE_SIZE})
endif()

if(SC_MAX_LINE_LENGTH)
  target_compile_definitions(SimilarityChecker PRIVATE SC_MAX_LINE_LENGTH=${SC_MAX_LINE_LENGTH})
endif()

if(SC_NUM_LINES_SWAP)
  target_compile_definitions(SimilarityChecker PRIVATE SC_NUM_LINES_SWAP=${SC_NUM_LINES_SWAP})
endif()

#
# Compiler-specific configuration.
# MSVC_RUNTIME_LIBRARY needs to be changed to static linkage when Sanitizers are
# enabled.
#
# Enable LTO unless Sanitizers are enabled.
#
if(CMAKE_BUILD_TYPE MATCHES "DebugSan" OR CMAKE_BUILD_TYPE MATCHES "DebugUnitTestingSan" OR CMAKE_BUILD_TYPE MATCHES "DebugFuzzTestingSan")
  set(msvc_rt_suffix "Debug")
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(msvc_rt_suffix "DebugDLL")
  set_property(TARGET SimilarityChecker PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfoFuzzTestingSan" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfoSan")
  set(msvc_rt_suffix "")
else()
  set(msvc_rt_suffix "DLL")
  set_property(TARGET SimilarityChecker PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
set_target_properties(
  SimilarityChecker PROPERTIES
  C_STANDARD 11
  C_EXTENSIONS OFF
  MSVC_RUNTIME_LIBRARY "MultiThreaded${msvc_rt_suffix}"
  )
if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GCC")
  # When using Clang on Windows, we unfortunately need to manually silence CRT warnings.
  set(base_opts  -funsigned-char -D_CRT_SECURE_NO_WARNINGS)
  set(warn_opts  -Werror -Wall -Wextra -pedantic)
  set(san_opts   -fsanitize=undefined,address)
  set(fuzz_opts  ${san_opts},fuzzer)

  if(CMAKE_BUILD_TYPE MATCHES "DebugFuzzTestingSan" AND CMAKE_C_COMPILER_ID MATCHES "GCC")
    message(FATAL_ERROR "Fuzzing is not supported for GCC.")
  endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
  # MSVC does not support several C standard concepts, attempt to work around that.
  set(base_opts  /J /Drestrict=__restrict "/Dmax_align_t=long double" /D_Static_assert=static_assert /D_CRT_SECURE_NO_WARNINGS)
  # MSVC by default issues pointless warnings like failed inlining, silence them.
  set(warn_opts  /Wall /WX /wd4200 /wd5045 /wd4127 /wd4710 /wd4711 /wd4820)
  if(CMAKE_BUILD_TYPE MATCHES "DebugSan" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfoSan")
    message(FATAL_ERROR "Sanitizing is not supported for MSVC.")
  endif()
else()
  message(FATAL_ERROR "Compiler ${CMAKE_C_COMPILER_ID} is currently not supported.")
endif()
target_compile_options(SimilarityChecker PRIVATE ${base_opts} ${warn_opts})

#
# Build type configuration.
#
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES};ReleaseMP;DebugSan;RelWithDebInfoSan;DebugUnitTestingSan;DebugFuzzTestingSan;RelWithDebInfoFuzzTestingSan" CACHE STRING "" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_BUILD_TYPE};ReleaseMP;DebugSan;RelWithDebInfoSan;DebugUnitTestingSan;DebugFuzzTestingSan;RelWithDebInfoFuzzTestingSan")
if(NOT CMAKE_BUILD_TYPE)
  message("Defaulting to ReleaseMP build.")
  set(CMAKE_BUILD_TYPE ReleaseMP)
endif()

find_package(OpenMP)
if(CMAKE_BUILD_TYPE MATCHES "ReleaseMP")
  if(NOT OPENMP_FOUND)
    message(FATAL_ERROR "OpenMP is unsupported.")
  elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    message(WARNING "OpenMP support may be limited.")
  endif()
endif()

set(CMAKE_C_FLAGS_RELEASEMP "${CMAKE_C_FLAGS_RELEASE} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASEMP "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASEMP "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${OpenMP_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS_DEBUGSAN "${CMAKE_C_FLAGS_DEBUG} ${san_opts}")
set(CMAKE_CXX_FLAGS_DEBUGSAN "${CMAKE_CXX_FLAGS_DEBUG} ${san_opts}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUGSAN "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${san_opts}")

set(CMAKE_C_FLAGS_RELWITHDEBINFOSAN "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${san_opts}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFOSAN "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${san_opts}")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFOSAN "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} ${san_opts}")

set(CMAKE_C_FLAGS_DEBUGUNITTESTINGSAN "${CMAKE_C_FLAGS_DEBUG} ${san_opts}")
set(CMAKE_CXX_FLAGS_DEBUGUNITTESTINGSAN "${CMAKE_CXX_FLAGS_DEBUG} ${san_opts}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUGUNITTESTINGSAN "${CMAKE_EXE_LINKER_FLAGS_DEBUGSAN}")

set(CMAKE_C_FLAGS_DEBUGFUZZTESTINGSAN "${CMAKE_C_FLAGS_DEBUG} ${fuzz_opts}")
set(CMAKE_CXX_FLAGS_DEBUGFUZZTESTINGSAN "${CMAKE_CXX_FLAGS_DEBUG} ${fuzz_opts}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUGFUZZTESTINGSAN "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${fuzz_opts}")

set(CMAKE_C_FLAGS_RELWITHDEBINFOFUZZTESTINGSAN "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} ${fuzz_opts}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFOFUZZTESTINGSAN "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} ${fuzz_opts}")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFOFUZZTESTINGSAN "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} ${fuzz_opts}")
