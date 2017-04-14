set(CMAKE_PREFIX_PATH 
	"D:/Qt/Qt5.4.1/5.4/msvc2013_64_opengl"
	)

find_package(Qt5Widgets REQUIRED)

set(target ReconstructLib)

include_directories(${GSL_INCLUDE_PATH})
include_directories(${EIGEN_DIR})
include_directories(${SSBA_DIR})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

set(MISTUDIO_HEADERS
	adaptiveweight.hpp
	camera.hpp
	geodesicweight.hpp
	linalg.hpp
	lineiter.hpp
	multiviewstereo.hpp
	plane.hpp
	ray.hpp
	surfel.h
	twoviewstereo.hpp
	vectorimage.hpp
	rectification.hpp
	)
	
set(MISTUDIO_SRCS
	adaptiveweight.cpp
	camera.cpp
	geodesicweight.cpp
	lineiter.cpp
	multiviewstereo.cpp
	plane.cpp
	ray.cpp
	surfel.cpp
	twoviewstereo.cpp
	vectorimage.cpp
	rectification.cpp
	)
	
add_library(${target} 
	${MISTUDIO_SRCS}
	)	
	
TARGET_LINK_LIBRARIES(${target} 
	${OpenCV_LIBS}
	Qt5::Widgets
	Qt5::Gui
	Qt5::Core
	${GSL_LIB_PATH}/libgsl-0.lib
	${GSL_LIB_PATH}/libgslcblas-0.lib
	${SSBA_LIB_PATH}/colamd.lib
	${SSBA_LIB_PATH}/V3D.lib
#	${SSBA_LIB_Release_PATH}/colamd.lib
#	${SSBA_LIB_Release_PATH}/V3D.lib}
	)		
	
	