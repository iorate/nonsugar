
# nonsugar
#
# Copyright iorate 2017-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_custom_target(example)

set(example_names compiler flexible subcmd subcmd_od usage)
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  list(APPEND example_names wide)
endif()

foreach(example_name IN LISTS example_names)
  add_executable(${example_name} ${example_name}.cpp)
  add_dependencies(example ${example_name})
endforeach()

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES GNU)
  target_link_libraries(wide PRIVATE -municode)
endif()
