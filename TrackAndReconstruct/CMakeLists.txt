cmake_minimum_required(VERSION 3.0)
set(CMAKE_BUILD_TYPE Debug)

project(TrackAndReconstruct)

set(3rd_root  "${CMAKE_CURRENT_SOURCE_DIR}/3rd")
set(OpenCV_SEARCH_PATH ${3rd_root}/OpenCV/build)
set(QT_SEARCH_PATH "" CACHE PATH "Prefix prepended to QT directories")
#set(BOOST_SEARCH_PATH "" CACHE PATH "Prefix prepended to BOOST directories")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_SEARCH_PATH} ${OpenCV_SEARCH_PATH})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(BUILD_STATIC_LIBS ON)

find_package( OpenCV REQUIRED )
set(QTDIR ${QT_SEARCH_PATH}/bin)
set(bin_dirs ${bin_dirs} ${QTDIR})
set(OpenCV_BIN_DIR ${OpenCV_CONFIG_PATH}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(GSL_DIR ${3rd_root}/gsl-64)
set(GSL_INCLUDE_PATH ${GSL_DIR}/include)
set(GSL_LIB_PATH ${GSL_DIR}/lib) 
set(GSL_BIN_DIR ${GSL_DIR}/bin)

set(bin_dirs ${bin_dirs} ${GSL_BIN_DIR})

set(EIGEN_DIR ${3rd_root}/eigen-3.3.2)

set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

set(BOOST_ROOT  ${3rd_root}/boost_msvc12_x64)
find_package(Boost REQUIRED COMPONENTS filesystem system unit_test_framework random date_time log_setup log regex)     
link_directories(${Boost_LIBRARY_DIRS})
set(bin_dirs ${bin_dirs} ${Boost_LIBRARY_DIRS})

set(SSBA_DIR ${3rd_root}/SSBA-3.0)
set(SSBA_LIB_PATH ${SSBA_DIR}/build/Debug)
set(SSBA_LIB_Release_PATH ${SSBA_DIR}/build/Release)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
	 ${Boost_INCLUDE_DIRS}
	 ${OpenCV_INCLUDE_DIRS}
	 ${SSBA_DIR}
	 )

add_subdirectory(TrackCameraLib)
add_subdirectory(ReconstructLib)
#add_subdirectory(TrackCamera)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TrackSeneor)
add_subdirectory(TrackAndReconstruct)

add_subdirectory(CommonDS)
add_subdirectory(Rectify)

add_subdirectory(test)