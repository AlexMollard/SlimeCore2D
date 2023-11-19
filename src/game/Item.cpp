#include "Item.h"
#include <engine/ConsoleLog.h>

// Constructor
Item::Item(const std::string& itemName, const std::function<void(Player&)>& onUse, const std::function<void(Player&)>& onPassive)
    : name(itemName), onUse(onUse), onPassive(onPassive)
{
}

// Function to use the item (placeholder implementation)
void Item::use(Player& player)
{
	// Trigger onUse event when the item is used
	if (onUse)
	{
		onUse(player);
	}
	SLIME_INFO("Used item: {0}", name);
}

// Function to apply the passive effect of the item (placeholder implementation)
void Item::applyPassive(Player& player)
{
	// Trigger onPassive event when applying the passive effect
	if (onPassive)
	{
		onPassive(player);
	}
}

// Event handler for different events
void Item::registerEventHandler(ItemEvent event, const std::function<void(Player&)>& handler)
{
	eventHandlers[static_cast<size_t>(event)].push_back(handler);
}

bool Item::HasEventHandler(ItemEvent event) const 
{
	return eventHandlers[static_cast<size_t>(event)].size() > 0;
}

void Item::handleEvent(ItemEvent event, Player& player) 
{
	for (auto& handler : eventHandlers[static_cast<size_t>(event)])
	{
		handler(player);
	}
}

// Getter function for the item name
const std::string& Item::getName() const
{
	return name;
}

// Destructor (virtual to allow proper polymorphic destruction)
Item::~Item() {}