
# nonsugar
#
# Copyright iorate 2017-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

find_package(Boost COMPONENTS unit_test_framework)
if(NOT Boost_FOUND)
  message("Boost.Test is required to build tests.")
  return()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
  add_compile_options(-Wno-deprecated-declarations)
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  add_compile_options(/wd4996)
endif()

set(test_names argument_test argument_order_test flag_default_read_test flag_help_or_version_test flag_long_test flag_short_test mock_test mock_wstring_test parse_test predicate_test subcommand_test usage_test)
if(CXX17)
  list(APPEND test_names parse_optional_test std_optional_test)
endif()

foreach(test_name IN LISTS test_names)
  add_executable(${test_name} ${test_name}.cpp)
  target_include_directories(${test_name} PRIVATE ${Boost_INCLUDE_DIRS})
  target_link_libraries(${test_name} Boost::unit_test_framework)
  add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
endforeach()
