#pragma once
#include <cstdio>
#include <assert.h>
#include <algorithm>
#include <stdint.h>

#define ASSERT_GL_ERROR_MACRO(); \
	do \
	{\
		GLenum error = glGetError();\
		if (error != GL_NO_ERROR)\
		{\
			/*std::printf("GL Error : Ox%X \n ", error);*/\
			assert(false); \
		}\
	}while(false)





void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void* userParam);