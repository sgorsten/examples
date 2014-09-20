#pragma once

#include <GLFW\glfw3.h>

class Font
{
    GLuint texture;
public:
    Font();

    void Print(int x, int y, const char * format, ...) const;
};