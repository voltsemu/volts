cmake_minimum_required (VERSION 3.8)

#create project
project ("RPCS3X" VERSION 0.0.0 DESCRIPTION "If RPCS3 is so good why isnt there RPCS3 2?")

#create main executable
add_executable(RPCS3X RPCS3X/Core/RPCS3X.cpp)

#enable C++17
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

include_directories(${CMAKE_SOURCE_DIR}/RPCS3X/Core)
#include external project for grabbing external libs from git
include(ExternalProject)


#these are some horrible hacks to get stuff to build
#it leaves some of the build paths in a mess but it does work
#so for the love of god dont touch this
# - Apache @ 0:32 
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/Extern)

ExternalProject_Add(CTULib
	GIT_REPOSITORY https://github.com/Apache-HB/CTULib.git
	INSTALL_COMMAND make DESTDIR=${EXTERNAL_INSTALL_LOCATION} install
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=INSTALL_DIR
)

install(DIRECTORY ${EXTERNAL_INSTALL_LOCATION}/ DESTINATION "/")

ExternalProject_Get_Property(CTULib SOURCE_DIR)
include_directories(${SOURCE_DIR}/Cthulhu)

include_directories(${EXTERNAL_INSTALL_LOCATION}/)

add_dependencies(RPCS3X CTULib)
target_link_libraries(RPCS3X -L${EXTERNAL_INSTALL_LOCATION}/)
