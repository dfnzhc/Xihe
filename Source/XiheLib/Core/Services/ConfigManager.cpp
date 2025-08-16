/**
 * @File ConfigManager.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/16
 * @Brief This file is part of Xihe.
 */

#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

#include "Core/Base/Portable.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wunsafe-buffer-usage")
XIHE_CLANG_DISABLE_WARNING("-Wswitch-enum")
XIHE_CLANG_DISABLE_WARNING("-Wcovered-switch-default")
#include <toml.hpp>
XIHE_POP_WARNING

using namespace xihe;

bool ConfigManager::loadFromFile(const std::filesystem::path& configPath)
{
    try {
        if (!std::filesystem::exists(configPath)) {
            // 如果文件不存在，创建默认配置文件
            setDefaults();
            auto _ = saveToFile(configPath);
            _bIsLoaded = true;
            return true;
        }

        std::ifstream file(configPath);
        if (!file.is_open()) { return false; }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        return loadFromString(buffer.str());
    } catch (...) { return false; }
}

bool ConfigManager::loadFromString(std::string_view configContent)
{
    try {
        setDefaults();
        bool result = parseToml(configContent);
        _bIsLoaded = result;
        return result;
    } catch (...) { return false; }
}

bool ConfigManager::saveToFile(const std::filesystem::path& configPath) const
{
    try {
        // 确保目录存在
        std::filesystem::create_directories(configPath.parent_path());

        toml::table config;

        // 窗口配置
        toml::table window;
        window.insert("title", _windowTitle);
        window.insert("width", static_cast<i64>(_windowWidth));
        window.insert("height", static_cast<i64>(_windowHeight));
        config.insert("window", std::move(window));

        // 资源配置
        toml::table resources;
        resources.insert("directory", _resourceDirectory.string());
        config.insert("resources", std::move(resources));

        std::ofstream file(configPath);
        if (!file.is_open()) { return false; }

        file << config;
        file.close();
        return true;
    } catch (...) { return false; }
}

std::string ConfigManager::getWindowTitle() const { return _windowTitle; }

void ConfigManager::setWindowTitle(const std::string& title) { _windowTitle = title; }

u32 ConfigManager::getWindowWidth() const { return _windowWidth; }

void ConfigManager::setWindowWidth(u32 width) { _windowWidth = width; }

u32 ConfigManager::getWindowHeight() const { return _windowHeight; }

void ConfigManager::setWindowHeight(u32 height) { _windowHeight = height; }

void ConfigManager::reset()
{
    setDefaults();
    _bIsLoaded = false;
}

void ConfigManager::setDefaults()
{
    _windowTitle = "Xihe Engine";
    _resourceDirectory = "./Resources";
    _windowWidth = 1280;
    _windowHeight = 720;
}

bool ConfigManager::parseToml(std::string_view content)
{
    try {
        toml::table config = toml::parse(content);

        // 解析窗口配置
        if (auto window = config["window"].as_table()) {
            if (auto title = (*window)["title"].as_string()) { _windowTitle = title->get(); }
            if (auto width = (*window)["width"].as_integer()) { _windowWidth = static_cast<u32>(width->get()); }
            if (auto height = (*window)["height"].as_integer()) { _windowHeight = static_cast<u32>(height->get()); }
        }

        // 解析资源配置
        if (auto resources = config["resources"].as_table()) {
            if (auto directory = (*resources)["directory"].as_string()) { _resourceDirectory = directory->get(); }
        }

        return true;
    } catch (const toml::parse_error&) { return false; }
    catch (...) { return false; }
}
