#include "raytrace.h"

#define GLFW_INCLUDE_GLU
#include "window.h"
#pragma comment(lib, "glu32.lib")

#include <algorithm>
#include <iostream>
#include <chrono>

struct RaytracedImage
{
    std::vector<float3> pixels;
    int2 dimensions;
    float3 viewPosition;
    float4 viewOrientation;
    int currentLine = 0;

    bool IsComplete() const { return currentLine == dimensions.y; }

    void Reset(const int2 & dimensions, const float3 & viewPosition, const float4 & viewOrientation)
    {
        pixels.resize(dimensions.x * dimensions.y);
        this->dimensions = dimensions;
        this->viewPosition = viewPosition;
        this->viewOrientation = viewOrientation;
        currentLine = 0;
    }

    void RaytracePixel(const Scene & scene, const int2 & coord)
    {
        auto halfDims = float2(dimensions - 1) * 0.5f;
        auto aspectRatio = (float)dimensions.x / dimensions.y;
        auto viewDirection = norm(float3((coord.x-halfDims.x)*aspectRatio/halfDims.x, (halfDims.y-coord.y)/halfDims.y, -1));
        pixels[coord.y * dimensions.x + coord.x] = scene.CastPrimaryRay({viewPosition, qrot(viewOrientation, viewDirection)}, viewPosition);
    }

    void RaytraceLine(const Scene & scene)
    {
        if(currentLine < dimensions.y)
        {
            for(int x=0; x<dimensions.x; ++x)
            {
                RaytracePixel(scene, {x,currentLine});
            }
            ++currentLine;
        }
    }
};

int main(int argc, char * argv[]) try
{
    Window window({1280,720}, "Raytracing Example");

    window.MakeContextCurrent();
    GLuint texture;
    glGenTextures(1, &texture);

    Scene scene;
    scene.ambientLight = float3(0.3f,0.3f,0.3f);
    scene.dirLight.direction = {0,1,0};
    scene.dirLight.color = {0.8f,0.8f,0.5f};
    scene.spheres.push_back({Material{{1,1,1}}, {0,0,-5}, 2});
    scene.spheres.push_back({Material{{1,0.5f,0.5f}}, {3,-1,-7}, 2});
    scene.spheres.push_back({Material{{0.3f,1,0.3f}}, {-3,-2,-6}, 2});
    scene.spheres.push_back({Material{{0.4f,0.4f,1}}, {-1.5f,+2,-6}, 2});

    float3 position;
    float4 orientation = {0,0,0,1};

    RaytracedImage image;

    window.SetKeyHandler([&](int key, int scancode, int action, int mods)
    {
        if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            image.Reset(window.GetFramebufferSize()/int2(2,1), position, orientation);
        }
    });

    auto mousePos = window.GetCursorPos();
    float pitch=0, yaw=0;

    image.Reset(window.GetFramebufferSize()/int2(2,1), position, orientation);
    image.RaytracePixel(scene, window.GetFramebufferSize()/int2(4,2));

    auto t0 = std::chrono::monotonic_clock::now();
    while(!window.WindowShouldClose())
    {
        glfwPollEvents();

        auto t1 = std::chrono::monotonic_clock::now();
        float timestep = std::chrono::duration<float>(t1 - t0).count();
        t0 = t1;

        auto newMousePos = window.GetCursorPos();
        auto mouseDelta = newMousePos - mousePos;
        mousePos = newMousePos;

        if(window.GetMouseButton(0))
        {
            yaw -= mouseDelta.x * 0.01f;
            pitch -= mouseDelta.y * 0.01f;
            orientation = qmul(float4(float3(0,1,0) * std::sin(yaw/2), std::cos(yaw/2)), float4(float3(1,0,0) * std::sin(pitch/2), std::cos(pitch/2)));
        }

        if(window.GetKey(GLFW_KEY_W)) position -= qzdir(orientation) * (timestep * 8);
        if(window.GetKey(GLFW_KEY_S)) position += qzdir(orientation) * (timestep * 8);
        if(window.GetKey(GLFW_KEY_A)) position -= qxdir(orientation) * (timestep * 8);
        if(window.GetKey(GLFW_KEY_D)) position += qxdir(orientation) * (timestep * 8);

        auto frameSize = window.GetFramebufferSize();
        window.MakeContextCurrent();
        if(!image.IsComplete())
        {
            for(int i=0; i<8; ++i) image.RaytraceLine(scene);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.dimensions.x, image.currentLine, 0, GL_RGB, GL_FLOAT, image.pixels.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }

        glPushAttrib(GL_ALL_ATTRIB_BITS);

        glViewport(0, 0, frameSize.x, frameSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, frameSize.x/2, frameSize.y);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(-1,1);
        glTexCoord2f(1,0); glVertex2f(+1,1);
        float completion = (float)image.currentLine/image.dimensions.y;
        glTexCoord2f(1,1); glVertex2f(+1,1-completion*2);
        glTexCoord2f(0,1); glVertex2f(-1,1-completion*2);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glViewport(frameSize.x/2, 0, frameSize.x/2, frameSize.y);
        DrawReferenceSceneGL(scene, position, orientation, frameSize.x*0.5f/frameSize.y);

        glViewport(0, 0, frameSize.x, frameSize.y);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glOrtho(0, frameSize.x, frameSize.y, 0, -1, +1);
      
        glColor3f(1,1,0);
        window.Print({32,32}, "Press space to raytrace scene");
        window.Print({frameSize.x/2+32,32}, "Reference render in OpenGL");
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