#pragma region includes
#include "Loader.h"
#include "Terrain.h"
#include "CameraInput.h"
#include "ConstantsAndStructs.h"
#include "InputHandler.h"
#include <iostream>
#include <random>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

// Include GLEW, use static library
#define GLEW_STATIC
#include <GL/glew.h>
#pragma comment(lib, "glew32s.lib") // Link with GLEW library

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

// Include FreeGLUT
#include <GL/freeglut.h>

// Include the most important GLM functions
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma endregion
// Current window size
const int WIN_WIDTH = 1920;
const int WIN_HEIGHT = 1080;

TerrainData terrain_data;
NatureData nature_data;
WaterData water_data;
Light lights[LIGHT_COUNT]; 
UBO ubo;
Camera camera;
Material material;
CameraInput camera_input;
InputHandler handle_input;

// Current time of the application in seconds, for animations
float app_time = 0.0f;
float animation_speed = 0.020f;

#pragma region input handle
// Called when the user presses a key
void key_down(unsigned char key, int mouseX, int mouseY)
{
	handle_input.OnKeyDown(key, mouseX, mouseY, &animation_speed);
}

// Called when the user lifts a key
void key_up(unsigned char key, int mouseX, int mouseY)
{
	handle_input.OnKeyUp(key, mouseX, mouseY);
}

// Called when the user moves with the mouse (when some mouse button is pressed)
void mouse_moved(int x, int y)
{
	handle_input.OnMouseMove(x, y, WIN_WIDTH, WIN_HEIGHT);
}
#pragma endregion

// Initializes OpenGL stuff
void createGeometries(int position_loc,int normal_loc, int tex_coord_loc) {
	terrain_data.geometry = Terrain::LoadHeightmapTerrain(MAYBEWIDE("resources/heightmap.png"), position_loc, normal_loc, tex_coord_loc);
	nature_data.tree_geometry = Loader::LoadOBJ("resources/tree1.obj", position_loc, normal_loc, tex_coord_loc);
	nature_data.bush_geometry = Loader::LoadOBJ("resources/bush.obj", position_loc, normal_loc, tex_coord_loc);
	water_data.geometry = Loader::CreateGrid(200, position_loc, normal_loc, tex_coord_loc);
	for (int i = 0; i < 12; ++i) {
		std::ostringstream buffer;
		buffer << "resources/grass" << std::to_string(i + 1) << ".obj";
		nature_data.long_grass_geometry[i] = Loader::LoadOBJ(buffer.str().c_str(), position_loc, normal_loc, tex_coord_loc);
	}
	nature_data.lamp_geometry = Loader::LoadOBJ("resources/lamp.obj", position_loc, normal_loc, tex_coord_loc);
}

#pragma region initialize
void initTerrain(int position_loc, int normal_loc, int tex_coord_loc) {
	//Initializer::initTerrain(position_loc, normal_loc, tex_coord_loc, &terrain_data);
	terrain_data.program = Loader::CreateAndLinkProgram("shaders/terrain_vertex.glsl", "shaders/terrain_fragment.glsl",
		position_loc, "position", normal_loc, "normal", tex_coord_loc, "tex_coord");
	if (0 == terrain_data.program)
		Loader::WaitForEnterAndExit();

	int terrain_light_loc = glGetUniformBlockIndex(terrain_data.program, "LightData");
	glUniformBlockBinding(terrain_data.program, terrain_light_loc, 0);

	int terrain_camera_loc = glGetUniformBlockIndex(terrain_data.program, "CameraData");
	glUniformBlockBinding(terrain_data.program, terrain_camera_loc, 1);

	int terrain_material_loc = glGetUniformBlockIndex(terrain_data.program, "MaterialData");
	glUniformBlockBinding(terrain_data.program, terrain_material_loc, 2);

	terrain_data.grass_tex_loc = glGetUniformLocation(terrain_data.program, "grass_tex");
	terrain_data.rocks_tex_loc = glGetUniformLocation(terrain_data.program, "rocks_tex");

	terrain_data.model_matrix_loc = glGetUniformLocation(terrain_data.program, "model_matrix");
}

void initNature(int position_loc, int normal_loc, int tex_coord_loc) {
	nature_data.program = Loader::CreateAndLinkProgram("shaders/tree_vertex.glsl", "shaders/tree_fragment.glsl",
		position_loc, "position", normal_loc, "normal", tex_coord_loc, "tex_coord");
	if (0 == nature_data.program)
		Loader::WaitForEnterAndExit();

	int tree_light_loc = glGetUniformBlockIndex(nature_data.program, "LightData");
	glUniformBlockBinding(nature_data.program, tree_light_loc, 0);

	int tree_camera_loc = glGetUniformBlockIndex(nature_data.program, "CameraData");
	glUniformBlockBinding(nature_data.program, tree_camera_loc, 1);

	int tree_material_loc = glGetUniformBlockIndex(nature_data.program, "MaterialData");
	glUniformBlockBinding(nature_data.program, tree_material_loc, 2);

	int tree_tree_data_loc = glGetUniformBlockIndex(nature_data.program, "TreeData");
	glUniformBlockBinding(nature_data.program, tree_tree_data_loc, 3);

	nature_data.tex_loc = glGetUniformLocation(nature_data.program, "tree_tex");

	nature_data.model_matrix_loc = glGetUniformLocation(nature_data.program, "model_matrix");

	nature_data.wind_height_loc = glGetUniformLocation(nature_data.program, "wind_height");

	nature_data.app_time_loc = glGetUniformLocation(nature_data.program, "app_time");

}

void initWater(int position_loc, int normal_loc, int tex_coord_loc) {
	water_data.program = Loader::CreateAndLinkProgram("shaders/water_vertex.glsl", "shaders/water_fragment.glsl",
		position_loc, "position", normal_loc, "normal", tex_coord_loc, "tex_coord");
	if (0 == water_data.program)
		Loader::WaitForEnterAndExit();

	int water_light_loc = glGetUniformBlockIndex(water_data.program, "LightData");
	glUniformBlockBinding(water_data.program, water_light_loc, 0);

	int water_camera_loc = glGetUniformBlockIndex(water_data.program, "CameraData");
	glUniformBlockBinding(water_data.program, water_camera_loc, 1);

	int water_material_loc = glGetUniformBlockIndex(water_data.program, "MaterialData");
	glUniformBlockBinding(water_data.program, water_material_loc, 2);

	water_data.model_matrix_loc = glGetUniformLocation(water_data.program, "model_matrix");
	water_data.app_time_loc = glGetUniformLocation(water_data.program, "app_time");
	water_data.normal_tex_loc = glGetUniformLocation(water_data.program, "water_normal_tex");
	water_data.reflection_tex_loc = glGetUniformLocation(water_data.program, "reflection_tex");
}

void initLight() {
	for (int i = 0; i < LIGHT_COUNT; ++i) {
		lights[i].position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0);
		lights[i].ambient_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		lights[i].diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		lights[i].specular_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	glGenBuffers(1, &(ubo.lights));
	glBindBuffer(GL_UNIFORM_BUFFER, ubo.lights);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * LIGHT_COUNT, &lights, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void randomGeneration() {
	Terrain::GenerateRandomModel(terrain_data.geometry, nature_data.tree_data, TREE_COUNT, [](float x, float y, float z) { return y < 0.2 ? 0.0 : y; });
	glGenBuffers(1, &(ubo.tree));
	glBindBuffer(GL_UNIFORM_BUFFER, ubo.tree);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(nature_data.tree_data), &(nature_data.tree_data), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	Terrain::GenerateRandomModel(terrain_data.geometry, nature_data.tree_data, BUSH_COUNT, [](float x, float y, float z) { return y < 0.3 ? 0.0f : 1.0; });
	glGenBuffers(1, &(ubo.bush));
	glBindBuffer(GL_UNIFORM_BUFFER, ubo.bush);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(nature_data.tree_data), &(nature_data.tree_data), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for (int i = 0; i < 12; ++i) {
		Terrain::GenerateRandomModel(terrain_data.geometry, nature_data.tree_data, GRASS_COUNT, [](float x, float y, float z) { return y < 0.15 ? 0.02 : 1 - y / 2; });
		glGenBuffers(1, &(ubo.long_grass[i]));
		glBindBuffer(GL_UNIFORM_BUFFER, ubo.long_grass[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(nature_data.tree_data), &(nature_data.tree_data), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

void initCamera() {
	camera.view_matrix = glm::mat4(1.0f);
	camera.projection_matrix = glm::mat4(1.0f);
	camera.eye_position = glm::vec3(0.0f);

	glGenBuffers(1, &(ubo.camera));
	glBindBuffer(GL_UNIFORM_BUFFER, (ubo.camera));
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Camera), &camera, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void initMaterial() {
	material.ambient_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular_color = glm::vec4(0.1f, 0.1f, 0.1f, 0.1f);
	material.shininess = 1.0f;

	glGenBuffers(1, &(ubo.material));
	glBindBuffer(GL_UNIFORM_BUFFER, ubo.material);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), &material, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void initReflection() {
	glGenRenderbuffers(1, &(water_data.reflection_depth));
	glBindRenderbuffer(GL_RENDERBUFFER, water_data.reflection_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, water_data.reflection_depth);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, water_data.reflection_tex, 0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
}

void applyTextures() {
	// Grass texture
	terrain_data.grass_tex = Loader::CreateAndLoadTexture(MAYBEWIDE("resources/grass.png"));
	glBindTexture(GL_TEXTURE_2D, terrain_data.grass_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Rocks texture
	terrain_data.rocks_tex = Loader::CreateAndLoadTexture(MAYBEWIDE("resources/rocks.png"));
	glBindTexture(GL_TEXTURE_2D, terrain_data.rocks_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Tree texture
	nature_data.tree_tex = Loader::CreateAndLoadTexture(MAYBEWIDE("resources/tree1.png"));
	glBindTexture(GL_TEXTURE_2D, nature_data.tree_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Bush texture
	nature_data.bush_tex = Loader::CreateAndLoadTexture(MAYBEWIDE("resources/bush.tga"));
	glBindTexture(GL_TEXTURE_2D, nature_data.bush_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Long grass texture
	nature_data.long_grass_tex = Loader::CreateAndLoadTexture(MAYBEWIDE("resources/long_grass.tga"));
	glBindTexture(GL_TEXTURE_2D, nature_data.long_grass_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Water normal texture
	water_data.normal_tex = Loader::CreateAndLoadTexture(MAYBEWIDE("resources/water_normal.png"));
	glBindTexture(GL_TEXTURE_2D, water_data.normal_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Reflection texture
	glGenFramebuffers(1, &(water_data.reflection_framebuffer));
	glBindFramebuffer(GL_FRAMEBUFFER, water_data.reflection_framebuffer);

	glGenTextures(1, &(water_data.reflection_tex));
	glBindTexture(GL_TEXTURE_2D, water_data.reflection_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_MIRRORED_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void init()
{
	glClearColor(0.33f, 0.38f, 0.45f, 1.0f);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);

	int position_loc = 0;
	int normal_loc = 1;
	int tex_coord_loc = 2;

	// Create geometries
	createGeometries(position_loc, normal_loc, tex_coord_loc);
	
	camera_input = CameraInput(&(terrain_data.geometry), 0.0f, 0.0f);
	handle_input = InputHandler(&camera_input);

	// Create terrain program
	initTerrain(position_loc, normal_loc, tex_coord_loc);

	// Create nature program
	initNature(position_loc, normal_loc, tex_coord_loc);

	// Create water program
	initWater(position_loc, normal_loc, tex_coord_loc);

	// Light
	initLight();

	// Camera
	initCamera();

	// Material
	initMaterial();

	// Tree locations buffer
	randomGeneration();

	applyTextures();

	initReflection();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Loader::WaitForEnterAndExit();
}
#pragma endregion

#pragma region render
void renderTerrain() {

	glUseProgram(terrain_data.program);

	glBindVertexArray(terrain_data.geometry.VertexArrayObject);

	material.ambient_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular_color = glm::vec4(0.1f, 0.1f, 0.1f, 0.1f);
	material.shininess = 1.0f;

	glBindBuffer(GL_UNIFORM_BUFFER, ubo.material);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), &material);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::mat4 model_matrix(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, -2.0f, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(100.0f, TERRAIN_HEIGHT, 100.0f));
	glUniformMatrix4fv(terrain_data.model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));

	glUniform1i(terrain_data.grass_tex_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrain_data.grass_tex);

	glUniform1i(terrain_data.rocks_tex_loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrain_data.rocks_tex);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(2643261405U);
	Loader::DrawGeometry(terrain_data.geometry);
	glDisable(GL_PRIMITIVE_RESTART);
}

void renderLamp() {

	glUseProgram(terrain_data.program);

	glBindVertexArray(nature_data.lamp_geometry.VertexArrayObject);

	material.ambient_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular_color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
	material.shininess = 6.0f;

	glBindBuffer(GL_UNIFORM_BUFFER, ubo.material);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), &material);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::mat4 model_matrix(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(-10.0f, terrain_data.geometry.height[103][103] * TERRAIN_HEIGHT - 2.0f, -10.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(terrain_data.model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));

	glUniform1i(terrain_data.grass_tex_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrain_data.grass_tex);

	glUniform1i(terrain_data.rocks_tex_loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrain_data.rocks_tex);

	Loader::DrawGeometryInstanced(nature_data.lamp_geometry, LIGHT_COUNT - 1);

}

void renderNature() {
	glUseProgram(nature_data.program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUniform1f(nature_data.app_time_loc, app_time);

	material.ambient_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular_color = glm::vec4(0.1f, 0.1f, 0.1f, 0.1f);
	material.shininess = 1.0f;

	glBindBuffer(GL_UNIFORM_BUFFER, ubo.material);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), &material);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	glm::mat4 model_matrix(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, -2.0f, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(nature_data.model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));

	//Tree render
	glBindVertexArray(nature_data.tree_geometry.VertexArrayObject);

	glUniform1f(nature_data.wind_height_loc, 20.0);

	glUniform1i(nature_data.tex_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, nature_data.tree_tex);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubo.tree);

	Loader::DrawGeometryInstanced(nature_data.tree_geometry, TREE_COUNT);

	//Bush render
	glBindVertexArray(nature_data.bush_geometry.VertexArrayObject);

	glUniform1f(nature_data.wind_height_loc, 5.0);

	glUniform1i(nature_data.tex_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, nature_data.bush_tex);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubo.bush);

	Loader::DrawGeometryInstanced(nature_data.bush_geometry, BUSH_COUNT);

	//Grass render
	glUniform1f(nature_data.wind_height_loc, 2.5);

	glUniform1i(nature_data.tex_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, nature_data.long_grass_tex);

	for (int i = 0; i < 12; ++i) {
		glBindVertexArray(nature_data.long_grass_geometry[i].VertexArrayObject);
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubo.long_grass[i]);
		Loader::DrawGeometryInstanced(nature_data.long_grass_geometry[i], GRASS_COUNT);
	}

	glDisable(GL_BLEND);
}

void renderWater() {
	glm::mat4 model_matrix;

	glUseProgram(water_data.program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(water_data.geometry.VertexArrayObject);

	material.ambient_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	material.diffuse_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	material.specular_color = glm::vec4(8.0f, 8.0f, 8.0f, 1.0f);
	material.shininess = 200.0f;

	glBindBuffer(GL_UNIFORM_BUFFER, ubo.material);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), &material);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 0.2f, 100.0f));
	glUniformMatrix4fv(water_data.model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));

	glUniform1f(water_data.app_time_loc, app_time * 0.02f);

	glUniform1i(water_data.normal_tex_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, water_data.normal_tex);

	glUniform1i(water_data.reflection_tex_loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, water_data.reflection_tex);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(2643261405U);
	Loader::DrawGeometry(water_data.geometry);
	glDisable(GL_PRIMITIVE_RESTART);

	glDisable(GL_BLEND);
}

// Light position, with a simple animation
void setLightPosition(float day_time) {
	lights[0].position =
		glm::rotate(glm::mat4(1.0f), app_time * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 20.0f, 0.0f)) *
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lights[0].diffuse_color = glm::vec4(1.2f, 1.2f, 1.2f, 1.0f) * day_time;
	lights[0].ambient_color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f) * day_time;
	lights[0].size = glm::vec4(10000.0f, 10000.0f, 10000.0f, 1.0f);

	lights[1].position = glm::vec4(-10.0f, terrain_data.geometry.height[103][103] * TERRAIN_HEIGHT, -10.0f, 1.0f);
	lights[1].diffuse_color = glm::vec4(3 * 1.00f, 3 * 0.98f, 3 * 0.56f, 1.0f) * (1 - day_time);
	lights[1].ambient_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) * (1 - day_time);
	lights[1].size = glm::vec4(20.0f, 20.0f, 20.0f, 1.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo.lights);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light) * LIGHT_COUNT, &lights);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// Camera matrices and eye position
void setCameraPosition(bool isReflection) {
	camera.projection_matrix = glm::perspective(glm::radians(45.0f), float(WIN_WIDTH) / float(WIN_HEIGHT), 0.1f, 1000.0f);
	camera.view_matrix = glm::lookAt(camera_input.GetEyePosition(), camera_input.GetViewOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	camera.eye_position = camera_input.GetEyePosition();

	if (isReflection) {
		camera.view_matrix = glm::scale(camera.view_matrix, glm::vec3(1.0, -1.0, 1.0));
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubo.camera);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Camera), &camera);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// Called when the window needs to be rerendered
void render()
{
	float day_time = 1 - pow(sin(app_time / 120.0f), 4.0f);

	glClearColor(0.66f * day_time, 0.76f * day_time, 0.90f * day_time, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo.lights);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo.camera);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo.material);

	setLightPosition(day_time);

	// Reflection rendering
	glBindFramebuffer(GL_FRAMEBUFFER, water_data.reflection_framebuffer);
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CLIP_DISTANCE0);

	setCameraPosition(true);

	// Geometries
	renderTerrain();
	renderLamp();
	renderNature();

	glDisable(GL_CLIP_DISTANCE0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
		Main rendering
	*/

	setCameraPosition(false);

	// Geometries
	renderTerrain();
	renderLamp();
	renderNature();
	renderWater();

	glBindVertexArray(0);
	glUseProgram(0);

	glutSwapBuffers();
}
#pragma endregion

// Callback function to be called when we make an error in OpenGL
void GLAPIENTRY simple_debug_callback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const char* message, const void* userParam)
{
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << message << std::endl;
		return;
	default:
		return;
	}
}

// Simple timer function for animations
void timer(int)
{
	app_time += animation_speed;
	camera_input.Move();
	glutTimerFunc(20, timer, 0);
	glutPostRedisplay();
}

int main(int argc, char** argv)
{

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Set OpenGL Context parameters
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);

	// Initialize window
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutCreateWindow("Draghici Bogdan, OpenGLApp");

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	glutFullScreenToggle();

	// Initialize DevIL library
	ilInit();

	// Set the debug callback
	Loader::SetDebugCallback(simple_debug_callback);

	// Initialize our OpenGL stuff
	init();

	// Register callbacks
	glutDisplayFunc(render);
	glutKeyboardFunc(key_down);
	glutKeyboardUpFunc(key_up);
	glutTimerFunc(20, timer, 0);
	glutMotionFunc(mouse_moved);
	glutPassiveMotionFunc(mouse_moved);

	glutSetCursor(GLUT_CURSOR_NONE);

	// Run the main loop
	glutMainLoop();

	return 0;
}
