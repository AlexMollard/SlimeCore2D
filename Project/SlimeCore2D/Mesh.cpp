#include "Mesh.h"
#include "glew.h"
#include "glfw3.h"

Mesh::Mesh(const char* name)
{
	this->name = name;
}

Mesh::~Mesh()
{
}

void Mesh::create(float width, float height)
{
	glm::vec3 topLeft = glm::vec3(-1.0f * width, 1.0f * height, 0.0f);
	glm::vec3 bottomLeft = glm::vec3(1.0f * width, 1.0f * height, 0.0f);
	glm::vec3 topRight = glm::vec3(-1.0f * width, -1.0f * height, 0.0f);
	glm::vec3 bottomRight = glm::vec3(1.0f * width, -1.0f * height, 0.0f);

	vertices.push_back(topLeft);	// Back-Left	0
	vertices.push_back(bottomLeft);	// Back-Right	1
	vertices.push_back(topRight);	// Front-Left	2
	vertices.push_back(bottomRight); //Front-Right	3

	unsigned int planeIndices[] =
	{
		0, 2, 1,	// first triangle
		1, 2, 3		// second triangle
	};

	uvs.push_back(glm::vec2(0, 1));
	uvs.push_back(glm::vec2(1, 1));
	uvs.push_back(glm::vec2(0, 0));
	uvs.push_back(glm::vec2(1, 0));

	int length = sizeof(planeIndices) / sizeof(unsigned int);

	indices = std::vector<unsigned int>(planeIndices, planeIndices + length);

	std::vector<float> newvertices;
	for (int i = 0; i < vertices.size(); i++)
	{
		// Postitions
		newvertices.push_back(vertices[i].x);
		newvertices.push_back(vertices[i].y);
		newvertices.push_back(vertices[i].z);

		// UVS
		if (uvs.size() < 1)
		{
			newvertices.push_back(0.5);
			newvertices.push_back(0.5);

		}
		else
		{
			newvertices.push_back(uvs[i].x);
			newvertices.push_back(uvs[i].y);
		}
	}

	// generate buffers
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glGenVertexArrays(1, &vao);

	// bind vertex array aka a mesh wrapper
	glBindVertexArray(vao);

	// Fill vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, newvertices.size() * sizeof(float), newvertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Enable first element as position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Enable third element as UVS
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind buffer
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
