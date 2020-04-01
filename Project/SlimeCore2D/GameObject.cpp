#include "GameObject.h"
#include <string>
GameObject::GameObject(Mesh* mesh)
{
	this->mesh = mesh;
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
	boundingBox.UpdateQuadBoundingBox(position, scale);
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

glm::vec2 GameObject::GetScale()
{
	return scale;
}

void GameObject::SetScale(glm::vec2 newScale)
{
	scale = newScale;

	model[0] *= scale[0];
	model[1] *= scale[1];
	//model[2] *= scale[2];	/// This line might break stuff??
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
		SetVelocity(glm::vec2(0));
		SetPos(inputManager->GetMousePos().x, inputManager->GetMousePos().y);
	}

	if (boundingBox.GetMouseColliding())
		OnHover();
	else
		SetColor(defaultColor);

	if (release && !isHeld)
		OnRelease();

	if (timer > 0.0f)
	{
		timer -= deltaTime;
		AddVelocity(-inputManager->GetDeltaMouse() * glm::vec2(2));
	}
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
	timer = 0.075f;
}