/**
 * @File TimeStepper.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Defines.hpp"
#include "Core/Math/Common/Basic.hpp"

namespace xihe::time {
class TimeStepper
{
public:
    explicit TimeStepper(f64 fixedDt, int maxCatchUp = 5) : _fixedDt(Max(1e-6, fixedDt)), _maxCatchUp(Max(1, maxCatchUp)) {}

    template<typename Callback>
    void update(f64 realDelta, Callback&& onFixedUpdate)
    {
        if (_paused)
            return;
        _accumulator += Max(0.0, realDelta * _scale);
        int steps = 0;
        while (_accumulator + 1e-12 >= _fixedDt && steps < _maxCatchUp) {
            onFixedUpdate(_fixedDt);
            _simulation += _fixedDt;
            _accumulator -= _fixedDt;
            ++steps;
        }
    }

    void setScale(f64 s) { _scale = Max(0.0, s); }
    void setPaused(bool p) { _paused = p; }
    void stepOnce() { _stepOnce = true; }

    XIHE_NODISCARD f64 simulationTime() const { return _simulation; }
    XIHE_NODISCARD f64 leftover() const { return _accumulator; }

private:
    f64 _fixedDt{1.0 / 60.0};
    int _maxCatchUp{5};
    f64 _accumulator{0.0};
    f64 _simulation{0.0};
    f64 _scale{1.0};
    bool _paused{false};
    bool _stepOnce{false};
};
} // namespace xihe::time