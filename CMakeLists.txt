cmake_minimum_required(VERSION 3.2.3 FATAL_ERROR)
project(DansSmv)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "static")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/SFML")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/SFML/include")
add_definitions(-DSFML_STATIC)
add_executable(DansSmv "${CMAKE_CURRENT_SOURCE_DIR}/sm.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/viewer.cpp")
target_link_libraries(DansSmv sfml-system sfml-window sfml-graphics)
