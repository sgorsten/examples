#include "raytrace.h"
#include <algorithm>

float3 DirectionalLight::ComputeContribution(const Hit & hit) const
{
    float diffuseTerm = std::max(dot(hit.normal, direction), 0.0f);
    return hit.material->albedo * color * diffuseTerm;
}

float3 Scene::ComputeLighting(const Hit & hit) const
{
    return hit.material->albedo * ambientLight + dirLight.ComputeContribution(hit);
}