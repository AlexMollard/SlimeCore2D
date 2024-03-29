#include "QuadBatchRenderer.h"
#include "engine/MemoryDebugging.h"

#include "Constants.h"

#include "Texture.h"

#include "engine/GameObject.h"
#include "glm.hpp"

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoords;
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
};

QuadBatchRenderer::QuadBatchRenderer()
{
	RendererData* data = SetData(new RendererData());
	data->quadBuffer   = new Vertex[MAX_VERTEX_COUNT];

	glCreateVertexArrays(1, &data->quadVA);
	glBindVertexArray(data->quadVA);

	glCreateBuffers(1, &data->quadVB);
	glBindBuffer(GL_ARRAY_BUFFER, data->quadVB);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_COUNT * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(data->quadVA, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexArrayAttrib(data->quadVA, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexArrayAttrib(data->quadVA, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

	glEnableVertexArrayAttrib(data->quadVA, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));

	glEnableVertexArrayAttrib(data->quadVA, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, maskIndex));

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

	glCreateBuffers(1, &data->quadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->quadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glCreateTextures(GL_TEXTURE_2D, 1, &data->whiteTexture);
	glBindTexture(GL_TEXTURE_2D, data->whiteTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	uint32_t color = 0xffffffff;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

	data->textureSlots[0] = data->whiteTexture;
	for (size_t i = 1; i < MAX_TEXTURE_COUNT; i++)
	{
		data->textureSlots[i] = 0;
	}
}

QuadBatchRenderer::~QuadBatchRenderer()
{
	ShutDown();
	BatchRenderer::~BatchRenderer();
}

void QuadBatchRenderer::SetSpriteUvs(glm::vec4 uvRect)
{
	m_uvs.clear();

	// Calculate UV coordinates based on the uvRect
	auto uv_down_left  = glm::vec2(uvRect.x, uvRect.y);
	auto uv_down_right = glm::vec2(uvRect.x + uvRect.z, uvRect.y);
	auto uv_up_right   = glm::vec2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
	auto uv_up_left    = glm::vec2(uvRect.x, uvRect.y + uvRect.w);

	m_uvs.push_back(uv_down_left);
	m_uvs.push_back(uv_down_right);
	m_uvs.push_back(uv_up_right);
	m_uvs.push_back(uv_up_left);
}

void QuadBatchRenderer::DrawQuad(const QuadBatchData& batchData)
{
	RendererData* data = GetData();
	if (data->indexCount >= MAX_INDEX_COUNT || data->textureSlotIndex >= MAX_TEXTURE_COUNT)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	uint32_t textureIndex = 0;
	if (batchData.texture)
	{
		textureIndex = GetTextureIndex(batchData.texture);
		if (textureIndex == 0)
		{
			textureIndex = AddTextureSlot(batchData.texture);
		}
	}
	SetSpriteUvs(batchData.uvRect);

	uint32_t maskIndex = 0;
	if (batchData.maskTexture)
	{
		maskIndex = GetTextureIndex(batchData.maskTexture);
		if (maskIndex == 0)
		{
			maskIndex = AddTextureSlot(batchData.maskTexture);
		}
	}

	float z                = batchData.position.z;
	glm::vec2 halfSize     = glm::vec2(batchData.size.x / 2.0f, batchData.size.y / 2.0f);
	glm::vec2 center       = glm::vec2(batchData.position.x + halfSize.x, batchData.position.y + halfSize.y);
	glm::vec2 anchorOffset = glm::vec2(batchData.anchorPoint.x * batchData.size.x, batchData.anchorPoint.y * batchData.size.y);

	glm::vec3 positions[4] = { { center - halfSize - anchorOffset, z },
		                       { center + glm::vec2(halfSize.x, -halfSize.y) - anchorOffset, z },
		                       { center + halfSize - anchorOffset, z },
		                       { center + glm::vec2(-halfSize.x, halfSize.y) - anchorOffset, z } };

	// Apply flip policy (can't just swap because of backface culling)
	if (batchData.flipPolicy == FlipPolicy::Horizontal || batchData.flipPolicy == FlipPolicy::Both)
	{
		std::swap(positions[0], positions[1]);
		std::swap(positions[2], positions[3]);
	}
	if (batchData.flipPolicy == FlipPolicy::Vertical || batchData.flipPolicy == FlipPolicy::Both)
	{
		std::swap(positions[0], positions[3]);
		std::swap(positions[1], positions[2]);
	}

	float tIndex = static_cast<float>(textureIndex);
	float mIndex = static_cast<float>(maskIndex);
	for (int i = 0; i < 4; ++i)
	{
		data->quadBufferPtr->position  = positions[i];
		data->quadBufferPtr->color     = batchData.color;
		data->quadBufferPtr->texCoords = m_uvs[i];
		data->quadBufferPtr->texIndex  = tIndex;
		data->quadBufferPtr->maskIndex = mIndex;
		data->quadBufferPtr++;
	}

	data->indexCount += 6;
}

void QuadBatchRenderer::Render(const glm::vec2& camPos, float distanceFromCenter)
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

		if (!object->GetRender())
			continue;

		if (m_occulsionCulling && glm::distance(camPos, glm::vec2(object->GetPos())) > distanceFromCenter)
			continue;

		QuadBatchData batchData = { object->GetPos(),          object->GetAnchorPoint(), object->GetScale(),      { object->GetColor(), 1.0f }, object->GetUVRect(),
			                        object->GetRotation(),     object->GetTexture(),     object->GetMaskTexture(),    object->GetFlipPolicy()};

		batchData.position.z += m_zOffset;
		DrawQuad(batchData);

		index++;
	}

	EndBatch();
	Flush();
}

void QuadBatchRenderer::BeginBatch()
{
	RendererData* data  = GetData();
	data->quadBufferPtr = data->quadBuffer;
}

void QuadBatchRenderer::EndBatch()
{
	RendererData* data = GetData();
	GLsizeiptr size    = (uint8_t*)data->quadBufferPtr - (uint8_t*)data->quadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, data->quadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data->quadBuffer);
}

void QuadBatchRenderer::Flush()
{
	RendererData* data = GetData();
	for (uint32_t i = 0; i < data->textureSlotIndex; ++i)
	{
		glBindTextureUnit(i, data->textureSlots[i]);
	}

	glBindVertexArray(data->quadVA);
	glDrawElements(GL_TRIANGLES, data->indexCount, GL_UNSIGNED_INT, nullptr);

	data->indexCount       = 0;
	data->textureSlotIndex = 1;
}

void QuadBatchRenderer::ShutDown()
{
	RendererData* data = GetData();
	glDeleteVertexArrays(1, &data->quadVA);
	glDeleteBuffers(1, &data->quadVB);
	glDeleteBuffers(1, &data->quadIB);

	glDeleteTextures(1, &data->whiteTexture);

	delete[] data->quadBuffer;
	data->quadBuffer = nullptr;
	delete data;
}

uint32_t QuadBatchRenderer::GetTextureIndex(Texture* texture)
{
	RendererData* data = GetData();
	for (uint32_t i = 0; i < data->textureSlotIndex; i++)
	{
		if (data->textureSlots[i] == texture->GetID())
		{
			return i;
		}
	}
	return 0;
}

uint32_t QuadBatchRenderer::AddTextureSlot(Texture* texture)
{
	RendererData* data = GetData();
	if (data->textureSlotIndex >= MAX_TEXTURE_COUNT)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	data->textureSlots[data->textureSlotIndex] = texture->GetID();
	return data->textureSlotIndex++;
}

uint32_t QuadBatchRenderer::AddTextureSlot(GLuint textureID)
{
	RendererData* data = GetData();
	if (data->textureSlotIndex >= MAX_TEXTURE_COUNT)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	data->textureSlots[data->textureSlotIndex] = textureID;
	return data->textureSlotIndex++;
}

float QuadBatchRenderer::GetZOffset() const
{
	return m_zOffset;
}

void QuadBatchRenderer::SetZOffset(float val)
{
	m_zOffset = val;
}

bool QuadBatchRenderer::GetOcclusionCulling() const
{
	return m_occulsionCulling;
}

void QuadBatchRenderer::SetOcclusionCulling(bool val)
{
	m_occulsionCulling = val;
}