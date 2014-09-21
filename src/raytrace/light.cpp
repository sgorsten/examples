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
    if(!CheckOcclusion({hit.point, dirLight.direction}, hit.material))
    {
        auto eyeDir = norm(viewPosition - hit.point);
        light += dirLight.ComputeContribution(hit, eyeDir);
    }
    return light;
}