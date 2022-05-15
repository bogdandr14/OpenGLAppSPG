#pragma once
#include "Geometry.h"

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

static const float TERRAIN_HEIGHT = 15.0f;
#include <glm/glm.hpp>
#include<functional>

class Terrain : public Geometry {
public:
	std::vector<std::vector<float>> height;

	static Terrain LoadHeightmapTerrain(const maybewchar* filename, GLint position_location, GLint normal_location, GLint tex_coord_location);


	static void RandomModel(const Terrain& terrain_geometry, glm::mat4* model_matrixes, int model_count, std::function<float(float, float, float)> callable); 
};
