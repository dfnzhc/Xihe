/**
 * @File ConfigManagerTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/16
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include <Core/Utils/ConfigManager.hpp>

using namespace xihe;

class ConfigManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 创建临时测试目录
        testDir = std::filesystem::temp_directory_path() / "xihe_config_test";
        std::filesystem::create_directories(testDir);
        testConfigPath = testDir / "test_config.toml";
    }

    void TearDown() override
    {
        // 清理测试文件
        if (std::filesystem::exists(testDir))
        {
            std::filesystem::remove_all(testDir);
        }
    }

    std::filesystem::path testDir;
    std::filesystem::path testConfigPath;
};

TEST_F(ConfigManagerTest, DefaultValues)
{
    ConfigManager config;

    EXPECT_EQ(config.getWindowTitle(), "Xihe Engine");
    EXPECT_EQ(config.getWindowWidth(), 1280u);
    EXPECT_EQ(config.getWindowHeight(), 720u);
    EXPECT_EQ(config.getResourceDirectory(), "./Resources");
    EXPECT_FALSE(config.isLoaded());
}

TEST_F(ConfigManagerTest, SettersAndGetters)
{
    ConfigManager config;

    config.setWindowTitle("Test Window");
    config.setWindowWidth(1920);
    config.setWindowHeight(1080);
    config.setResourceDirectory("./TestResources");

    EXPECT_EQ(config.getWindowTitle(), "Test Window");
    EXPECT_EQ(config.getWindowWidth(), 1920u);
    EXPECT_EQ(config.getWindowHeight(), 1080u);
    EXPECT_EQ(config.getResourceDirectory(), "./TestResources");
}

TEST_F(ConfigManagerTest, LoadFromString)
{
    ConfigManager config;

    std::string tomlContent = R"(
[window]
title = "Test Application"
width = 1600
height = 900

[resources]
directory = "./Assets"
)";

    EXPECT_TRUE(config.loadFromString(tomlContent));
    EXPECT_TRUE(config.isLoaded());

    EXPECT_EQ(config.getWindowTitle(), "Test Application");
    EXPECT_EQ(config.getWindowWidth(), 1600u);
    EXPECT_EQ(config.getWindowHeight(), 900u);
    EXPECT_EQ(config.getResourceDirectory(), "./Assets");
}

TEST_F(ConfigManagerTest, SaveAndLoadFromFile)
{
    ConfigManager config1;

    // 设置一些值
    config1.setWindowTitle("Saved Config Test");
    config1.setWindowWidth(2560);
    config1.setWindowHeight(1440);
    config1.setResourceDirectory("./SavedResources");

    // 保存到文件
    EXPECT_TRUE(config1.saveToFile(testConfigPath));
    EXPECT_TRUE(std::filesystem::exists(testConfigPath));

    // 创建新的配置管理器并加载
    ConfigManager config2;
    EXPECT_TRUE(config2.loadFromFile(testConfigPath));
    EXPECT_TRUE(config2.isLoaded());

    // 验证加载的值
    EXPECT_EQ(config2.getWindowTitle(), "Saved Config Test");
    EXPECT_EQ(config2.getWindowWidth(), 2560u);
    EXPECT_EQ(config2.getWindowHeight(), 1440u);
    EXPECT_EQ(config2.getResourceDirectory(), "./SavedResources");
}

TEST_F(ConfigManagerTest, LoadNonExistentFile)
{
    ConfigManager config;

    // 加载不存在的文件应该创建默认配置
    EXPECT_TRUE(config.loadFromFile(testConfigPath));
    EXPECT_TRUE(config.isLoaded());
    EXPECT_TRUE(std::filesystem::exists(testConfigPath));

    // 验证默认值
    EXPECT_EQ(config.getWindowTitle(), "Xihe Engine");
    EXPECT_EQ(config.getWindowWidth(), 1280u);
    EXPECT_EQ(config.getWindowHeight(), 720u);
    EXPECT_EQ(config.getResourceDirectory(), "./Resources");
}

TEST_F(ConfigManagerTest, InvalidTomlContent)
{
    ConfigManager config;

    std::string invalidToml = "invalid toml content [[[";

    EXPECT_FALSE(config.loadFromString(invalidToml));
    EXPECT_FALSE(config.isLoaded());

    // 应该保持默认值
    EXPECT_EQ(config.getWindowTitle(), "Xihe Engine");
}

TEST_F(ConfigManagerTest, PartialTomlContent)
{
    ConfigManager config;

    // 只包含部分配置的TOML
    std::string partialToml = R"(
[window]
title = "Partial Config"
)";

    EXPECT_TRUE(config.loadFromString(partialToml));
    EXPECT_TRUE(config.isLoaded());

    // 指定的值应该被更新
    EXPECT_EQ(config.getWindowTitle(), "Partial Config");
    // 未指定的值应该保持默认
    EXPECT_EQ(config.getWindowWidth(), 1280u);
    EXPECT_EQ(config.getWindowHeight(), 720u);
    EXPECT_EQ(config.getResourceDirectory(), "./Resources");
}

TEST_F(ConfigManagerTest, Reset)
{
    ConfigManager config;

    // 设置一些非默认值
    config.setWindowTitle("Modified Title");
    config.setWindowWidth(1920);

    // 重置
    config.reset();

    // 验证恢复到默认值
    EXPECT_EQ(config.getWindowTitle(), "Xihe Engine");
    EXPECT_EQ(config.getWindowWidth(), 1280u);
    EXPECT_FALSE(config.isLoaded());
}
