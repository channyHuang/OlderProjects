cmake_minimum_required(VERSION 3.0)

set(target UpdateVolumn)

file (GLOB HEADERS "*.h")
file (GLOB SOURCES "[^_]*.cpp")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/.."
	)
	
add_library(${target} ${SOURCES} ${HEADERS})
target_link_libraries(${target} 
	)	