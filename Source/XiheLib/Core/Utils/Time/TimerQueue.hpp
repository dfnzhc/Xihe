/**
 * @File TimerQueue.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <unordered_set>

#include "Core/Base/Defines.hpp"
#include "Core/Utils/Time/Clock.hpp"
#include "Core/Base/Error.hpp"

namespace xihe {
struct TimerHandle
{
    u64 id{0};
};

class TimerQueue
{
public:
    using Callback = std::function<void()>;

    TimerQueue() : _running(true), _thread([this] { run(); }) {}

    ~TimerQueue()
    {
        {
            std::lock_guard<std::mutex> lk(_mtx);
            _running.store(false, std::memory_order_release);
        }
        _cv.notify_one();
        if (_thread.joinable())
            _thread.join();
    }

    TimerHandle scheduleOnce(TimePoint when, Callback cb) { return scheduleImpl(when, Seconds{0}, std::move(cb)); }

    TimerHandle scheduleEvery(Seconds interval, Callback cb, TimePoint startAt = Clock::now())
    {
        XIHE_CHECK(interval.count() > 0.0, "scheduleEvery: interval must be positive");
        return scheduleImpl(startAt, interval, std::move(cb));
    }

    bool cancel(const TimerHandle& h)
    {
        std::lock_guard<std::mutex> lk(_mtx);
        return _cancelled.insert(h.id).second; // 标记取消，实际剔除在消费端惰性执行
    }

private:
    struct Item
    {
        TimePoint deadline{};
        Seconds interval{0}; // 0 表示一次性
        u64 id{0};
        Callback cb{};
        bool operator>(const Item& other) const { return deadline > other.deadline; }
    };

    TimerHandle scheduleImpl(TimePoint when, Seconds interval, Callback cb)
    {
        std::lock_guard<std::mutex> lk(_mtx);
        const u64 id = ++_nextId;
        _queue.push(Item{when, interval, id, std::move(cb)});
        _cv.notify_one();
        return TimerHandle{id};
    }

    void run()
    {
        std::unique_lock<std::mutex> lk(_mtx);
        while (_running.load(std::memory_order_acquire)) {
            if (_queue.empty()) {
                _cv.wait(lk, [&] { return !_running.load(std::memory_order_acquire) || !_queue.empty(); });
                if (!_running.load(std::memory_order_acquire))
                    break;
            }

            auto item = _queue.top();
            const auto now = Clock::now();
            if (item.deadline > now) {
                _cv.wait_until(lk, item.deadline);
                continue;
            }

            _queue.pop();

            if (_cancelled.contains(item.id)) {
                _cancelled.erase(item.id);
                continue;
            }

            // 执行回调时释放锁，避免阻塞注册/取消与其他定时触发
            lk.unlock();

            try { item.cb(); } catch (const std::exception& e) { std::cerr << "TimerQueue callback exception: " << e.what() << std::endl; }
            catch (...) { std::cerr << "TimerQueue callback unknown exception" << std::endl; }
            lk.lock();

            // 周期任务：基于理论下一次时间推进，减少漂移
            if (item.interval.count() > 0.0) {
                item.deadline += std::chrono::duration_cast<std::chrono::steady_clock::duration>(item.interval);
                if (!_cancelled.contains(item.id)) { _queue.push(item); }
                else { _cancelled.erase(item.id); }
            }
        }
    }

    std::priority_queue<Item, std::vector<Item>, std::greater<>> _queue;
    std::mutex _mtx;
    std::condition_variable _cv;
    std::atomic<bool> _running{false};
    std::thread _thread;
    u64 _nextId{0};
    std::unordered_set<u64> _cancelled;
};
} // namespace xihe