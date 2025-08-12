/**
 * @File VulkanInterop.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/12
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <vector>

#include "Core/Base/Defines.hpp"
#include "Platform/Window.hpp"

namespace xihe {

// 查询在当前平台/窗口上创建 Vulkan 实例所需的扩展名列表
XIHE_API bool GetVulkanRequiredInstanceExtensions(IWindow* window, std::vector<const char*>& outExtensions);

// 基于窗口与已创建的 VkInstance 构建 VkSurfaceKHR（类型擦除为 void* 返回）
XIHE_API bool CreateVulkanSurfaceFromWindow(IWindow* window, void* vkInstance, void** outSurface);

} // namespace xihe


