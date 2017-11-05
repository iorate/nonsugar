
# nonsugar
#
# Copyright iorate 2017.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0.2)

option(CXX17 "C++17" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  if(CXX17)
    add_compile_options(-std=c++17)
  else()
    add_compile_options(-std=c++14)
  endif()
  add_compile_options(-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  if(CXX17)
    add_compile_options(-std=c++17)
  else()
    add_compile_options(-std=c++14)
  endif()
  add_compile_options(-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-disabled-macro-expansion -Wno-global-constructors -Wno-padded -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  if(CXX17)
    add_compile_options(/std:c++17)
  else()
    add_compile_options(/std:c++14)
  endif()
  add_compile_options(/W4 /WX)
endif()

include_directories(${PROJECT_SOURCE_DIR})

enable_testing()

add_subdirectory(example)
add_subdirectory(test)
