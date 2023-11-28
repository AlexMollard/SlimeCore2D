#include "engine/MemoryDebugging.h"
#include "engine/Input.h"
#include "engine/ConsoleLog.h"
#include "engine/ResourceManager.h"
#include "engine/Vulkan/VulkanEngine.h"

Input* Input::m_instance                     = 0;
ResourceManager* ResourceManager::m_instance = 0;

int main(int argc, char* argv[])
{
	// Fully flush the standard output buffer. (Reloading was causing it not too :/)
	std::cout << std::unitbuf;

	VulkanEngine engine;

	engine.Init();
	engine.Update();
	engine.Cleanup();

	return 0;
}