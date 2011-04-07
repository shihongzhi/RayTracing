#include "camera.h"
#include "math.h"
#include "ray.h"

#define PI 3.1415926

// size = [height,width]
OrthographicCamera::OrthographicCamera(Vec3f cen, Vec3f pro, Vec3f u, float si)
{
	center = cen;
	projection = pro;
	projection.Normalize();
	up = u;
	up.Normalize();
	Vec3f::Cross3(horizontal,projection,up);
	horizontal.Normalize();
	size = si;
}

//point (0,0)->(1,1)
Ray OrthographicCamera::generateRay(Vec2f point)
{
	Ray r;
	r.setOrigin(center+up*(point[0]-0.5)*size+horizontal*(point[1]-0.5)*size);
	projection.Normalize();
	r.setDirection(projection); //direction need normalize
	return r;
}

Ray PerspectiveCamera::generateRay(Vec2f point)
{
	Ray r;
	r.setOrigin(center);
	float fovScale = tan(angle*0.5) * 2;  //parser's angle is already a Radians
	up.Normalize();
	Vec3f uAdd = up;
	Vec3f rAdd;
	direction.Normalize();
	Vec3f::Cross3(rAdd,direction,uAdd);
	Vec3f rDirNormal = direction + uAdd*(point[0]-0.5)*fovScale + rAdd*(point[1]-0.5)*fovScale;  
	rDirNormal.Normalize();
	r.setDirection(rDirNormal);  //dirction need normalization

	return r;
}