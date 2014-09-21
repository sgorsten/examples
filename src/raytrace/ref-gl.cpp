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

void SetupMaterial(const Material & material)
{
    glMaterial(GL_FRONT, GL_AMBIENT, {material.albedo,1});
    glMaterial(GL_FRONT, GL_DIFFUSE, {material.albedo,1});
    glMaterial(GL_FRONT, GL_SPECULAR, {material.albedo,1});
    glMateriali(GL_FRONT, GL_SHININESS, 64);
}

void DrawReferenceSceneGL(const Scene & scene, const Pose & viewPose, float aspectRatio)
{
    static GLUquadric * quad = gluNewQuadric();

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glClearColor(scene.skyColor.x, scene.skyColor.y, scene.skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    gluPerspective(90, aspectRatio, 0.25f, 64.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    float3 eye = viewPose.position, center = eye - viewPose.GetZDir(), up = viewPose.GetYDir();
    gluLookAt(eye.x,eye.y,eye.z, center.x,center.y,center.z, up.x,up.y,up.z); 

    SetupLightingEnvironment(scene);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    for(auto & sphere : scene.spheres)
    {
        SetupMaterial(sphere.material);

        glPushMatrix();
        glTranslatef(sphere.position.x, sphere.position.y, sphere.position.z);
        gluSphere(quad, sphere.radius, 24, 24);
        glPopMatrix();
    }
    
    for(auto & mesh : scene.meshes)
    {
        SetupMaterial(mesh.material);

        glBegin(GL_TRIANGLES);
        for(auto & tri : mesh.triangles)
        {
            auto & v0 = mesh.vertices[tri.x], & v1 = mesh.vertices[tri.y], & v2 = mesh.vertices[tri.z];
            auto n = norm(cross(v1-v0, v2-v0));
            glNormal3fv(&n.x);
            glVertex3fv(&v0.x);
            glVertex3fv(&v1.x);
            glVertex3fv(&v2.x);
        }
        glEnd();
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib();
}