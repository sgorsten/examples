#include "raytrace.h"
#include <memory>

#define GLFW_INCLUDE_GLU
#include "GLFW\glfw3.h"
#pragma comment(lib, "glu32.lib")

void glLightModel(GLenum pname, float4 value) { glLightModelfv(pname, &value.x); }
void glLight(GLenum light, GLenum pname, float4 value) { glLightfv(light, pname, &value.x); }

void SetupLightingEnvironment(const Scene & scene)
{
    // Set up global lighting settings
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glLightModel(GL_LIGHT_MODEL_AMBIENT, {scene.ambientLight,1});

    // Set up directional light
    glEnable(GL_LIGHT0);
    glLight(GL_LIGHT0, GL_POSITION, {scene.dirLight.direction,0});
    glLight(GL_LIGHT0, GL_DIFFUSE, {scene.dirLight.color,1});
    glLight(GL_LIGHT0, GL_SPECULAR, {0,0,0,0});
}

void DrawReferenceSceneGL(const Scene & scene)
{
    static GLUquadric * quad = gluNewQuadric();

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    SetupLightingEnvironment(scene);

    glEnable(GL_DEPTH_TEST);
    for(auto & sphere : scene.spheres)
    {
        glPushMatrix();
        glTranslatef(sphere.position.x, sphere.position.y, sphere.position.z);
        glColor3fv(&sphere.material.albedo.x);
        gluSphere(quad, sphere.radius, 24, 24);
        glPopMatrix();
    }

    glPopAttrib();
}