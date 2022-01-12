cmake_minimum_required(VERSION 3.19)
project(PiratesSimulation)

set(CMAKE_CXX_STANDARD 17)


add_executable(PiratesSimulation main.cpp Monitor.cpp Monitor.h World.cpp World.h Ship/Ship.cpp Ship/Ship.h Ship/ShipLayout.cpp Ship/ShipLayout.h Util/Vec2.h Util/Vec2.cpp Wind.cpp Wind.h Ship/Mast.cpp Ship/Mast.h Ship/Sailor.cpp Ship/Sailor.h Ship/MastDistributor.cpp Ship/MastDistributor.h Util/Util.h Util/Util.cpp Ship/Cannon.cpp Ship/Cannon.h Ship/Cannonball.cpp Ship/Cannonball.h Ship/ShipObject.h WorldObject.h WorldObject.cpp Ship/ShipBody.cpp Ship/ShipBody.h Ship/Crew.cpp Ship/Crew.h Ship/ShipController.cpp Ship/ShipController.h Ship/ShipState.h Util/NcursesUtil.cpp Util/NcursesUtil.h Util/Rect.cpp Util/Rect.h Util/Vec2i.cpp Util/Vec2i.h)
target_link_libraries(PiratesSimulation LINK_PUBLIC ncurses)
target_link_libraries(PiratesSimulation LINK_PUBLIC pthread)
