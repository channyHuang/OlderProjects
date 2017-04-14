set(target testAll)

include_directories(${EIGEN_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ReconstructLib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../TrackCameraLib)

set(MISTUDIO_HEADERS
	../TrackAndReconstruct/readAndRun.hpp
	)

set(MISTUDIO_SRCS
	test.cpp
	)
	
ADD_EXECUTABLE(${target}
	${MISTUDIO_SRCS}
	)
	
TARGET_LINK_LIBRARIES(${target}
	ReconstructLib
	TrackCameraLib
	)	
	
add_custom_command(TARGET ${target} POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy
     "$<TARGET_FILE:${target}>"
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set( APPS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}.exe)
install(CODE "include(BundleUtilities)
fixup_bundle(\"${APPS}\"   \"\"   \"${bin_dirs}\") ") 		