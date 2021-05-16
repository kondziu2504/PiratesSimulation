cmake_minimum_required(VERSION 3.19)
project(PiratesSimulation)

set(CMAKE_CXX_STANDARD 17)


add_executable(PiratesSimulation main.cpp Monitor.cpp Monitor.h World.cpp World.h Ship.cpp Ship.h Vec2.h Vec2.cpp Wind.cpp Wind.h Mast.cpp Mast.h Sailor.cpp Sailor.h MastDistributor.cpp MastDistributor.h)
target_link_libraries(PiratesSimulation LINK_PUBLIC noise)
target_link_libraries(PiratesSimulation LINK_PUBLIC ncurses)
target_link_libraries(PiratesSimulation LINK_PUBLIC pthread)
