#include "pch.h"
#include "Renderer2D.h"

#include "Math.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <numeric>

constexpr size_t maxQuadCount = 2000;
constexpr size_t maxVertexCount = maxQuadCount * 4;
constexpr size_t maxIndexCount = maxQuadCount * 6;
constexpr size_t maxTextures = 31;

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

	std::array<uint32_t, maxTextures> textureSlots = {};
	uint32_t textureSlotIndex = 1;
};

static RendererData data;

static constexpr glm::vec2 basicUVs[4] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };

Renderer2D::Renderer2D(Camera* camera) : m_camera(camera)
{
	m_texturePool.push_back(new Texture(ResourceManager::GetTexturePath("hotbar_background")));
	m_texturePool.push_back(new Texture(ResourceManager::GetTexturePath("hotbar_slot")));

	m_basicShader.Use();

	const auto loc = glGetUniformLocation(m_basicShader.GetID(), "Textures");
	std::array<int, maxTextures> samplers{};
	std::iota(samplers.begin(), samplers.end(), 0);

	glUniform1iv(loc, maxTextures, samplers.data());

	m_uiShader.Use();

	const auto loc2 = glGetUniformLocation(m_uiShader.GetID(), "Textures");

	glUniform1iv(loc2, maxTextures, samplers.data());

	Init();
}

Renderer2D::~Renderer2D()
{
	for (auto texture : m_texturePool)
	{
		delete texture;
		texture = nullptr;
	}
	m_texturePool.clear();

	// Objects are deleted in ObjectManager
	m_objectPool.clear();

	ShutDown();
	m_camera = nullptr;

	delete[] data.quadBuffer;
	data.quadBuffer = nullptr;
}

void Renderer2D::AddObject(GameObject* newObject)
{
	auto it = std::find(m_objectPool.begin(), m_objectPool.end(), newObject);

	if (it != m_objectPool.end())
	{
		std::cout << "GameObject already in Renderer: " << &it << '\n';
		return;
	}

	newObject->SetID(m_objectPool.size());
	newObject->SetShader(&m_basicShader);

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

Texture* Renderer2D::LoadTexture(const std::string& dir)
{
	auto texture = new Texture(dir);
	m_texturePool.at(m_texturePool.size()) = texture;
	return texture;
}

void Renderer2D::Draw()
{
	BeginBatch();

	m_basicShader.Use();
	m_basicShader.setMat4("OrthoMatrix", m_camera->GetTransform());
	m_basicShader.setMat4("Model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	m_basicShader.setVec4("SunColor", glm::vec4(1.0f));

	glm::vec2 camPos = m_camera->GetPosition();
	float distanceFromCenter = -m_camera->GetAspectRatio().x + 6;
	int index = 0;
	for (auto object : m_objectPool)
	{
		if (object == nullptr || index == m_objectPool.size()) // This must be in order so if this object is null everyone after it should also be null.
		{
			EndBatch();
			Flush();
			DrawUI();

			return;
		}

		if (!object->GetRender())
			continue;

		if (glm::distance(camPos, glm::vec2(object->GetPos())) > distanceFromCenter)
			continue;

		Texture* texture = object->GetTexture();
		if (texture && texture != nullptr) // Check if texture is valid memory
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
	DrawUI();
}

void Renderer2D::DrawUI()
{
	BeginBatch();

	m_uiShader.Use();
	m_uiShader.setMat4("OrthoMatrix", m_uiMatrix);
	m_uiShader.setMat4("Model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

	static int testingINT = 0;
	glm::vec3 lit = glm::vec3(1.0f);
	glm::vec3 unLit = glm::vec3(0.05f);

	// Slots
	auto textureSize = glm::vec2(m_texturePool[1]->GetWidth() / 16, m_texturePool[1]->GetHeight() / 16) * 0.35f;
	glm::vec2 pos(-3.35f, -8.35f);
	for (int i = 0; i < 6; i++, pos.x += 1.35f)
	{
		auto color = (testingINT == i) ? lit : unLit;
		DrawUIQuad(pos, 8, textureSize, color, m_texturePool[1]);
	}

	DrawUIQuad(glm::vec2(14, 7), 8, glm::vec2(3.5f), glm::vec3(0));

	testingINT += Input::GetScroll();
	if (testingINT > 5)
		testingINT = 0;
	else if (testingINT < 0)
		testingINT = 5;

	// Bar on bottom
	DrawUIQuad(glm::vec2(0, -8.15f), 10, textureSize, glm::vec3(1), m_texturePool[0]);

	EndBatch();
	Flush();
}

Shader* Renderer2D::GetBasicShader()
{
	return &m_basicShader;
}

void Renderer2D::DrawUIQuad(glm::vec2 pos, int layer, glm::vec2 size, glm::vec3 color, Texture* texture)
{
	if (texture == nullptr)
		DrawQuad(glm::vec3(pos, 2 + layer * 0.01f), size, { color, 1.0f });
	else
		DrawQuad(glm::vec3(pos, 2 + layer * 0.01f), size, { color, 1.0f }, texture, 0, texture->GetWidth());
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	if (data.indexCount >= maxIndexCount)
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
		data.quadBufferPtr->position = positions[i];
		data.quadBufferPtr->color = color;
		data.quadBufferPtr->texCoords = basicUVs[i];
		data.quadBufferPtr->texIndex = textureIndex;
		data.quadBufferPtr++;
	}

	data.indexCount += 6;
}

void Renderer2D::DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color, Texture* texture, int frame, int spriteWidth)
{
	if (data.indexCount >= maxIndexCount || data.textureSlotIndex >= maxTextures)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}
	
	uint32_t textureIndex = 0;
	for (uint32_t i = 0; i < data.textureSlotIndex; i++)
	{
		if (data.textureSlots[i] == texture->GetID())
		{
			textureIndex = i;
			break;
		}
	}

	if (textureIndex == 0)
	{
		if (data.textureSlotIndex >= maxTextures)
		{
			EndBatch();
			Flush();
			BeginBatch();
		}
		textureIndex = data.textureSlotIndex;

		data.textureSlots[data.textureSlotIndex] = texture->GetID();
		data.textureSlotIndex++;
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

	glm::vec3 positions[4] = { glm::vec3(position.x - size.x / 2.0f, position.y - size.y / 2.0f, position.z),
							   glm::vec3(position.x + size.x / 2.0f, position.y - size.y / 2.0f, position.z),
							   glm::vec3(position.x + size.x / 2.0f, position.y + size.y / 2.0f, position.z),
							   glm::vec3(position.x - size.x / 2.0f, position.y + size.y / 2.0f, position.z) };

	for (int i = 0; i < 4; i++)
	{
		data.quadBufferPtr->position = positions[i];
		data.quadBufferPtr->color = color;
		data.quadBufferPtr->texCoords = (useBasicUVs) ? basicUVs[i] : m_uvs[i];
		data.quadBufferPtr->texIndex = static_cast<float>(textureIndex);
		data.quadBufferPtr++;
	}

	data.indexCount += 6;
}

void Renderer2D::RemoveQuad(const GameObject& object)
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

int Renderer2D::GetObjectIndex(const GameObject& object)
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

void Renderer2D::SetActiveRegion(Texture* texture, int regionIndex, int spriteWidth)
{
	m_uvs.clear();

	//					  (int) textureSize / spriteWidth;
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

void Renderer2D::Init()
{
	data.quadBuffer = new Vertex[maxVertexCount];

	glCreateVertexArrays(1, &data.quadVA);
	glBindVertexArray(data.quadVA);

	glCreateBuffers(1, &data.quadVB);
	glBindBuffer(GL_ARRAY_BUFFER, data.quadVB);
	glBufferData(GL_ARRAY_BUFFER, maxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(data.quadVA, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexArrayAttrib(data.quadVA, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexArrayAttrib(data.quadVA, 2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

	glEnableVertexArrayAttrib(data.quadVA, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));

	uint32_t indices[maxIndexCount];
	uint32_t offset = 0;
	for (int i = 0; i < maxIndexCount; i += 6)
	{
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}

	glCreateBuffers(1, &data.quadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.quadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glCreateTextures(GL_TEXTURE_2D, 1, &data.whiteTexture);
	glBindTexture(GL_TEXTURE_2D, data.whiteTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	uint32_t color = 0xffffffff;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

	data.textureSlots[0] = data.whiteTexture;
	for (size_t i = 1; i < maxTextures; i++)
	{
		data.textureSlots[i] = 0;
	}
}

void Renderer2D::ShutDown()
{
	glDeleteVertexArrays(1, &data.quadVA);
	glDeleteBuffers(1, &data.quadVB);
	glDeleteBuffers(1, &data.quadIB);

	glDeleteTextures(1, &data.whiteTexture);

	delete[] data.quadBuffer;
	data.quadBuffer = nullptr;
}

void Renderer2D::BeginBatch()
{
	data.quadBufferPtr = data.quadBuffer;
}

void Renderer2D::EndBatch()
{
	GLsizeiptr size = (uint8_t*)data.quadBufferPtr - (uint8_t*)data.quadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, data.quadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.quadBuffer);
}

void Renderer2D::Flush()
{
	for (uint32_t i = 0; i < data.textureSlotIndex; ++i)
	{
		glBindTextureUnit(i, data.textureSlots[i]);
	}

	glBindVertexArray(data.quadVA);
	glDrawElements(GL_TRIANGLES, data.indexCount, GL_UNSIGNED_INT, nullptr);

	data.indexCount = 0;
	data.textureSlotIndex = 1;
}
