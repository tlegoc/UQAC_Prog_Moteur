cmake_minimum_required(VERSION 3.16)

project(GL_CPP VERSION 0.0.1 LANGUAGES CXX)

include(../../cmake/common.cmake)

FetchContent_Declare(glfw
  GIT_REPOSITORY  https://github.com/glfw/glfw
  GIT_TAG         3.3.8
)

FetchContent_Declare(glm
  GIT_REPOSITORY  https://github.com/g-truc/glm
  GIT_TAG         0.9.9.8
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(TARGET_NAME gfxapi)

file(GLOB_RECURSE gfxapi_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${TARGET_NAME} STATIC ${gfxapi_SOURCES})

target_precompile_headers(${TARGET_NAME} PRIVATE include/gfxapi/pch/precomp.h)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${glfw_SOURCE_DIR}/include)
target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${glm_SOURCE_DIR})

find_package(Vulkan REQUIRED)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC Vulkan::Vulkan)
