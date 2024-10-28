cmake_minimum_required(VERSION 3.19)

project(ChickenDodge VERSION 0.0.1 LANGUAGES CXX)

FetchContent_Declare(yamlcpp
  GIT_REPOSITORY  https://github.com/jbeder/yaml-cpp.git
  GIT_TAG         yaml-cpp-0.7.0
)

FetchContent_GetProperties(yamlcpp)
if(NOT yamlcpp_POPULATED)
  FetchContent_Populate(yamlcpp)
  add_subdirectory(${yamlcpp_SOURCE_DIR} ${yamlcpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

find_package(Vulkan REQUIRED)
file(GLOB_RECURSE shader_SOURCES CONFIGURE_DEPENDS data/*.frag data/*.vert)
foreach(shader ${shader_SOURCES})
  set(shader_SOURCE "${shader}")
  set(shader_OUTPUT "${shader}.spv")
  add_custom_command(
    OUTPUT ${shader_OUTPUT}
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${shader_SOURCE} -g -O0 -o ${shader_OUTPUT}
    DEPENDS ${shader_SOURCE}
  )

  list(APPEND shader_OUTPUTS ${shader_OUTPUT})
endforeach()
add_custom_target(chickendodge_shaders DEPENDS ${shader_OUTPUTS})

set(TARGET_NAME chickendodge)

file(GLOB_RECURSE chickendodge_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(${TARGET_NAME} ${chickendodge_SOURCES})
target_precompile_headers(${TARGET_NAME} PRIVATE include/chickendodge/pch/precomp.h)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_dependencies(${TARGET_NAME} simplege chickendodge_shaders)
target_link_libraries(${TARGET_NAME} PRIVATE simplege)
target_link_libraries(${TARGET_NAME} PRIVATE yaml-cpp)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E create_symlink
${CMAKE_SOURCE_DIR}/examples/chickendodge/data $<TARGET_FILE_DIR:chickendodge>/data)
