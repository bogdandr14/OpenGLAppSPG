#pragma once
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#define _USE_MATH_DEFINES
#pragma comment(lib, "glew32s.lib")
//-------------------------------------------
//----           GEOMETRY CLASS          ----
//-------------------------------------------

	/// This is a class to contain all buffers and vertex array objects for geometries of
	///
	/// When drawing the geometry, bind its VAO and call the draw command. The whole geometry is always
	/// drawn using a single draw call.
class Geometry
{
public:
	Geometry();
	Geometry(const Geometry& rhs);
	Geometry& operator =(const Geometry& rhs);

	// Up to three buffers with the data of the geometry (positions, normals, texture coordinates).
	GLuint VertexBuffers[3];

	// Buffer with the indices of the geometry
	GLuint IndexBuffer;

	// Vertex Array Object with the geometry
	GLuint VertexArrayObject;

	// Type of the primitives to be drawn
	GLenum Mode;
	// Number of vertices to be drawn using glDrawArrays
	GLsizei DrawArraysCount;
	// Number of vertices to be drawn using glDrawElements
	GLsizei DrawElementsCount;
};