/**
 * @File EventBus.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/27
 * @Brief This file is part of Xihe.
 */

#include "EventBus.hpp"

#include <algorithm>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <thread>
#include <stop_token>
#include <atomic>
#include <queue>


using namespace xihe;


BatchEventProcessor::BatchEventProcessor(size_t batchSize, std::chrono::milliseconds timeout) :
    _batchSize(batchSize)
  , _timeout(timeout)
  , _lastFlush(Clock::now())
{
}

void BatchEventProcessor::setProcessor(std::function<void(const std::vector<EventPtr>&)> processor)
{
    _processor = std::move(processor);
}

void BatchEventProcessor::addEvent(EventPtr event)
{
    _batch.push_back(std::move(event));

    if (_batch.size() >= _batchSize ||
        Clock::now() - _lastFlush >= _timeout)
    {
        flush();
    }
}

void BatchEventProcessor::flush()
{
    if (_batch.empty())
        return;

    if (_processor)
        _processor(_batch);

    _batch.clear();
    _lastFlush = Clock::now();
}

size_t BatchEventProcessor::pendingCount() const
{
    return _batch.size();
}

