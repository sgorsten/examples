#include "raytrace.h"
#include <algorithm>

float3 DirectionalLight::ComputeContribution(const Hit & hit, const float3 & eyeDir) const
{
    auto halfDir = norm(direction + eyeDir);
    float diffuseTerm = std::max(dot(hit.normal, direction), 0.0f);   
    float specularTerm = std::pow(std::max(dot(hit.normal, halfDir), 0.0f), 64.0f);
    return hit.material->albedo * color * (diffuseTerm + specularTerm);
}

float3 Scene::ComputeLighting(const Hit & hit, const float3 & viewPosition) const
{
    auto light = hit.material->albedo * ambientLight;

    bool lit = true;
    Ray ray = {hit.point, dirLight.direction};
    for(auto & sphere : spheres)
    {
        if(hit.material == &sphere.material) continue;
        auto hit = sphere.Intersect(ray);
        if(hit.IsHit())
        {
            lit = false;
            break;
        }        
    }
    if(lit)
    {
        auto eyeDir = norm(viewPosition - hit.point);
        light += dirLight.ComputeContribution(hit, eyeDir);
    }
    return light;
}