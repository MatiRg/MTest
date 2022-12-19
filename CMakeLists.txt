cmake_minimum_required(VERSION 3.5)

project(MTest CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "In-source builds are not permitted.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	message(STATUS "Detected Clang compiler")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	message(STATUS "Detected GCC compiler")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message(STATUS "Detected MSVC compiler")
else()
	message(FATAL_ERROR "Unsupported compiler.")
endif()

set(SOURCES_EXAMPLES
	Include/MTest.hpp
	Example/Main.cpp
)

add_executable(Example ${SOURCES_EXAMPLES})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(Example PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic -Wshadow -Wnon-virtual-dtor -Wno-cast-function-type)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(Example PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(Example PRIVATE /W4 /w44265 /w44062 /wd4201 /wd4458 /wd4996 /wd4244 /wd4061 /wd4456 /wd4457 /wd4701 /wd4267 /wd4245 /wd4100)
endif()
