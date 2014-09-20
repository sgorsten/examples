#include "font.h"

#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>

int main() try
{
    if(!glfwInit()) throw std::runtime_error("glfwInit() failed.");
    auto window = glfwCreateWindow(1280, 720, "Development Sandbox", nullptr, nullptr);
    if(!window) throw std::runtime_error("glfwCreateWindow(...) failed.");

    glfwMakeContextCurrent(window);
    
    Font font;

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glOrtho(0, width, height, 0, -1, +1);
        font.Print(32, 32, "This is a test of basic font rendering, using an %d x %d monospaced font.", 8, 14);
        glPopMatrix();

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