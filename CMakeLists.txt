
# nonsugar
#
# Copyright iorate 2017-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0.2)

project(nonsugar)

option(CXX17 "C++17" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
  if(CXX17)
    add_compile_options(-std=c++17)
  else()
    add_compile_options(-std=c++14)
  endif()
  add_compile_options(-Werror -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  if(CXX17)
    add_compile_options(/std:c++17)
  else()
    add_compile_options(/std:c++14)
  endif()
  add_compile_options(/WX /W4)
endif()

include_directories(${PROJECT_SOURCE_DIR})

enable_testing()

add_subdirectory(example)
add_subdirectory(test)
