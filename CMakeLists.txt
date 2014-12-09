# 
# This file is part of clutbench.
# 
# Copyright (c) 2014 Flössie <floessie.mail@gmail.com>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
# 

project(clutbench)

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")

set(
	SOURCES
	Application.cpp
	Exception.cpp
	Image.cpp
	IntegerClutMethod.cpp
	OptimizedClutMethod.cpp
	OriginalClutMethod.cpp
	PpmImageReader.cpp
	PpmImageWriter.cpp
	SseClutMethod.cpp
	TestBench.cpp
	Timer.cpp
)

add_executable(clutbench clutbench.cpp ${SOURCES})

target_link_libraries(clutbench rt)
