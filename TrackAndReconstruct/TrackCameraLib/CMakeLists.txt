set(target TrackCameraLib)

set(MISTUDIO_HEADERS
	BundleAdjuster.h
	data.h
	FindCameraMatrices.h
	MultiCameraDistance.h
	MultiCameraPnP.h
	RichFeatureMatcher.h
	Triangulation.h
	)
	
set(MISTUDIO_SRCS
	BundleAdjuster.cpp
	data.cpp
	FindCameraMatrices.cpp
	MultiCameraDistance.cpp
	MultiCameraPnP.cpp
	RichFeatureMatcher.cpp
	Triangulation.cpp
	)
	
add_library(${target} 
	${SRC_LIST}
	${MISTUDIO_SRCS}
	)
	
TARGET_LINK_LIBRARIES(${target} 
	${OpenCV_LIBS}
	)