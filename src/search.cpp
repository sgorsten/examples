#include "font.h"

#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <vector>

class Map
{
    std::vector<int> tiles;
    int width, height;
public:
    Map(int width, int height) : tiles(width * height, 0), width(width), height(height) {}

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    bool IsObstruction(int x, int y) const { return tiles[y*width+x] != 0; }
    void SetObstruction(int x, int y, bool isObstruction) { tiles[y*width+x] = isObstruction ? 1 : 0; }
};

#include <iostream>

int main() try
{
    if(!glfwInit()) throw std::runtime_error("glfwInit() failed.");
    auto window = glfwCreateWindow(1280, 720, "Map Search Example", nullptr, nullptr);
    if(!window) throw std::runtime_error("glfwCreateWindow(...) failed.");

    glfwMakeContextCurrent(window);
    
    Font font;

    Map map(40, 30);
    map.SetObstruction(4,5,true);
    map.SetObstruction(5,5,true);
    map.SetObstruction(38,2,true);
    map.SetObstruction(6,27,true);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        int mapPixelScale = 16;
        int mapPixelWidth = map.GetWidth() * mapPixelScale;
        int mapPixelHeight = map.GetHeight() * mapPixelScale;
        int mapOffsetX = (width - mapPixelWidth)/2;
        int mapOffsetY = (height - mapPixelHeight)/2;

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        int tileX = static_cast<int>(floor((mouseX-mapOffsetX) / mapPixelScale)), tileY = static_cast<int>(floor((mouseY-mapOffsetY) / mapPixelScale));
        if(tileX >= 0 && tileX < map.GetWidth() && tileY >= 0 && tileY < map.GetHeight())
        {
            if(glfwGetMouseButton(window, 0) == GLFW_PRESS)
            {
                map.SetObstruction(tileX, tileY, true);
            }
            else if(glfwGetMouseButton(window, 1) == GLFW_PRESS)
            {
                map.SetObstruction(tileX, tileY, false);
            }
        }

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glOrtho(0, width, height, 0, -1, +1);
        glColor3f(1,1,0);
        font.Print(32, 32, "Left-click to add obstruction, right-click to clear obstruction.", 8, 14);

        glPushMatrix();
        glTranslated(mapOffsetX, mapOffsetY, 0);
        glBegin(GL_QUADS);
        glColor3f(0.2f,0.2f,0.2f);
        glVertex2i(0,0);
        glVertex2i(map.GetWidth()*mapPixelScale,0);
        glVertex2i(map.GetWidth()*mapPixelScale,map.GetHeight()*mapPixelScale);
        glVertex2i(0,map.GetHeight()*mapPixelScale);
        for(int y=0; y<map.GetHeight(); ++y)
        {
            for(int x=0; x<map.GetWidth(); ++x)
            {
                if(map.IsObstruction(x,y)) glColor3f(0.5f,0.5f,0.5f);                
                else glColor3f(0,0,0);

                glVertex2i(x*mapPixelScale+1, y*mapPixelScale+1);
                glVertex2i((x+1)*mapPixelScale-1, y*mapPixelScale+1);
                glVertex2i((x+1)*mapPixelScale-1, (y+1)*mapPixelScale-1);
                glVertex2i(x*mapPixelScale+1, (y+1)*mapPixelScale-1);
            }
        }
        glEnd();
        glPopMatrix();

        glBegin(GL_QUADS);
        glColor3f(1,1,0);
        glVertex2d(mouseX-2, mouseY-2);
        glVertex2d(mouseX+2, mouseY-2);
        glVertex2d(mouseX+2, mouseY+2);
        glVertex2d(mouseX-2, mouseY+2);
        glEnd();

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