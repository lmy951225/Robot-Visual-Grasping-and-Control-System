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
include CMakeFiles/ForceDeviceIP.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ForceDeviceIP.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ForceDeviceIP.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ForceDeviceIP.dir/flags.make

CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o: CMakeFiles/ForceDeviceIP.dir/flags.make
CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o: ../ForceDeviceIP/main.cpp
CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o: CMakeFiles/ForceDeviceIP.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kk/robot_taike10/camport3-master/sample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o -MF CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o.d -o CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o -c /home/kk/robot_taike10/camport3-master/sample/ForceDeviceIP/main.cpp

CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kk/robot_taike10/camport3-master/sample/ForceDeviceIP/main.cpp > CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.i

CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kk/robot_taike10/camport3-master/sample/ForceDeviceIP/main.cpp -o CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.s

# Object files for target ForceDeviceIP
ForceDeviceIP_OBJECTS = \
"CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o"

# External object files for target ForceDeviceIP
ForceDeviceIP_EXTERNAL_OBJECTS =

bin/ForceDeviceIP: CMakeFiles/ForceDeviceIP.dir/ForceDeviceIP/main.cpp.o
bin/ForceDeviceIP: CMakeFiles/ForceDeviceIP.dir/build.make
bin/ForceDeviceIP: libsample_common.a
bin/ForceDeviceIP: /usr/local/lib/libopencv_gapi.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_highgui.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_ml.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_objdetect.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_photo.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_stitching.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_video.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_videoio.so.4.6.0
bin/ForceDeviceIP: /usr/lib/x86_64-linux-gnu/libglut.so
bin/ForceDeviceIP: /usr/lib/x86_64-linux-gnu/libXmu.so
bin/ForceDeviceIP: /usr/lib/x86_64-linux-gnu/libXi.so
bin/ForceDeviceIP: libcloud_viewer.a
bin/ForceDeviceIP: /usr/local/lib/libopencv_imgcodecs.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_dnn.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_calib3d.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_features2d.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_flann.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_imgproc.so.4.6.0
bin/ForceDeviceIP: /usr/local/lib/libopencv_core.so.4.6.0
bin/ForceDeviceIP: /usr/lib/x86_64-linux-gnu/libglut.so
bin/ForceDeviceIP: /usr/lib/x86_64-linux-gnu/libXmu.so
bin/ForceDeviceIP: /usr/lib/x86_64-linux-gnu/libXi.so
bin/ForceDeviceIP: /usr/lib/x86_64-linux-gnu/libGL.so
bin/ForceDeviceIP: /usr/lib/x86_64-linux-gnu/libGLU.so
bin/ForceDeviceIP: CMakeFiles/ForceDeviceIP.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kk/robot_taike10/camport3-master/sample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/ForceDeviceIP"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ForceDeviceIP.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ForceDeviceIP.dir/build: bin/ForceDeviceIP
.PHONY : CMakeFiles/ForceDeviceIP.dir/build

CMakeFiles/ForceDeviceIP.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ForceDeviceIP.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ForceDeviceIP.dir/clean

CMakeFiles/ForceDeviceIP.dir/depend:
	cd /home/kk/robot_taike10/camport3-master/sample/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kk/robot_taike10/camport3-master/sample /home/kk/robot_taike10/camport3-master/sample /home/kk/robot_taike10/camport3-master/sample/build /home/kk/robot_taike10/camport3-master/sample/build /home/kk/robot_taike10/camport3-master/sample/build/CMakeFiles/ForceDeviceIP.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ForceDeviceIP.dir/depend

