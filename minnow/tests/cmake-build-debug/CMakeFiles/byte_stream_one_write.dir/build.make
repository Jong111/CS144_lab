# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cs144/minnow/tests

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cs144/minnow/tests/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/byte_stream_one_write.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/byte_stream_one_write.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/byte_stream_one_write.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/byte_stream_one_write.dir/flags.make

CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o: CMakeFiles/byte_stream_one_write.dir/flags.make
CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o: /home/cs144/minnow/tests/byte_stream_one_write.cc
CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o: CMakeFiles/byte_stream_one_write.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cs144/minnow/tests/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o -MF CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o.d -o CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o -c /home/cs144/minnow/tests/byte_stream_one_write.cc

CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cs144/minnow/tests/byte_stream_one_write.cc > CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.i

CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cs144/minnow/tests/byte_stream_one_write.cc -o CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.s

# Object files for target byte_stream_one_write
byte_stream_one_write_OBJECTS = \
"CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o"

# External object files for target byte_stream_one_write
byte_stream_one_write_EXTERNAL_OBJECTS =

byte_stream_one_write: CMakeFiles/byte_stream_one_write.dir/byte_stream_one_write.o
byte_stream_one_write: CMakeFiles/byte_stream_one_write.dir/build.make
byte_stream_one_write: libminnow_testing_debug.a
byte_stream_one_write: CMakeFiles/byte_stream_one_write.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cs144/minnow/tests/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable byte_stream_one_write"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/byte_stream_one_write.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/byte_stream_one_write.dir/build: byte_stream_one_write
.PHONY : CMakeFiles/byte_stream_one_write.dir/build

CMakeFiles/byte_stream_one_write.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/byte_stream_one_write.dir/cmake_clean.cmake
.PHONY : CMakeFiles/byte_stream_one_write.dir/clean

CMakeFiles/byte_stream_one_write.dir/depend:
	cd /home/cs144/minnow/tests/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cs144/minnow/tests /home/cs144/minnow/tests /home/cs144/minnow/tests/cmake-build-debug /home/cs144/minnow/tests/cmake-build-debug /home/cs144/minnow/tests/cmake-build-debug/CMakeFiles/byte_stream_one_write.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/byte_stream_one_write.dir/depend

