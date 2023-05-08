
cmake_minimum_required(VERSION 3.5)

project(plan LANGUAGES C CXX)

aux_source_directory(. PLAN)

add_library(plan ${PLAN})


target_link_libraries(${PROJECT_NAME} atomic)
