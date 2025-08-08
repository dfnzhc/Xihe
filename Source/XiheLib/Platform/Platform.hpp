/**
 * @File Platform.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */
 
#pragma once

#include <memory>
#include <string_view>

#include "Core/Base/Defines.hpp"
#include "Platform/Window.hpp"

namespace xihe {

class XIHE_API IPlatform
{
public:
    virtual ~IPlatform() = default;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    virtual std::unique_ptr<IWindow> createWindow(const WindowDesc& desc) = 0;

    // 基础设施
    virtual double timeSeconds() const = 0;
    virtual const char* appDataPath() const = 0;
};

} // namespace xihe