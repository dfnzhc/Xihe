/**
 * @File Strings.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/6
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Defines.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cctype>

namespace xihe {
// trim
inline bool IsSpace(unsigned char ch) { return std::isspace(ch) != 0; }

inline std::string& TrimLeftInPlace(std::string& s)
{
    auto it = std::ranges::find_if_not(s, IsSpace);
    s.erase(s.begin(), it);
    return s;
}

inline std::string& TrimRightInPlace(std::string& s)
{
    auto it = std::ranges::find_if_not(s | std::views::reverse, IsSpace).base();
    s.erase(it, s.end());
    return s;
}

inline std::string& TrimInPlace(std::string& s)
{
    TrimRightInPlace(s);
    TrimLeftInPlace(s);
    return s;
}

inline std::string TrimLeft(std::string_view s)
{
    auto it = std::ranges::find_if_not(s, [](char c) { return IsSpace(static_cast<unsigned char>(c)); });
    return std::string(it, s.end());
}

inline std::string TrimRight(std::string_view s)
{
    auto it = std::ranges::find_if_not(s | std::views::reverse, [](char c) { return IsSpace(static_cast<unsigned char>(c)); }).base();
    return std::string(s.begin(), it);
}

inline std::string Trim(std::string_view s) { return TrimLeft(TrimRight(s)); }

// case
inline std::string& ToLowerInPlace(std::string& s)
{
    std::ranges::transform(s, s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

inline std::string ToLower(std::string_view s)
{
    std::string out(s);
    ToLowerInPlace(out);
    return out;
}

inline std::string& ToUpperInPlace(std::string& s)
{
    std::ranges::transform(s, s.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return s;
}

inline std::string ToUpper(std::string_view s)
{
    std::string out(s);
    ToUpperInPlace(out);
    return out;
}

// predicates
inline bool StartsWith(std::string_view s, std::string_view prefix) { return s.starts_with(prefix); }
inline bool EndsWith(std::string_view s, std::string_view suffix) { return s.ends_with(suffix); }
inline bool Contains(std::string_view s, std::string_view sub) { return s.find(sub) != std::string_view::npos; }

// split
inline std::vector<std::string_view> SplitView(std::string_view s, char delim, bool skipEmpty = true)
{
    std::vector<std::string_view> parts;
    Size start = 0;
    for (Size i = 0; i <= s.size(); ++i) {
        if (i == s.size() || s[i] == delim) {
            std::string_view token = s.substr(start, i - start);
            if (!skipEmpty || !token.empty())
                parts.push_back(token);
            start = i + 1;
        }
    }
    return parts;
}

inline std::vector<std::string> Split(std::string_view s, char delim, bool skipEmpty = true)
{
    std::vector<std::string> parts;
    Size start = 0;
    for (Size i = 0; i <= s.size(); ++i) {
        if (i == s.size() || s[i] == delim) {
            std::string_view token = s.substr(start, i - start);
            if (!skipEmpty || !token.empty())
                parts.emplace_back(token);
            start = i + 1;
        }
    }
    return parts;
}
} // namespace xihe