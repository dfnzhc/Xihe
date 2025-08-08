/**
 * @File EventBusTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/Events/Event.hpp>
#include <Core/Events/EventBus.hpp>

using namespace xihe;

static Event MakeKeyDown(KeyCode key, bool repeat = false)
{
    Event e;
    e.header.type = EventType::KeyDown;
    e.header.category = EventCategory::Input;
    e.payload = KeyDownEvent{key, repeat};
    return e;
}

TEST(EventBus, SubscribeAndEmitByType)
{
    EventBus bus;
    int count = 0;
    auto h = bus.subscribe(EventType::KeyDown,
                           [&](const Event& e) {
                               ASSERT_TRUE(e.is<KeyDownEvent>());
                               auto* kd = e.as<KeyDownEvent>();
                               ASSERT_NE(kd, nullptr);
                               EXPECT_EQ(kd->key, KeyCode::A);
                               ++count;
                           });

    bus.emit(MakeKeyDown(KeyCode::A));
    EXPECT_EQ(count, 1);

    bus.unsubscribe(h);
    bus.emit(MakeKeyDown(KeyCode::A));
    EXPECT_EQ(count, 1);
}

TEST(EventBus, SubscribeByCategory)
{
    EventBus bus;
    int count = 0;
    bus.subscribeCategory(EventCategory::Input,
                          [&](const Event& e) {
                              if (e.is<KeyDownEvent>())
                                  ++count;
                          });

    bus.emit(MakeKeyDown(KeyCode::B));
    EXPECT_EQ(count, 1);
}

TEST(EventBus, PostAndDispatch)
{
    EventBus bus;
    int count = 0;
    bus.subscribe(EventType::KeyDown, [&](const Event&) { ++count; });

    bus.post(MakeKeyDown(KeyCode::C));
    EXPECT_EQ(count, 0); // not delivered yet
    bus.dispatch();
    EXPECT_EQ(count, 1);
}