#pragma once
#include <string>
#include <functional>
#include "ext/vector_int2.hpp"
#include "engine/GameObject.h"
class Player;

enum class ItemEvent
{
	OnDamage,
	OnPickup,
	OnShoot,
	Count
};

class Item : public GameObject
{
public:
	Item(const std::string& itemName, glm::ivec2 atlasPos, const std::function<void(Player&)>& onUse, const std::function<void(Player&)>& onPassive);

	// Function to use the item (e.g., when the player activates it)
	void use(Player& player);

	// Function to apply the passive effect of the item
	void applyPassive(Player& player);

	// Event handler for different events
	void registerEventHandler(ItemEvent event, const std::function<void(Player&)>& handler);
	bool HasEventHandler(ItemEvent event) const;
	void handleEvent(ItemEvent event, Player& player);

	// Getter function for the item name
	const std::string& getName() const;

	// Destructor (virtual to allow proper polymorphic destruction)
	virtual ~Item();

private:
	std::string m_name;
	glm::ivec2 m_atlasPosition;
	std::function<void(Player&)> m_onUse;
	std::function<void(Player&)> m_onPassive;
	std::vector<std::function<void(Player&)>> m_eventHandlers[static_cast<size_t>(ItemEvent::Count)];
};