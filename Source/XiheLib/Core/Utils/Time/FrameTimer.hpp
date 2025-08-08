/**
 * @File FrameTimer.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Defines.hpp"
#include "Core/Utils/Time/Clock.hpp"

namespace xihe::time {
struct FrameTime
{
    u64 frameIndex{0};
    f64 deltaSeconds{0.0};
    f64 elapsedSeconds{0.0};
};

class FrameTimer
{
public:
    FrameTimer() = default;

    void setScale(f64 s) { _scale = std::max<f64>(0.0, s); }
    void setPaused(bool p) { _paused = p; }
    void setMaxDelta(f64 clampSec) { _maxDelta = std::max<f64>(0.0, clampSec); }

    XIHE_NODISCARD FrameTime tick()
    {
        const auto now = Clock::now();
        if (!_initialized) {
            _last = now;
            _initialized = true;
            return {}; // 首帧返回 0
        }

        f64 delta = std::chrono::duration_cast<Seconds>(now - _last).count();
        _last = now;

        if (_paused)
            delta = 0.0;
        delta *= _scale;
        if (_maxDelta > 0.0)
            delta = std::min(delta, _maxDelta);

        _elapsed += delta;
        return FrameTime{_frameIndex++, delta, _elapsed};
    }

private:
    bool _initialized{false};
    bool _paused{false};
    f64 _scale{1.0};
    f64 _maxDelta{0.0};
    f64 _elapsed{0.0};
    u64 _frameIndex{0};
    TimePoint _last{};
};
} // namespace xihe::time