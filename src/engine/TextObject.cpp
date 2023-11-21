#include "TextObject.h"

#include <bit>
#include "ConsoleLog.h"
#include "RenderTarget.h"
#include "SDL_surface.h"

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
	if (auto font = std::bit_cast<TTF_Font*>(resourceManager->GetResource(fontName)))
	{
		m_font = font;
	}

	// Generate all the textures for std::vector<SDL_Texture*> m_textures, now that the font has changed
	GenerateTextures();
}

void TextObject::GenerateTextures()
{
	// Clear existing textures
	m_textures.clear();

	// Set the color for the text (you can customize this)
	SDL_Color textColor = { 255, 255, 255, 255 }; // White color

	// Set the font size (you can customize this)
	int fontSize = 24;

	// Starting position for the text
	float x = 0.0f;
	float y = 0.0f;

	if (!m_font)
	{
		SLIME_ERROR("Font not loaded/set.");
	}

	// Loop through the characters you want to render
	for (char c : m_text)
	{    
		if (c == '\0') {
			continue;
		}

		// Render each character to a surface
		SDL_Surface* textSurface = TTF_RenderText_Blended(m_font, std::string(1, c).c_str(), textColor);
		if (!textSurface)
		{
			// Handle surface creation failure
			SLIME_WARN("Unable to create text surface for character {}: {}", c, TTF_GetError());
			continue;
		}

		// Store texture information in FontTexture structure
		FontTexture fontTexture;
		fontTexture.texture = Texture((float*)textSurface->pixels, textSurface->w, textSurface->h);
		fontTexture.width   = textSurface->w;
		fontTexture.height  = textSurface->h;
		fontTexture.x       = x;
		fontTexture.y       = y;

		// Update x for the next character
		x += fontTexture.width;

		// Add the FontTexture to the vector
		m_textures.push_back(fontTexture);

		// Free the surface
		SDL_FreeSurface(textSurface);
	}
}


void TextObject::SetFont(TTF_Font* font) 
{
	m_font = font;
}
