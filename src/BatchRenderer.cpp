#include "pch.h"
#include "BatchRenderer.h"
#include "engine/GameObject.h"
#include <iostream>

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoords;
	float texIndex;
};

struct RendererData
{
	GLuint quadVA = 0;
	GLuint quadVB = 0;
	GLuint quadIB = 0;

	GLuint whiteTexture = 0;
	uint32_t whiteTextureSlot = 0;

	uint32_t indexCount = 0;

	Vertex* quadBuffer = nullptr;
	Vertex* quadBufferPtr = nullptr;

	std::array<uint32_t, MAX_TEXTURE_COUNT> textureSlots = {};
	uint32_t textureSlotIndex = 1;
};

BatchRenderer::BatchRenderer()
{
	m_data = new RendererData();
	m_data->quadBuffer = new Vertex[MAX_VERTEX_COUNT];

	glCreateVertexArrays(1, &m_data->quadVA);
	glBindVertexArray(m_data->quadVA);

	glCreateBuffers(1, &m_data->quadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_data->quadVB);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_COUNT * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(m_data->quadVA, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexArrayAttrib(m_data->quadVA, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexArrayAttrib(m_data->quadVA, 2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

	glEnableVertexArrayAttrib(m_data->quadVA, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));

	uint32_t indices[MAX_INDEX_COUNT];
	uint32_t offset = 0;
	for (int i = 0; i < MAX_INDEX_COUNT; i += 6)
	{
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}

	glCreateBuffers(1, &m_data->quadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->quadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_data->whiteTexture);
	glBindTexture(GL_TEXTURE_2D, m_data->whiteTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	uint32_t color = 0xffffffff;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

	m_data->textureSlots[0] = m_data->whiteTexture;
	for (size_t i = 1; i < MAX_TEXTURE_COUNT; i++)
	{
		m_data->textureSlots[i] = 0;
	}
}

BatchRenderer::~BatchRenderer()
{
	for (auto texture : m_texturePool)
	{
		delete texture;
		texture = nullptr;
	}
	m_texturePool.clear();

	// Objects are deleted in ObjectManager
	m_objectPool.clear();

	delete[] m_data->quadBuffer;
	m_data->quadBuffer = nullptr;

	delete m_data;
}

void BatchRenderer::AddObject(GameObject* newObject)
{
	auto it = std::find(m_objectPool.begin(), m_objectPool.end(), newObject);

	if (it != m_objectPool.end())
	{
		std::cout << "GameObject already in Renderer: " << &it << '\n';
		return;
	}

	newObject->SetID(m_objectPool.size());

	if (m_objectPool.empty() || m_objectPool.back()->GetPos().z <= newObject->GetPos().z)
	{
		m_objectPool.push_back(newObject);
	}
	else
	{
		for (auto i = m_objectPool.begin(); i != m_objectPool.end(); ++i)
		{
			if ((*i)->GetPos().z >= newObject->GetPos().z)
			{
				m_objectPool.insert(i, newObject);
				return;
			}
		}
	}
}

Texture* BatchRenderer::LoadTexture(const std::string& dir)
{
	auto texture = new Texture(dir);
	m_texturePool.emplace_back(texture);
	return texture;
}

void BatchRenderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	if (m_data->indexCount >= MAX_INDEX_COUNT)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	const float textureIndex = 0.0f;

	const glm::vec3 positions[4] = { { position.x - size.x / 2.0f, position.y - size.y / 2.0f, position.z },
									 { position.x + size.x / 2.0f, position.y - size.y / 2.0f, position.z },
									 { position.x + size.x / 2.0f, position.y + size.y / 2.0f, position.z },
									 { position.x - size.x / 2.0f, position.y + size.y / 2.0f, position.z } };

	for (int i = 0; i < 4; i++)
	{
		m_data->quadBufferPtr->position = positions[i];
		m_data->quadBufferPtr->color = color;
		m_data->quadBufferPtr->texCoords = basicUVs[i];
		m_data->quadBufferPtr->texIndex = textureIndex;
		m_data->quadBufferPtr++;
	}

	m_data->indexCount += 6;
}

void BatchRenderer::DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color, Texture* texture, int frame, int spriteWidth)
{
	if (m_data->indexCount >= MAX_INDEX_COUNT || m_data->textureSlotIndex >= MAX_TEXTURE_COUNT)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	uint32_t textureIndex = GetTextureIndex(texture);
	if (textureIndex == 0)
	{
		textureIndex = AddTextureSlot(texture);
	}

	bool useBasicUVs = false;
	if (texture->GetWidth() == 16)
	{
		useBasicUVs = true;
	}
	else
	{
		SetActiveRegion(texture, frame, spriteWidth);
	}

	glm::vec3 positions[4] = {
		glm::vec3(position.x - size.x / 2.0f, position.y - size.y / 2.0f, position.z),
		glm::vec3(position.x + size.x / 2.0f, position.y - size.y / 2.0f, position.z),
		glm::vec3(position.x + size.x / 2.0f, position.y + size.y / 2.0f, position.z),
		glm::vec3(position.x - size.x / 2.0f, position.y + size.y / 2.0f, position.z)
	};

	for (int i = 0; i < 4; i++)
	{
		m_data->quadBufferPtr->position = positions[i];
		m_data->quadBufferPtr->color = color;
		m_data->quadBufferPtr->texCoords = (useBasicUVs) ? basicUVs[i] : m_uvs[i];
		m_data->quadBufferPtr->texIndex = static_cast<float>(textureIndex);
		m_data->quadBufferPtr++;
	}

	m_data->indexCount += 6;
}

uint32_t BatchRenderer::GetTextureIndex(Texture* texture)
{
	for (uint32_t i = 0; i < m_data->textureSlotIndex; i++)
	{
		if (m_data->textureSlots[i] == texture->GetID())
		{
			return i;
		}
	}
	return 0;
}

uint32_t BatchRenderer::AddTextureSlot(Texture* texture)
{
	if (m_data->textureSlotIndex >= MAX_TEXTURE_COUNT)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	m_data->textureSlots[m_data->textureSlotIndex] = texture->GetID();
	return m_data->textureSlotIndex++;
}

void BatchRenderer::SetActiveRegion(Texture* texture, int regionIndex, int spriteWidth)
{
	m_uvs.clear();

	// (int) textureSize / spriteWidth;
	int numberOfRegions = texture->GetWidth() / spriteWidth;

	float uv_x = (regionIndex % numberOfRegions) / (float)numberOfRegions;
	float uv_y = (regionIndex / (float)numberOfRegions) * (float)numberOfRegions;

	glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y);
	glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / numberOfRegions, uv_y);
	glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / numberOfRegions, (uv_y + 1.0f));
	glm::vec2 uv_up_left = glm::vec2(uv_x, (uv_y + 1.0f));

	m_uvs.push_back(uv_down_left);
	m_uvs.push_back(uv_down_right);
	m_uvs.push_back(uv_up_right);
	m_uvs.push_back(uv_up_left);
}

void BatchRenderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	DrawQuad(glm::vec3(position, -0.9f), size, color);
}

void BatchRenderer::RemoveQuad(const GameObject& object)
{
	int index = 0;
	for (auto it = m_objectPool.begin(); it != m_objectPool.end(); ++it)
	{
		if (*it == &object)
		{
			// Shuffle everything down/over
			for (int i = m_objectPool.size() - 1; i >= index; i--)
			{
				m_objectPool.at(i + 1) = m_objectPool.at(i);
			}
			break;
		}
		index++;
	}
}

int BatchRenderer::GetObjectIndex(const GameObject& object)
{
	for (int i = 0; i < m_objectPool.size(); i++)
	{
		if (m_objectPool[i] == &object)
		{
			return i;
		}
	}

	return -404;
}

void BatchRenderer::BeginBatch()
{
	m_data->quadBufferPtr = m_data->quadBuffer;
}

void BatchRenderer::EndBatch()
{
	GLsizeiptr size = (uint8_t*)m_data->quadBufferPtr - (uint8_t*)m_data->quadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, m_data->quadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, m_data->quadBuffer);
}

void BatchRenderer::Flush()
{
	for (uint32_t i = 0; i < m_data->textureSlotIndex; ++i)
	{
		glBindTextureUnit(i, m_data->textureSlots[i]);
	}

	glBindVertexArray(m_data->quadVA);
	glDrawElements(GL_TRIANGLES, m_data->indexCount, GL_UNSIGNED_INT, nullptr);

	m_data->indexCount = 0;
	m_data->textureSlotIndex = 1;
}

void BatchRenderer::Render(const glm::vec2& camPos, float distanceFromCenter)
{
	BeginBatch();

	int index = 0;
	for (auto object : m_objectPool)
	{
		if (object == nullptr || index == m_objectPool.size()) // This must be in order so if this object is null everyone after it should also be null.
		{
			EndBatch();
			Flush();

			return;
		}

		if (!object->GetRender())
			continue;

		if (glm::distance(camPos, glm::vec2(object->GetPos())) > distanceFromCenter)
			continue;

		if (Texture* texture = object->GetTexture()) // Check if texture is valid memory
		{
			DrawQuad(object->GetPos(), object->GetScale(), { object->GetColor(), 1.0f }, texture, object->GetFrame(), object->GetSpriteWidth());
		}
		else
		{
			DrawQuad(object->GetPos(), object->GetScale(), { object->GetColor(), 1.0f });
		}

		index++;
	}

	EndBatch();
	Flush();
}

void BatchRenderer::ShutDown()
{
	glDeleteVertexArrays(1, &m_data->quadVA);
	glDeleteBuffers(1, &m_data->quadVB);
	glDeleteBuffers(1, &m_data->quadIB);

	glDeleteTextures(1, &m_data->whiteTexture);

	delete[] m_data->quadBuffer;
	m_data->quadBuffer = nullptr;
}
