/**
 * @File Portable.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

// -------------------------
// Platform
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#  include <Windows.h>
#  include <intrin.h>
#  define XIHE_ON_WINDOWS
#elif defined(__unix__) || defined(__unix) || defined(__linux__)
#  define XIHE_ON_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
#  define XIHE_ON_MAC
#else
#  error "Unsupport Platform."
#endif

#if XIHE_ON_LINUX || XIHE_ON_MAC
#warning "Xihe is currently developed on Windows platform."
#endif

// -------------------------
// Compiler
#if defined(_MSC_VER) && !defined(__clang__)
#  define XIHE_COMPILER_MSVC 1
#elif defined(__clang__)
#  define XIHE_COMPILER_CLANG 1
#elif defined(__GNUC__) && !defined(__clang__)
#  define XIHE_COMPILER_GCC 1
#else
#  error "Unknown Compiler."
#endif

// -------------------------
// Cpp version
#if XIHE_COMPILER_MSVC
#  define XIHE_CPLUSPLUS _MSVC_LANG
#else
#  define XIHE_CPLUSPLUS __cplusplus
#endif

static_assert(XIHE_CPLUSPLUS >= 202'302L, "Xihe using modern C++ features.");

// -------------------------
// noinline
#if XIHE_COMPILER_MSVC
#  define XIHE_NOINLINE __declspec(noinline)
#elif XIHE_COMPILER_CLANG
#  define XIHE_NOINLINE __attribute__((__noinline__))
#else
#  define XIHE_NOINLINE
#endif

// -------------------------
// always inline
#if XIHE_COMPILER_MSVC
#  define XIHE_ALWAYS_INLINE __forceinline
#elif XIHE_COMPILER_CLANG
#  define XIHE_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#  define XIHE_ALWAYS_INLINE inline
#endif

// -------------------------
// force inline
#if XIHE_COMPILER_MSVC
#  define XIHE_FORCE_INLINE __forceinline
#elif XIHE_COMPILER_GCC || XIHE_COMPILER_CLANG
#  define XIHE_FORCE_INLINE __attribute__((always_inline)) inline
#else
#  define XIHE_FORCE_INLINE inline
#endif

// -------------------------
// no inline
#if XIHE_COMPILER_MSVC
#  define XIHE_NO_INLINE __declspec(noinline)
#elif XIHE_COMPILER_GCC || XIHE_COMPILER_CLANG
#  define XIHE_NO_INLINE __attribute__((noinline))
#else
#  define XIHE_NO_INLINE
#endif

// -------------------------
// Generalize warning push/pop.
#if XIHE_COMPILER_CLANG || XIHE_COMPILER_GCC
// Clang & GCC
#  define XIHE_PUSH_WARNING                               _Pragma("GCC diagnostic push")
#  define XIHE_POP_WARNING                                _Pragma("GCC diagnostic pop")
#  define XIHE_GNU_DISABLE_WARNING_INTERNAL2(warningName) #warningName
#  define XIHE_GNU_DISABLE_WARNING(warningName)           _Pragma(XIHE_GNU_DISABLE_WARNING_INTERNAL2(GCC diagnostic ignored warningName))
#  ifdef __clang__
#    define XIHE_CLANG_DISABLE_WARNING(warningName) XIHE_GNU_DISABLE_WARNING(warningName)
#    define XIHE_GCC_DISABLE_WARNING(warningName)
#  else
#    define XIHE_CLANG_DISABLE_WARNING(warningName)
#    define XIHE_GCC_DISABLE_WARNING(warningName) XIHE_GNU_DISABLE_WARNING(warningName)
#  endif
#  define XIHE_MSVC_DISABLE_WARNING(warningNumber)
#elif XIHE_COMPILER_MSVC
#  define XIHE_PUSH_WARNING __pragma(warning(push))
#  define XIHE_POP_WARNING  __pragma(warning(pop))
// Disable the GCC warnings.
#  define XIHE_GNU_DISABLE_WARNING(warningName)
#  define XIHE_GCC_DISABLE_WARNING(warningName)
#  define XIHE_CLANG_DISABLE_WARNING(warningName)
#  define XIHE_MSVC_DISABLE_WARNING(warningNumber) __pragma(warning(disable : warningNumber))
#else
#  define XIHE_PUSH_WARNING
#  define XIHE_POP_WARNING
#  define XIHE_GNU_DISABLE_WARNING(warningName)
#  define XIHE_GCC_DISABLE_WARNING(warningName)
#  define XIHE_CLANG_DISABLE_WARNING(warningName)
#  define XIHE_MSVC_DISABLE_WARNING(warningNumber)
#endif

// -------------------------
// has builtin
#ifdef __has_builtin
#  define XIHE_HAS_BUILTIN(x) __has_builtin(x)
#else
#  define XIHE_HAS_BUILTIN(x) 0
#endif

// -------------------------
// likely & unlikely
#if XIHE_COMPILER_GCC || XIHE_COMPILER_CLANG
#  define XIHE_LIKELY(x)   __builtin_expect(!!(x), 1)
#  define XIHE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#  define XIHE_LIKELY(x)   (x)
#  define XIHE_UNLIKELY(x) (x)
#endif

// -------------------------
// assume
#if XIHE_COMPILER_MSVC
#  define XIHE_ASSUME(condition) __assume(condition)

#elif XIHE_COMPILER_CLANG
#  if XIHE_HAS_BUILTIN(__builtin_assume)
#    define XIHE_ASSUME(condition) __builtin_assume(condition)
#  else
#    define XIHE_ASSUME(condition)       \
        do {                             \
            if (!(condition))            \
                __builtin_unreachable(); \
        } while (0)
#  endif

#elif XIHE_COMPILER_GCC
#  define XIHE_ASSUME(condition)           \
      do {                                 \
          if (XIHE_UNLIKELY(!(condition))) \
              __builtin_unreachable();     \
      } while (0)
#else
#  define XIHE_ASSUME(condition) ((void)0)
#endif

// -------------------------
// alignas
#if XIHE_COMPILER_MSVC
#  define XIHE_ALIGNAS(N) __declspec(align(N))
#elif XIHE_COMPILER_GCC || XIHE_COMPILER_CLANG
#  define XIHE_ALIGNAS(N) __attribute__((aligned(N)))
#else
#  define XIHE_ALIGNAS(N) alignas(N)
#endif

// -------------------------
// debug
#if _DEBUG
#define XIHE_DEBUG 1
#endif

// -------------------------
// debug break
#if XIHE_ENABLE_DEBUG

#  if defined(XIHE_ON_WINDOWS) && defined(XIHE_COMPILER_MSVC)
#    define XIHE_DEBUG_BREAK() __debugbreak()
#  elif XIHE_COMPILER_GCC
#    if XIHE_HAS_BUILTIN(__builtin_trap)
#      define XIHE_DEBUG_BREAK() __builtin_trap()
#    else
#      define XIHE_DEBUG_BREAK() __asm__ volatile("int $0x03")
#    endif

#  elif XIHE_COMPILER_CLANG
#    if XIHE_HAS_BUILTIN(__builtin_debugtrap)
#      define XIHE_DEBUG_BREAK() __builtin_debugtrap()
#    else
#      define XIHE_DEBUG_BREAK() __asm__ volatile("int $0x03")
#    endif

#  else
#    include <cassert> // Required for assert
#    define XIHE_DEBUG_BREAK() assert(false && "XIHE_DEBUG_BREAK: Reached generic breakpoint")
#  endif

#else
#  define XIHE_DEBUG_BREAK() ((void)0) // No-op in release or CUDA device code by default
#endif

// -------------------------
// restrict
#if XIHE_COMPILER_MSVC
#  define XIHE_RESTRICT __restrict
#elif XIHE_COMPILER_GCC || XIHE_COMPILER_CLANG
#  define XIHE_RESTRICT __restrict__
#else
#  define XIHE_RESTRICT
#endif

// -------------------------
// attributes
#define XIHE_NO_UNIQUE_ADDRESS [[no_unique_address]]
#define XIHE_UNUSED            [[maybe_unused]]
#define XIHE_NODISCARD         [[nodiscard]]
#define XIHE_NORETURN          [[noreturn]]
#define XIHE_DEPRECATED(...)   [[deprecated(__VA_ARGS__)]]
#define XIHE_FALLTHROUGH(...)  [[fallthrough]]