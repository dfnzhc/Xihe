/**
 * @File ConfigManager.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/16
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <optional>

#include "Core/Base/Defines.hpp"

namespace xihe {
class XIHE_API ConfigManager
{
public:
    ConfigManager() = default;
    ~ConfigManager() = default;

    // 禁用拷贝和移动
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) = delete;
    ConfigManager& operator=(ConfigManager&&) = delete;

    // 配置文件默认在可执行目录下 
    bool loadFromFile(const std::filesystem::path& configPath = "./XiheConfig.toml");
    bool loadFromString(std::string_view configContent);
    bool saveToFile(const std::filesystem::path& configPath) const;

    std::string getWindowTitle() const;
    u32 getWindowWidth() const;
    u32 getWindowHeight() const;

    void setWindowTitle(const std::string& title);
    void setWindowWidth(u32 width);
    void setWindowHeight(u32 height);

    bool isLoaded() const { return _bIsLoaded; }

    void reset();

private:
    void setDefaults();
    bool parseToml(std::string_view content);

private:
    bool _bIsLoaded = false;

    // 配置项
    std::string _windowTitle = "Xihe Engine";
    std::filesystem::path _resourceDirectory = "./Resources";
    u32 _windowWidth = 1280;
    u32 _windowHeight = 720;
};
} // namespace xihe
