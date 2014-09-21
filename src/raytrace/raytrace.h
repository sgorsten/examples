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

    Hit Intersect(const Ray & ray) const;
};

struct Triangle
{
    Material material;
    float3 v0,v1,v2;

    Hit Intersect(const Ray & ray) const
    {
        auto e1 = v1 - v0, e2 = v2 - v0;
        auto h = cross(ray.direction, e2);
        auto a = dot(e1, h);
        if (a > -0.00001f && a < 0.00001f) return {};

        auto f = 1/a;
        auto s = ray.origin - v0;
        auto u = f * dot(s,h);
        if (u < 0 || u > 1) return {};

        auto q = cross(s,e1);
        auto v = f * dot(ray.direction,q);
        if (v < 0 || u + v > 1) return {};

        auto t = f * dot(e2,q);
        if(t < 0) return {};

        return Hit(t, norm(cross(e1,e2)), &material);
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

void DrawReferenceSceneGL(const Scene & scene, const float3 & viewPosition, const float4 & viewOrientation, float aspectRatio);