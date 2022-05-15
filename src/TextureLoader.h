#pragma once
#include <GL/glew.h>
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
//-----------------------------------------
//----         TEXTURE LOADER          ----
//-----------------------------------------
static class TextureLoader {
public:

	static bool LoadAndSetTexture(const maybewchar* filename, GLenum target);

	static GLuint CreateAndLoadTexture(const maybewchar* filename);

	static GLuint CreateAndLoadTextureCube(
		const maybewchar* filename_px, const maybewchar* filename_nx,
		const maybewchar* filename_py, const maybewchar* filename_ny,
		const maybewchar* filename_pz, const maybewchar* filename_nz);
};