cmake_minimum_required(VERSION 3.24)
project(sfml_rpg_game)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(sfml_rpg_game main.cpp)

target_link_libraries(sfml_rpg_game sfml-graphics sfml-audio)