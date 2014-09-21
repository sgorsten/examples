#define GLFW_INCLUDE_GLU
#pragma comment(lib, "glu32.lib")

#include "window.h"

#include <vector>
#include <algorithm>

struct Ray
{
    float3 origin;
    float3 direction;
};

struct Material
{
    float3 albedo;
};

struct Hit
{
    float distance;
    float3 normal;
    const Material * material;

    Hit() : distance(std::numeric_limits<float>::infinity()), material() {}
    Hit(float distance, float3 normal, const Material * material) : distance(distance), normal(normal), material(material) {}

    bool IsHit() const { return distance < std::numeric_limits<float>::infinity(); }
};

struct Sphere
{
    Material material;
    float3 position;
    float radius;

    Hit Intersect(const Ray & ray) const
    {
        // ray.direction must be of unit length
        auto delta = position - ray.origin;
        float b = dot(ray.direction, delta), disc = b*b + radius*radius - mag2(delta);
        if(disc < 0) return {};
        float t = b - sqrt(disc);
        return Hit(t, (ray.direction * t - delta) / radius, &material);
    }
};

struct Scene
{
    std::vector<Sphere> spheres;
};

#include <iostream>

int main(int argc, char * argv[]) try
{
    Window window({1280,720}, "Raytracing Example");

    Scene scene;
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
            Ray ray;
            ray.origin = {0,0,0};
            for(int x=0; x<256; ++x)
            {
                auto ambient = float3(0.3f,0.3f,0.3f);
                ray.direction = norm(float3((x-127.5f)/127.5f, (127.5f-line)/127.5f, -1));
                Hit bestHit;
                for(auto & sphere : scene.spheres)
                {
                    auto hit = sphere.Intersect(ray);
                    if(hit.distance < bestHit.distance) bestHit = hit;
                }
                if(bestHit.IsHit())
                {  
                    auto diffuse = float3(0.8f,0.8f,0.5f) * std::max(dot(bestHit.normal, float3(0,1,0)), 0.0f);
                    image[line*256+x] = bestHit.material->albedo * (ambient + diffuse);
                }
                else
                {
                    image[line*256+x] = float3(0,0,0);
                }
            }
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

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        float lightPos[] = {0,1,0,0}, lightDiffuse[] = {0.8f,0.8f,0.5f,1.0f}, lightSpecular[] = {0,0,0,0}, lightAmbient[] = {0.3f,0.3f,0.3f,0};
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmbient);
        
        for(auto & sphere : scene.spheres)
        {
            glPushMatrix();
            glTranslatef(sphere.position.x, sphere.position.y, sphere.position.z);
            glColor3fv(&sphere.material.albedo.x);
            gluSphere(quad, sphere.radius, 24, 24);
            glPopMatrix();
        }

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