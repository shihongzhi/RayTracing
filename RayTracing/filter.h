#ifndef _FILTER_H_
#define _FILTER_H_

#include "vectors.h"

class Film;

class Filter
{
public:
	Filter(){}
	~Filter(){}
	virtual Vec3f getColor(int i,int j,Film *film);
	virtual float getWeight(float x,float y) = 0;
	virtual int getSupportRadius() = 0;
};

class BoxFilter : public Filter
{
public:
	BoxFilter(float _radius):radius(_radius){}
	~BoxFilter(){}
	float getWeight(float x,float y);
	int getSupportRadius();
private:
	float radius;
};


class TentFilter : public Filter
{
public:
	TentFilter(float _radius):radius(_radius){}
	~TentFilter(){}
	float getWeight(float x,float y);
	int getSupportRadius();
private:
	float radius;
};


class GaussionFilter : public Filter
{
public:
	GaussionFilter(float _sigma):sigma(_sigma){}
	~GaussionFilter(){}
	float getWeight(float x,float y);
	int getSupportRadius();
private:
	float sigma;
};
#endif