add_executable(Sandbox main.cpp)
target_link_libraries(Sandbox LINK_PUBLIC C:/Users/arlev/Mars/cmake-build-debug/Mars/Mars.a)
target_include_directories(Sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Mars/include)
target_include_directories(Sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Mars/vendor/glfw/include)

target_link_libraries(Sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Mars/vendor/glfw/lib-mingw-w64/libglfw3.a)