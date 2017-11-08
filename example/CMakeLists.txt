
# nonsugar
#
# Copyright iorate 2017.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_custom_target(example)

find_package(Boost)
if(NOT Boost_FOUND)
  message("Boost is required to build some examples.")
endif()

set(example_names compiler subcmd subcmd_od usage)
if(Boost_FOUND)
  list(APPEND example_names custom custom_predicate)
endif()
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  list(APPEND example_names wide)
endif()

foreach(example_name IN LISTS example_names)
  add_executable(${example_name} ${example_name}.cpp)
  add_dependencies(example ${example_name})
endforeach()

if(Boost_FOUND)
  target_include_directories(custom PRIVATE ${Boost_INCLUDE_DIRS})
  target_include_directories(custom_predicate PRIVATE ${Boost_INCLUDE_DIRS})
endif()

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES GNU)
  target_link_libraries(wide PRIVATE -municode)
endif()
