cmake_minimum_required(VERSION 3.0.0)

set(target PairwiseStereo)

set(CMAKE_PREFIX_PATH 
	"E:/thirdLib/OpenCV/opencv/build"
	"D:/Qt/Qt5.4.1/5.4/msvc2013_64_opengl"
#	"E:/thirdLib/vxl-1.14.0"
	)
	
set(vxl_include_dir "E:/thirdLib/vxl-1.14.0/core" "E:/thirdLib/vxl-1.14.0/vcl" "E:/thirdLib/vxl-1.14.0/vnl" "E:/thirdLib/vxl-1.14.0/build/vcl" "E:/thirdLib/vxl-1.14.0/build/core")
set(vxl_library_dir "E:/thirdLib/vxl-1.14.0/build/lib/debug"
	"E:/thirdLib/vxl-1.14.0/build/lib/debug")	
	
project(${target})
find_package(OpenCV REQUIRED)
find_package(Qt5Core REQUIRED)
#find_package(vxl REQUIRED)

file (GLOB HEADERS "*.h" "*.hpp")
file (GLOB SOURCES "*.cu" "*.c" "*.cpp")

include_directories(${target}
	${OpenCV_INCLUDE_DIRS}
	${vxl_include_dir}
	)

link_directories(${target}
	${vxl_library_dir}
)	
	
add_executable(${target}
	${HEADERS}
	${SOURCES}
)	
	
target_link_libraries(${target}
	${OpenCV_LIBS}
	Qt5::Core
	vcl.lib
	vnl.lib
	vnl_algo.lib
	v3p_netlib.lib
)		