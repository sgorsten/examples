#pragma once

#include "linalg.h"
#include <vector>

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

    Hit Intersect(const Ray & ray) const;
};

struct DirectionalLight
{
    float3 direction;
    float3 color;

    float3 ComputeContribution(const Hit & hit) const;
};

struct Scene
{
    float3 ambientLight;
    DirectionalLight dirLight;

    std::vector<Sphere> spheres;

    float3 ComputeLighting(const Hit & hit) const;

    float3 CastPrimaryRay(const Ray & ray) const
    {
        Hit bestHit;
        for(auto & sphere : spheres)
        {
            auto hit = sphere.Intersect(ray);
            if(hit.distance < bestHit.distance) bestHit = hit;
        }

        return bestHit.IsHit() ? ComputeLighting(bestHit) : float3(0,0,0);
    }
};

void DrawReferenceSceneGL(const Scene & scene);