
#ifndef GL_ERRORH
#define GL_ERRORH

#define glCheckError() glCheckError_(__FILE__, __LINE__)


#include <GL/glew.h>

GLenum glCheckError_(const char *file, int line);

#endif
