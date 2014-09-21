#pragma once

#include "geometry.h"
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

struct Triangle
{
    Material material;
    float3 v0,v1,v2;

    bool CheckOcclusion(const Ray & ray) const { return IntersectRayTriangle(ray, v0, v1, v2); }
    Hit Intersect(const Ray & ray) const
    {
        float t;
        return IntersectRayTriangle(ray, v0, v1, v2, &t) ? Hit(t, norm(cross(v1-v0, v2-v0)), &material) : Hit();
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
    std::vector<Triangle> triangles;

    float3 ComputeLighting(const Hit & hit, const float3 & viewPosition) const;

    bool CheckOcclusion(const Ray & ray, const Material * ignore) const
    {
        for(auto & sphere : spheres) if(&sphere.material != ignore && sphere.CheckOcclusion(ray)) return true;
        for(auto & triangle : triangles) if(&triangle.material != ignore && triangle.CheckOcclusion(ray)) return true;
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
        for(auto & triangle : triangles)
        {
            auto hit = triangle.Intersect(ray);
            if(hit.distance < bestHit.distance) bestHit = hit;
        }
        bestHit.point = ray.origin + ray.direction * bestHit.distance;
        return bestHit.IsHit() ? ComputeLighting(bestHit, viewPosition) : skyColor;
    }
};

void DrawReferenceSceneGL(const Scene & scene, const Pose & viewPose, float aspectRatio);