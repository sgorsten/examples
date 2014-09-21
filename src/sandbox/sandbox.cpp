#include "window.h"

#include <iostream>

int main() try
{
    Window window({1280,720}, "Development Sandbox");

    while(!window.WindowShouldClose())
    {
        glfwPollEvents();

        window.MakeContextCurrent();

        auto size = window.GetFramebufferSize();
        glViewport(0, 0, size.x, size.y);
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glOrtho(0, size.x, size.y, 0, -1, +1);
        window.Print({32,32}, "This is a test of basic font rendering, using an %d x %d monospaced font.", 8, 14);
        glPopMatrix();

        window.SwapBuffers();
    }

    return 0;
}
catch(const std::exception & e)
{
    std::cerr << "Unhandled exception: " << e.what() << std::endl;
    return -1;
}