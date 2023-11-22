#include "TextBatchRenderer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "Constants.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoords;  // Texture coordinates for regular sprites
	glm::vec2 textCoords; // Texture coordinates for text rendering
	float texIndex;
	float maskIndex;
};

struct RendererData
{
	GLuint quadVA = 0;
	GLuint quadVB = 0;
	GLuint quadIB = 0;

	GLuint whiteTexture       = 0;
	uint32_t whiteTextureSlot = 0;

	uint32_t indexCount = 0;

	Vertex* quadBuffer    = nullptr;
	Vertex* quadBufferPtr = nullptr;

	std::array<uint32_t, MAX_TEXTURE_COUNT> textureSlots = {};
	uint32_t textureSlotIndex                            = 1;

	TTF_Font* font = nullptr;
};

void TextBatchRenderer::AddObject(GameObject* newObject) 
{

}

uint32_t TextBatchRenderer::GetTextureIndex(Texture* texture) 
{
	return 0;
}

uint32_t TextBatchRenderer::AddTextureSlot(Texture* texture) 
{
	return 0;
}

uint32_t TextBatchRenderer::AddTextureSlot(GLuint textureID) 
{
	return 0;
}

void TextBatchRenderer::RemoveObject(const GameObject& object) 
{

}

int TextBatchRenderer::GetObjectIndex(const GameObject& object) 
{
	return 0;
}

void TextBatchRenderer::ShutDown() 
{

}

void TextBatchRenderer::Flush() 
{

}

void TextBatchRenderer::Render(const glm::vec2& camPos, float distanceFromCenter)
{
	BeginBatch();

	int index = 0;

	// We want to iterate backwards so that way first quad in is the first to be rendered
	size_t objectCount = GetObjectPool().size();
	for (int i = objectCount - 1; i >= 0; --i)
	{
		GameObject* object = GetObjectPool()[i];
		if (object == nullptr || index == GetObjectPool().size()) // This must be in order so if this object is null everyone after it should also be null.
		{
			EndBatch();
			Flush();

			return;
		}



		index++;
	}

	EndBatch();
	Flush();
}

void TextBatchRenderer::BeginBatch()
{
	RendererData* data  = GetData();
	data->quadBufferPtr = data->quadBuffer;

	// Assuming rendererData.font is your loaded TTF_Font
	SDL_Color textColor  = { 255, 255, 255, 255 }; // White color
	SDL_Surface* surface = TTF_RenderText_Blended(data->font, "YourTextHere", textColor);

	// Convert the surface to a texture
	GLuint textTexture;
	glGenTextures(1, &textTexture);
	glBindTexture(GL_TEXTURE_2D, textTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	// Set texture parameters (you might want to adjust these based on your needs)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Free the surface
	SDL_DestroySurface(surface);

	// Now render stuff with your texture
	glBindTexture(GL_TEXTURE_2D, textTexture);

	// Draw a quad with the texture
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(1, 0);
	glVertex2f(100, 0);
	glTexCoord2f(1, 1);
	glVertex2f(100, 100);
	glTexCoord2f(0, 1);
	glVertex2f(0, 100);
	glEnd();

	// Delete the texture when you're done with it
	glDeleteTextures(1, &textTexture);

// You can also use SDL_ttf to render text to a surface
// and then convert it to a texture using the above code
// (just replace TTF_RenderText with TTF_RenderUTF8)
}

void TextBatchRenderer::EndBatch()
{
	RendererData* data = GetData();
	GLsizeiptr size    = (uint8_t*)data->quadBufferPtr - (uint8_t*)data->quadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, data->quadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data->quadBuffer);
}