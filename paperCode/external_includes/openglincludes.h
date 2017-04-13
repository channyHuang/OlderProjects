#ifndef __OPENGL_INCLUDES_H__
#define __OPENGL_INCLUDES_H__

// #if defined( VC_WINDOWS )
// #include "windows.h"
// #ifdef VC_GLEW
// 
// #include "GL/glew.h"
// 
// #endif
// 
// 
// #include "GL/GLU.h"
// #include "GL/gl.h"
// 
// #elif defined( VC_APPLE )
// 
// #ifdef VC_GLEW
// 
// #include "GL/glew.h"
// 
// #endif
// 
// #include "OpenGL/gl.h"
// #include "OpenGL/glu.h"

// #elif defined(VC_UNIX)

#ifdef VC_GLEW

#include "GL/glew.h"

#endif

#include "GL/glu.h"
#include "GL/gl.h"



#endif


#ifdef VC_QOPENGL_FUNCTIONS

#include "QOpenGLFunctions"
#include "QOpenGLFunctions_3_3_Core"

#endif


// #endif
