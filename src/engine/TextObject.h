#pragma once
#include "GameObject.h"
#include "ResourceManager.h"
#include "Texture.h"

struct FontTexture
{
	Texture texture;
	int width;
	int height;
	float x;
	float y;
};

class TextObject : public GameObject
{
public:
	TextObject(const std::string& text);
	~TextObject() override = default;

	void SetFont(const std::string& fontPath, int size);

	void Update(float dt) override;
	void Draw();

	void SetText(const std::string& text);
	void SetFont(const std::string& fontName);
	void GenerateTextures();
	void SetFont(TTF_Font* font);

private:
	std::string m_text;
	TTF_Font* m_font;

	std::vector<FontTexture> m_textures;
};
