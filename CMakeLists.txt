cmake_minimum_required(VERSION 3.19)
project(PiratesSimulation)

set(CMAKE_CXX_STANDARD 14)


add_executable(PiratesSimulation main.cpp Monitor.cpp Monitor.h World.cpp World.h Ship.cpp Ship.h Pos.h Pos.cpp)

target_link_libraries(PiratesSimulation LINK_PUBLIC ncurses)
target_link_libraries(PiratesSimulation LINK_PUBLIC pthread)