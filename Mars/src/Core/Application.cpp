//
// Created by arlev on 11.11.2022.
//

#include "Application.hpp"

namespace Mars
{
    Application::Application(const ApplicationSpecifications &specs): running(true)
    {
        if(!glfwInit())
        {
            std::cout << "Error, could not initialise GLFW!";
            running = false;
            return;
        }

        std::cout << "Created Application: {" << specs.name << "}" << std::endl;

        window = glfwCreateWindow(specs.width, specs.height, specs.name.data(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);

        glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height){
            auto data = static_cast<Application*>(glfwGetWindowUserPointer(window));
            auto event = Event(WindowSizeEvent(width, height));
            data->onEvent(event);
        });

        glfwSetWindowCloseCallback(window, [](GLFWwindow *window){
            auto data = static_cast<Application*>(glfwGetWindowUserPointer(window));
            auto event = Event(WindowCloseEvent());
            data->onEvent(event);
        });

        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods){
            auto data = static_cast<Application*>(glfwGetWindowUserPointer(window));

            switch(action)
            {
                case GLFW_PRESS:
                {
                    auto event = Event(KeyPressEvent(KeyCode(key), mods));
                    data->onEvent(event);
                }
                case GLFW_RELEASE:
                case GLFW_REPEAT:
                default:
                    break;
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y){
            auto data = static_cast<Application*>(glfwGetWindowUserPointer(window));
            auto event = Event(MouseMoveEvent(float(x), float(y)));
            data->onEvent(event);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods){
            auto data = static_cast<Application*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
                case GLFW_PRESS:
                {
                    auto event = Event(MouseButtonPressEvent(MouseButton(button)));
                    data->onEvent(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    auto event = Event(MouseButtonReleaseEvent(MouseButton(button)));
                    data->onEvent(event);
                    break;
                }
                default: break;
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow *window, double x, double y){
            auto data = static_cast<Application*>(glfwGetWindowUserPointer(window));
            auto event = Event(ScrollWheelEvent(float(x), float(y)));
            data->onEvent(event);
        });
    }

    Application::~Application()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Application::run()
    {
        while(running)
        {
            glfwPollEvents();
            const float timestep = glfwGetTime();

            // TODO: Update UI
            // TODO: Begin and end render
        }
    }

    void Application::onEvent(Event &event)
    {
        switch (event.type)
        {
            case EventType::WindowClose:
            {
                running = false;
                return;
            }

            default: break;
        }
    }
}