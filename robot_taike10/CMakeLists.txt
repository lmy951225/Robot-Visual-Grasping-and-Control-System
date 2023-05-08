cmake_minimum_required(VERSION 3.5)

project(amc VERSION 1.5.0)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DUNIX)
add_definitions(-fPIC)

set(RELY_LINK_LIB atomic pthread stdc++fs dl)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/config)
add_definitions(-DAMC_INSTALL_PATH="${CMAKE_INSTALL_PREFIX}")

set(RT_TIMER "XENOMAI3" CACHE STRING "")

add_definitions(-DAMC_USE_XENOMAI3)
set(XENOMAI_ROOT_DIR "/usr/xenomai")
find_package(Xenomai REQUIRED QUIET)
set(RELY_INCL_DIR ${XENOMAI_INCLUDE_DIRS} ${RELY_INCL_DIR})
set(RELY_LINK_DIR "/usr/xenomai/lib" ${RELY_LINK_DIR})
set(RELY_LINK_LIB ${XENOMAI_LIBRARIES} ${RELY_LINK_LIB})

message("RT_TIMER : ${RT_TIMER}")

# ethercat #
set(ETHERCAT "ETHERLAB" CACHE STRING "")

add_definitions(-DAMC_USE_ETHERLAB)
set(RELY_INCL_DIR "/opt/etherlab/include" ${RELY_INCL_DIR})
set(RELY_LINK_DIR "/opt/etherlab/lib" ${RELY_LINK_DIR})
set(RELY_LINK_LIB ethercat ${RELY_LINK_LIB})
message("ETHERCAT : ${ETHERCAT}")

message("ethercat and xenomai libs: ${RELY_LINK_LIB}")
link_directories(${RELY_LINK_DIR})

#include and src
include_directories(./ ./control ./core ./dynamic ./plan ./robot ./server ./sensor)
aux_source_directory(. DIRSRCS)

add_subdirectory(control)
add_subdirectory(core)
add_subdirectory(dynamic)
add_subdirectory(plan)
add_subdirectory(robot)
add_subdirectory(server)
add_subdirectory(sensor)

link_directories(/usr/xenomai/lib/)

add_executable(${PROJECT_NAME} ${DIRSRCS})

target_link_libraries(${PROJECT_NAME} control core dynamic plan robot server sensor ${RELY_LINK_LIB})

