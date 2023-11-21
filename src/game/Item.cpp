#include "Item.h"

#include <engine/ConsoleLog.h>
#include <engine/ResourceManager.h>

Item::Item(const std::string& itemName, glm::ivec2 atlasPos, const std::function<void(Player&)>& onUse, const std::function<void(Player&)>& onPassive)
    : m_name(itemName), m_atlasPosition(atlasPos), m_onUse(onUse), m_onPassive(onPassive)
{
	Texture* itemAtlas = ResourceManager::GetInstance()->LoadTexture(ResourceManager::GetTexturePath("item_atlas"));
	SetTexture(itemAtlas);
	float spriteSize = 32.0f;

	float mod = 1.0f / spriteSize;

	float atlasPosX = static_cast<float>(atlasPos.x);
	float atlasPosY = static_cast<float>(atlasPos.y) + 1.0f;

	// Flip both X and Y coordinates of the UV rectangle
	SetScale(glm::vec2(1.0f, 1.0f));
	SetUVRect(glm::vec4(1.0f + (mod * atlasPosX), 1.0f - (mod * atlasPosY), mod, mod));
}

// Function to use the item (placeholder implementation)
void Item::use(Player& player)
{
	// Trigger onUse event when the item is used
	if (m_onUse)
	{
		m_onUse(player);
	}
	SLIME_INFO("Used item: {0}", m_name);
}

// Function to apply the passive effect of the item (placeholder implementation)
void Item::applyPassive(Player& player)
{
	// Trigger onPassive event when applying the passive effect
	if (m_onPassive)
	{
		m_onPassive(player);
	}
}

// Event handler for different events
void Item::registerEventHandler(ItemEvent event, const std::function<void(Player&)>& handler)
{
	m_eventHandlers[static_cast<size_t>(event)].push_back(handler);
}

bool Item::HasEventHandler(ItemEvent event) const
{
	return m_eventHandlers[static_cast<size_t>(event)].size() > 0;
}

void Item::handleEvent(ItemEvent event, Player& player)
{
	for (auto& handler : m_eventHandlers[static_cast<size_t>(event)])
	{
		handler(player);
	}
}

// Getter function for the item name
const std::string& Item::getName() const
{
	return m_name;
}

// Destructor (virtual to allow proper polymorphic destruction)
Item::~Item() {}