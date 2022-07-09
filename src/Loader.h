#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "Geometry.h"
#include "ObjectLoader.h"
#include "TextureLoader.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <GL/freeglut.h>

#define _USE_MATH_DEFINES
#include <math.h>

// Include DevIL for image loading
#if defined(_WIN32)
#pragma comment(lib, "glew32s.lib")
// On Windows, we use Unicode dll of DevIL
// That also means we need to use wide strings
#ifndef _UNICODE
#define _UNICODE
#include <IL/il.h>
#undef _UNICODE
#else
#include <IL/il.h>
#endif
#pragma comment(lib, "DevIL.lib") // Link with DevIL library
typedef wchar_t maybewchar;
#define MAYBEWIDE(s) L##s
#else // On Linux, we need regular (not wide) strings
#include <IL/il.h>
typedef char maybewchar;
#define MAYBEWIDE(s) s
#endif

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

static class Loader: public ObjectLoader, public TextureLoader {
public:

	static void SetDebugCallback(GLDEBUGPROCARB callback);

	/// Loads a file and returns its content as std::string
	static std::string LoadFileToString(const char* file_name);

	/// Waits for Enter and exits the application with exit(1)
	static void WaitForEnterAndExit();

	/// Returns shader object on success or 0 if failed.
	static GLuint LoadAndCompileShader(GLenum shader_type, const char* file_name);

	/// Creates a shader program, loads, compiles and sets the vertex and fragment shaders, links it,
	/// and prints errors if some occur.
	///
	/// Returns program object on success or 0 if failed.
	static GLuint CreateAndLinkProgram(const char* vertex_shader, const char* fragment_shader,
		GLint bind_attrib_0_idx, const char* bind_attrib_0_name,
		GLint bind_attrib_1_idx, const char* bind_attrib_1_name,
		GLint bind_attrib_2_idx, const char* bind_attrib_2_name);

	/// Creates a shader program, loads, compiles and sets the vertex and fragment shaders, links it,
	/// and prints errors if some happens.
	///
	/// Returns program object on success or 0 if failed.
	static GLuint CreateAndLinkProgram(const char* vertex_shader, const char* fragment_shader);

	/// Chooses glDrawArrays or glDrawElements to draw the geometry.
	static void DrawGeometry(const Geometry& geom);

	/// Chooses glDrawArraysInstanced or glDrawElementsInstanced to draw the geometry.
	static void DrawGeometryInstanced(const Geometry& geom, int primcount);
};