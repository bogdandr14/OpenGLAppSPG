#include "ObjectLoader.h"
using namespace std;

bool ObjectLoader::ParseOBJFile(const char* file_name, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec3>& out_normals, std::vector<glm::vec2>& out_tex_coords)
{
	auto error_msg = [file_name] {
		cout << "Failed to read OBJ file " << file_name << ", its format is not supported" << endl;
	};

	struct OBJTriangle
	{
		int v0, v1, v2;
		int n0, n1, n2;
		int t0, t1, t2;
	};

	// Prepare the arrays for the data from the file.
	std::vector<glm::vec3> raw_vertices;        raw_vertices.reserve(1000);
	std::vector<glm::vec3> raw_normals;            raw_normals.reserve(1000);
	std::vector<glm::vec2> raw_tex_coords;        raw_tex_coords.reserve(1000);
	std::vector<OBJTriangle> raw_triangles;        raw_triangles.reserve(1000);

	// Load OBJ file
	ifstream file(file_name);
	if (!file.is_open())
	{
		cout << "Cannot open OBJ file " << file_name << endl;
		return false;
	}

	while (!file.fail())
	{
		string prefix;
		file >> prefix;

		if (prefix == "v")
		{
			glm::vec3 v;
			file >> v.x >> v.y >> v.z;
			raw_vertices.push_back(v);
			file.ignore(numeric_limits<streamsize>::max(), '\n');        // Ignore the rest of the line
		}
		else if (prefix == "vt")
		{
			glm::vec2 vt;
			file >> vt.x >> vt.y;
			raw_tex_coords.push_back(vt);
			file.ignore(numeric_limits<streamsize>::max(), '\n');        // Ignore the rest of the line
		}
		else if (prefix == "vn")
		{
			glm::vec3 vn;
			file >> vn.x >> vn.y >> vn.z;
			raw_normals.push_back(vn);
			file.ignore(numeric_limits<streamsize>::max(), '\n');        // Ignore the rest of the line
		}
		else if (prefix == "f")
		{
			OBJTriangle t;
			char slash;

			// And now check whether the geometry is of a correct format (that it contains only triangles,
			// and all vertices have their position, normal, and texture coordinate set).

			// Read the first vertex
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.v0;
			file >> ws;        if (file.peek() != '/') { error_msg();        return false; }
			file >> slash;
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.t0;
			file >> ws;        if (file.peek() != '/') { error_msg();        return false; }
			file >> slash;
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.n0;

			// Read the second vertex
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.v1;
			file >> ws;        if (file.peek() != '/') { error_msg();        return false; }
			file >> slash;
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.t1;
			file >> ws;        if (file.peek() != '/') { error_msg();        return false; }
			file >> slash;
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.n1;

			// Read the third vertex
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.v2;
			file >> ws;        if (file.peek() != '/') { error_msg();        return false; }
			file >> slash;
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.t2;
			file >> ws;        if (file.peek() != '/') { error_msg();        return false; }
			file >> slash;
			file >> ws;        if (!isdigit(file.peek())) { error_msg();        return false; }
			file >> t.n2;

			// Check that this polygon has only three vertices.
			// It also skips all white spaces, effectively ignoring the rest of the line.
			file >> ws;        if (isdigit(file.peek())) { error_msg();        return false; }

			// Subtract one because the OBJ indexes are from 1, not from 0
			t.v0--;        t.v1--;        t.v2--;
			t.n0--;        t.n1--;        t.n2--;
			t.t0--;        t.t1--;        t.t2--;

			raw_triangles.push_back(t);
		}
		else
		{
			// Ignore other cases
			file.ignore(numeric_limits<streamsize>::max(), '\n');        // Ignore the rest of the line
		}
	}
	file.close();

	// Indices in OBJ file cannot be used, we need to convert the geometry in a way we could draw it
	// with glDrawArrays.
	out_vertices.clear();        out_vertices.reserve(raw_triangles.size() * 3);
	out_normals.clear();        out_normals.reserve(raw_triangles.size() * 3);
	out_tex_coords.clear();        out_tex_coords.reserve(raw_triangles.size() * 3);
	for (size_t i = 0; i < raw_triangles.size(); i++)
	{
		if ((raw_triangles[i].v0 >= int(raw_vertices.size())) ||
			(raw_triangles[i].v1 >= int(raw_vertices.size())) ||
			(raw_triangles[i].v2 >= int(raw_vertices.size())) ||
			(raw_triangles[i].n0 >= int(raw_normals.size())) ||
			(raw_triangles[i].n1 >= int(raw_normals.size())) ||
			(raw_triangles[i].n2 >= int(raw_normals.size())) ||
			(raw_triangles[i].t0 >= int(raw_tex_coords.size())) ||
			(raw_triangles[i].t1 >= int(raw_tex_coords.size())) ||
			(raw_triangles[i].t2 >= int(raw_tex_coords.size())))
		{
			// Invalid out-of-range indices
			error_msg();
			return false;
		}

		out_vertices.push_back(raw_vertices[raw_triangles[i].v0]);
		out_vertices.push_back(raw_vertices[raw_triangles[i].v1]);
		out_vertices.push_back(raw_vertices[raw_triangles[i].v2]);
		out_normals.push_back(raw_normals[raw_triangles[i].n0]);
		out_normals.push_back(raw_normals[raw_triangles[i].n1]);
		out_normals.push_back(raw_normals[raw_triangles[i].n2]);
		out_tex_coords.push_back(raw_tex_coords[raw_triangles[i].t0]);
		out_tex_coords.push_back(raw_tex_coords[raw_triangles[i].t1]);
		out_tex_coords.push_back(raw_tex_coords[raw_triangles[i].t2]);
	}

	return true;
}

Geometry ObjectLoader::LoadOBJ(const char* file_name, GLint position_location, GLint normal_location, GLint tex_coord_location)
{
	Geometry geometry;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coords;
	if (!ParseOBJFile(file_name, vertices, normals, tex_coords))
	{
		return geometry;        // Return empty geometry, the error message was already printed
	}

	// Create buffers for vertex data
	glGenBuffers(3, geometry.VertexBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.VertexBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.VertexBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.VertexBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(float) * 2, tex_coords.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// No indices
	geometry.IndexBuffer = 0;

	// Create a vertex array object for the geometry
	glGenVertexArrays(1, &geometry.VertexArrayObject);

	// Set the parameters of the geometry
	glBindVertexArray(geometry.VertexArrayObject);
	if (position_location >= 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, geometry.VertexBuffers[0]);
		glEnableVertexAttribArray(position_location);
		glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (normal_location >= 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, geometry.VertexBuffers[1]);
		glEnableVertexAttribArray(normal_location);
		glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (tex_coord_location >= 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, geometry.VertexBuffers[2]);
		glEnableVertexAttribArray(tex_coord_location);
		glVertexAttribPointer(tex_coord_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	geometry.Mode = GL_TRIANGLES;
	geometry.DrawArraysCount = vertices.size();
	geometry.DrawElementsCount = 0;

	return geometry;
}

Geometry ObjectLoader::CreateGrid(int size, GLint position_location, GLint normal_location, GLint tex_coord_location) {

	/*
		Vecticies, normals, tex. coords
	*/
	std::vector< std::vector< glm::vec3> > vertexes(size, std::vector<glm::vec3>(size));
	std::vector< std::vector< glm::vec2> > tex_coords(size, std::vector<glm::vec2>(size));
	std::vector< std::vector<glm::vec3> > normals(size, std::vector<glm::vec3>(size));

	ILubyte* imageData = ilGetData();

	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			float s = float(x) / float(size);
			float t = float(y) / float(size);

			vertexes[x][y] = glm::vec3(-0.5f + s, 0.0f, -0.5f + t);
			tex_coords[x][y] = glm::vec2(s, t);
			normals[x][y] = glm::vec3(0.0f, 1.0f, 0.0f);
		}
	}

	/*
	Indices
	*/
	std::vector<unsigned int> indices;
	for (int y = 0; y < size - 1; y++) {
		for (int x = 0; x < size - 1; x++) {
			for (int r = 0; r < 2; r++) {
				int row = y + (1 - r);
				int index = row * size + x;
				indices.push_back(index);
			}
		}
		// Restart triangle strips
		indices.push_back(2643261405U);
	}

	/*
	Normalize data
	*/
	std::vector<float> vertexData(size * size * 8);
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			vertexData[(x + y * size) * 8 + 0] = vertexes[x][y].x;
			vertexData[(x + y * size) * 8 + 1] = vertexes[x][y].y;
			vertexData[(x + y * size) * 8 + 2] = vertexes[x][y].z;

			vertexData[(x + y * size) * 8 + 3] = normals[x][y].x;
			vertexData[(x + y * size) * 8 + 4] = normals[x][y].y;
			vertexData[(x + y * size) * 8 + 5] = normals[x][y].z;

			vertexData[(x + y * size) * 8 + 6] = tex_coords[x][y].x;
			vertexData[(x + y * size) * 8 + 7] = tex_coords[x][y].y;
		}
	}

	/*
	Load to opengl
	*/
	Geometry grid;

	// Create a single buffer for vertex data
	glGenBuffers(1, &grid.VertexBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, grid.VertexBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a buffer for indices
	glGenBuffers(1, &grid.IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grid.IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Create a vertex array object for the geometry
	glGenVertexArrays(1, &grid.VertexArrayObject);

	// Set the parameters of the geometry
	glBindVertexArray(grid.VertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, grid.VertexBuffers[0]);
	if (position_location >= 0)
	{
		glEnableVertexAttribArray(position_location);
		glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	}
	if (normal_location >= 0)
	{
		glEnableVertexAttribArray(normal_location);
		glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
	}
	if (tex_coord_location >= 0)
	{
		glEnableVertexAttribArray(tex_coord_location);
		glVertexAttribPointer(tex_coord_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 6));
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grid.IndexBuffer);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	grid.Mode = GL_TRIANGLE_STRIP;
	grid.DrawArraysCount = 0;
	grid.DrawElementsCount = indices.size();

	return grid;
}