/**
 * @File Window.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/22
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <string_view>
#include "Core/Base/Defines.hpp"

namespace xihe {
struct WindowDesc
{
    u32 width{1280};
    u32 height{720};
    bool resizable{true};
    std::string_view title{"Xihe"};
};

class XIHE_API Window
{
public:
    virtual ~Window() = default;
};
} // namespace xihe
