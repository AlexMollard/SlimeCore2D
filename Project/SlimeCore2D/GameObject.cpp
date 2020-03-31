#include "GameObject.h"

GameObject::GameObject(Mesh* mesh, Shader* shader, Texture* tex)
{
	this->mesh = mesh;
	this->shader = shader;
	
	if (tex != nullptr)
		this->texture = tex;
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
	boundingBox.UpdateBoundingBox(position, scale);

}

void GameObject::Create(glm::vec3 pos, glm::vec3 color, glm::vec3 scale, int id)
{
	SetPos(pos);
	defaultColor = color;
	SetColor(color);
	SetScale(scale);
	SetID(id);
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

void GameObject::Respawn()
{
	SetVelocity(glm::vec2(0));
	SetPos(spawnPoint);
}

void GameObject::SetSpawn(glm::vec2 newSpawn)
{
	spawnPoint = newSpawn;
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

glm::vec3 GameObject::GetScale()
{
	return scale;
}

void GameObject::SetScale(glm::vec3 newScale)
{
	scale = newScale;

	model[0] *= scale[0];
	model[1] *= scale[1];
	model[2] *= scale[2];
}

void GameObject::SetID(int id)
{
	ID = id;
}

int GameObject::GetID()
{
	return ID;
}

Texture* GameObject::GetTexture()
{
	return texture;
}

void GameObject::SetTexture(Texture* tex)
{
	texture = tex;
}
