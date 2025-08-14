/**
 * @File Main.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include "Application.hpp"
#include "Core/Base/Error.hpp"
#include "Core/Base/Log.hpp"
#include "Platform/Window.hpp"
#include "Platform/Input.hpp"

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
        desc.width = 1280;
        desc.height = 720;
        desc.resizable = true;
        desc.title = "羲和编辑器";

        _window = platform()->createWindow(desc);
        if (!_window) { return false; }
        _window->show();
        return true;
    }

    void onTick() override
    {
        // 帧开始：推进输入快照
        if (auto* input = platform()->getInput()) { input->newFrame(); }

        // 简单事件循环：遇到关闭事件则退出
        Event e;
        while (_window && _window->pollEvent(e)) { if (e.type == EventType::Close) { stop(); } }

        // 键盘：按下 ESC 退出
        if (auto* input = platform()->getInput()) { if (input->wasKeyPressed(KeyCode::Escape)) { stop(); } }
    }

    void onShutdown() override
    {
        XIHE_INFO("编辑器正在退出");
        _window.reset();
    }

private:
    std::unique_ptr<IWindow> _window;
};

int RunEditorApp(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    EditorApp app;
    return app.run() ? 0 : 1;
}


int main(int argc, char** argv) { return Guardian([&] { return RunEditorApp(argc, argv); }); }
