#include "Geometry.h"
//-------------------------------------------
//----           GEOMETRY CLASS          ----
//-------------------------------------------

Geometry::Geometry()
{
    VertexBuffers[0] = 0;
    VertexBuffers[1] = 0;
    VertexBuffers[2] = 0;
    IndexBuffer = 0;
    VertexArrayObject = 0;
    Mode = GL_POINTS;
    DrawArraysCount = 0;
    DrawElementsCount = 0;
}

Geometry::Geometry(const Geometry& rhs)
{
    *this = rhs;
}

Geometry& Geometry::operator =(const Geometry& rhs)
{
    VertexBuffers[0] = rhs.VertexBuffers[0];
    VertexBuffers[1] = rhs.VertexBuffers[1];
    VertexBuffers[2] = rhs.VertexBuffers[2];
    IndexBuffer = rhs.IndexBuffer;
    VertexArrayObject = rhs.VertexArrayObject;
    Mode = rhs.Mode;
    DrawArraysCount = rhs.DrawArraysCount;
    DrawElementsCount = rhs.DrawElementsCount;
    return *this;
}