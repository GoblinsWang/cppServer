# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
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
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wzm/project/network/cppServer/example

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wzm/project/network/cppServer/example/build

# Include any dependencies generated for this target.
include CMakeFiles/tcp_server_main.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tcp_server_main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tcp_server_main.dir/flags.make

CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.o: ../tcp_server_main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.o -c /home/wzm/project/network/cppServer/example/tcp_server_main.cpp

CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/example/tcp_server_main.cpp > CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.i

CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/example/tcp_server_main.cpp -o CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.o: /home/wzm/project/network/cppServer/src/acceptor.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.o -c /home/wzm/project/network/cppServer/src/acceptor.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/acceptor.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/acceptor.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.o: /home/wzm/project/network/cppServer/src/buffer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.o -c /home/wzm/project/network/cppServer/src/buffer.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/buffer.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/buffer.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.o: /home/wzm/project/network/cppServer/src/channel.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.o -c /home/wzm/project/network/cppServer/src/channel.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/channel.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/channel.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.o: /home/wzm/project/network/cppServer/src/eventDispatcher.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.o -c /home/wzm/project/network/cppServer/src/eventDispatcher.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/eventDispatcher.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/eventDispatcher.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.o: /home/wzm/project/network/cppServer/src/eventLoop.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.o -c /home/wzm/project/network/cppServer/src/eventLoop.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/eventLoop.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/eventLoop.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.o: /home/wzm/project/network/cppServer/src/eventLoopThread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.o -c /home/wzm/project/network/cppServer/src/eventLoopThread.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/eventLoopThread.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/eventLoopThread.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.o: /home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.o -c /home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.o: /home/wzm/project/network/cppServer/src/tcpConnection.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.o -c /home/wzm/project/network/cppServer/src/tcpConnection.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/tcpConnection.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/tcpConnection.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.s

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.o: CMakeFiles/tcp_server_main.dir/flags.make
CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.o: /home/wzm/project/network/cppServer/src/tcpServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.o -c /home/wzm/project/network/cppServer/src/tcpServer.cpp

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wzm/project/network/cppServer/src/tcpServer.cpp > CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.i

CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wzm/project/network/cppServer/src/tcpServer.cpp -o CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.s

# Object files for target tcp_server_main
tcp_server_main_OBJECTS = \
"CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.o" \
"CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.o"

# External object files for target tcp_server_main
tcp_server_main_EXTERNAL_OBJECTS =

tcp_server_main: CMakeFiles/tcp_server_main.dir/tcp_server_main.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/acceptor.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/buffer.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/channel.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventDispatcher.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoop.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThread.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/eventLoopThreadPool.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpConnection.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/home/wzm/project/network/cppServer/src/tcpServer.cpp.o
tcp_server_main: CMakeFiles/tcp_server_main.dir/build.make
tcp_server_main: CMakeFiles/tcp_server_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wzm/project/network/cppServer/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX executable tcp_server_main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tcp_server_main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tcp_server_main.dir/build: tcp_server_main

.PHONY : CMakeFiles/tcp_server_main.dir/build

CMakeFiles/tcp_server_main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tcp_server_main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tcp_server_main.dir/clean

CMakeFiles/tcp_server_main.dir/depend:
	cd /home/wzm/project/network/cppServer/example/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wzm/project/network/cppServer/example /home/wzm/project/network/cppServer/example /home/wzm/project/network/cppServer/example/build /home/wzm/project/network/cppServer/example/build /home/wzm/project/network/cppServer/example/build/CMakeFiles/tcp_server_main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tcp_server_main.dir/depend
