//
// Created by arlev on 11.11.2022.
//

#pragma once

#include "InputCodes.hpp"

namespace Mars
{
    enum class EventType
    {
        Handled,
        WindowSize,
        WindowClose,
        KeyPress,
        // KeyRelease,
        MouseMove,
        MouseButtonPress,
        MouseButtonRelease,
        Scrollwheel
    };

    struct WindowSizeEvent
    {
        WindowSizeEvent(int32_t w, int32_t h): width(w), height(h){}

        int32_t width, height;
    };

    struct WindowCloseEvent
    {
        WindowCloseEvent() = default;
    };

    struct KeyPressEvent
    {
        KeyPressEvent(KeyCode k, Modifier m): key(k), mod(m){}

        KeyCode key;
        Modifier mod;
    };

    struct MouseMoveEvent
    {
        MouseMoveEvent(float xPos, float yPos): x(xPos), y(yPos){}

        float x, y;
    };

    struct MouseButtonPressEvent
    {
        MouseButtonPressEvent(MouseButton b): button(b){}

        MouseButton button;
    };

    struct MouseButtonReleaseEvent
    {
        MouseButtonReleaseEvent(MouseButton b): button(b){}

        MouseButton button;
    };

    struct ScrollWheelEvent
    {
        ScrollWheelEvent(double xVal, double yVal): x(xVal), y(yVal){}

        float x, y;
    };

    struct Event
    {
        Event(WindowSizeEvent event)
        {
            type = EventType::WindowSize;
            windowSize = event;
        }

        Event(WindowCloseEvent event)
        {
            type = EventType::WindowClose;
            windowClose = event;
        }

        Event(KeyPressEvent event)
        {
            type = EventType::KeyPress;
            keyPress = event;
        }

        Event(MouseMoveEvent event)
        {
            type = EventType::MouseMove;
            mouseMove = event;
        }

        Event(MouseButtonPressEvent event)
        {
            type = EventType::MouseButtonPress;
            mousePress = event;
        }

        Event(MouseButtonReleaseEvent event)
        {
            type = EventType::MouseButtonRelease;
            mouseRelease = event;
        }

        Event(ScrollWheelEvent event)
        {
            type = EventType::Scrollwheel;
            scrollWheel = event;
        }

        EventType type;

        union
        {
            WindowSizeEvent windowSize;
            WindowCloseEvent windowClose;
            KeyPressEvent keyPress;
            MouseMoveEvent mouseMove;
            MouseButtonPressEvent mousePress;
            MouseButtonReleaseEvent mouseRelease;
            ScrollWheelEvent scrollWheel;
        };
    };
}