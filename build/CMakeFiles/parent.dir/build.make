# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /home/shtykhnoia/university/os_labs/lab1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/shtykhnoia/university/os_labs/build

# Include any dependencies generated for this target.
include CMakeFiles/parent.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/parent.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/parent.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/parent.dir/flags.make

CMakeFiles/parent.dir/parent.c.o: CMakeFiles/parent.dir/flags.make
CMakeFiles/parent.dir/parent.c.o: /home/shtykhnoia/university/os_labs/lab1/parent.c
CMakeFiles/parent.dir/parent.c.o: CMakeFiles/parent.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/shtykhnoia/university/os_labs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/parent.dir/parent.c.o"
	/usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/parent.dir/parent.c.o -MF CMakeFiles/parent.dir/parent.c.o.d -o CMakeFiles/parent.dir/parent.c.o -c /home/shtykhnoia/university/os_labs/lab1/parent.c

CMakeFiles/parent.dir/parent.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/parent.dir/parent.c.i"
	/usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/shtykhnoia/university/os_labs/lab1/parent.c > CMakeFiles/parent.dir/parent.c.i

CMakeFiles/parent.dir/parent.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/parent.dir/parent.c.s"
	/usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/shtykhnoia/university/os_labs/lab1/parent.c -o CMakeFiles/parent.dir/parent.c.s

# Object files for target parent
parent_OBJECTS = \
"CMakeFiles/parent.dir/parent.c.o"

# External object files for target parent
parent_EXTERNAL_OBJECTS =

parent: CMakeFiles/parent.dir/parent.c.o
parent: CMakeFiles/parent.dir/build.make
parent: CMakeFiles/parent.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/shtykhnoia/university/os_labs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable parent"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/parent.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/parent.dir/build: parent
.PHONY : CMakeFiles/parent.dir/build

CMakeFiles/parent.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/parent.dir/cmake_clean.cmake
.PHONY : CMakeFiles/parent.dir/clean

CMakeFiles/parent.dir/depend:
	cd /home/shtykhnoia/university/os_labs/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/shtykhnoia/university/os_labs/lab1 /home/shtykhnoia/university/os_labs/lab1 /home/shtykhnoia/university/os_labs/build /home/shtykhnoia/university/os_labs/build /home/shtykhnoia/university/os_labs/build/CMakeFiles/parent.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/parent.dir/depend

