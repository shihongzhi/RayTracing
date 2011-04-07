#ifndef _PRIMITIVES_H
#define _PRIMITIVES_H

//#include "hit.h"
//#include "ray.h"
#include "vectors.h"
#include "matrix.h"
#include "object3dvector.h"

class Hit;
class Ray;
class Material;
class MarchingInfo;


class Object3D{
public:
	Object3D():material(NULL){}
	~Object3D(){}
	virtual bool intersect(const Ray &r,Hit &h,float tmin) = 0;
	Material* getMaterial(){ return material;}
	void setMaterial(Material* m){ material = m;}
	Vec3f getMax(){ return max;}
	Vec3f getMin(){ return min;}
protected:
	Material *material;
	Vec3f min;
	Vec3f max;
};

class Sphere : public Object3D{
public:
	Sphere(Vec3f &cen, float ra, Material *m){ center = cen; radius = ra; material = m; min = Vec3f(center.x()-radius,center.y()-radius,center.z()-radius); max = Vec3f(center.x()+radius,center.y()+radius,center.z()+radius);}
	~Sphere(){}
	virtual bool intersect(const Ray &r,Hit &h,float tmin);
private:
	Vec3f center;
	float radius;
};

//plane's min and max is infinite
class Plane : public Object3D{
public:
	Plane(Vec3f &_normal,float _d, Material *m){_normal.Normalize(); normal = _normal; d = _d; material = m;}
	~Plane(){}
	virtual bool intersect(const Ray &r,Hit &h,float tmin);
private:
	Vec3f normal;
	float d;
};

class Triangle : public Object3D{
public:
	Triangle(Vec3f &_a, Vec3f &_b, Vec3f &_c, Material *m);  //起初这里求normal错误了
	~Triangle(){}
	virtual bool intersect(const Ray &r,Hit &h,float tmin);
	Vec3f getA(){ return a;}
	Vec3f getB(){ return b;}
	Vec3f getC(){ return c;}
private:
	Vec3f a;
	Vec3f b;
	Vec3f c;
	Vec3f normal;
	int i0;
	float d;
};

class Transform : public Object3D{
public:
	Transform(Matrix &_m,Object3D *o);
	Transform(){}
	virtual bool intersect(const Ray &r,Hit &h,float tmin);
private:
	Matrix m;
	Object3D *instance;
};

class Grid : public Object3D{
public:
	Grid(Vec3f &_min, Vec3f &_max, int _nx, int _ny, int _nz);
	~Grid();
	void rasterizeSphere(Vec3f center, float radius){}
	void initialRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;
	virtual bool intersect(const Ray &r,Hit &h,float tmin);
	bool shadowIntersect(const Ray &r,Hit &h,float tmin);
	//bool*** GetisOpaque(){ return isOpaque;}
	int GetNx(){ return nx;}
	int GetNy(){ return ny;}
	int GetNz(){ return nz;}
	float GetDx(){ return dx;}
	float GetDy(){ return dy;}
	float GetDz(){ return dz;}
	Object3DVector& GetObject3d(int i,int j,int k){ return isOpaque[i][j][k];}
	void SetObject3d(Object3DVector o,int i,int j,int k){ isOpaque[i][j][k] = o;}
private:
	int nx,ny,nz;
	float dx,dy,dz;
	Object3DVector ***isOpaque;
};

class Group : public Object3D{
public:
	Group(int nu);
	~Group(){}
	virtual bool intersect(const Ray &r,Hit &h,float tmin);
	void addObject(int index, Object3D *obj);
	int getNumber(){return number;}
	Object3D* getObject(int id){return instances[id];}
	void interObjToGrid(Grid* grid);
	void calutateMinMax();  //加完object之后 计算min和max
private:
	int number;
	Object3D** instances;
};

#endif