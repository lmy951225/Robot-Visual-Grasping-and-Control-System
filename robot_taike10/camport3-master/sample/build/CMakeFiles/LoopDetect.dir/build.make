# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kk/robot_taike10/camport3-master/sample

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kk/robot_taike10/camport3-master/sample/build

# Include any dependencies generated for this target.
include CMakeFiles/LoopDetect.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/LoopDetect.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/LoopDetect.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LoopDetect.dir/flags.make

CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o: CMakeFiles/LoopDetect.dir/flags.make
CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o: ../LoopDetect/main.cpp
CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o: CMakeFiles/LoopDetect.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kk/robot_taike10/camport3-master/sample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o -MF CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o.d -o CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o -c /home/kk/robot_taike10/camport3-master/sample/LoopDetect/main.cpp

CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kk/robot_taike10/camport3-master/sample/LoopDetect/main.cpp > CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.i

CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kk/robot_taike10/camport3-master/sample/LoopDetect/main.cpp -o CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.s

# Object files for target LoopDetect
LoopDetect_OBJECTS = \
"CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o"

# External object files for target LoopDetect
LoopDetect_EXTERNAL_OBJECTS =

bin/LoopDetect: CMakeFiles/LoopDetect.dir/LoopDetect/main.cpp.o
bin/LoopDetect: CMakeFiles/LoopDetect.dir/build.make
bin/LoopDetect: libsample_common.a
bin/LoopDetect: /usr/local/lib/libopencv_gapi.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_highgui.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_ml.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_objdetect.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_photo.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_stitching.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_video.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_videoio.so.4.6.0
bin/LoopDetect: /usr/lib/x86_64-linux-gnu/libglut.so
bin/LoopDetect: /usr/lib/x86_64-linux-gnu/libXmu.so
bin/LoopDetect: /usr/lib/x86_64-linux-gnu/libXi.so
bin/LoopDetect: libcloud_viewer.a
bin/LoopDetect: /usr/local/lib/libopencv_imgcodecs.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_dnn.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_calib3d.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_features2d.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_flann.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_imgproc.so.4.6.0
bin/LoopDetect: /usr/local/lib/libopencv_core.so.4.6.0
bin/LoopDetect: /usr/lib/x86_64-linux-gnu/libglut.so
bin/LoopDetect: /usr/lib/x86_64-linux-gnu/libXmu.so
bin/LoopDetect: /usr/lib/x86_64-linux-gnu/libXi.so
bin/LoopDetect: /usr/lib/x86_64-linux-gnu/libGL.so
bin/LoopDetect: /usr/lib/x86_64-linux-gnu/libGLU.so
bin/LoopDetect: CMakeFiles/LoopDetect.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kk/robot_taike10/camport3-master/sample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/LoopDetect"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LoopDetect.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LoopDetect.dir/build: bin/LoopDetect
.PHONY : CMakeFiles/LoopDetect.dir/build

CMakeFiles/LoopDetect.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LoopDetect.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LoopDetect.dir/clean

CMakeFiles/LoopDetect.dir/depend:
	cd /home/kk/robot_taike10/camport3-master/sample/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kk/robot_taike10/camport3-master/sample /home/kk/robot_taike10/camport3-master/sample /home/kk/robot_taike10/camport3-master/sample/build /home/kk/robot_taike10/camport3-master/sample/build /home/kk/robot_taike10/camport3-master/sample/build/CMakeFiles/LoopDetect.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/LoopDetect.dir/depend

