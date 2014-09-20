#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>

int main() try
{
    if(!glfwInit()) throw std::runtime_error("glfwInit() failed.");
    auto window = glfwCreateWindow(1280, 720, "Development Sandbox", nullptr, nullptr);
    if(!window) throw std::runtime_error("glfwCreateWindow(...) failed.");

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glfwMakeContextCurrent(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
catch(const std::exception & e)
{
    std::cerr << "Unhandled exception: " << e.what() << std::endl;
    return -1;
}