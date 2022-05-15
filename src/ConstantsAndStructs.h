// Buffer structures
static const int LIGHT_COUNT = 2;
static const int TREE_COUNT = 500;
static const int BUSH_COUNT = 500;
static const int GRASS_COUNT = 2000;

struct Light
{
	glm::vec4 position;
	glm::vec4 ambient_color;
	glm::vec4 diffuse_color;
	glm::vec4 specular_color;
	glm::vec4 size;
};
struct Camera
{
	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;
	glm::vec3 eye_position;
};
struct Material
{
	glm::vec4 ambient_color;
	glm::vec4 diffuse_color;
	glm::vec4 specular_color;
	float shininess;
};

struct TerrainData {
	GLuint program;

	Terrain geometry;

	GLuint grass_tex;
	GLuint rocks_tex;
	GLint grass_tex_loc;
	GLint rocks_tex_loc;
	GLint model_matrix_loc;
};

struct NatureData {
	GLuint program;

	glm::mat4 tree_data[GRASS_COUNT];
	Geometry tree_geometry;
	Geometry bush_geometry;
	Geometry long_grass_geometry[12];
	Geometry lamp_geometry;

	GLuint tree_tex;
	GLuint bush_tex;
	GLuint long_grass_tex;
	GLint tex_loc;
	GLint model_matrix_loc;
	GLint wind_height_loc;
	GLint app_time_loc;
};

struct WaterData {
	GLuint program;
	Geometry geometry;

	GLuint normal_tex;
	GLint normal_tex_loc;
	GLint model_matrix_loc;
	GLint app_time_loc;
	GLint reflection_tex_loc;
	GLuint reflection_framebuffer;
	GLuint reflection_tex;
	GLuint reflection_depth;
};

struct UBO {
	GLuint lights;
	GLuint camera;
	GLuint material;
	GLuint tree;
	GLuint bush;
	GLuint long_grass[12];
};