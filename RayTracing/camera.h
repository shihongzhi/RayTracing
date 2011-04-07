#ifndef _CAMERA_H
#define _CAMERA_H

#include "ray.h"
#include "vectors.h"

class Camera{
public:
	Camera(){};
	~Camera(){};
	virtual Ray generateRay(Vec2f point) = 0; //一定要加上virtual，来说明这个函数由subclass实现，不然会出现error LNK2019
};

class OrthographicCamera : public Camera{
public:
	OrthographicCamera(Vec3f cen, Vec3f pro, Vec3f u, float si);
	~OrthographicCamera(){}
	Ray generateRay(Vec2f point);
private:
	Vec3f center;
	Vec3f projection;
	Vec3f up;
	Vec3f horizontal;
	float size;
};

class PerspectiveCamera : public Camera{
public:
	PerspectiveCamera(Vec3f &_center,Vec3f &_direction,Vec3f &_up,float &_angle){center = _center; direction = _direction; up = _up; angle = _angle;}
	~PerspectiveCamera(){}
	Ray generateRay(Vec2f point);
private:
	Vec3f center;
	Vec3f direction;
	Vec3f up;
	float angle;
};

#endif