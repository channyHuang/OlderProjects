cmake_minimum_required(VERSION 3.0.0)

project(RelightingLine)

find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})


find_package(PCL)
IF(NOT PCL_DIR)
	MESSAGE(FATAL_ERROR "Please set PCL_DIR.")
ENDIF(NOT PCL_DIR)
include_directories(${PCL_DIR}/../include/pcl-1.7)
include_directories(${PCL_DIR}/../3rdParty/FLANN/include)
include_directories(${PCL_DIR}/../3rdParty/Eigen/eigen3)
include_directories(${PCL_DIR}/../3rdParty/VTK/include/vtk-6.2)

list(APPEND USER_H
	estimate.h
	ply.h
	vectorimage.hpp)
	
list(APPEND USER_CPP
	estimate.cpp
	ply.cpp
	vectorimage.cpp
	)	
	
add_library(RelightingLine	
	STATIC
	${USER_CPP}
	${USER_H}
	)
	
TARGET_LINK_LIBRARIES(RelightingLine
	Qt5::Widgets
	Qt5::Gui
	Qt5::Core
	Qt5::OpenGL
	)	