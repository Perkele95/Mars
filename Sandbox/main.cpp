#include <Mars.hpp>

Mars::Application *Mars::CreateApplication()
{
    Mars::ApplicationSpecifications specs;
    specs.name = "Sandbox";
    specs.width = 1280;
    specs.height = 720;
    return new Application(specs);
};