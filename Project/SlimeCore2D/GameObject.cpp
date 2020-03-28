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
	if (isHeld)
		SetPos(position.x - inputManager->GetDeltaMouse().x, position.y - inputManager->GetDeltaMouse().y);
	
	boundingBox.UpdateBoundingBox(position, scale);

	if (boundingBox.GetMouseColliding())
	{
		SetColor(defaultColor * 0.75f);
		if (inputManager->GetMouseDown(0))
		{
			SetColor(defaultColor * 0.5f);
			isHeld = true;
		}
		else
		{
			isHeld = false;
		}
	}
	else
	{
		SetColor(defaultColor);
	}
}

void GameObject::Create(glm::vec3 pos, glm::vec3 color, glm::vec3 scale, int id)
{
	SetPos(pos);
	defaultColor = color;
	SetColor(color);
	SetScale(scale);
	SetID(id);
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

BoundingBox* GameObject::GetBoundingBox()
{
	return &boundingBox;
}
