#include "GameObject.h"
#include <string>
#include "gtc/quaternion.hpp"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
	switch (type) 
	{
	case ObjectType::Quad:
		Debug::PrintDelete("Quad");
		break;
	case ObjectType::Circle:
		Debug::PrintDelete("Circle");
		break;
	case ObjectType::Line:
		Debug::PrintDelete("Line");
		break;
	}
}

void GameObject::Draw()
{
	mesh->draw();
}

void GameObject::Update(float deltaTime)
{

}

void GameObject::Create(glm::vec2 pos, glm::vec3 color, glm::vec2 scale, int id)
{
	SetPos(pos);
	SetSpawn(pos);
	defaultColor = color;
	SetColor(color);
	SetScale(scale);
	SetID(id);


	switch (type)
	{
	case ObjectType::Quad:
		Debug::PrintCreate("Quad");
		break;
	case ObjectType::Circle:
		Debug::PrintCreate("Circle");
		break;
	case ObjectType::Line:
		Debug::PrintCreate("Line");
		break;
	}

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

void GameObject::SetShader(Shader* newShader)
{
	shader = newShader;
}

Shader* GameObject::GetShader()
{
	return shader;
}

void GameObject::SetRotate(float rotation)
{
	this->rotation = rotation;
	float updatedRotation = rotation * 3.141592f / 180.0f;


	float sin = glm::sin(updatedRotation);
	float cos = glm::cos(updatedRotation);

	float tx = 0;
	float ty = 1;
	normal.x = (cos * tx) - (sin * ty);
	normal.y = (sin * tx) + (cos * ty);
	
	glm::mat4 tempModel(1);

	tempModel = glm::rotate(tempModel, updatedRotation, glm::vec3(0,0,1));
	
	tempModel[0] *= scale[0];
	tempModel[1] *= scale[1];

	model = tempModel;
	SetPos(position);
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

void GameObject::UpdateInteraction(float deltaTime)
{
	if (inputManager->GetMouseDown(3))
	{
		Respawn();
	}
	
	if (isHeld)
	{
		if (!inputManager->GetMouseDown(0))
			isHeld = false;
	
		if (isKinematic)
		{
			SetPos(inputManager->GetMousePos());
		}
		else
		{
			glm::vec2 forceToMouse = glm::normalize(inputManager->GetMousePos() - GetPos());
			ApplyForce(forceToMouse);
		}
	}
	
	if (boundingBox.GetMouseColliding())
		OnHover();
	else
		SetColor(defaultColor);
	
	if (release && !isHeld)
		OnRelease();
}


void GameObject::OnHover()
{
	if (inputManager->GetMouseDown(0))
		OnPress();
	else
	{
		SetColor(defaultColor * 0.75f);
		isHeld = false;
	}
}

void GameObject::OnPress()
{
	SetColor(defaultColor * 0.5f);
	isHeld = true;
	release = true;
}

void GameObject::OnRelease()
{
	release = false;
}