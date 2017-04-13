cmake_minimum_required(VERSION 3.0)

set(target depthToModel)

file (GLOB HEADERS "*.h")
file (GLOB SOURCES "[^_]*.cpp")

set(CMAKE_PREFIX_PATH  
	"E:/thirdLib/OpenCV/opencv/build"
	"D:/Qt/Qt5.4.1/5.4/msvc2013_64_opengl"
	)
set(EIGEN_DIR E:/thirdLib/eigen-3.3.2)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR} 
	${EIGEN_DIR}
	${OpenCV_INCLUDE_DIRS}
	)

add_library(${target} ${SOURCES} ${HEADERS})
#add_executable(${target} ${SOURCES} ${HEADERS})
target_link_libraries(${target} 
	${OpenCV_LIBS}
	Qt5::Core
	Qt5::Gui
	)