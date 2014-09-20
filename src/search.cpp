#include "font.h"
#include "linalg.h"

#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <vector>
#include <algorithm>

class Map
{
    std::vector<int> tiles;
    int2 dims;

    int GetIndex(const int2 & coord) const { return coord.y * dims.x + coord.x; }
public:
    Map(int2 dims) : tiles(dims.x * dims.y, 0), dims(dims) {}

    int2 GetDimensions() const { return dims; }
    int GetWidth() const { return dims.x; }
    int GetHeight() const { return dims.y; }
    bool IsValidCoord(const int2 & coord) const { return coord.x >= 0 && coord.y >= 0 && coord.x < dims.x && coord.y < dims.y; }
    bool IsObstruction(const int2 & coord) const { return tiles[GetIndex(coord)] != 0; }

    void SetObstruction(const int2 & coord, bool isObstruction) { tiles[GetIndex(coord)] = isObstruction ? 1 : 0; }

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
            if(closed[state.y * dims.x + state.x] != -1) continue;

            closed[state.y * dims.x + state.x] = node.lastAction;
            if(state == goal)
            {
                std::vector<int2> path(1,state);
                while(state != start)
                {
                    
                    int action = closed[state.y * dims.x + state.x];
                    state -= directions[action];
                    path.push_back(state);
                }
                std::reverse(begin(path), end(path));
                return path;
            }

            for(int i=0; i<8; ++i)
            {
                auto newState = state + directions[i];
                if(!IsValidCoord(newState)) continue;
                if(IsObstruction(newState)) continue;
                if(IsObstruction({state.x, newState.y}) && IsObstruction({newState.x, state.y})) continue;
                open.push_back({newState, i, node.gCost + costs[i]});
                std::push_heap(begin(open), end(open));
            }
        }

        return {};
    }
};

#include <iostream>

void DrawRect(int2 a, int2 b)
{
    glVertex2i(a.x,a.y);
    glVertex2i(b.x,a.y);
    glVertex2i(b.x,b.y);
    glVertex2i(a.x,b.y);
}

int main() try
{
    if(!glfwInit()) throw std::runtime_error("glfwInit() failed.");
    auto window = glfwCreateWindow(1280, 720, "Map Search Example", nullptr, nullptr);
    if(!window) throw std::runtime_error("glfwCreateWindow(...) failed.");

    glfwMakeContextCurrent(window);
    
    Font font;

    Map map({40, 30});

    int2 startTile;
    bool middleClicked = false;
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int2 frameSize;
        glfwGetFramebufferSize(window, &frameSize.x, &frameSize.y);

        int mapPixelScale = 16;
        int2 mapPixelSize = map.GetDimensions() * mapPixelScale;
        int2 mapOffset = (frameSize - mapPixelSize)/2;

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        int2 tile = {static_cast<int>(floor((mouseX-mapOffset.x) / mapPixelScale)), static_cast<int>(floor((mouseY-mapOffset.y) / mapPixelScale))};
        if(map.IsValidCoord(tile))
        {
            if(glfwGetMouseButton(window, 0) == GLFW_PRESS) map.SetObstruction(tile, true);
            else if(glfwGetMouseButton(window, 1) == GLFW_PRESS) map.SetObstruction(tile, false);

            if(!middleClicked && glfwGetMouseButton(window, 2) == GLFW_PRESS)
            {
                startTile = tile;
                middleClicked = true;
            }
        }
        if(glfwGetMouseButton(window, 2) == GLFW_RELEASE) middleClicked = false;

        std::vector<int2> path;
        if(middleClicked) path = map.Search(startTile, tile);

        glViewport(0, 0, frameSize.x, frameSize.y);
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glOrtho(0, frameSize.x, frameSize.y, 0, -1, +1);
        glColor3f(1,1,0);
        font.Print(32, 32, "Left-click to add obstruction, right-click to clear obstruction.");
        font.Print(32, 48, "Middle-click and drag to find a path between two points.");

        glPushMatrix();
        glTranslated(mapOffset.x, mapOffset.y, 0);
        glBegin(GL_QUADS);
        glColor3f(0.2f,0.2f,0.2f);
        DrawRect({0,0}, mapPixelSize);
        for(int y=0; y<map.GetHeight(); ++y)
        {
            for(int x=0; x<map.GetWidth(); ++x)
            {
                if(map.IsObstruction({x,y})) glColor3f(0.5f,0.5f,0.5f);                
                else glColor3f(0,0,0);
                DrawRect(int2{x,y}*mapPixelScale+1, int2{x,y}*mapPixelScale+(mapPixelScale-1));
            }
        }
        glEnd();

        if(middleClicked)
        {
            if(path.empty()) glColor3f(1,0,0);
            else glColor3f(0,1,0);

            glBegin(GL_LINE_STRIP);
            for(auto tile : path) glVertex2i(tile.x * mapPixelScale + mapPixelScale/2, tile.y * mapPixelScale + mapPixelScale/2);
            glEnd();

            glBegin(GL_QUADS);
            DrawRect(startTile*mapPixelScale + (mapPixelScale/2 - 2), startTile*mapPixelScale + (mapPixelScale/2 + 2));
            DrawRect(tile*mapPixelScale + (mapPixelScale/2 - 2), tile*mapPixelScale + (mapPixelScale/2 + 2));
            glEnd();
        }

        glPopMatrix();

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