cmake_minimum_required(VERSION 3.0)

set(target sensorFusion)

file (GLOB HEADERS "*.h")
file (GLOB SOURCES "[^_]*.cpp")

set(CMAKE_PREFIX_PATH  
	"D:/Qt/Qt5.4.1/5.4/msvc2013_64_opengl"
	"E:/thirdLib/OpenCV/opencv/build"
	)
set(BOOST_ROOT E:/thirdLib/boost_1_56_0)
set(BOOST_INCLUDEDIR E:/thirdLib/boost_1_56_0)
set(BOOST_LIBRARYDIR E:/thirdLib/boost_1_56_0/lib64-msvc-12.0)	
set(EIGEN_DIR E:/thirdLib/eigen-3.3.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../capgSensor)

include_directories(${BOOST_INCLUDEDIR})	
link_directories(${BOOST_LIBRARYDIR})
	
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenCV REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/.." 
	${EIGEN_DIR}
	${OpenCV_INCLUDE_DIRS}
	)

add_library(${target} ${SOURCES} ${HEADERS})
target_link_libraries(${target} 
	${OpenCV_LIBS}
	Qt5::Core
	Qt5::Gui
	capgSensor.lib
	)