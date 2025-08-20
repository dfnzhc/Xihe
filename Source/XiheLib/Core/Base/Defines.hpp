/**
 * @File Defines.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Portable.hpp"

// -------------------------
// dynamic symbols export/import
#if defined(_MSC_VER)
#  define XIHE_API_EXPORT __declspec(dllexport)
#  define XIHE_API_IMPORT __declspec(dllimport)
#elif defined(__clang__) || defined(__GNUC__)
#  define XIHE_API_EXPORT __attribute__((visibility("default")))
#  define XIHE_API_IMPORT __attribute__((visibility("default")))
#else
#  define XIHE_API_EXPORT
#  define XIHE_API_IMPORT
#endif

#ifdef XIHE_DLL
#  define XIHE_API XIHE_API_EXPORT
#else
#  define XIHE_API XIHE_API_IMPORT
#endif // XIHE_DLL

#include <cstdint>
#include <cstddef>

// -------------------------
// Invalid Some Defines
#ifdef max
#  undef max
#endif
#ifdef min
#  undef min
#endif
#ifdef isnan
#  undef isnan
#endif
#ifdef isinf
#  undef isinf
#endif
#ifdef log2
#  undef log2
#endif

#include <limits>
#include <type_traits>

namespace xihe {
template <typename T>
using numeric_limits = std::numeric_limits<T>;

// -------------------------
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8   = uint8_t;
using u16  = uint16_t;
using u32  = uint32_t;
using u64  = uint64_t;
using uint = unsigned int;

using f32 = float;
using f64 = double;

using Size = std::size_t;
} // namespace xihe
