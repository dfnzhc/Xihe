/**
 * @File SDLPlatformTest.cpp
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
    const char* path = platform->appDataPath();
    ASSERT_NE(path, nullptr);
    EXPECT_NE(std::string(path).size(), 0u);
}

TEST(Platform_SDL, CreateWindowAndPollEvents)
{
    auto platform = CreatePlatformSDL();
    ASSERT_TRUE(platform);
    ASSERT_TRUE(platform->initialize());

    WindowDesc desc;
    desc.width = 320;
    desc.height = 240;
    desc.title = "SDL Test Window";
    auto window = platform->createWindow(desc);
    ASSERT_TRUE(window);
    window->show();
    EXPECT_EQ(window->width(), 320u);
    EXPECT_EQ(window->height(), 240u);

    Event ev;
    int polls = 0;
    while (polls++ < 3) {
        (void)window->pollEvent(ev);
    }
}