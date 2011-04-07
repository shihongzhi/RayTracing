#include "material.h"
#include "vectors.h"
#include "math.h"
#include "ray.h"
#include "hit.h"

Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
	Vec3f difSpec;
	Vec3f r;
	r = hit.getNormal()*2*(hit.getNormal().Dot3(dirToLight)) - dirToLight;
	Vec3f::MultRow(difSpec,(diffuseColor*(hit.getNormal().Dot3(dirToLight))) + (specularColor*pow(ray.getDirection().Dot3(r)*(-1.0f),exponent)),lightColor);  //之前这里没有乘以-1.0
	return difSpec;
}
