cmake_minimum_required(VERSION 3.15)
project(pmplib_bindings LANGUAGES CXX)

find_package(Python 3.12 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(
  pmplib
  STABLE_ABI
  pmp_bindings.cpp
)

target_link_libraries(pmplib PRIVATE pmp)

install(TARGETS pmplib LIBRARY DESTINATION .)