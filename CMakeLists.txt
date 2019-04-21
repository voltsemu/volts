cmake_minimum_required (VERSION 3.8)

#create project
project ("RPCS3X" VERSION 0.0.0 DESCRIPTION "If RPCS3 is so good why isnt there RPCS3 2?")

#enable C++17
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/RPCS3X)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/RPCS3X/Core)

include(ExternalProject)

ExternalProject_Add(ProjectCthulhu
	GIT_REPOSITORY https://github.com/Apache-HB/CTULib.git
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Cthulhu
)

ExternalProject_Get_Property(ProjectCthulhu INSTALL_DIR)
add_library(Cthulhu STATIC IMPORTED)
set_property(TARGET Cthulhu PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/bin/Cthulhu)
add_dependencies(Cthulhu ProjectCthulhu)


file(GLOB_RECURSE RPCS3X_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} RPCS3X/Core/*.cpp)
add_executable(RPCS3X ${RPCS3X_SOURCES})
target_link_directories(RPCS3X PRIVATE)
target_link_libraries(RPCS3X PRIVATE Core FileSystem)
target_link_directories(RPCS3X PRIVATE ${INSTALL_DIR}/src/ProjectCthulhu-build)
target_include_directories(RPCS3X PRIVATE ${INSTALL_DIR}/src/ProjectCthulhu/Cthulhu)
#target_include_directories(RPCS3X PRIVATE "/Users/student/Documents/GitHub/RPCS3X/Build/Cthulhu/src/ProjectCthulhu/Cthulhu")


# #add in source directory
# include_directories(${CMAKE_SOURCE_DIR}/RPCS3X/Core)

# find_library(CTHULHU CTULib)
# include(ExternalProject)

# if(NOT CTHULHU_FOUND)
# 	message(STATUS "Cthulhu not found, grabbing from github")

# 	ExternalProject_Add(CTULib
# 		GIT_REPOSITORY https://github.com/Apache-HB/CTULib.git
# 		PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Cthulhu
# 		EXCLUDE_FROM_ALL TRUE
# 	)
# endif()

# install(DIRECTORY ${COMMON_BASE_PROJECT_DIR}/ DESTINATION "/")

# include_directories(${CMAKE_BINARY_DIR}/Cthulhu)

# ExternalProject_Get_Property(CTULib SOURCE_DIR)
# include_directories(${SOURCE_DIR}/Cthulhu/src/CTULib-build)
# include_directories(${SOURCE_DIR}/Cthulhu/src/CTULib/Cthulhu)
# include_directories(${SOURCE_DIR}/Cthulhu)

# ExternalProject_Get_Property(CTULib BINARY_DIR)
# link_directories(${CMAKE_CURRENT_BINARY_DIR}/Cthulhu)
# link_directories(${BINARY_DIR}/Cthulhu/src/CTULib-build)

# #create main executable
# add_executable(RPCS3X RPCS3X/Core/RPCS3X.cpp)
# target_link_libraries(RPCS3X "${CTULib}")

# add_dependencies(RPCS3X CTULib)
# target_link_libraries(RPCS3X CthulhuCore CthulhuFileSystem)

#these are some horrible hacks to get stuff to build
#it leaves some of the build paths in a mess but it does work
#so for the love of god dont touch this
# - Apache @ 0:32 

#ExternalProject_Add(CTULib
#	GIT_REPOSITORY https://github.com/Apache-HB/CTULib.git
#	SOURCE_DIR ${COMMON_BASE_PROJECT_DIR}
#	BINARY_DIR ${COMMON_BASE_PROJECT_DIR}/Build
#	STEP_TARGETS Build
#	CMAKE_ARGS -DRELEASE=ON -DCMAKE_INSTALL_PREFIX:PATH=INSTALL_DIR
#	EXCLUDE_FROM_ALL TRUE
#)

#install(DIRECTORY ${COMMON_BASE_PROJECT_DIR}/ DESTINATION "/")

#link_directories(${COMMON_BASE_PROJECT_DIR}/Build)

#ExternalProject_Get_Property(CTULib SOURCE_DIR)
#include_directories(${SOURCE_DIR}/Cthulhu)
#link_directories(${CMAKE_BINARY_DIR}/lib/CTULib-prefix/lib)
#target_link_libraries(RPCS3X CthulhuCore CthulhuFileSystem)
#add_dependencies(RPCS3X CTULib)
