#include "raytrace.h"

#define GLFW_INCLUDE_GLU
#include "window.h"
#pragma comment(lib, "glu32.lib")

#include <algorithm>
#include <iostream>

int main(int argc, char * argv[]) try
{
    Window window({1280,720}, "Raytracing Example");

    Scene scene;
    scene.ambientLight = float3(0.3f,0.3f,0.3f);
    scene.dirLight.direction = {0,1,0};
    scene.dirLight.color = {0.8f,0.8f,0.5f};
    scene.spheres.push_back({Material{{1,1,1}}, {0,0,-5}, 2});
    scene.spheres.push_back({Material{{1,0.5f,0.5f}}, {3,-1,-7}, 2});
    scene.spheres.push_back({Material{{0.3f,1,0.3f}}, {-3,-2,-6}, 2});
    scene.spheres.push_back({Material{{0.4f,0.4f,1}}, {-1.5f,+2,-6}, 2});
    std::vector<float3> image(256*256);
    int line = 0;

    window.SetKeyHandler([&line](int key, int scancode, int action, int mods)
    {
        if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) line = 0;
    });

    auto quad = gluNewQuadric();

    while(!window.WindowShouldClose())
    {
        glfwPollEvents();

        if(line < 256)
        {
            for(int x=0; x<256; ++x) image[line*256+x] = scene.CastPrimaryRay({{0,0,0}, norm(float3((x-127.5f)/127.5f, (127.5f-line)/127.5f, -1))});
            ++line;
        }

        auto frameSize = window.GetFramebufferSize();
        window.MakeContextCurrent();

        glPushAttrib(GL_ALL_ATTRIB_BITS);

        glViewport(0, 0, frameSize.x, frameSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glOrtho(0, frameSize.x, frameSize.y, 0, -1, +1);

        glBegin(GL_POINTS);
        for(int y=0; y<line; ++y)
        {
            for(int x=0; x<256; ++x)
            {
                glColor3fv(&image[y*256+x].x);
                glVertex2i(64+x, 64+y);
            }
        }
        glEnd();

        glColor3f(1,1,0);
        window.Print({32,32}, "Press space to raytrace scene");
        window.Print({512+32,32}, "Reference render in OpenGL");
        glPopMatrix();

        glViewport(512, frameSize.y-64-256, 256, 256);
        glPushMatrix();
        gluPerspective(90, 1.0f, 1.0f, 16.0f);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        gluLookAt(0,0,0, 0,0,-1, 0,1,0);

        DrawReferenceSceneGL(scene);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glPopAttrib();

        window.SwapBuffers();
    }

    return 0;
}
catch(const std::exception & e)
{
    std::cerr << "Unhandled exception: " << e.what() << std::endl;
    return -1;
}