/**
 * @File TimeTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/Utils/Time/Clock.hpp>
#include <Core/Utils/Time/Stopwatch.hpp>
#include <Core/Utils/Time/FrameTimer.hpp>
#include <Core/Utils/Time/FpsCounter.hpp>
#include <Core/Utils/Time/TimeStepper.hpp>
#include <Core/Utils/Time/TimerQueue.hpp>

using namespace xihe;

TEST(StopwatchTest, BasicStartStop)
{
    Stopwatch sw;
    sw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sw.stop();
    double e = sw.elapsedSeconds();
    EXPECT_GT(e, 0.0);
}

TEST(FrameTimerTest, TickIncreases)
{
    FrameTimer ft;
    auto t0 = ft.tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto t1 = ft.tick();
    EXPECT_GE(t1.deltaSeconds, 0.0);
}

TEST(FpsCounterTest, BasicAverages)
{
    FpsCounter fps(10);
    for (int i = 0; i < 10; ++i)
        fps.addFrame(0.01); // 100 FPS
    EXPECT_NEAR(fps.average(), 100.0, 5.0);
    EXPECT_GT(fps.instantaneous(), 0.0);
}

TEST(TimeStepperTest, FixedSteps)
{
    TimeStepper stepper(0.01, 10);
    int count = 0;
    stepper.update(0.05,
                   [&](double dt)
                   {
                       count++;
                       EXPECT_DOUBLE_EQ(dt, 0.01);
                   });
    EXPECT_EQ(count, 5);
}

TEST(TimerQueueTest, OnceAndEvery)
{
    TimerQueue q;
    std::atomic<int> sum{0};
    auto now = Clock::now();
    q.scheduleOnce(now + std::chrono::milliseconds(10), [&]
    {
        sum.fetch_add(1);
    });
    auto h = q.scheduleEvery(Seconds{0.01}, [&]
    {
        sum.fetch_add(2);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    q.cancel(h);
    auto before = sum.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    auto after = sum.load();
    EXPECT_GE(before, 3);
    EXPECT_EQ(before, after); // 取消后不再增长
}
