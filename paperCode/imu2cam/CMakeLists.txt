cmake_minimum_required(VERSION 3.0)

set(target imu2cam)

file (GLOB HEADERS "*.h")
file (GLOB SOURCES "[^_]*.cpp")

set(CMAKE_PREFIX_PATH  
	"D:/Qt/Qt5.4.1/5.4/msvc2013_64_opengl"
	)
set(EIGEN_DIR E:/thirdLib/eigen-3.3.2)
set(BOOST_ROOT E:/thirdLib/boost_1_56_0)
set(BOOST_INCLUDEDIR E:/thirdLib/boost_1_56_0)
set(BOOST_LIBRARYDIR E:/thirdLib/boost_1_56_0/lib64-msvc-12.0)		
	
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../sensorFusion
	${CMAKE_CURRENT_SOURCE_DIR}/../capgSensor
	${CMAKE_CURRENT_SOURCE_DIR}/../KeyframeSelection
)	

include_directories(
	${BOOST_INCLUDEDIR}
	${EIGEN_DIR}
	)	
link_directories(
	${BOOST_LIBRARYDIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../build/sensorFusion/Debug
	${CMAKE_CURRENT_SOURCE_DIR}/../build/capgSensor/Debug
	${CMAKE_CURRENT_SOURCE_DIR}/../build/KeyframeSelection/Debug
	)
	
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)	

add_executable(${target} ${SOURCES} ${HEADERS})

target_link_libraries(${target}
	${Boost_LIBRARIES} 
	Qt5::Core
	Qt5::Gui
	capgSensor.lib
	sensorFusion.lib
	KeyframeSelection.lib
)