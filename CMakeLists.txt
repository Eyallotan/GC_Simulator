cmake_minimum_required(VERSION 3.10)
project(FlashGC)

set(CMAKE_CXX_STANDARD 11)

add_executable(FlashGC main.cpp ScheduledGC.h ListItem.h Auxilaries.h Auxilaries.cpp)