#include "GameObject.h"

GameObject::GameObject(Mesh* mesh, Shader* shader)
{
	this->mesh = mesh;
	this->shader = shader;
}

GameObject::~GameObject()
{
}

void GameObject::Draw()
{
	shader->setVec3("color", color);
	shader->setMat4("Model", model);
	mesh->draw();
}

void GameObject::Update(float deltaTime)
{
}

void GameObject::SetPos(glm::vec2 newPos)
{
	SetPos(newPos.x, newPos.y);
}

void GameObject::SetPos(float x, float y)
{
	position.x = x;
	position.y = y;

	model[3] = glm::vec4(position, 1);
}

Shader* GameObject::GetShader()
{
	return shader;
}

void GameObject::SetShader(Shader* newShader)
{
	shader = newShader;
}

Mesh* GameObject::GetMesh()
{
	return mesh;
}

void GameObject::SetMesh(Mesh* newMesh)
{
	mesh = newMesh;
}

glm::vec3 GameObject::GetColor()
{
	return color;
}

void GameObject::SetColor(glm::vec3 newColor)
{
	color = newColor;
}

void GameObject::SetColor(float r, float g, float b)
{
	SetColor(glm::vec3(r, g, b));
}
