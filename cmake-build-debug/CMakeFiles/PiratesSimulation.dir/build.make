# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/konrad/clion-2021.1.1/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/konrad/clion-2021.1.1/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/konrad/CLionProjects/PiratesSimulation

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/PiratesSimulation.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/PiratesSimulation.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PiratesSimulation.dir/flags.make

CMakeFiles/PiratesSimulation.dir/main.cpp.o: CMakeFiles/PiratesSimulation.dir/flags.make
CMakeFiles/PiratesSimulation.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/PiratesSimulation.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PiratesSimulation.dir/main.cpp.o -c /home/konrad/CLionProjects/PiratesSimulation/main.cpp

CMakeFiles/PiratesSimulation.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PiratesSimulation.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/konrad/CLionProjects/PiratesSimulation/main.cpp > CMakeFiles/PiratesSimulation.dir/main.cpp.i

CMakeFiles/PiratesSimulation.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PiratesSimulation.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/konrad/CLionProjects/PiratesSimulation/main.cpp -o CMakeFiles/PiratesSimulation.dir/main.cpp.s

CMakeFiles/PiratesSimulation.dir/Monitor.cpp.o: CMakeFiles/PiratesSimulation.dir/flags.make
CMakeFiles/PiratesSimulation.dir/Monitor.cpp.o: ../Monitor.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/PiratesSimulation.dir/Monitor.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PiratesSimulation.dir/Monitor.cpp.o -c /home/konrad/CLionProjects/PiratesSimulation/Monitor.cpp

CMakeFiles/PiratesSimulation.dir/Monitor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PiratesSimulation.dir/Monitor.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/konrad/CLionProjects/PiratesSimulation/Monitor.cpp > CMakeFiles/PiratesSimulation.dir/Monitor.cpp.i

CMakeFiles/PiratesSimulation.dir/Monitor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PiratesSimulation.dir/Monitor.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/konrad/CLionProjects/PiratesSimulation/Monitor.cpp -o CMakeFiles/PiratesSimulation.dir/Monitor.cpp.s

CMakeFiles/PiratesSimulation.dir/World.cpp.o: CMakeFiles/PiratesSimulation.dir/flags.make
CMakeFiles/PiratesSimulation.dir/World.cpp.o: ../World.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/PiratesSimulation.dir/World.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PiratesSimulation.dir/World.cpp.o -c /home/konrad/CLionProjects/PiratesSimulation/World.cpp

CMakeFiles/PiratesSimulation.dir/World.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PiratesSimulation.dir/World.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/konrad/CLionProjects/PiratesSimulation/World.cpp > CMakeFiles/PiratesSimulation.dir/World.cpp.i

CMakeFiles/PiratesSimulation.dir/World.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PiratesSimulation.dir/World.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/konrad/CLionProjects/PiratesSimulation/World.cpp -o CMakeFiles/PiratesSimulation.dir/World.cpp.s

CMakeFiles/PiratesSimulation.dir/Ship.cpp.o: CMakeFiles/PiratesSimulation.dir/flags.make
CMakeFiles/PiratesSimulation.dir/Ship.cpp.o: ../Ship.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/PiratesSimulation.dir/Ship.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PiratesSimulation.dir/Ship.cpp.o -c /home/konrad/CLionProjects/PiratesSimulation/Ship.cpp

CMakeFiles/PiratesSimulation.dir/Ship.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PiratesSimulation.dir/Ship.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/konrad/CLionProjects/PiratesSimulation/Ship.cpp > CMakeFiles/PiratesSimulation.dir/Ship.cpp.i

CMakeFiles/PiratesSimulation.dir/Ship.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PiratesSimulation.dir/Ship.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/konrad/CLionProjects/PiratesSimulation/Ship.cpp -o CMakeFiles/PiratesSimulation.dir/Ship.cpp.s

CMakeFiles/PiratesSimulation.dir/Pos.cpp.o: CMakeFiles/PiratesSimulation.dir/flags.make
CMakeFiles/PiratesSimulation.dir/Pos.cpp.o: ../Pos.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/PiratesSimulation.dir/Pos.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PiratesSimulation.dir/Pos.cpp.o -c /home/konrad/CLionProjects/PiratesSimulation/Pos.cpp

CMakeFiles/PiratesSimulation.dir/Pos.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PiratesSimulation.dir/Pos.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/konrad/CLionProjects/PiratesSimulation/Pos.cpp > CMakeFiles/PiratesSimulation.dir/Pos.cpp.i

CMakeFiles/PiratesSimulation.dir/Pos.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PiratesSimulation.dir/Pos.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/konrad/CLionProjects/PiratesSimulation/Pos.cpp -o CMakeFiles/PiratesSimulation.dir/Pos.cpp.s

# Object files for target PiratesSimulation
PiratesSimulation_OBJECTS = \
"CMakeFiles/PiratesSimulation.dir/main.cpp.o" \
"CMakeFiles/PiratesSimulation.dir/Monitor.cpp.o" \
"CMakeFiles/PiratesSimulation.dir/World.cpp.o" \
"CMakeFiles/PiratesSimulation.dir/Ship.cpp.o" \
"CMakeFiles/PiratesSimulation.dir/Pos.cpp.o"

# External object files for target PiratesSimulation
PiratesSimulation_EXTERNAL_OBJECTS =

PiratesSimulation: CMakeFiles/PiratesSimulation.dir/main.cpp.o
PiratesSimulation: CMakeFiles/PiratesSimulation.dir/Monitor.cpp.o
PiratesSimulation: CMakeFiles/PiratesSimulation.dir/World.cpp.o
PiratesSimulation: CMakeFiles/PiratesSimulation.dir/Ship.cpp.o
PiratesSimulation: CMakeFiles/PiratesSimulation.dir/Pos.cpp.o
PiratesSimulation: CMakeFiles/PiratesSimulation.dir/build.make
PiratesSimulation: CMakeFiles/PiratesSimulation.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable PiratesSimulation"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PiratesSimulation.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PiratesSimulation.dir/build: PiratesSimulation

.PHONY : CMakeFiles/PiratesSimulation.dir/build

CMakeFiles/PiratesSimulation.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PiratesSimulation.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PiratesSimulation.dir/clean

CMakeFiles/PiratesSimulation.dir/depend:
	cd /home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/konrad/CLionProjects/PiratesSimulation /home/konrad/CLionProjects/PiratesSimulation /home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug /home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug /home/konrad/CLionProjects/PiratesSimulation/cmake-build-debug/CMakeFiles/PiratesSimulation.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PiratesSimulation.dir/depend

