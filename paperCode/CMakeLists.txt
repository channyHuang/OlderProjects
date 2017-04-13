cmake_minimum_required (VERSION 2.6)
set(CMAKE_BUILD_TYPE Debug) 
project(dtam)
cmake_policy(SET CMP0012 NEW)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/local_store/Qt/5.3/gcc_64/lib/cmake")# "/local_store/opencv-2.4.9/debug/unix-install/")
#message(STATUS ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ")#-O0 -g -ggdb -fpermissive

if(WIN32)
set(CMAKE_PREFIX_PATH 
	"E:/thirdLib/OpenCV/opencv/build"
	"D:/Qt/Qt5.4.1/5.4/msvc2013_64_opengl"
	"E:/thirdLib/boost_1_56_0"
	)
set(BOOST_LIBRARY_DIR "E:/thirdLib/boost_1_56_0/lib64-msvc-12.0")

set(Thread_INCLUDE_DIR "E:/thirdLib/Thread/Pre-built.2/include")
set(Thread_LIBRARY_DIR "E:/thirdLib/Thread/Pre-built.2/lib")	

set(EIGEN_DIR "E:/thirdLib/eigen-3.3.2")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/capgSensor
	${CMAKE_CURRENT_SOURCE_DIR}/sensorFusion
	)
link_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/build/sensorFusion/Debug
	${CMAKE_CURRENT_SOURCE_DIR}/build/capgSensor/Debug)

include_directories(${Thread_INCLUDE_DIR})
link_directories(${Thread_LIBRARY_DIR})
endif()

#if( WIN32 )
#  set( CMAKE_PREFIX_PATH  "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x64")

#endif()
#set( OpenCV_STATIC ON )
find_package( OpenCV REQUIRED )
# find_package( Qt5Core REQUIRED)
find_package(CUDA REQUIRED)
find_package(Boost COMPONENTS system thread)
find_package(Qt5Widgets)
find_package(Qt5Declarative)
find_package(Qt5MultimediaWidgets)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui)
find_package(Qt5Multimedia)
find_package(Qt5Sql)
find_package(Qt5WebKitWidgets)
#add_definitions( -D__OPENCV_BUILD=1 )
#message(STATUS ${OpenCV_CONSIDERED_CONFIGS})

#set( EIGEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eigen3 CACHE PATH "set eigen dir" ) 
 

add_definitions( -DDATASET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/dataset/DSCF0159.AVI" )
 
#add_library(GraphicsLib SHARED graphics.cpp)#must be in own library so we can link against it.

set( QT_INCLUDES 
                 ${Qt5Widgets_INCLUDE_DIRS}
                 ${Qt5Core_INCLUDE_DIRS} 
				 ${Qt5MultimediaWidgets_INCLUDE_DIRS}
				 ${Qt5OpenGL_INCLUDE_DIRS}
				 ${Qt5Multimedia_INCLUDE_DIRS}
				 ${Qt5Gui_INCLUDE_DIRS}
				 ${Qt5Declarative_INCLUDE_DIRS}
				 ${Qt5WebKitWidgets_INCLUDE_DIRS}
 )
 
 set ( QT_LIBRARIES ${Qt5Widgets_LIBRARIES} 
                    ${Qt5Core_LIBRARIES}
                    ${Qt5MultimediaWidgets_LIBRARIES}
                    ${Qt5OpenGL_LIBRARIES}
                    ${Qt5Multimedia_LIBRARIES}
                    ${Qt5Gui_LIBRARIES}		
                    ${Qt5Declarative_LIBRARIES}		
                    ${Qt5Sql_LIBRARIES}
                    ${Qt5WebKitWidgets_LIBRARIES}
		)
		
set( GLFW3_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/link_libraries/glfw.so CACHE PATH "set glfw3 library" )	

if( WIN32 )

add_definitions( -DVC_WINDOWS )

link_directories(  ${BOOST_LIBRARY_DIR})

message( STATUS " library dir ${BOOST_LIBRARY_DIR} ${Boost_LIBRARIES}" )
else()
set( PTAM_LIBRARIES cvd GVars3 )
endif()
macro (add_sources)
    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND DTAM_SOURCES "${_relPath}/${_src}")
        else()
            list (APPEND DTAM_SOURCES "${_src}")
        endif()
    endforeach()
    if (_relPath)
        # propagate SRCS to parent directory
        set (DTAM_SOURCES ${DTAM_SOURCES} PARENT_SCOPE)
    endif()
endmacro(add_sources)



set(BASEPATH "${CMAKE_SOURCE_DIR}")
 
include_directories( "${BASEPATH}" ${CUDA_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty_includes )
#add_library(GraphicsLib SHARED graphics.cpp)#must be in own library so we can link against it.


add_subdirectory(CostVolume)
add_subdirectory(Track)
add_subdirectory(Optimizer)
add_subdirectory(DepthmapDenoiseWeightedHuber)
add_subdirectory(utils)
add_subdirectory(Scheduler)
add_subdirectory(thirdparty_includes)

add_subdirectory(capgSensor)
add_subdirectory(sensorFusion)
add_subdirectory(stereoMatch)
add_subdirectory(mainFunction)
add_subdirectory(KeyframeSelection)
add_subdirectory(depthToModel)
add_subdirectory(imu2cam)

message( STATUS ${OpenCV_LIBS} )

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -gencode arch=compute_30,code=sm_30; -gencode arch=compute_35,code=sm_35; -gencode arch=compute_50,code=sm_50; --use_fast_math ;--restrict; )#-lineinfo; -O3
cuda_add_library(OpenDTAM ${DTAM_SOURCES} OpenDTAM.cpp)
target_link_libraries(OpenDTAM ${OpenCV_LIBS} opencv_gpu)
add_executable( paramMain main.cpp convertAhandaPovRayToStandard.cpp graphics.cpp)
target_link_libraries( paramMain  
	OpenDTAM 
	${OpenCV_LIBS} 
	${Boost_LIBRARIES} 
	Qt5::Core
	capgSensor.lib
	sensorFusion.lib
	)#${Boost_LIBRARIES} boost_thread

