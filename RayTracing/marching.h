#ifndef _MARCHING_H_
#define _MARCHING_H_

#include "vectors.h"

class MarchingInfo{
public:
	MarchingInfo(){};
	~MarchingInfo(){};
	void Set(float _tmin, int _i,int _j,int _k,float _tnext_x,float _tnext_y,float _tnext_z,float _dtx,float _dty,float _dtz,int _signx,int _signy,int _signz,Vec3f n)
	{
		tmin = _tmin; i = _i; j = _j; k = _k; tnext_x = _tnext_x; tnext_y = _tnext_y; tnext_z = _tnext_z; 
		dtx = _dtx; dty = _dty; dtz = _dtz; signx = _signx; signy = _signy; signz = _signz;  normal = n;
	}
	void nextCell();
	int GetI(){ return i;}
	int GetJ(){ return j;}
	int GetK(){ return k;}
	float GetT(){ return tmin;}
	float GetNext_x(){ return tnext_x;}
	float GetNext_y(){ return tnext_y;}
	float GetNext_z(){ return tnext_z;}
	Vec3f GetNormal(){ return normal;}
private:
	float tmin;
	int i,j,k;
	float tnext_x,tnext_y,tnext_z;
	float dtx,dty,dtz;
	int signx,signy,signz;
	Vec3f normal;
};

#endif