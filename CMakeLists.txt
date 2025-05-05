cmake_minimum_required(VERSION 3.10)
project(llama_inference)






if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  option(LLAMA_CURL "Enable CURL support" OFF)  # 默认关闭CURL
endif()


set(CMAKE_CXX_STANDARD 17)
set(LLAMA_BUILD_COMMON On)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp" EXCLUDE_FROM_ALL)
add_subdirectory("godot-cpp")

if (APPLE)
  set(ARCH_NAME "universal")
else()
  set(ARCH_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()


set(LIB_NAME lib.${CMAKE_SYSTEM_NAME}.${ARCH_NAME})
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(src_files
 "src/main.cpp"
 "src/gdllamanpc.cpp"
 "src/gdllamanpc.h"
 "src/gdllamaholder.cpp"
 "src/gdllamaholder.h"
 "src/register_types.cpp"
 "src/register_types.h"
 "src/godot-llama.hpp"
 "src/conversion.cpp"
 "src/conversion.hpp"
)


add_library(${LIB_NAME} SHARED ${src_files})

target_link_libraries(
        ${LIB_NAME} PRIVATE godot-cpp common llama 
)





