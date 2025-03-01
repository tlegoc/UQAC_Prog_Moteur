cmake_minimum_required(VERSION 3.25)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT WIN32)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)
    add_link_options(-stdlib=libc++)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
  GIT_TAG         11.0.2
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_Declare(gsl
  GIT_REPOSITORY  https://github.com/microsoft/GSL.git
  GIT_TAG         v4.1.0
  EXCLUDE_FROM_ALL
  SYSTEM
)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
  FetchContent_Populate(fmt)
  add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()

FetchContent_GetProperties(gsl)
if(NOT gsl_POPULATED)
  FetchContent_Populate(gsl)
  add_subdirectory(${gsl_SOURCE_DIR} ${gsl_BINARY_DIR})
endif()

include_directories(SYSTEM ${fmt_SOURCE_DIR}/include)
include_directories(SYSTEM ${gsl_SOURCE_DIR}/include)
