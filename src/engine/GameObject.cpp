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
	n_defaultColor = color;
	SetColor(color);
	SetScale(scale);
	SetID(id);
}

void GameObject::Respawn()
{
	SetVelocity(glm::vec2(0));
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

Texture* GameObject::GetTexture()
{
	return m_texture;
}

void GameObject::SetTexture(Texture* tex)
{
	m_texture = tex;
}

Texture* GameObject::GetMaskTexture()
{
	return m_maskTexture;
}

void GameObject::SetMaskTexture(Texture* tex)
{
	m_maskTexture = tex;
}

void GameObject::SetFrame(int Frame)
{
	m_frame = Frame;
}

void GameObject::AdvanceFrame()
{
	m_frame++;

	if (m_frame >= m_texture->GetWidth() / m_spriteWidth)
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

bool GameObject::GetIsMemoryManaged()
{
	return m_isMemoryManaged;
}

bool GameObject::GetRender()
{
	return m_render;
}

void GameObject::SetRender(bool value)
{
	m_render = value;
}

FlipPolicy GameObject::GetFlipPolicy() const
{
	return m_flipPolicy;
}

void GameObject::SetFlipPolicy(FlipPolicy val)
{
	m_flipPolicy = val;
}

glm::vec2 GameObject::GetAnchorPoint() const
{
	return m_anchorPoint;
}

void GameObject::SetAnchorPoint(glm::vec2 val)
{
	m_anchorPoint = val;
}
