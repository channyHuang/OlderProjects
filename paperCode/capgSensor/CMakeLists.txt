file (GLOB HEADERS "*.h")
file (GLOB SOURCES "[^_]*.cpp")

set(target capgSensor)

set(BOOST_ROOT E:/thirdLib/boost_1_56_0)
set(BOOST_INCLUDEDIR E:/thirdLib/boost_1_56_0)
set(BOOST_LIBRARYDIR E:/thirdLib/boost_1_56_0/lib64-msvc-12.0)	

include_directories(${BOOST_INCLUDEDIR})

add_library(${target} STATIC ${SOURCES} ${HEADERS})