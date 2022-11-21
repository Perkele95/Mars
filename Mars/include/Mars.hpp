//
// Created by arlev on 13.11.2022.
//

#pragma once

#include "../src/Core/Application.hpp"

namespace Mars
{
    // To be defined in target source

    extern Application *CreateApplication();
}

// :: Entrypoint ::

int main()
{
    auto app = Mars::CreateApplication();
    app->run();
    delete app;
    return 0;
}