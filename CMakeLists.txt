cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
cmake_policy(SET CMP0054 OLD)
cmake_policy(SET CMP0045 OLD)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ProjectDependency)

project(uerr C)
project_dependency(uty GIT_REPOSITORY https://github.com/uael/uty.git)

set(uerr_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(uerr_TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)

file(GLOB_RECURSE uerr_HDRS ${uerr_HDRS} ${uerr_INC_DIR}/uerr/*.h)
set(uerr_HDR ${uerr_INC_DIR}/uerr.h)

add_library(uerr INTERFACE)
target_include_directories(uerr INTERFACE ${uerr_INC_DIR})
target_link_libraries(uerr INTERFACE uty)

get_directory_property(uerr_PARENT PARENT_DIRECTORY)
if (NOT ${uerr_PARENT})
  set(uerr_DEVEL TRUE)
elseif (NOT ${uerr_DEVEL})
  set(uerr_DEVEL FALSE)
endif ()
if (${uerr_DEVEL} AND EXISTS ${uerr_TEST_DIR})
  if (NOT uerr_PARENT)
    enable_testing()
  endif ()

  file(GLOB uerr_TEST_SRCS ${uerr_TEST_SRCS} ${uerr_TEST_DIR}/*.c)
  foreach (uerr_TEST_SRC ${uerr_TEST_SRCS})
    get_filename_component(uerr_TEST_NAME ${uerr_TEST_SRC} NAME_WE)
    add_executable(test_${uerr_TEST_NAME} ${uerr_TEST_SRC})
    add_dependencies(test_${uerr_TEST_NAME} uerr)
    target_link_libraries(test_${uerr_TEST_NAME} uerr)
    add_test(${uerr_TEST_NAME} test_${uerr_TEST_NAME})
  endforeach ()
endif ()

install(FILES ${uerr_HDRS}
  DESTINATION include/uerr)
install(FILES ${uerr_HDR}
  DESTINATION include)
