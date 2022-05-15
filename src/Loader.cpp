#include "Loader.h"
#include<iostream>
#include<sstream>
using namespace std;

void Loader::SetDebugCallback(GLDEBUGPROCARB callback)
{
    // Setup callback that will inform us when we make an error.
    // glDebugMessageCallbackARB is sometimes missed by glew, due to a bug in it.

#if defined(_WIN32)
    // On Windows, use this:
    PFNGLDEBUGMESSAGECALLBACKARBPROC myglDebugMessageCallbackARB =
        (PFNGLDEBUGMESSAGECALLBACKARBPROC)wglGetProcAddress("glDebugMessageCallbackARB");
    if (myglDebugMessageCallbackARB)
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        myglDebugMessageCallbackARB(callback, nullptr);
    }
#elif defined(__APPLE__)
    // On MacOS, use this (not tested):
    if (glDebugMessageCallbackARB)
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallbackARB(callback, nullptr);
    }
#else
    // On Linux, use this:
    PFNGLDEBUGMESSAGECALLBACKARBPROC myglDebugMessageCallbackARB =
        (PFNGLDEBUGMESSAGECALLBACKARBPROC)glXGetProcAddress((unsigned char*)"glDebugMessageCallbackARB");
    if (myglDebugMessageCallbackARB)
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        myglDebugMessageCallbackARB(callback, nullptr);
    }
#endif
}

string Loader::LoadFileToString(const char* file_name)
{
    ifstream file(file_name);
    stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void Loader::WaitForEnterAndExit()
{
    cout << "Press Enter to exit" << endl;
    getchar();
    exit(1);
}

GLuint Loader::LoadAndCompileShader(GLenum shader_type, const char* file_name)
{
    // Load the file from the disk
    string s_source = LoadFileToString(file_name);
    if (s_source.empty())
    {
        cout << "File " << file_name << " is empty or failed to load" << endl;
        return 0;
    }

    // Create shader object and set the source
    GLuint shader = glCreateShader(shader_type);
    const char* source = s_source.c_str();
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Compile and get errors
    int compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (GL_FALSE == compile_status)
    {
        switch (shader_type)
        {
        case GL_VERTEX_SHADER:          cout << "Failed to compile vertex shader " << file_name << endl;                    break;
        case GL_FRAGMENT_SHADER:        cout << "Failed to compile fragment shader " << file_name << endl;                    break;
        case GL_GEOMETRY_SHADER:        cout << "Failed to compile geometry shader " << file_name << endl;                    break;
        case GL_TESS_CONTROL_SHADER:    cout << "Failed to compile tessellation control shader " << file_name << endl;        break;
        case GL_TESS_EVALUATION_SHADER: cout << "Failed to compile tessellation evaluation shader " << file_name << endl;    break;
        default:                        cout << "Failed to compile shader " << file_name << endl;                            break;
        }

        int log_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        unique_ptr<char[]> log(new char[log_len]);
        glGetShaderInfoLog(shader, log_len, nullptr, log.get());
        cout << log.get() << endl;

        glDeleteShader(shader);
        return 0;
    }
    else return shader;
}

GLuint Loader::CreateAndLinkProgram(const char* vertex_shader, const char* fragment_shader,
    GLint bind_attrib_0_idx, const char* bind_attrib_0_name,
    GLint bind_attrib_1_idx, const char* bind_attrib_1_name,
    GLint bind_attrib_2_idx, const char* bind_attrib_2_name)
{
    // Load the vertex shader
    GLuint vs_shader = LoadAndCompileShader(GL_VERTEX_SHADER, vertex_shader);
    if (0 == vs_shader)
    {
        return 0;
    }

    // Load the fragment shader
    GLuint fs_shader = LoadAndCompileShader(GL_FRAGMENT_SHADER, fragment_shader);
    if (0 == fs_shader)
    {
        glDeleteShader(vs_shader);
        return 0;
    }

    // Create program and attach shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vs_shader);
    glAttachShader(program, fs_shader);

    // Bind attributes
    if (bind_attrib_0_idx != -1)
        glBindAttribLocation(program, bind_attrib_0_idx, bind_attrib_0_name);
    if (bind_attrib_1_idx != -1)
        glBindAttribLocation(program, bind_attrib_1_idx, bind_attrib_1_name);
    if (bind_attrib_2_idx != -1)
        glBindAttribLocation(program, bind_attrib_2_idx, bind_attrib_2_name);

    // Link program
    glLinkProgram(program);

    // Link and get errors
    int link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (GL_FALSE == link_status)
    {
        cout << "Failed to link program with vertex shader " << vertex_shader << " and fragment shader " << fragment_shader << endl;

        int log_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
        unique_ptr<char[]> log(new char[log_len]);
        glGetProgramInfoLog(program, log_len, nullptr, log.get());
        cout << log.get() << endl;

        glDeleteShader(vs_shader);
        glDeleteShader(fs_shader);
        glDeleteProgram(program);
        return 0;
    }
    else return program;
}

GLuint Loader::CreateAndLinkProgram(const char* vertex_shader, const char* fragment_shader)
{
    return CreateAndLinkProgram(vertex_shader, fragment_shader,
        -1, nullptr, -1, nullptr, -1, nullptr);
}

void Loader::DrawGeometry(const Geometry& geom)
{
    if (geom.DrawArraysCount > 0)
        glDrawArrays(geom.Mode, 0, geom.DrawArraysCount);
    if (geom.DrawElementsCount > 0)
        glDrawElements(geom.Mode, geom.DrawElementsCount, GL_UNSIGNED_INT, nullptr);
}

void Loader::DrawGeometryInstanced(const Geometry& geom, int primcount)
{
    if (geom.DrawArraysCount > 0)
        glDrawArraysInstanced(geom.Mode, 0, geom.DrawArraysCount, primcount);
    if (geom.DrawElementsCount > 0)
        glDrawElementsInstanced(geom.Mode, geom.DrawElementsCount, GL_UNSIGNED_INT, (void*)0, primcount);
}