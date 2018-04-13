#include "stdafx.h"
#include "CheckGl.h"

const char*
ETB_GL_DEBUG_SOURCE_STR(GLenum source)
{
	static const char* sources[] = {
		"API",   "Window System", "Shader Compiler", "Third Party", "Application",
		"Other", "Unknown"
	};

	int str_idx =
		std::min(source - GL_DEBUG_SOURCE_API,
			sizeof(sources) / sizeof(const char *));

	return sources[str_idx];
}

const char*
ETB_GL_DEBUG_TYPE_STR(GLenum type)
{
	static const char* types[] = {
		"Error",       "Deprecated Behavior", "Undefined Behavior", "Portability",
		"Performance", "Other",               "Unknown"
	};

	int str_idx =
		std::min(type - GL_DEBUG_TYPE_ERROR,
			sizeof(types) / sizeof(const char *));

	return types[str_idx];
}

const char*
ETB_GL_DEBUG_SEVERITY_STR(GLenum severity)
{
	static const char* severities[] = {
		"High", "Medium", "Low", "Unknown"
	};

	int str_idx =
		std::min(severity - GL_DEBUG_SEVERITY_HIGH,
			sizeof(severities) / sizeof(const char *));

	return severities[str_idx];
}


void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void* userParam)
{
	if (GL_DEBUG_TYPE_OTHER == type)
	{
		return;
	}
	std::fprintf(stderr, "OpenGL Error:\n");
	std::fprintf(stderr, "=============\n");

	std::fprintf(stderr, " Object ID: ");
	std::fprintf(stderr, "%d\n", id);

	std::fprintf(stderr, " Severity:  ");
	std::fprintf(stderr, "%s\n", ETB_GL_DEBUG_SEVERITY_STR(severity));

	std::fprintf(stderr, " Type:      ");
	std::fprintf(stderr, "%s\n", ETB_GL_DEBUG_TYPE_STR(type));

	std::fprintf(stderr, " Source:    ");
	std::fprintf(stderr, "%s\n", ETB_GL_DEBUG_SOURCE_STR(source));

	std::fprintf(stderr, " Message:   ");
	std::fprintf(stderr, "%s\n\n", message);
}
