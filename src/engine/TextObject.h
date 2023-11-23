#pragma once
#include "GameObject.h"
#include "ResourceManager.h"
#include "Texture.h"

class TextObject : public GameObject
{
public:
	TextObject(const std::string& text);
	~TextObject() override = default;

	void SetFont(const std::string& fontPath, int size);

	void Update(float dt) override;
	void Draw();

	void SetText(const std::string& text);
	void SetFont(int sizePt, glm::vec3 color);
	void GenerateTextures(int sizePt, glm::vec3 color);
	void SetFont(TTF_Font* font);

private:
	std::string m_text;
	TTF_Font* m_font;
};
