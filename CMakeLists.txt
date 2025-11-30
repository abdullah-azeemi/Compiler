cmake_minimum_required(VERSION 3.13)
project(LoopInterchangePass)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_DIR}")
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_library(LoopInterchangePass MODULE LoopInterchange.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader passes analysis transformutils)

target_link_libraries(LoopInterchangePass PRIVATE ${llvm_libs})
set_target_properties(LoopInterchangePass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
