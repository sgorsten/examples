#pragma once

#include "linalg.h"
#include <GLFW\glfw3.h>

class Window
{
    GLFWwindow * window = 0;
    GLuint fontTexture = 0;

    Window() {}
public:
    Window(const int2 & dimensions, const char * title);
    ~Window();

    bool WindowShouldClose() const { return !!glfwWindowShouldClose(window); }
    int2 GetFramebufferSize() const { int2 r; glfwGetFramebufferSize(window, &r.x, &r.y); return r; }
    double2 GetCursorPos() const { double2 r; glfwGetCursorPos(window, &r.x, &r.y); return r; }
    bool GetMouseButton(int button) const { return glfwGetMouseButton(window, button) == GLFW_PRESS; }

    void MakeContextCurrent() { glfwMakeContextCurrent(window); }
    void Print(const int2 & coord, const char * format, ...);
    void SwapBuffers() { glfwSwapBuffers(window); }
};