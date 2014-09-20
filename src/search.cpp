#include "font.h"

#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <vector>
#include <algorithm>

struct int2 { int x,y; };

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

    std::vector<int2> Search(const int2 & start, const int2 & goal) const
    {
        const int2 directions[] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
        const int costs[] = {5,7,5,7,5,7,5,7};

        struct OpenNode { int2 state; int lastAction, gCost; bool operator < (const OpenNode & r) const { return r.gCost < gCost; } };
        std::vector<OpenNode> open;
        std::vector<int> closed(tiles.size(), -1);

        open.push_back({start, 0, 0});
        while(!open.empty())
        {
            auto node = open.front();
            std::pop_heap(begin(open), end(open));
            open.pop_back();

            auto state = node.state;
            if(closed[state.y * width + state.x] != -1) continue;

            closed[state.y * width + state.x] = node.lastAction;
            if(state.x == goal.x && state.y == goal.y)
            {
                std::vector<int2> path(1,state);
                while(state.x != start.x || state.y != start.y)
                {
                    
                    int action = closed[state.y * width + state.x];
                    state.x -= directions[action].x;
                    state.y -= directions[action].y;
                    path.push_back(state);
                }
                std::reverse(begin(path), end(path));
                return path;
            }

            for(int i=0; i<8; ++i)
            {
                auto newState = state;
                newState.x += directions[i].x;
                newState.y += directions[i].y;
                if(newState.x < 0 || newState.y < 0 || newState.x >= width || newState.y >= height) continue;
                if(IsObstruction(newState.x, newState.y)) continue;
                if(IsObstruction(state.x, newState.y) && IsObstruction(newState.x, state.y)) continue;
                open.push_back({newState, i, node.gCost + costs[i]});
                std::push_heap(begin(open), end(open));
            }
        }

        return {};
    }
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

    int2 startTile;
    bool middleClicked = false;
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

            if(!middleClicked && glfwGetMouseButton(window, 2) == GLFW_PRESS)
            {
                startTile = {tileX, tileY};
                middleClicked = true;
            }
        }
        if(glfwGetMouseButton(window, 2) == GLFW_RELEASE) middleClicked = false;

        std::vector<int2> path;
        if(middleClicked) path = map.Search(startTile, {tileX,tileY});

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glOrtho(0, width, height, 0, -1, +1);
        glColor3f(1,1,0);
        font.Print(32, 32, "Left-click to add obstruction, right-click to clear obstruction.");
        font.Print(32, 48, "Middle-click and drag to find a path between two points.");

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

        if(middleClicked)
        {
            glBegin(GL_LINE_STRIP);
            glColor3f(1,1,0);
            for(auto tile : path) glVertex2i(tile.x * mapPixelScale + mapPixelScale/2, tile.y * mapPixelScale + mapPixelScale/2);
            glEnd();

            glBegin(GL_QUADS);
            glColor3f(0,1,0);
            glVertex2d(startTile.x*mapPixelScale + mapPixelScale/2 - 2, startTile.y*mapPixelScale + mapPixelScale/2-2);
            glVertex2d(startTile.x*mapPixelScale + mapPixelScale/2 + 2, startTile.y*mapPixelScale + mapPixelScale/2-2);
            glVertex2d(startTile.x*mapPixelScale + mapPixelScale/2 + 2, startTile.y*mapPixelScale + mapPixelScale/2+2);
            glVertex2d(startTile.x*mapPixelScale + mapPixelScale/2 - 2, startTile.y*mapPixelScale + mapPixelScale/2+2);
            glEnd();

            /*glBegin(GL_QUADS);
            glColor3f(0,1,1);
            glVertex2d(startTile.x-2, startTile.y-2);
            glVertex2d(startTile.x+2, startTile.y-2);
            glVertex2d(startTile.x+2, startTile.y+2);
            glVertex2d(startTile.x-2, startTile.y+2);
            glEnd();*/
        }

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