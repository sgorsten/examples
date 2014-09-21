#pragma once

#include "geometry.h"
#include <algorithm>
#include <vector>

struct Material
{
    float3 albedo;
};

struct Hit
{
    float distance;
    float3 point, normal;
    const Material * material;

    Hit() : distance(std::numeric_limits<float>::infinity()), material() {}
    Hit(float distance, float3 normal, const Material * material) : distance(distance), normal(normal), material(material) {}

    bool IsHit() const { return distance < std::numeric_limits<float>::infinity(); }
};

struct Mesh
{
    Material material;
    std::vector<float3> vertices;
    std::vector<int3> triangles;

    float3 boundCenter;
    float boundRadius;

    void ComputeBounds()
    {
        boundCenter = {0,0,0};
        for(auto & vert : vertices) boundCenter += vert;
        boundCenter *= 1.0f/vertices.size();
        boundRadius = 0;
        for(auto & vert : vertices) boundRadius = std::max(boundRadius, mag(vert - boundCenter));
    }

    bool CheckOcclusion(const Ray & ray) const 
    {
        if(!IntersectRaySphere(ray, boundCenter, boundRadius)) return false;
        for(auto & tri : triangles) if(IntersectRayTriangle(ray, vertices[tri.x], vertices[tri.y], vertices[tri.z])) return true;
        return false;
    }
    Hit Intersect(const Ray & ray) const
    {
        if(!IntersectRaySphere(ray, boundCenter, boundRadius)) return Hit();
        const int3 * bestTri = 0;
        float bestT = std::numeric_limits<float>::infinity();
        for(auto & tri : triangles)
        {
            float t;
            if(IntersectRayTriangle(ray, vertices[tri.x], vertices[tri.y], vertices[tri.z], &t))
            if(t < bestT)
            {
                bestTri = &tri;
                bestT = t;
            }
        }
        return bestTri ? Hit(bestT, norm(cross(vertices[bestTri->y] - vertices[bestTri->x], vertices[bestTri->z] - vertices[bestTri->x])), &material) : Hit();
    }
};

struct Sphere
{
    Material material;
    float3 position;
    float radius;

    bool CheckOcclusion(const Ray & ray) const { return IntersectRaySphere(ray, position, radius); }
    Hit Intersect(const Ray & ray) const
    {
        float t;
        return IntersectRaySphere(ray, position, radius, &t) ? Hit(t, (ray.origin + ray.direction * t - position) / radius, &material) : Hit();
    }
};

struct DirectionalLight
{
    float3 direction;
    float3 color;

    float3 ComputeContribution(const Hit & hit, const float3 & eyeDir) const;
};

struct Scene
{
    float3 skyColor;
    float3 ambientLight;
    DirectionalLight dirLight;

    std::vector<Sphere> spheres;
    std::vector<Mesh> meshes;

    float3 ComputeLighting(const Hit & hit, const float3 & viewPosition) const;

    bool CheckOcclusion(const Ray & ray, const Material * ignore) const
    {
        for(auto & sphere : spheres) if(&sphere.material != ignore && sphere.CheckOcclusion(ray)) return true;
        for(auto & mesh : meshes) if(&mesh.material != ignore && mesh.CheckOcclusion(ray)) return true;
        return false;
    }

    float3 CastPrimaryRay(const Ray & ray, const float3 & viewPosition) const
    {
        Hit bestHit;
        for(auto & sphere : spheres)
        {
            auto hit = sphere.Intersect(ray);
            if(hit.distance < bestHit.distance) bestHit = hit;
        }
        for(auto & mesh : meshes)
        {
            auto hit = mesh.Intersect(ray);
            if(hit.distance < bestHit.distance) bestHit = hit;
        }
        bestHit.point = ray.origin + ray.direction * bestHit.distance;
        return bestHit.IsHit() ? ComputeLighting(bestHit, viewPosition) : skyColor;
    }
};

void DrawReferenceSceneGL(const Scene & scene, const Pose & viewPose, float aspectRatio);