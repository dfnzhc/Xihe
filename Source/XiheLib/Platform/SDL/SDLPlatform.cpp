/**
 * @File SDLPlatform.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/12
 * @Brief This file is part of Xihe.
 */

#include <memory>
#include <string>

#include "Core/Base/Defines.hpp"
#include "Platform/Platform.hpp"
#include "Platform/Input.hpp"
#include "Platform/Window.hpp"
#include "PlatformFactory.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

namespace xihe {
std::unique_ptr<IWindow> CreateSDLWindow(const WindowDesc& desc);

class SDLPlatform final : public IPlatform
{
public:
    ~SDLPlatform() override { shutdown(); }

    bool initialize() override
    {
        if (_initialized) { return true; }

        // Initialize at least VIDEO + EVENTS; input/audio can be enabled later on demand
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) { return false; }
        _initialized = true;
        return true;
    }

    void shutdown() override
    {
        if (!_initialized) { return; }
        _appDataPath.clear();
        SDL_Quit();
        _initialized = false;
    }

    std::unique_ptr<IWindow> createWindow(const WindowDesc& desc) override { return CreateSDLWindow(desc); }

    double timeSeconds() const override
    {
        // SDL_GetTicksNS is monotonic. Convert nanoseconds to seconds.
        const Uint64 ns = SDL_GetTicksNS();
        return static_cast<double>(ns) * 1e-9;
    }

    const char* appDataPath() const override
    {
        if (_appDataPath.empty()) {
            // TODO: 选择适当的文件路径
            const char* path = SDL_GetBasePath(); // SDL_GetPrefPath("dfnzhc", "Xihe");
            if (path) {
                _appDataPath = path;
            }
        }
        return _appDataPath.c_str();
    }

    const char* clipboardText() const override
    {
        return SDL_GetClipboardText();
    }

    bool setClipboardText(const char* text) override
    {
        return SDL_SetClipboardText(text) == 0;
    }

    bool showMessageBox(const char* title, const char* message) override
    {
        return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title ? title : "Message", message ? message : "", nullptr) == 0;
    }

    IInput* getInput() const override
    {
        extern IInput* CreateSDLInputSingleton();
        return CreateSDLInputSingleton();
    }

private:
    mutable std::string _appDataPath;
    bool _initialized{false};
};

std::unique_ptr<IPlatform> CreatePlatformSDL() { return std::make_unique<SDLPlatform>(); }
} // namespace xihe
