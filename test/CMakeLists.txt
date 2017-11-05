
# nonsugar
#
# Copyright iorate 2017.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(test_names argument_test flag_default_read_test flag_help_or_version_test flag_long_test flag_short_test mock_test mock_wstring_test predicate_test subcommand_test usage_test)
if(CXX17)
  set(test_names ${test_names} std_optional_test)
endif()

foreach(test_name IN LISTS test_names)
  add_executable(${test_name} ${test_name}.cpp)
  target_include_directories(${test_name} PRIVATE ${Boost_INCLUDE_DIRS})
  target_link_libraries(${test_name} Boost::unit_test_framework)
  add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
endforeach()
