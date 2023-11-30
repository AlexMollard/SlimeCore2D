#include "VulkanTypes.h"

 VulkanBuffer::VulkanBuffer() : bufferHandle(VK_NULL_HANDLE), allocation(VK_NULL_HANDLE), size(0) {}

void VulkanBuffer::SetBufferHandle(VkBuffer handle)
{
	bufferHandle = handle;
}

void VulkanBuffer::SetAllocation(VmaAllocation alloc)
{
	allocation = alloc;
}

void VulkanBuffer::SetSize(size_t bufferSize)
{
	size = bufferSize;
}

VkBuffer VulkanBuffer::GetBufferHandle() const
{
	return bufferHandle;
}

VmaAllocation VulkanBuffer::GetAllocation() const
{
	return allocation;
}

size_t VulkanBuffer::GetSize() const
{
	return size;
}
