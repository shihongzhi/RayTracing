#include "RayTracer.h"
#include "primitives.h"
#include "light.h"
#include "hit.h"
#include "material.h"
#include "raytracing_stats.h"

#define EPSILON	  0.0001f

//没用 weight
Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float indexOfRefraction, Hit &hit, Grid *grid) const
{
	if(bounces >= max_bounces)
		return Vec3f(0,0,0);
	RayTracingStats::IncrementNumNonShadowRays();
	//Group *group = s->getGroup();
	//group->intersect(ray,hit,tmin);  这里是没用grid的代码
	grid->intersect(ray,hit,tmin);
	
	if(hit.getMaterial()==NULL)
		return s->getBackgroundColor();
	else
	{
		RayTracingStats::IncrementNumShadowRays();
		Vec3f col(0,0,0);
		Vec3f hitPoint = hit.getIntersectionPoint();
		Vec3f tempAmb;
		Vec3f::MultRow(tempAmb,s->getAmbientLight(),hit.getMaterial()->getDiffuse(hitPoint));  //Kd La
		col += tempAmb;
		int lightNumber = s->getNumLights();
		Light *light;
		for(int i=0; i<lightNumber; i++)
		{
			light = s->getLight(i);
			Vec3f lightColor;
			Vec3f dirToLight;
			//Vec3f interPoint = hit.getIntersectionPoint();
			float distanceToLight;
			light->getIllumination(hitPoint,dirToLight,lightColor,distanceToLight);
			if(!castShadowRayGrid(hitPoint-ray.getDirection()*EPSILON,dirToLight,distanceToLight,grid))
			{
				Vec3f tempShade = hit.getMaterial()->Shade(ray,hit,dirToLight,lightColor);  //diffuse specular
				col += tempShade;
			}
		}
		if(hit.getMaterial()->isReflect(hitPoint))  
		{
			Ray rayReflect(mirrorDirection(hit.getNormal(),ray.getDirection()),hitPoint);
			Vec3f tempRefl;
			Hit hit2(1000,NULL,Vec3f(1,1,1));
			Vec3f::MultRow(tempRefl,hit.getMaterial()->getReflect(hitPoint),traceRay(rayReflect,tmin,bounces+1,indexOfRefraction,hit2,grid)); //weight,indexOfRefrection
			col += tempRefl;
		}
		if(hit.getMaterial()->isTransparent(hitPoint))
		{
			bool notTotalReflective;
			Vec3f transmittedDir;
			if(ray.getDirection().Dot3(hit.getNormal())>0) //ray is inside the object
			{
				notTotalReflective = transmittedDirection(hit.getNormal()*(-1.0f),ray.getDirection(),hit.getMaterial()->getIndexOfRefrac(hitPoint),indexOfRefraction,transmittedDir);
			}
			else  //outside
			{
				notTotalReflective = transmittedDirection(hit.getNormal(),ray.getDirection(),indexOfRefraction,hit.getMaterial()->getIndexOfRefrac(hitPoint),transmittedDir);
			}
			
			if(notTotalReflective)
			{
				Ray rayTransparent(transmittedDir,hitPoint);
				Vec3f tempTrans;
				Hit hit3(10000,NULL,Vec3f(1,1,1));
				Vec3f::MultRow(tempTrans,hit.getMaterial()->getTrans(hitPoint),traceRay(rayTransparent,tmin,bounces+1,indexOfRefraction,hit3,grid));
				col += tempTrans;
			}
			else
			{
				Vec3f tempTotalTrans;
				Vec3f::MultRow(tempTotalTrans,hit.getMaterial()->getTrans(hitPoint),hit.getMaterial()->getDiffuse(hitPoint));
				col += tempTotalTrans;
			}
		}
		return col;
	}
}

Vec3f RayTracer::mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const
{
	Vec3f reflectionDir;
	reflectionDir = incoming - normal * (incoming.Dot3(normal)) * 2;
	reflectionDir.Normalize();
	return reflectionDir;
}

//何时返回true？非全反射时 何时返回false
bool RayTracer::transmittedDirection(const Vec3f &normal, const Vec3f &incoming, float index_i, float index_t, Vec3f &transmitted) const
{
	float nr = index_i / index_t;
	Vec3f I = incoming*(-1.0f);
	float cosI = I.Dot3(normal);
	float isAllTrans = 1 - nr*nr*(1 - cosI*cosI);
	if(isAllTrans < 0)  //全反射
		return false;

	float cosT = sqrt(isAllTrans);
	transmitted = normal*(nr*cosI - cosT) - I*nr;
	transmitted.Normalize();
	return true;
}

//non-accelerate castShadowRay
bool RayTracer::castShadowRay(Vec3f point, Vec3f lightDir, float distanceToLight) const
{
	Group *group = s->getGroup();
	int numOfPrim = group->getNumber();
	Object3D* instance;
	Ray r(lightDir,point);

	Material* material =NULL;
	Hit hit(distanceToLight,material,Vec3f(1,1,1));
	for(int i=0; i<numOfPrim; i++)
	{
		instance = group->getObject(i);
		if(instance->intersect(r,hit,0) == 1) //设置tmin为EPSILON
		{
			return 1;
		}
	}
	//没有相交
	return 0;
}

//grid castShadowRay
bool RayTracer::castShadowRayGrid(Vec3f point, Vec3f lightDir, float distanceToLight, Grid *grid) const
{
	Ray r(lightDir,point);
	Hit hit(distanceToLight,NULL,Vec3f(1,1,1));
	return grid->shadowIntersect(r,hit,EPSILON);  //又是因为这里没加EPSILON，又出现了影子的乱点情况
}