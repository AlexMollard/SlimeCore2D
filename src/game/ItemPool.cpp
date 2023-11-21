#include "ItemPool.h"

ItemPool::ItemPool() : m_randomEngine(m_randomDevice())
{
	// Initialize the random engine with a seed from the random device
}

ItemPool::~ItemPool()
{
	// Destructor: items will be automatically destroyed when the vector is cleared
}

void ItemPool::addItem(const std::shared_ptr<Item>& item)
{
	m_items.push_back(item);
}

void ItemPool::addItems(const std::vector<std::shared_ptr<Item>>& items)
{
	m_items.insert(m_items.end(), items.begin(), items.end());
}

std::shared_ptr<Item> ItemPool::spawnRandomItem()
{
	if (m_items.empty())
	{
		// No items in the pool
		return nullptr;
	}

	// Generate a random index to choose a random item
	std::uniform_int_distribution<size_t> distribution(0, m_items.size() - 1);
	size_t randomIndex = distribution(m_randomEngine);

	// Return a copy of the randomly selected item
	return std::make_shared<Item>(*m_items[randomIndex]);
}
