#include "VulkanSwapchain.h"
#include "engine/MemoryDebugging.h"
#include <engine/ConsoleLog.h>
#include "VulkanFramebuffers.h"

#include <iostream>

VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, const vk::SurfaceKHR& surface) : m_device(device), m_renderPass(device, surface)
{
	if (!m_device.GetPhysicalDevice() || !m_device.GetLogicalDevice())
	{
		SLIME_ERROR("Failed to create swapchain: device is invalid");
	}

	CreateSwapchain(surface);
	PrintDebugInfo(surface);
}

VulkanSwapchain::~VulkanSwapchain()
{
	DestroySwapchain();
}

void VulkanSwapchain::CreateSwapchain(const vk::SurfaceKHR& surface)
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = GetSurfaceCapabilities(surface);
	if (surfaceCapabilities.minImageCount == 0)
	{
		SLIME_ERROR("Failed to get surface capabilities");
	}

	vk::SurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(surface);
	vk::PresentModeKHR presentMode     = ChoosePresentMode(surface);
	if (surfaceFormat.format == vk::Format::eUndefined)
	{
		SLIME_ERROR("Failed to choose surface format");
	}

	// Determine the number of images in the swapchain
	uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
	{
		imageCount = surfaceCapabilities.maxImageCount;
	}

	m_swapchainExtent = surfaceCapabilities.currentExtent;

	// Create the swapchain create info
	vk::SwapchainCreateInfoKHR swapchainCreateInfo;
	swapchainCreateInfo.setSurface(surface);
	swapchainCreateInfo.setMinImageCount(imageCount);
	swapchainCreateInfo.setImageFormat(surfaceFormat.format);
	swapchainCreateInfo.setImageColorSpace(surfaceFormat.colorSpace);
	swapchainCreateInfo.setImageExtent(surfaceCapabilities.currentExtent);
	swapchainCreateInfo.setImageArrayLayers(1);
	swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapchainCreateInfo.setPreTransform(surfaceCapabilities.currentTransform);
	swapchainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapchainCreateInfo.setPresentMode(presentMode);
	swapchainCreateInfo.setClipped(VK_TRUE);
	swapchainCreateInfo.setOldSwapchain(nullptr);

	// Get the queue family indices
	std::vector<uint32_t> queueFamilyIndices = { m_device.GetQueueFamilyIndex(VulkanDevice::QueueType::GRAPHICS),
		                                         m_device.GetQueueFamilyIndex(VulkanDevice::QueueType::PRESENT) };

	// Check if the graphics and present queue family indices are the same
	if (queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		swapchainCreateInfo.setQueueFamilyIndexCount(2);
		swapchainCreateInfo.setPQueueFamilyIndices(queueFamilyIndices.data());
	}
	else
	{
		swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
		swapchainCreateInfo.setQueueFamilyIndexCount(0);
		swapchainCreateInfo.setPQueueFamilyIndices(nullptr);
	}

	m_swapchain       = m_device.GetLogicalDevice().createSwapchainKHR(swapchainCreateInfo);
	m_swapchainImages = m_device.GetLogicalDevice().getSwapchainImagesKHR(m_swapchain);

	m_framebuffers = new VulkanFramebuffers(m_device.GetLogicalDevice(), m_renderPass, m_swapchainExtent, m_swapchainImages);
}

void VulkanSwapchain::DestroySwapchain()
{
	delete m_framebuffers;
	m_device.GetLogicalDevice().destroySwapchainKHR(m_swapchain);
}

vk::SwapchainKHR VulkanSwapchain::GetSwapchain() const
{
	return m_swapchain;
}

VulkanRenderPass& VulkanSwapchain::GetRenderPass() 
{
	return m_renderPass;
}

vk::Extent2D VulkanSwapchain::GetSwapchainExtent() const
{
	return m_swapchainExtent;
}

vk::SurfaceCapabilitiesKHR VulkanSwapchain::GetSurfaceCapabilities(const vk::SurfaceKHR& surface)
{
	return m_device.GetPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
}

vk::SurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const vk::SurfaceKHR& surface)
{
	auto formats = m_device.GetPhysicalDevice().getSurfaceFormatsKHR(surface);

	for (const auto& format : formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}

	// If the preferred format is not available, return the first available format
	return formats[0];
}

vk::PresentModeKHR VulkanSwapchain::ChoosePresentMode(const vk::SurfaceKHR& surface)
{
	std::vector<vk::PresentModeKHR> presentModes = m_device.GetPhysicalDevice().getSurfacePresentModesKHR(surface);

	// Assume you have a default present mode
	vk::PresentModeKHR preferredPresentMode = vk::PresentModeKHR::eFifo;

	for (const auto& mode : presentModes)
	{
		if (mode == preferredPresentMode)
		{
			return mode;
		}
	}

	SLIME_WARN("Preferred present mode not found, falling back to the first available mode.");
	return presentModes[0];
}

// Debug functions
void VulkanSwapchain::PrintDebugInfo(const vk::SurfaceKHR& surface)
{
	PrintSurfaceCapabilities(GetSurfaceCapabilities(surface));
	PrintPresentModes(m_device.GetPhysicalDevice().getSurfacePresentModesKHR(surface));
}

void VulkanSwapchain::AddValueToBuffer(const char* name, const auto& value)
{
	std::cout << std::setw(20) << std::left << name << ": " << value << "\n";
}

void VulkanSwapchain::PrintSurfaceCapabilities(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities)
{
	std::cout << "Surface Capabilities:" << std::endl;
	AddValueToBuffer("  Min Image Count", surfaceCapabilities.minImageCount);
	AddValueToBuffer("  Max Image Count", surfaceCapabilities.maxImageCount);

	std::string extentString = std::format("[width: {}, height {}]", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
	AddValueToBuffer("  Current Extent", extentString.c_str());
	std::cout << std::endl;
}

void VulkanSwapchain::PrintPresentModes(const std::vector<vk::PresentModeKHR>& presentModes)
{
	std::cout << "Present Modes:" << std::endl;
	for (const auto& mode : presentModes)
	{
		AddValueToBuffer("  Present Mode", vk::to_string(mode));
	}
	std::cout << std::endl;
}
