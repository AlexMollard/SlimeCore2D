#include "TextObject.h"

#include "engine/MemoryDebugging.h"

#include "ConsoleLog.h"
#include "RenderTarget.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
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

void TextObject::SetFont(int sizePt, glm::vec3 color)
{
	GenerateTextures(sizePt, color);
}

void TextObject::GenerateTextures(int sizePt, glm::vec3 color)
{
	TTF_Font* font = TTF_OpenFont(ResourceManager::GetFontPath("DoppioOne-Regular").c_str(), sizePt);
	if (!font)
	{
		SLIME_ERROR("Font not loaded/set.");
	}

	// Render each character to a surface
	SDL_Color textColor      = { static_cast<Uint8>(color.r * 255), static_cast<Uint8>(color.g * 255), static_cast<Uint8>(color.b * 255), 255 };
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, m_text.c_str(), textColor);
	if (!textSurface)
	{
		// Handle surface creation failure
		SLIME_WARN("Unable to create text surface for character {}: {}", m_text.c_str(), TTF_GetError());
		return;
	}

	IMG_SavePNG(textSurface, "output.png");

	// Store texture information in FontTexture structure
	Texture* newTexture = new Texture(textSurface);
	SetTexture(newTexture);

	float aspectRatio = (static_cast<float>(textSurface->w) / textSurface->h) * 10.0f;
	SetScale(glm::vec2(aspectRatio, 10.0f));

	// Free the surface
	SDL_DestroySurface(textSurface);
	TTF_CloseFont(font);
}


void TextObject::SetFont(TTF_Font* font)
{
	m_font = font;
}
