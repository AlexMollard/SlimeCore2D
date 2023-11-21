#include "Item.h"
#include <memory>
#include <random>
#include <vector>

class ItemPool
{
public:
	// Constructor
	ItemPool();

	// Destructor
	~ItemPool();

	// Add a single item to the pool
	void addItem(const std::shared_ptr<Item>& item);

	// Add items in bulk
	void addItems(const std::vector<std::shared_ptr<Item>>& items);

	// Spawn a random item from the pool
	std::shared_ptr<Item> spawnRandomItem();

private:
	std::vector<std::shared_ptr<Item>> m_items;
	std::random_device m_randomDevice;
	std::mt19937 m_randomEngine;
};