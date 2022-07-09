#pragma once
//--------------------------
//----    OBJ LOADER    ----
//--------------------------
#include "Geometry.h"
#include<iostream>
#include<fstream>
#include<vector>

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

#include <glm/glm.hpp>
static class ObjectLoader
{
public:
	/// Parses an OBJ file. It handles only geometries with triangles, and each vertex must have its 
	/// position, normal, and texture coordinate defined. 
	///
	/// When the file is correctly parsed, the function returns true and 'out_vertices', 'out_normals' and
	/// 'out_tex_coords' contains the data of individual triangles (use glDrawArrays with GL_TRIANGLES).
	static bool ParseOBJFile(const char* file_name, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec3>& out_normals, std::vector<glm::vec2>& out_tex_coords);

	/// Loads an OBJ file and creates a corresponding Geometry object.
	///
	/// 'position_location', 'normal_location', and 'tex_coord_location' are locations of vertex attributes,
	/// obtained by glGetAttribLocation. Use -1 if not necessary.
	static Geometry LoadOBJ(const char* file_name, GLint position_location, GLint normal_location = -1, GLint tex_coord_location = -1);

	/// Creates a simple grid object. The center of the grid is in (0,0,0) and the length of its side is 2, its splitted to size * size squares
	/// (positions of its vertices are from -0.5 to 0.5).
	///
	/// 'position_location', 'normal_location', and 'tex_coord_location' are locations of vertex attributes,
	/// obtained by glGetAttribLocation. Use -1 if not necessary.
	static Geometry CreateGrid(int size, GLint position_location, GLint normal_location = -1, GLint tex_coord_location = -1);
};