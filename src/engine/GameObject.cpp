#include "pch.h"
#include "GameObject.h"

#include "gtc/quaternion.hpp"
#include <string>

GameObject::GameObject(glm::vec3 pos, glm::vec3 color, glm::vec2 scale, int id)
{
	Create(pos, color, scale, id);
}

void GameObject::Update(float deltaTime) {}

void GameObject::Create(glm::vec3 pos, glm::vec3 color, glm::vec2 scale, int id)
{
	SetPos(pos);
	SetSpawn(pos);
	n_defaultColor = color;
	SetColor(color);
	SetScale(scale);
	SetID(id);
}

void GameObject::Respawn()
{
	SetVelocity(glm::vec2(0));
	SetPos(m_spawnPoint);
}

void GameObject::SetSpawn(glm::vec3 newSpawn)
{
	m_spawnPoint = newSpawn;
}

glm::vec3 GameObject::GetColor()
{
	return m_color;
}

void GameObject::SetColor(glm::vec3 newColor)
{
	m_color = newColor;
}

void GameObject::SetColor(float r, float g, float b)
{
	SetColor(glm::vec3(r, g, b));
}

void GameObject::SetShader(Shader* newShader)
{
	m_shader = newShader;
}

Shader* GameObject::GetShader()
{
	return m_shader;
}

void GameObject::SetRotate(float rotation)
{
	SetRotation(rotation);
	float updatedRotation = rotation * 3.141592f / 180.0f;

	float sin = glm::sin(updatedRotation);
	float cos = glm::cos(updatedRotation);

	float tx = 0;
	float ty = 1;
	glm::vec2 newNormal;
	newNormal.x = (cos * tx) - (sin * ty);
	newNormal.y = (sin * tx) + (cos * ty);
	SetNormal(newNormal);

	glm::mat4 tempModel(1);

	tempModel = glm::rotate(tempModel, updatedRotation, glm::vec3(0, 0, 1));

	tempModel[0] *= GetScale()[0];
	tempModel[1] *= GetScale()[1];

	SetModel(tempModel);
	SetPos(GetPosition());
}

Texture* GameObject::GetTexture()
{
	return m_texture;
}

void GameObject::SetTexture(Texture* tex)
{
	m_texture = tex;

	if (m_texture != nullptr)
	{
		if (!m_hasAnimation)
			SetScale(glm::vec2(m_texture->GetWidth() / 16, m_texture->GetHeight() / 16));
		else
			SetScale(glm::vec2(m_spriteWidth, m_texture->GetHeight()) / glm::vec2(m_spriteWidth));

		SetTextureWidth(m_texture->GetWidth());
	}
}

void GameObject::SetFrame(int Frame)
{
	m_frame = Frame;
}

void GameObject::AdvanceFrame()
{
	m_frame++;

	if (m_frame >= m_textureWidth / m_spriteWidth)
	{
		m_frame = 0;
	}
}

int GameObject::GetFrame()
{
	return m_frame;
}

int GameObject::GetSpriteWidth()
{
	return m_spriteWidth;
}

void GameObject::SetSpriteWidth(int newWidth)
{
	m_spriteWidth = newWidth;
	if (m_textureWidth > m_spriteWidth)
		m_hasAnimation = true;
}

int GameObject::GetTextureWidth()
{
	return m_textureWidth;
}

void GameObject::SetTextureWidth(int newWidth)
{
	m_textureWidth = newWidth;

	if (m_textureWidth > m_spriteWidth)
		m_hasAnimation = true;
}

bool GameObject::GetHasAnimation()
{
	return m_hasAnimation;
}

void GameObject::SetHasAnimation(bool value)
{
	m_hasAnimation = value;
}

void GameObject::SetFrameRate(float frameRate)
{
	this->m_frameRate = frameRate;
}

float GameObject::GetFrameRate()
{
	return m_frameRate;
}

void GameObject::UpdateSpriteTimer(float deltaTime)
{
	if (m_hasAnimation)
	{
		m_frameRateTimer += deltaTime;

		if (m_frameRateTimer > m_frameRate)
		{
			m_frameRateTimer = 0;
			AdvanceFrame();
		}
	}
}

bool GameObject::GetIsPlayer()
{
	return m_isPlayer;
}

bool GameObject::GetRender()
{
	return m_render;
}

void GameObject::SetRender(bool value)
{
	m_render = value;
}