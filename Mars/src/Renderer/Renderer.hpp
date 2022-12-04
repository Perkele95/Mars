//
// Created by arlev on 02.12.2022.
//

#pragma once

#include "../Core/Base.hpp"
#include "../Core/Events.hpp"

namespace Mars
{
    namespace Renderer
    {
        void Init();
        void Shutdown();
        void OnEvent(Event &event);
        void BeginRender();
        void EndRender();
    };
}