#This project required Qt5, QWT
#The QWT head files should be put under Qt include files, {Qtdir}/include/QWT
#and the lib files of QWT should be put under Qt lib files, {Qtdir}/lib/qwtd.lib etc.

PROJECT(CMakeExe)
cmake_minimum_required(VERSION 3.0.0)
#set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOUIC ON)

SET(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIR})
endif()

find_package(GLUT)
if(GLUT_FOUND)
	include_directories(${GLUT_INCLUDE_DIR})
endif()

if(Qt5Widgets_FOUND)
	include_directories(${Qt5Widgets_DIR}/../../../include/QWT)
	link_directories(${Qt5Widgets_DIR}/../../../lib)
endif()

add_definitions(-DQWT_DLL)

LINK_DIRECTORIES(${CMakeExe_SOURCE_DIR}/lib)
AUX_SOURCE_DIRECTORY(. SRC_LIST)

set(MISTUDIO_HEADERS 
	visualization.h
	database.h
	singlewidget.h
	processbar.h
	showtable.h
	showtable2.h
	showtable3.h
	showtable4.h
	drawidget.h
	DrawTerminal.h
	MainTerminal.h
	newterminal.h
	timeCity.h
	UV_PV.h
	plot.h
	legend.h
	mainwindow.h
	)
	
set(MISTUDIO_UIS 
	visualization.ui
	mainwindow.ui
	processbar.ui
	showtable.ui
	showtable2.ui
	showtable3.ui
	showtable4.ui
	timeCity.ui
	UVPVwidget.ui
	mainterminal.ui
	)
	
set(MISTUDIO_QRCS visualization.qrc)

set(MISTUDIO_SRCS
	main.cpp 
	data.cpp 
	database.cpp 
	visualization.cpp 
	singlewidget.cpp
	processbar.cpp
	apriori.cpp
	FPtree.cpp
	PRtree.cpp
	showtable.cpp
	showtable2.cpp
	showtable3.cpp
	showtable4.cpp
	drawidget.cpp
	cityDistWindow.cpp
	plot.cpp
	legend.cpp
	griditem.cpp
	quotefactory.cpp
	mainwindow.cpp
	DrawTerminal.cpp
	MainTerminal.cpp
	newTerminal.cpp
	timeCity.cpp
	UV_PV.cpp
	${MISTUDIO_QRCS})

QT5_WRAP_CPP (MISTUDIO_HEADERS_MOC ${MISTUDIO_HEADERS})
QT5_WRAP_UI (MISTUDIO_UIS_UIC ${MISTUDIO_UIS})
QT5_ADD_RESOURCES (MISTUDIO_QRC_RCC ${MISTUDIO_QRCS})

ADD_EXECUTABLE(Visualization
	${SRC_LIST}
	${MISTUDIO_HEADERS_MOC}
	${MISTUDIO_UIS_UIC}
	${MISTUDIO_QRC_RCC}
	visualization.qrc)
	
TARGET_LINK_LIBRARIES(Visualization 
	Qt5::Widgets
	Qt5::Gui
	Qt5::Core
	Qt5::Sql
	Qt5::PrintSupport
	zlibstat
	qwtd.lib
	)