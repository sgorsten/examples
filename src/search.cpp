#include "window.h"

#include <vector>
#include <algorithm>

int EstimateCost(const int2 & a, const int2 & b)
{
    auto dx = abs(b.x-a.x), dy = abs(b.y-a.y);
    return dx > dy ? dx*5 + dy*2 : dx*2 + dy*5;
}

class Map
{
    int2 dims;
    std::vector<int> tiles;

    struct OpenNode { int2 state, pred; int gCost, fCost; bool operator < (const OpenNode & r) const { return std::tie(r.fCost,-r.gCost) < std::tie(fCost,-gCost); } };
    std::vector<OpenNode> open;
    std::vector<int2> closed;

    static const int2 directions[8];
    static const int costs[8];

    int GetIndex(const int2 & coord) const { return coord.y * dims.x + coord.x; }    
public:
    Map(int2 dims) : dims(dims), tiles(dims.x * dims.y, 0) {}

    int2 GetDimensions() const { return dims; }
    int GetWidth() const { return dims.x; }
    int GetHeight() const { return dims.y; }
    bool IsValidCoord(const int2 & coord) const { return coord.x >= 0 && coord.y >= 0 && coord.x < dims.x && coord.y < dims.y; }
    bool IsObstruction(const int2 & coord) const { return tiles[GetIndex(coord)] != 0; }

    bool IsClosed(const int2 & coord) const { return closed[GetIndex(coord)].x != -1; }
    const int2 & GetPredecessor(const int2 & coord) const { return closed[GetIndex(coord)]; }

    void SetObstruction(const int2 & coord, bool isObstruction) { tiles[GetIndex(coord)] = isObstruction ? 1 : 0; }

    template<class H> std::vector<int2> Search(const int2 & start, const int2 & goal, H heuristic)
    {
        open.clear();
        open.push_back({start, start, 0, heuristic(start, goal)});

        closed.clear();
        closed.resize(tiles.size(), {-1,0});

        while(!open.empty())
        {
            auto node = open.front();
            std::pop_heap(begin(open), end(open));
            open.pop_back();

            auto state = node.state;
            if(IsClosed(state)) continue;

            closed[GetIndex(state)] = node.pred;
            if(state == goal)
            {
                std::vector<int2> path(1,state);
                while(state != start)
                {    
                    state = GetPredecessor(state);
                    path.push_back(state);
                }
                std::reverse(begin(path), end(path));
                return path;
            }

            for(int i=0; i<8; ++i)
            {
                auto newState = state + directions[i];
                if(!IsValidCoord(newState)) continue;
                if(IsClosed(newState)) continue;
                if(IsObstruction(newState)) continue;
                if(IsObstruction({state.x, newState.y}) || IsObstruction({newState.x, state.y})) continue;
                auto gCost = node.gCost + costs[i];
                open.push_back({newState, state, gCost, gCost + heuristic(newState, goal)});
                std::push_heap(begin(open), end(open));
            }
        }

        return {};
    }

    bool CheckStraight(int2 & state, int & gCost, int action, const int2 & goal)
    {
        struct Check { int2 blockOffset, openOffset; };
        static const Check checks[4][2] = {
            {{{-1,-1},{0,-1}}, {{-1,+1},{0,+1}}},
            {{{-1,-1},{-1,0}}, {{+1,-1},{+1,0}}},
            {{{+1,-1},{0,-1}}, {{+1,+1},{0,+1}}},
            {{{-1,+1},{-1,0}}, {{+1,+1},{+1,0}}}
        };

        auto startState = state;
        state += directions[action];
        gCost += costs[action];    

        while(true)
        {
            if(!IsValidCoord(state)) return false;
            if(IsClosed(state)) return false;
            if(IsObstruction(state)) return false;

            if( state.x == goal.x && state.y == goal.y )
            {
                return true;
            }

            for(auto check : checks[action])
            {
                if(IsValidCoord(state + check.blockOffset) && IsObstruction(state + check.blockOffset) && IsValidCoord(state + check.openOffset) && !IsObstruction(state + check.openOffset))
                {
                    return true;
                }
            }

            state += directions[action];
            gCost += costs[action];
        }
    }

    template<class H> std::vector<int2> JumpPointSearch(const int2 & start, const int2 & goal, H heuristic)
    {
        open.clear();
        open.push_back({start, start, 0, heuristic(start, goal)});

        closed.clear();
        closed.resize(tiles.size(), {-1,0});

        while(!open.empty())
        {
            const auto node = open.front();
            std::pop_heap(begin(open), end(open));
            open.pop_back();

            if(IsClosed(node.state)) continue;

            closed[GetIndex(node.state)] = node.pred;
            if(node.state == goal)
            {
                auto state = node.state;   
                std::vector<int2> path(1,state);
                while(state != start)
                {    
                    state = GetPredecessor(state);
                    path.push_back(state);
                }
                std::reverse(begin(path), end(path));
                return path;
            }

            int2 checkOffsets[4][2] = {
                {{-1,-1},{-1,+1}},
                {{-1,-1},{+1,-1}},
                {{+1,-1},{+1,+1}},
                {{-1,+1},{+1,+1}}
            };

            // Adjacent moves
            for(int i=0; i<4; ++i)
            {
                auto newState = node.state;
                auto gCost = node.gCost;
                if(CheckStraight(newState, gCost, i, goal))
                {
                    open.push_back({newState, node.state, gCost, gCost + heuristic(newState, goal)});
                    std::push_heap(begin(open), end(open));
                }
            }

            static const int adjDirections[4][2] = {{0,1},{1,2},{2,3},{3,0}};

            // Diagonal moves
            for(int i=0; i<4; ++i)
            {
                int action = i+4;
                auto prevState = node.state;
                auto newState = node.state + directions[action];
                auto gCost = node.gCost + costs[action];

                // || CheckStraight(s1, g, adjDirections[i][0], goal) || CheckStraight(s2, g, adjDirections[i][1], goal) )

                while(true)
                {
                    if(!IsValidCoord(newState)) break;
                    if(IsClosed(newState)) break;
                    if(IsObstruction(newState)) break;
                    if(IsObstruction({prevState.x, newState.y}) || IsObstruction({newState.x, prevState.y})) break;

                    if( newState.x == goal.x || newState.y == goal.y ) 
                    {
                        open.push_back({newState, node.state, gCost, gCost + heuristic(newState, goal)});
                        std::push_heap(begin(open), end(open));
                        break;
                    }                    

                    auto s1 = newState, s2 = newState; auto g1 = gCost, g2 = gCost;
                    bool p1 = CheckStraight(s1, g1, adjDirections[i][0], goal);
                    bool p2 = CheckStraight(s2, g2, adjDirections[i][1], goal);
                    if(p1 || p2)
                    {
                        open.push_back({newState, node.state, gCost, gCost + heuristic(newState, goal)});
                        std::push_heap(begin(open), end(open));
                        break;
                    }                    

                    prevState = newState;
                    newState += directions[action];
                    gCost += costs[action];
                }
            }
        }

        return {};
    }

    std::vector<int2> DijkstraSearch(const int2 & start, const int2 & goal) { return Search(start, goal, [](const int2 & a, const int2 & b) { return 0; }); }
    std::vector<int2> AStarSearch(const int2 & start, const int2 & goal) { return Search(start, goal, [](const int2 & a, const int2 & b) { return EstimateCost(a,b); }); }
    std::vector<int2> AStarJPSearch(const int2 & start, const int2 & goal) { return JumpPointSearch(start, goal, [](const int2 & a, const int2 & b) { return EstimateCost(a,b); }); }
};

const int2 Map::directions[8] = {{1,0},{0,1},{-1,0},{0,-1},{1,1},{-1,1},{-1,-1},{1,-1}};
const int Map::costs[8] = {5,5,5,5,7,7,7,7};

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
    Window window({1280,720}, "Map Search Example");

    int algorithm = 0;
    window.SetKeyHandler([&algorithm](int key, int, int action, int)
    {
        if(key == GLFW_KEY_A && action == GLFW_PRESS) algorithm = (algorithm + 1) % 3;
    });

    Map map({40, 30});

    int2 startTile;
    bool middleClicked = false;
    while(!window.WindowShouldClose())
    {
        glfwPollEvents();

        int mapPixelScale = 16;
        int2 mapPixelSize = map.GetDimensions() * mapPixelScale;
        int2 frameSize = window.GetFramebufferSize();
        int2 mapOffset = (frameSize - mapPixelSize)/2;       

        double2 mouse = window.GetCursorPos();
        int2 tile = {static_cast<int>(floor((mouse.x-mapOffset.x) / mapPixelScale)), static_cast<int>(floor((mouse.y-mapOffset.y) / mapPixelScale))};
        if(map.IsValidCoord(tile))
        {
            if(window.GetMouseButton(0)) map.SetObstruction(tile, true);
            else if(window.GetMouseButton(1)) map.SetObstruction(tile, false);

            if(!middleClicked && window.GetMouseButton(2))
            {
                startTile = tile;
                middleClicked = true;
            }
        }
        if(!window.GetMouseButton(2)) middleClicked = false;

        std::vector<int2> path;
        if(middleClicked)
        {
            switch(algorithm)
            {
            case 0: path = map.AStarSearch(startTile, tile); break;
            case 1: path = map.DijkstraSearch(startTile, tile); break;
            case 2: path = map.AStarJPSearch(startTile, tile); break;
            }
        }

        window.MakeContextCurrent();
        glViewport(0, 0, frameSize.x, frameSize.y);
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glOrtho(0, frameSize.x, frameSize.y, 0, -1, +1);
        glColor3f(1,1,0);
        window.Print({32,32}, "Left-click to add obstruction, right-click to clear obstruction.");
        window.Print({32,48}, "Middle-click and drag to find a path between two points.");
        const char * labels[] = {"A* search", "Dijkstra search", "A* Jump Point Search"};
        window.Print({32,64}, "Press 'A' to change search algorithm (currently %s)", labels[algorithm]);
        if(!path.empty())
        {
            int cost = 0;
            for(size_t i=1; i<path.size(); ++i)
            {
                cost += EstimateCost(path[i-1], path[i]);
            }
            window.Print({32,80}, "A path was found using %d moves with a cost of %d", path.size(), cost);
        }

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
            glBegin(GL_LINES);
            glColor3f(1,1,0);
            for(int y=0; y<map.GetHeight(); ++y)
            {
                for(int x=0; x<map.GetWidth(); ++x)
                {
                    if(map.IsClosed({x,y}))
                    {
                        int2 state = {x,y}, parent = map.GetPredecessor(state);
                        glVertex2i(state.x * mapPixelScale + mapPixelScale/2, state.y * mapPixelScale + mapPixelScale/2);
                        glVertex2i(parent.x * mapPixelScale + mapPixelScale/2, parent.y * mapPixelScale + mapPixelScale/2);
                    }
                }
            }
            glEnd();

            glBegin(GL_LINE_STRIP);
            if(path.empty()) glColor3f(1,0,0);
            else glColor3f(0,1,0);
            for(auto tile : path) glVertex2i(tile.x * mapPixelScale + mapPixelScale/2, tile.y * mapPixelScale + mapPixelScale/2);
            glEnd();

            glBegin(GL_QUADS);
            glColor3f(0,0.5f,1);
            for(int y=0; y<map.GetHeight(); ++y)
            {
                for(int x=0; x<map.GetWidth(); ++x)
                {
                    int2 tile = {x,y};
                    if(map.IsClosed(tile))
                    {
                        DrawRect(tile*mapPixelScale + (mapPixelScale/2 - 2), tile*mapPixelScale + (mapPixelScale/2 + 2));
                    }
                }
            }

            if(path.empty()) glColor3f(1,0,0);
            else glColor3f(0,1,0);
            DrawRect(startTile*mapPixelScale + (mapPixelScale/2 - 2), startTile*mapPixelScale + (mapPixelScale/2 + 2));
            DrawRect(tile*mapPixelScale + (mapPixelScale/2 - 2), tile*mapPixelScale + (mapPixelScale/2 + 2));
            glEnd();
        }

        glPopMatrix();

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