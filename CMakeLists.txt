cmake_minimum_required (VERSION 3.8)

project ("RPCS3X" VERSION 0.0.0 DESCRIPTION "A PS3 Emulator designed to be more flexible")

add_subdirectory ("RPCS3X")

#enable C++17
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

