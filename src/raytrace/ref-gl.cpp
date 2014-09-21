#include "raytrace.h"
#include <memory>

#define GLFW_INCLUDE_GLU
#include "GLFW\glfw3.h"
#pragma comment(lib, "glu32.lib")

void glLightModel(GLenum pname, float4 value) { glLightModelfv(pname, &value.x); }
void glLight(GLenum light, GLenum pname, float4 value) { glLightfv(light, pname, &value.x); }
void glMaterial(GLenum face, GLenum pname, float4 value) { glMaterialfv(face, pname, &value.x); }

void SetupLightingEnvironment(const Scene & scene)
{
    // Set up global lighting settings
    glEnable(GL_LIGHTING);
    glLightModel(GL_LIGHT_MODEL_AMBIENT, {scene.ambientLight,1});

    // Set up directional light
    glEnable(GL_LIGHT0);
    glLight(GL_LIGHT0, GL_POSITION, {scene.dirLight.direction,0});
    glLight(GL_LIGHT0, GL_DIFFUSE, {scene.dirLight.color,1});
    glLight(GL_LIGHT0, GL_SPECULAR, {scene.dirLight.color,1});
}

void DrawReferenceSceneGL(const Scene & scene, const float3 & viewPosition, const float4 & viewOrientation, float aspectRatio)
{
    static GLUquadric * quad = gluNewQuadric();

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glClearColor(scene.skyColor.x, scene.skyColor.y, scene.skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    gluPerspective(90, aspectRatio, 0.1f, 16.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    float3 eye = viewPosition, center = viewPosition - qzdir(viewOrientation), up = qydir(viewOrientation);
    gluLookAt(eye.x,eye.y,eye.z, center.x,center.y,center.z, up.x,up.y,up.z); 

    SetupLightingEnvironment(scene);

    glEnable(GL_DEPTH_TEST);
    for(auto & sphere : scene.spheres)
    {
        glPushMatrix();
        glTranslatef(sphere.position.x, sphere.position.y, sphere.position.z);
        glMaterial(GL_FRONT, GL_AMBIENT, {sphere.material.albedo,1});
        glMaterial(GL_FRONT, GL_DIFFUSE, {sphere.material.albedo,1});
        glMaterial(GL_FRONT, GL_SPECULAR, {sphere.material.albedo,1});
        glMateriali(GL_FRONT, GL_SHININESS, 64);
        gluSphere(quad, sphere.radius, 24, 24);
        glPopMatrix();
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib();
}