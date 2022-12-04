//
// Created by arlev on 11.11.2022.
//

#pragma once

#include "Base.hpp"
#include <GLFW/glfw3.h>
#include "../Renderer/Renderer.hpp"

namespace Mars
{
    struct ApplicationSpecifications
    {
        std::string_view name;
        int32_t width, height;
    };

    class Application
    {
    public:
        explicit Application(const ApplicationSpecifications &specs);
        ~Application();

        void run();
        void onEvent(Event &event);

    private:
        GLFWwindow *window;
        bool running;
    };
}