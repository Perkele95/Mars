add_library(Mars STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/Renderer.cpp src/Renderer/Renderer.cpp
        )

set_target_properties(Mars PROPERTIES PREFIX "")
set_target_properties(Mars PROPERTIES OUTPUT_NAME "Mars")

target_include_directories(Mars PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Mars PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include)