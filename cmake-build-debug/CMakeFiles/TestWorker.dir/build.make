# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /tmp/tmp.VCoEUiMimF

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.VCoEUiMimF/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/TestWorker.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/TestWorker.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/TestWorker.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TestWorker.dir/flags.make

CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o: CMakeFiles/TestWorker.dir/flags.make
CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o: ../tests/TestWorker.cc
CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o: CMakeFiles/TestWorker.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.VCoEUiMimF/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o -MF CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o.d -o CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o -c /tmp/tmp.VCoEUiMimF/tests/TestWorker.cc

CMakeFiles/TestWorker.dir/tests/TestWorker.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TestWorker.dir/tests/TestWorker.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.VCoEUiMimF/tests/TestWorker.cc > CMakeFiles/TestWorker.dir/tests/TestWorker.cc.i

CMakeFiles/TestWorker.dir/tests/TestWorker.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TestWorker.dir/tests/TestWorker.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.VCoEUiMimF/tests/TestWorker.cc -o CMakeFiles/TestWorker.dir/tests/TestWorker.cc.s

# Object files for target TestWorker
TestWorker_OBJECTS = \
"CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o"

# External object files for target TestWorker
TestWorker_EXTERNAL_OBJECTS =

TestWorker: CMakeFiles/TestWorker.dir/tests/TestWorker.cc.o
TestWorker: CMakeFiles/TestWorker.dir/build.make
TestWorker: CMakeFiles/TestWorker.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.VCoEUiMimF/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable TestWorker"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TestWorker.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TestWorker.dir/build: TestWorker
.PHONY : CMakeFiles/TestWorker.dir/build

CMakeFiles/TestWorker.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TestWorker.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TestWorker.dir/clean

CMakeFiles/TestWorker.dir/depend:
	cd /tmp/tmp.VCoEUiMimF/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.VCoEUiMimF /tmp/tmp.VCoEUiMimF /tmp/tmp.VCoEUiMimF/cmake-build-debug /tmp/tmp.VCoEUiMimF/cmake-build-debug /tmp/tmp.VCoEUiMimF/cmake-build-debug/CMakeFiles/TestWorker.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TestWorker.dir/depend
