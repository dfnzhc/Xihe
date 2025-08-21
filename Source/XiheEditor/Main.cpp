/**
 * @File Main.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include <ranges>

#include "Application.hpp"
#include "Core/Base/Error.hpp"
#include "Core/Base/Log.hpp"
#include "Core/Platform/Platform.hpp"
#include "Core/Context.hpp"
#include "Core/Events/EventBus.hpp"

using namespace xihe;


class EditorApp final : public Application
{
public:
    ~EditorApp() override = default;

protected:
    bool onInit() override
    {
        XIHE_INFO("编辑器启动中...");

        WindowDesc desc;
        desc.width     = 1280;
        desc.height    = 720;
        desc.resizable = true;
        desc.title     = "羲和编辑器";

        _window = platform()->createWindow(desc);
        if (!_window)
        {
            return false;
        }
        _window->show();

        return true;
    }

    void onTick() override
    {
        Event event;
        if (platform()->pollEvent(event))
        {
            if (event.header.type == EventType::WindowCloseRequested)
            {
                stop();
            }
            if (event.header.type == EventType::KeyDown && event.as<KeyDownEvent>()->key == KeyCode::Escape)
            {
                stop();
            }
        }

        Context::Get().events().dispatch();
    }

    void onShutdown() override
    {
        XIHE_INFO("编辑器正在退出");
        if (_window)
        {
            _window.reset();
            _window = nullptr;
        }
    }

private:
    std::unique_ptr<Window> _window = nullptr;
};

int RunEditorApp(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    EditorApp app;
    return app.run() ? 0 : 1;
}


int main(int argc, char** argv)
{
    return Guardian([&]
    {
        return RunEditorApp(argc, argv);
    });
}
