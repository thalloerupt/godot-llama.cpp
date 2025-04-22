cmake_minimum_required(VERSION 3.10)
project(llama_inference)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        option(LLAMA_CURL "Enable CURL support" OFF)  # 默认关闭
endif()
set(CMAKE_CXX_STANDARD 17)
set(LLAMA_BUILD_COMMON On)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp")



add_executable(
        chat
        src/main.cpp
)
target_link_libraries(
        chat
        PRIVATE
        common llama ggml
)

