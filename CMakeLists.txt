cmake_minimum_required(VERSION 3.13)
project(KaleidoscopeTutorial LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

set(LINK_LIBS)

include(FetchContent)

set(LLVM_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../llvm-project/llvm/build_release/lib/cmake/llvm"
)
find_package(LLVM REQUIRED CONFIG)

list(APPEND LINK_LIBS LLVMCore)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

FetchContent_Declare(
    backward
    GIT_REPOSITORY https://github.com/bombela/backward-cpp
    GIT_TAG master
)
FetchContent_MakeAvailable(backward)
list(APPEND LINK_LIBS Backward::Interface)

get_filename_component(LIB_SRC_PATH "lib" ABSOLUTE)
file(GLOB_RECURSE LIB_SRC CONFIGURE_DEPENDS "${LIB_SRC_PATH}/*.cpp")

add_subdirectory(test)
add_subdirectory(exe)
