#pragma once
#include "RigidBody.h"
#include "Shader.h"
#include "Texture.h"
#include "CommonEnums.h"

class GameObject : public RigidBody
{
public:
	GameObject() = default;
	virtual ~GameObject() = default;
	GameObject(glm::vec3 pos, glm::vec3 color, glm::vec2 scale, int id);

	virtual void Update(float deltaTime);
	virtual void Create(glm::vec3 pos, glm::vec3 color, glm::vec2 scale, int id);

	void Respawn();

	glm::vec3 GetColor();
	void SetColor(glm::vec3 newColor);
	void SetColor(float r, float g, float b);

	void SetShader(Shader* newShader);
	Shader* GetShader();

	Texture* GetTexture();
	void SetTexture(Texture* tex);

	// Sprite Stuff (Probably should be in own class)
	void SetFrame(int Frame);
	void AdvanceFrame();
	int GetFrame();

	int GetSpriteWidth();
	void SetSpriteWidth(int newWidth);

	bool GetHasAnimation();
	void SetHasAnimation(bool value);

	void SetFrameRate(float frameRate);
	float GetFrameRate();

	void UpdateSpriteTimer(float deltaTime);

	bool GetIsMemoryManaged();

	bool GetRender();
	void SetRender(bool value);

	FlipPolicy GetFlipPolicy() const;
	void SetFlipPolicy(FlipPolicy val);

	glm::vec2 GetAnchorPoint() const;
	void SetAnchorPoint(glm::vec2 val);

protected:
	glm::vec3 m_color = glm::vec3(1);
	glm::vec3 n_defaultColor = glm::vec3(1);

	glm::vec2 m_anchorPoint = glm::vec2(0.5f);

	bool m_render = true;
	bool m_isMemoryManaged = false;

	Texture* m_texture = nullptr;
	Shader* m_shader = nullptr;

	FlipPolicy m_flipPolicy = FlipPolicy::None;

	// Sprite Stuff
	int m_frame = 0;
	int m_spriteWidth = 16;
	float m_frameRate = 0.5f;
	float m_frameRateTimer = 0.0f;
	bool m_hasAnimation = false;
};
