/**
 * @File FpsCounter.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <deque>

#include "Core/Base/Concepts.hpp"
#include "Core/Base/Defines.hpp"

namespace xihe::time {
class FpsCounter
{
public:
    explicit FpsCounter(Size window = 120) : _window(window)
    {
        if (_window == 0)
            _window = 1;
    }

    void addFrame(f64 deltaSeconds)
    {
        _sum += deltaSeconds;
        _samples.push_back(deltaSeconds);
        if (_samples.size() > _window) {
            _sum -= _samples.front();
            _samples.pop_front();
        }
        // EMA
        if (!_emaInitialized) {
            _ema = deltaSeconds;
            _emaInitialized = true;
        }
        else { _ema = _alpha * deltaSeconds + (1.0 - _alpha) * _ema; }
    }

    XIHE_NODISCARD f64 instantaneous() const { return (_samples.empty() || _samples.back() == 0) ? 0.0 : (1.0 / _samples.back()); }
    XIHE_NODISCARD f64 average() const { return (_sum <= 1e-12) ? 0.0 : (As<f64>(_samples.size()) / _sum); }
    XIHE_NODISCARD f64 ema() const { return (_ema <= 1e-12) ? 0.0 : (1.0 / _ema); }

private:
    Size _window;
    std::deque<f64> _samples;
    f64 _sum{0.0};
    f64 _ema{0.0};
    bool _emaInitialized{false};
    f64 _alpha{0.2}; // EMA 平滑系数
};
} // namespace xihe::time