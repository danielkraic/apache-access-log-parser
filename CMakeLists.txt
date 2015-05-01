cmake_minimum_required(VERSION 2.8)
project(apache_log_parser)

add_executable(apache_log_parser Parser.cpp DataStore.cpp main.cpp)

#SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET (CMAKE_CXX_COMPILER "/usr/bin/g++")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall")


target_link_libraries(apache_log_parser mongoclient boost_system boost_regex boost_thread boost_filesystem boost_program_options)