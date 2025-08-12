/**
 * @File PlatformFactory.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/12
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>

#include "Core/Base/Defines.hpp"
#include "Platform/Platform.hpp"

namespace xihe {

XIHE_API std::unique_ptr<IPlatform> CreatePlatformSDL();

} // namespace xihe


