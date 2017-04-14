set(target CommonDS)

set(MISTUDIO_HEADERS
	CalibrateParameters.h
	Edge.h
	MathAction.h
	Matrix.h
	MatrixGao.h
	Point2D.h
	Point3D.h
	SpotMath.h
	)
	
set(MISTUDIO_SRCS
	CalibrateParameters.cpp
	MathAction.cpp
	Matrix.cpp
	MatrixGao.cpp
	Point2D.cpp
	Point3D.cpp
	SpotMath.cpp
	)
	
add_library(${target} 
	${MISTUDIO_SRCS}
	)
	
TARGET_LINK_LIBRARIES(${target} 
	)