
cmake_minimum_required(VERSION 3.5)

project(control LANGUAGES C CXX)

aux_source_directory(. CONTROL)

add_library(control ${CONTROL})

include_directories("/usr/xenomai/include/")
#include_directories("/usr/xenomai/include/cobalt/")

target_link_libraries(${PROJECT_NAME} pthread ethercat rt)
