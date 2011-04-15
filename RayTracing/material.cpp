#include <math.h>
#include "material.h"
#include "vectors.h"
#include "math.h"
#include "ray.h"
#include "hit.h"
#include "matrix.h"
#include "perlin_noise.h"

Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
	Vec3f difSpec;
	Vec3f r;
	r = hit.getNormal()*2*(hit.getNormal().Dot3(dirToLight)) - dirToLight;
	Vec3f::MultRow(difSpec,(diffuseColor*(hit.getNormal().Dot3(dirToLight))) + (specularColor*pow(ray.getDirection().Dot3(r)*(-1.0f),exponent)),lightColor);  //之前这里没有乘以-1.0
	return difSpec;
}

Vec3f Checkerboard::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
	Vec3f hitpoint = hit.getIntersectionPoint();
	m->Transform(hitpoint);
	int i,j,k;
	i = floor(hitpoint.x());
	j = floor(hitpoint.y());
	k = floor(hitpoint.z());
	if((i+j+k)%2 == 0)
	{
		Hit tempHit(hit.getT(),mat1,hit.getNormal());
		return mat1->Shade(ray,tempHit,dirToLight,lightColor);
	}
	else
	{
		Hit tempHit(hit.getT(),mat2,hit.getNormal());
		return mat2->Shade(ray,tempHit,dirToLight,lightColor);
	}
}

Vec3f Checkerboard::getDiffuse(Vec3f point)
{
	Vec3f tranPoint = point;
	m->Transform(tranPoint);
	int i,j,k;
	i = floor(tranPoint.x());
	j = floor(tranPoint.y());
	k = floor(tranPoint.z());
	if((i+j+k)%2 == 0)
		return mat1->getDiffuse(point);
	else
		return mat2->getDiffuse(point);
}

float Checkerboard::getIndexOfRefrac(Vec3f point)
{
	Vec3f tranPoint = point;
	m->Transform(tranPoint);
	int i,j,k;
	i = floor(tranPoint.x());
	j = floor(tranPoint.y());
	k = floor(tranPoint.z());
	if((i+j+k)%2 == 0)
		return mat1->getIndexOfRefrac(point);
	else
		return mat2->getIndexOfRefrac(point);
}

Vec3f Checkerboard::getReflect(Vec3f point)
{
	Vec3f tranPoint = point;
	m->Transform(tranPoint);
	int i,j,k;
	i = floor(tranPoint.x());
	j = floor(tranPoint.y());
	k = floor(tranPoint.z());
	if((i+j+k)%2 == 0)
		return mat1->getReflect(point);
	else
		return mat2->getReflect(point); 
}

Vec3f Checkerboard::getTrans(Vec3f point)
{
	Vec3f tranPoint = point;
	m->Transform(tranPoint);
	int i,j,k;
	i = floor(tranPoint.x());
	j = floor(tranPoint.y());
	k = floor(tranPoint.z());
	if((i+j+k)%2 == 0)
		return mat1->getTrans(point);
	else
		return mat2->getTrans(point);  
}

bool Checkerboard::isReflect(Vec3f point)
{
	Vec3f temp = getReflect(point);
	return ((temp.x()>0)||(temp.y()>0)||(temp.z()>0));
}

bool Checkerboard::isTransparent(Vec3f point)
{
	Vec3f temp = getTrans(point);
	return ((temp.x()>0)||(temp.y()>0)||(temp.z()>0));
}

double Noise::caculateNoise(Vec3f point) const
{
	Vec3f tranPoint = point;
	m->Transform(tranPoint);
	double tempInter = 0;
	int tempOctaves = octaves;
	int count = 1;
	while(tempOctaves>0)
	{
		tempInter += ((PerlinNoise::noise(tranPoint.x()*count,tranPoint.y()*count,tranPoint.z()*count))/count);
		count *= 2;
		tempOctaves--;
	}
	return fabs(tempInter);
}

Vec3f Noise::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
	double tempInter = caculateNoise(hit.getIntersectionPoint());

	Hit tempHit1(hit.getT(),mat1,hit.getNormal());
	Hit tempHit2(hit.getT(),mat2,hit.getNormal());
	return (mat1->Shade(ray,tempHit1,dirToLight,lightColor)*tempInter + mat2->Shade(ray,tempHit2,dirToLight,lightColor)*(1-tempInter));
}

Vec3f Noise::getDiffuse(Vec3f point)
{
	double tempInter = caculateNoise(point);
	return (mat1->getDiffuse(point)*tempInter + mat2->getDiffuse(point)*(1-tempInter));
}

Vec3f Noise::getReflect(Vec3f point)
{
	double tempInter = caculateNoise(point);
	return (mat1->getReflect(point)*tempInter + mat2->getReflect(point)*(1-tempInter));
}

Vec3f Noise::getTrans(Vec3f point)
{
	double tempInter = caculateNoise(point);
	return (mat1->getTrans(point)*tempInter + mat2->getTrans(point)*(1-tempInter));
}

float Noise::getIndexOfRefrac(Vec3f point)
{
	double tempInter = caculateNoise(point);
	float temp = mat1->getIndexOfRefrac(point)*tempInter + mat2->getIndexOfRefrac(point)*(1-tempInter);
	return temp;
}

bool Noise::isReflect(Vec3f point)
{
	Vec3f temp = getReflect(point);
	return ((temp.x()>0)||(temp.y()>0)||(temp.z()>0));
}

bool Noise::isTransparent(Vec3f point)
{
	Vec3f temp = getTrans(point);
	return ((temp.x()>0)||(temp.y()>0)||(temp.z()>0));
}

double Marble::caculateMarble(Vec3f point) const
{
	Vec3f tranPoint = point;
	m->Transform(tranPoint);
	double tempInter = 0;
	int tempOctaves = octaves;
	int count = 1;
	while(tempOctaves>0)
	{
		tempInter += ((PerlinNoise::noise(tranPoint.x()*count,tranPoint.y()*count,tranPoint.z()*count))/count);
		count *= 2;
		tempOctaves--;
	}
	return fabs(sin(frequency*tranPoint.x() + amplitude*tempInter));
}

Vec3f Marble::getDiffuse(Vec3f point)
{
	double tempInter = caculateMarble(point);
	return (mat1->getDiffuse(point)*tempInter + mat2->getDiffuse(point)*(1-tempInter));
}

Vec3f Marble::getReflect(Vec3f point)
{
	double tempInter = caculateMarble(point);
	return (mat1->getReflect(point)*tempInter + mat2->getReflect(point)*(1-tempInter));
}

Vec3f Marble::getTrans(Vec3f point)
{
	double tempInter = caculateMarble(point);
	return (mat1->getTrans(point)*tempInter + mat2->getTrans(point)*(1-tempInter));
}

float Marble::getIndexOfRefrac(Vec3f point)
{
	double tempInter = caculateMarble(point);
	return (mat1->getIndexOfRefrac(point)*tempInter + mat2->getIndexOfRefrac(point)*(1-tempInter));
}

bool Marble::isReflect(Vec3f point)
{
	Vec3f temp = getReflect(point);
	return ((temp.x()>0)||(temp.y()>0)||(temp.z()>0));
}

bool Marble::isTransparent(Vec3f point)
{
	Vec3f temp = getTrans(point);
	return ((temp.x()>0)||(temp.y()>0)||(temp.z()>0));
}

Vec3f Marble::Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const
{
	double tempInter = caculateMarble(hit.getIntersectionPoint());

	Hit tempHit1(hit.getT(),mat1,hit.getNormal());
	Hit tempHit2(hit.getT(),mat2,hit.getNormal());
	return (mat1->Shade(ray,tempHit1,dirToLight,lightColor)*tempInter + mat2->Shade(ray,tempHit2,dirToLight,lightColor)*(1-tempInter));
}

Wood::Wood(Matrix *_m, Material *_mat1, Material *_mat2, int _octaves, float _frequency, float _amplitude)
{
	m = _m;
	mat1 = _mat1;
	mat2 = _mat2;
	octaves = _octaves;
	frequency = _frequency;
	amplitude = _amplitude;
	ringWidth = 0.15f;
	grainWidth = 0.10f;
}

bool Wood::isMat1(Vec3f point) const
{
	Vec3f tempPoint = point;
	m->Transform(tempPoint);
	float r = sqrt(tempPoint.x()*tempPoint.x()+tempPoint.y()*tempPoint.y());
	float d = r / ringWidth;
	d = (d - ((int)d))*ringWidth;
	if(d < grainWidth)
		return true;
	else
		return false;
}

Vec3f Wood::getDiffuse(Vec3f point)
{
	if(isMat1(point))
		return mat1->getDiffuse(point);
	else
		return mat2->getDiffuse(point);
}

Vec3f Wood::getReflect(Vec3f point)
{
	if(isMat1(point))
		return mat1->getReflect(point);
	else
		return mat2->getReflect(point);
}

Vec3f Wood::getTrans(Vec3f point)
{
	if(isMat1(point))
		return mat1->getTrans(point);
	else
		return mat2->getTrans(point);
}

float Wood::getIndexOfRefrac(Vec3f point)
{
	if(isMat1(point))
		return mat1->getIndexOfRefrac(point);
	else
		return mat2->getIndexOfRefrac(point);
}

bool Wood::isReflect(Vec3f point)
{
	if(isMat1(point))
		return mat1->isReflect(point);
	else
		return mat2->isReflect(point);
}

bool Wood::isTransparent(Vec3f point)
{
	if(isMat1(point))
		return mat1->isReflect(point);
	else
		return mat2->isReflect(point);
}

Vec3f Wood::Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const
{
	if(isMat1(hit.getIntersectionPoint()))
	{
		Hit tempHit(hit.getT(),mat1,hit.getNormal());
		return mat1->Shade(ray,tempHit,dirToLight,lightColor);
	}
	else
	{
		Hit tempHit(hit.getT(),mat2,hit.getNormal());
		return mat2->Shade(ray,tempHit,dirToLight,lightColor);
	}
}
