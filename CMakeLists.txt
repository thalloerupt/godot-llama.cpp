cmake_minimum_required(VERSION 3.10)
project(llama_inference)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        option(LLAMA_CURL "Enable CURL support" OFF)  # 默认关闭
endif()
set(CMAKE_CXX_STANDARD 17)
set(LLAMA_BUILD_COMMON On)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp" EXCLUDE_FROM_ALL)
add_subdirectory("godot-cpp")

if (APPLE)
  set(ARCH_NAME "universal")
else()
  set(ARCH_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()

set(LIB_NAME lib.${CMAKE_SYSTEM_NAME}.${ARCH_NAME})
set(src_files
 "src/main.cpp"
 "src/gdllamanpc.cpp"
 "src/gdllamanpc.h"
 "src/register_types.cpp"
 "src/register_types.h"
 "src/godot-llama.hpp"

)

string(TOLOWER ${LIB_NAME} LIB_NAME)

add_library(${LIB_NAME} SHARED ${src_files})

add_executable(
        chat
        src/main.cpp
)
target_link_libraries(
        chat
        PRIVATE
        common llama ggml
)
target_link_libraries(
        ${LIB_NAME}
        PRIVATE
        common llama ggml godot-cpp
)
