# SimilarityChecker
This project is intended for code similarity checking of several code base, e.g. in context of assignments in an educational environment.

## Repository structure
* **Include**: Contains all exposed data and APIs.
* **CleanseConfig**: Contains configurations on how cleansing operations should be performed per supported programming language.
* **Modules**: Contains general purpose library code.
* **EntryPoints**: Contains the code for all entry point variants (main, unit testing, fuzzing).

## Building
This project uses CMake to build. It's a first-level build system that generates build files for a build system supported by the environment. Common generators are `Visual Studio 16 2019` for Windows (though it is strongly recommended to use `Ninja` and Clang) and `Unix Makefiles` for UNIX-like systems. Currently supported compilers are Clang, GCC and MSVC. Validation has been performed with Clang 10.0.0 on Windows 10 and Debian.  
  
Download: https://cmake.org/download/  
Documentation: https://cmake.org/cmake/help/latest/

### Build types
Several build types are available for easy debugging and deployment. Currently supported Sanitizers are ASan and UBSan.
* **Debug**: Disable optimizations - include debug information.
* **Release**: Optimize for speed - exclude debug information.
* **ReleaseMP**: Optimize for speed and enable multithreading - exclude debug information.
* **MinSizeRel**: Optimize for smallest binary size - exclude debug information.
* **RelWithDebInfo**: Optimize for speed - include debug information.
* **DebugSan**: Disable optimizations and enable Sanitizers - include debug information.
* **RelWithDebInfoSan**: Optimize for speed and enable Sanitizers - include debug information.
* **DebugUnitTestingSan**: Perform unit testing. Disable optimizations and enable Sanitizers - include debug information.
* **DebugFuzzTestingSan**: Disable optimizations and enable fuzzing - include debug information.
* **RelWithDebInfoFuzzTestingSan**: Optimize for speed and enable fuzzing - include debug information.

The default build type (if none is provided explicitly) is ReleaseMP. It is strongly recommended to use it for production usage.  
Please note that not all types are available with every environment.  

### Configuration
The following macros can be defined at build time to configure the runtime behaviour:
* **SC_MAX_FILE_SIZE**: The maximum file size, in bytes, for each of the inputs. The default is 1 MB.
* **SC_MAX_LINE_LENGTH**: The maximum length, in characters, of a single line of the input files. The default is 512 characters.
* **SC_NUM_LINES_SWAP**: The radius to compare lines in the second file to the one of the first file. The default is 3 lines. 

### Getting started
When CMake is invoked, it will auto-detect the environment specifics to generate supported build files. For example, on Linux with 'make' installed, it will generate a 'Makefile' using the compiler 'cc' by default. However, the [generator](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html#cmake-generators) can be overriden using the `-G` option. Please note that you need to manually invoke your second-level build system after generation.  
Build macros, including those above for configuration purposes, can be defined with the `-D` option. To override the default build type, the macro `CMAKE_BUILD_TYPE` needs to be specified.
  
The build command is structured as follows:  
`cmake [options] <path-to-source>`  
  
From the project directory, you can for example build the project using UNIX Makefiles for the 'DebugSan' target with a maximum supported line length of 256 characters as such:  
`cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=DebugSan -DSC_MAX_LINE_LENGTH=256 . && make`  

## Functionality
Several heuristics are intended to be used in order to allow for a very flexible usage.
### Source code analysis
The analysis of the source code is done within mostly two stages.  
The first stage, **Cleansing**, modifies the input into a format that is easy to compare in the context of the used algorithm. Unimportant information is discarded, unnecessary details are generalised and the format is mostly unified.  
The second stage, **Rating**, compares two such cleansed inputs in regards to the configuration and outputs a score of similarity.  

**Cleansing** performs for example: 
  * Removal of whitespaces (such tab characters)
  * Removal of starting, trailing or double new lines
  * Removal of comments
  * Removal of keywords
  * Removal of unimportant constructs (e.g. preprocessor directives for the C Programming Language)
  * Generalisation of primitive types
  * Introduction of new lines where appropiate (e.g. after statement termination)

**Rating** is based on the concept of editing distances. The distance algorithm is applied on an per-line basis and there is an optional window within which all lines are cross-compared. This design is expected to yield good results in cases which involve for example:
* Reordering of statements
* Increased or decreased spacing between statements (including changing between sharing and not sharing the same line)
* Insertion of pointless statements
* Slight modifications are made that do not change the semantics significantly

### Output format
For every successful comparison, a line is output in the following syntax to stdout:
`index1 index2 score`, where both 'index' instances are the file path indices from the launch arguments (starting with 0 for the first file path) and 'score' is a floating-point value between 0 and 1 (with 0 indicating no and 1 indicating highest possible similarity) or `inf` if a comparison was not successful.
In case an error occurs, a diagnostic message is logged onto stderr.
