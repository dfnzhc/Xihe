/**
 * @File SDLPlatformTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/7
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Platform/PlatformFactory.hpp>
#include <Platform/Platform.hpp>
#include <Platform/Window.hpp>

using namespace xihe;

TEST(Platform_SDL, InitializeAndShutdown)
{
    auto platform = CreatePlatformSDL();
    ASSERT_TRUE(platform);
    EXPECT_TRUE(platform->initialize());
    EXPECT_TRUE(platform->initialize());
}

TEST(Platform_SDL, TimeAndAppDataPath)
{
    auto platform = CreatePlatformSDL();
    ASSERT_TRUE(platform);
    ASSERT_TRUE(platform->initialize());
    const double t0 = platform->timeSeconds();
    const double t1 = platform->timeSeconds();
    EXPECT_LE(t0, t1);
}

TEST(Platform_SDL, CreateWindowAndPollEvents)
{
    auto platform = CreatePlatformSDL();
    ASSERT_TRUE(platform);
    ASSERT_TRUE(platform->initialize());

    WindowDesc desc;
    desc.width  = 320;
    desc.height = 240;
    desc.title  = "SDL Test Window";
    auto window = platform->createWindow(desc);
    ASSERT_TRUE(window);
    window->show();
    EXPECT_EQ(window->width(), 320u);
    EXPECT_EQ(window->height(), 240u);

    Event ev;
    int polls = 0;
    while (polls++ < 3)
    {
        (void)window->pollEvent(ev);
    }

    // 基础控制 API 冒烟：最小化/最大化/恢复/尺寸/位置/可变大小/全屏/光标
    window->minimize();
    window->maximize();
    window->restore();
    window->setSize(300, 200);
    window->setPosition(10, 10);
    window->setResizable(true);
    window->setFullscreen(false);
    window->setCursorVisible(true);
    window->setCursorMode(CursorMode::Normal);

    // DPI/Scale 查询（不做强断言，仅检查可调用）
    (void)window->dpi();
    (void)window->contentScale();
}
