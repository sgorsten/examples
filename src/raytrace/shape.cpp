#include "raytrace.h"

Hit Sphere::Intersect(const Ray & ray) const
{
    // ray.direction must be of unit length
    auto delta = position - ray.origin;
    float b = dot(ray.direction, delta), disc = b*b + radius*radius - mag2(delta);
    if(disc < 0) return {};
    float t = b - sqrt(disc);
    if(t <= 0) return {};
    return Hit(t, (ray.direction * t - delta) / radius, &material);
}