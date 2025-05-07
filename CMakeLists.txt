cmake_minimum_required(VERSION 3.10)
project(llama_inference)






if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  option(LLAMA_CURL "Enable CURL support" OFF)  # 默认关闭CURL
endif()


set(CMAKE_CXX_STANDARD 17)
set(LLAMA_BUILD_COMMON On)
set(BUILD_SHARED_LIBS OFF)


if (APPLE)
  set(ARCH_NAME "universal")
else()
  set(ARCH_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()



add_subdirectory(godot-cpp)
add_subdirectory(llama.cpp EXCLUDE_FROM_ALL)

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
 "llama.cpp/examples/llava/llava.cpp"
 "llama.cpp/examples/llava/llava.h"
 "llama.cpp/examples/llava/clip.cpp"
 "llama.cpp/examples/llava/clip.h"
)

set(LIB_NAME lib.${CMAKE_SYSTEM_NAME}.${ARCH_NAME})
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(${LIB_NAME} SHARED ${src_files})

target_link_libraries(
        ${LIB_NAME} PRIVATE godot-cpp common llama 
)

set(LLM_LIB_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glc)

if (WIN32)
  install(TARGETS ${LIB_NAME}
    RUNTIME
    DESTINATION ${LLM_LIB_INSTALL_DIR}/bin
  )
else()
  install(TARGETS ${LIB_NAME}
    LIBRARY
    DESTINATION ${LLM_LIB_INSTALL_DIR}/bin
  )
endif()




