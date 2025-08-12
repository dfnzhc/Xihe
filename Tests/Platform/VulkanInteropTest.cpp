/**
 * @File VulkanInteropTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/7
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Platform/PlatformFactory.hpp>
#include <Platform/Platform.hpp>
#include <Platform/Window.hpp>
#include <Platform/VulkanInterop.hpp>

#if __has_include(<SDL3/SDL_vulkan.h>)
#  include <SDL3/SDL_vulkan.h>
#  define XIHE_HAS_SDL_VULKAN 1
#endif

#if __has_include(<vulkan/vulkan.h>)
#  include <vulkan/vulkan.h>
#  define XIHE_HAS_VULKAN 1
#endif

using namespace xihe;

TEST(Platform_SDL_Vulkan, QueryRequiredExtensions)
{
    auto platform = CreatePlatformSDL();
    ASSERT_TRUE(platform);
    ASSERT_TRUE(platform->initialize());

    WindowDesc desc;
    auto window = platform->createWindow(desc);
    ASSERT_TRUE(window);

    std::vector<const char*> exts;
    EXPECT_TRUE(GetVulkanRequiredInstanceExtensions(window.get(), exts));
    EXPECT_GE(exts.size(), 1u);
    for (const char* e : exts) { ASSERT_NE(e, nullptr); }
}

#if defined(XIHE_HAS_SDL_VULKAN) && defined(XIHE_HAS_VULKAN)
TEST(Platform_SDL_Vulkan, CreateInstanceAndSurfaceSmoke)
{
    auto platform = CreatePlatformSDL();
    ASSERT_TRUE(platform);
    ASSERT_TRUE(platform->initialize());

    WindowDesc desc;
    desc.width = 320; desc.height = 240; desc.title = "Vulkan Smoke";
    auto window = platform->createWindow(desc);
    ASSERT_TRUE(window);
    window->show();

    // Load Vulkan loader via SDL
    ASSERT_EQ(SDL_Vulkan_LoadLibrary(nullptr), 0) << SDL_GetError();
    auto vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
    ASSERT_NE(vkGetInstanceProcAddr, nullptr);

    // Build instance extensions
    std::vector<const char*> exts;
    ASSERT_TRUE(GetVulkanRequiredInstanceExtensions(window.get(), exts));

    VkApplicationInfo app{};
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "XiheTest";
    app.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app.pEngineName = "Xihe";
    app.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo = &app;
    ci.enabledExtensionCount = static_cast<uint32_t>(exts.size());
    ci.ppEnabledExtensionNames = exts.data();

    auto vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(vkGetInstanceProcAddr(nullptr, "vkCreateInstance"));
    ASSERT_NE(vkCreateInstance, nullptr);
    VkInstance instance = VK_NULL_HANDLE;
    ASSERT_EQ(vkCreateInstance(&ci, nullptr, &instance), VK_SUCCESS);
    ASSERT_NE(instance, VK_NULL_HANDLE);

    void* surfaceVoid = nullptr;
    ASSERT_TRUE(CreateVulkanSurfaceFromWindow(window.get(), reinterpret_cast<void*>(instance), &surfaceVoid));
    auto surface = reinterpret_cast<VkSurfaceKHR>(surfaceVoid);
    ASSERT_NE(surface, VK_NULL_HANDLE);

    auto vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(vkDestroySurfaceKHR, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);

    auto vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(vkGetInstanceProcAddr(nullptr, "vkDestroyInstance"));
    ASSERT_NE(vkDestroyInstance, nullptr);
    vkDestroyInstance(instance, nullptr);

    SDL_Vulkan_UnloadLibrary();
}
#endif


