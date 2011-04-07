#include "scene_parser.h"

class Ray;
class Vec3f;
class Hit;

class RayTracer{
public:
	RayTracer(SceneParser *_s, int _max_bounces):s(_s),max_bounces(_max_bounces){}
	~RayTracer(){}
	Vec3f traceRay(Ray &ray,float tmin,int bounces,float indexOfRefraction,Hit &hit,Grid *grid) const;
	Vec3f mirrorDirection(const Vec3f &normal,const Vec3f &incoming) const;  //Ҫ����const ��Ȼ�����error�����ܽ���this��ָ��ӡ�const RayTracer��ת��Ϊ��RayTracer &��
	bool transmittedDirection(const Vec3f &normal,const Vec3f &incoming,float index_i,float index_t,Vec3f &transmitted) const;
	//return true if intersection something 
	bool castShadowRay(Vec3f point,Vec3f lightDir,float distanceToLight) const;
	bool castShadowRayGrid(Vec3f point,Vec3f lightDir,float distanceToLight,Grid *grid) const;
private:
	SceneParser *s;
	int max_bounces;
	//float min_weight;
};