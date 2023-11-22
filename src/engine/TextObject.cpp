#include "TextObject.h"

#include "ConsoleLog.h"
#include "RenderTarget.h"
#include "SDL3/SDL_surface.h"
#include <bit>
#include "ResourceManager.h"
#include "SDL3_ttf/SDL_ttf.h"

TextObject::TextObject(const std::string& text)
{
	SetText(text);
}

void TextObject::Update(float dt) {}

void TextObject::SetText(const std::string& text)
{
	if (m_text == text)
	{
		return;
	}

	m_text = text;
}

void TextObject::SetFont(const std::string& fontName)
{
	auto resourceManager = ResourceManager::GetInstance();

	// Attempt to convert the void* from the resourceManager to a TTF_Font*
// 	if (auto font = std::bit_cast<TTF_Font*>(resourceManager->GetResource(fontName)))
// 	{
// 		m_font = font;
// 	}
// 	else
// 	{
// 		SLIME_ERROR("Unable to convert void* to TTF_Font*");
// 	}

	// Generate all the textures for std::vector<SDL_Texture*> m_textures, now that the font has changed
	GenerateTextures();
}

void TextObject::GenerateTextures()
{
	TTF_Font* font = TTF_OpenFont(ResourceManager::GetFontPath("DoppioOne-Regular").c_str(), 16);
	if (!font)
	{
		SLIME_ERROR("Font not loaded/set.");
	}

	// Render each character to a surface
	SDL_Color textColor = { 255, 255, 255, 255 }; // White color
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, m_text.c_str(), textColor);
	if (!textSurface)
	{
		// Handle surface creation failure
		SLIME_WARN("Unable to create text surface for character {}: {}", m_text.c_str(), TTF_GetError());
		return;
	}

	// Store texture information in FontTexture structure
	Texture* newTexture = new Texture((float*)textSurface->pixels, textSurface->w, textSurface->h, GL_RGBA, GL_UNSIGNED_BYTE);
	SetTexture(newTexture);

	float mod = textSurface->w / textSurface->h;
	SetScale(glm::vec2(mod, 1.0f));

	// Free the surface
	SDL_DestroySurface(textSurface);
	TTF_CloseFont(font);
}

void TextObject::SetFont(TTF_Font* font)
{
	m_font = font;
}
