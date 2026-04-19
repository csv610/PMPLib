cmake_minimum_required(VERSION 3.15)
project(pmplib_bindings LANGUAGES CXX)

find_package(Python 3.9 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(
  _pmplib
  STABLE_ABI
  pmp_bindings.cpp
)

target_link_libraries(_pmplib PRIVATE pmp)

install(TARGETS _pmplib LIBRARY DESTINATION pmplib)