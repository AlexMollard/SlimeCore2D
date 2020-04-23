#include "Renderer2D.h"
#include <algorithm>
#include <array>

std::vector<glm::vec2> Renderer2D::UVs;


static const size_t maxQuadCount = 2000;
static const size_t maxVertexCount = maxQuadCount * 4;
static const size_t maxIndexCount = maxQuadCount * 6;
static const size_t maxTextures = 21;

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

	std::array<uint32_t, maxTextures> textureSlots;
	uint32_t textureSlotIndex = 1;
};

static RendererData data;


Renderer2D::Renderer2D(MeshManager* meshManager, Camera* camera)
{
	this->meshManager = meshManager;

	basicShader = new Shader("Basic Shader", "..\\Shaders\\BasicVertex.shader", "..\\Shaders\\BasicFragment.shader");

	this->camera = camera;

	basicShader->Use();

	auto loc = glGetUniformLocation(basicShader->GetID(), "Textures");
	int samplers[maxTextures];
	for (int i = 0; i < maxTextures; i++)
		samplers[i] = i;

	glUniform1iv(loc, maxTextures, samplers);

	Init();
}

Renderer2D::~Renderer2D()
{

	ShutDown();
	for (int i = 0; i < texturePool.size(); i++)
	{
		delete texturePool[i];
		texturePool[i] = nullptr;
	}

	delete basicShader;
	basicShader = nullptr;

	delete camera;
	camera = nullptr;
}

void Renderer2D::AddObject(GameObject* newObject)
{
	std::vector<GameObject*>::iterator it = find(objectPool.begin(), objectPool.end(), newObject);

	if (it != objectPool.end())
	{
		std::cout << "GameObject already in Renderer: " << *it << '\n';
		return;
	}

	GameObject* go = newObject;
	go->SetID(objectPool.size());
	go->SetShader(basicShader);
	objectPool.push_back(go);
}

Texture* Renderer2D::LoadTexture(std::string dir)
{
	Texture* tempTex = new Texture(dir);

	texturePool.push_back(tempTex);
	return tempTex;
}


void Renderer2D::Draw()
{
	basicShader->Use();
	
	basicShader->setMat4("OrthoMatrix", camera->GetTransform());
	basicShader->setMat4("Model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.0f)));
	BeginBatch();

	for (int i = 0; i < objectPool.size(); i++)
	{
		if (objectPool[i]->GetType() == ObjectType::Quad && objectPool[i]->GetTexture() == nullptr)
		{
			DrawQuad(objectPool[i]->GetPos(), objectPool[i]->GetScale(), { objectPool[i]->GetColor() ,1.0f });
		}
		else if (objectPool[i]->GetType() == ObjectType::Quad && objectPool[i]->GetTexture() != nullptr)
		{
			DrawQuad(objectPool[i]->GetPos(), objectPool[i]->GetScale(), objectPool[i]->GetTexture(), objectPool[i]->GetFrame());
		}
		else if (objectPool[i]->GetType() == ObjectType::Circle)
		{
			basicShader->setMat4("Model", objectPool[i]->GetModel());
			meshManager->Draw(objectPool[i]);
			basicShader->setMat4("Model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
		}
	}

	EndBatch();
	Flush();
}


Shader* Renderer2D::GetBasicShader()
{
	return basicShader;
}

void Renderer2D::DrawQuad(glm::vec2 position, glm::vec2 size, glm::vec4 color)
{
	if (data.indexCount >= maxIndexCount)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	float textureIndex = 0.0f;

	data.quadBufferPtr->position = { position.x - size.x / 2, position.y - size.y / 2, 0.0f };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 0.0f,0.0f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x + size.x / 2,position.y - size.y / 2, 0.0f };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 1.0f,0.0f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x + size.x / 2,position.y + size.y / 2, 0.0f };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 1.0f,1.0f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x - size.x / 2,position.y + size.y / 2, 0.0f };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 0.0f,1.0f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer2D::DrawQuad(glm::vec2 position, glm::vec2 size, Texture* texture, int frame)
{
	if (data.indexCount >= maxIndexCount || data.textureSlotIndex > maxTextures)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	constexpr glm::vec4 color = { 1.0f,1.0f ,1.0f ,1.0f };

	float textureIndex = 0.0f;
	for (uint32_t i = 1; i < data.textureSlotIndex; i++)
	{
		if (data.textureSlots[i] == texture->GetID())
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		textureIndex = (float)data.textureSlotIndex;
		data.textureSlots[data.textureSlotIndex] = texture->GetID();
		data.textureSlotIndex++;
	}

	setActiveRegion(texture, frame);

	data.quadBufferPtr->position = { position.x - size.x / 2, position.y - size.y / 2, 0.0f };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = UVs[0];
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x + size.x / 2,position.y - size.y / 2, 0.0f };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = UVs[1];
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x + size.x / 2,position.y + size.y / 2, 0.0f };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = UVs[2];
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x - size.x / 2,position.y + size.y / 2, 0.0f };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = UVs[3];
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer2D::setActiveRegion(Texture* texture, int regionIndex)
{
	UVs.clear();

	//					  (int) textureSize / spriteWidth;
	int numberOfRegions = texture->GetWidth() / 16;

	float uv_x = (regionIndex % numberOfRegions) / (float)numberOfRegions;
	float uv_y = (regionIndex / (float)numberOfRegions) * (float)numberOfRegions;

	glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y);
	glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / numberOfRegions, uv_y);
	glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / numberOfRegions, (uv_y + 1.0f));
	glm::vec2 uv_up_left = glm::vec2(uv_x, (uv_y + 1.0f));

	UVs.push_back(uv_down_left);
	UVs.push_back(uv_down_right);
	UVs.push_back(uv_up_right);
	UVs.push_back(uv_up_left);
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
	for (uint32_t i = 0; i < data.textureSlotIndex; i++)
		glBindTextureUnit(i, data.textureSlots[i]);

	glBindVertexArray(data.quadVA);
	glDrawElements(GL_TRIANGLES, data.indexCount, GL_UNSIGNED_INT, nullptr);

	data.indexCount = 0;
	data.textureSlotIndex = 1;
}
