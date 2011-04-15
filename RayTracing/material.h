#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "vectors.h"

class Ray;
class Hit;
class Matrix;

class Material{
public:
	Material(){}
	Material(const Vec3f &_diffuseColor,
		const Vec3f &_transparentColor,
		const Vec3f &_reflectiveColor,
		float _indexOfRefraction):diffuseColor(_diffuseColor),transparentColor(_transparentColor),reflectiveColor(_reflectiveColor),indexOfRefraction(_indexOfRefraction){}
	~Material(){}
	virtual Vec3f Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const = 0;
	virtual Vec3f getDiffuse(Vec3f point){ return diffuseColor;}
	virtual Vec3f getReflect(Vec3f point){ return reflectiveColor;}
	virtual Vec3f getTrans(Vec3f point){ return transparentColor;}
	virtual float getIndexOfRefrac(Vec3f point){ return indexOfRefraction;}

	virtual bool isReflect(Vec3f point){return ((reflectiveColor.x()>0)||(reflectiveColor.y()>0)||(reflectiveColor.z()>0));}
	virtual bool isTransparent(Vec3f point){return ((transparentColor.x()>0)||(transparentColor.y()>0)||(transparentColor.z()>0));}
protected:
	Vec3f diffuseColor; //Âþ·´Éä
	Vec3f transparentColor; //refraction
	Vec3f reflectiveColor;  //mirror reflect
	float indexOfRefraction; //
};

class PhongMaterial : public Material{
public:
	PhongMaterial(){}
	PhongMaterial(const Vec3f &_diffuseColor,
		const Vec3f &_specularColor,
		float _exponent,
		const Vec3f &_transparentColor,
		const Vec3f &_reflectiveColor,
		float _indexOfRefraction){diffuseColor =_diffuseColor; specularColor = _specularColor; exponent=_exponent; transparentColor=_transparentColor; reflectiveColor=_reflectiveColor; indexOfRefraction=_indexOfRefraction;}
	~PhongMaterial(){}
	Vec3f Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const;
private:
	Vec3f specularColor;	//
	float exponent;
};

class Checkerboard : public Material{
public:
	Checkerboard(Matrix *_m ,Material *_mat1, Material *_mat2){ m=_m; mat1=_mat1; mat2=_mat2; }
	~Checkerboard(){}
	Vec3f Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const;
	Vec3f getDiffuse(Vec3f point);
	Vec3f getReflect(Vec3f point);
	Vec3f getTrans(Vec3f point);
	float getIndexOfRefrac(Vec3f point);
	bool isReflect(Vec3f point);
	bool isTransparent(Vec3f point);
private:
	Matrix* m;
	Material *mat1;
	Material *mat2;
};

class Noise : public Material{
public:
	Noise(Matrix *_m,Material *_mat1,Material *_mat2,int _octaves):m(_m),mat1(_mat1),mat2(_mat2),octaves(_octaves){}
	Vec3f Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const;
	Vec3f getDiffuse(Vec3f point);
	Vec3f getReflect(Vec3f point);
	Vec3f getTrans(Vec3f point);
	float getIndexOfRefrac(Vec3f point);
	bool isReflect(Vec3f point);
	bool isTransparent(Vec3f point);
private:
	double caculateNoise(Vec3f point) const;
	Matrix *m;
	Material *mat1;
	Material *mat2;
	int octaves;
};

class Marble : public Material{
public:
	Marble(Matrix *_m, Material *_mat1, Material *_mat2, int _octaves, float _frequency, float _amplitude):m(_m),mat1(_mat1),mat2(_mat2),octaves(_octaves),frequency(_frequency),amplitude(_amplitude){}
	Vec3f Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const;
	Vec3f getDiffuse(Vec3f point);
	Vec3f getReflect(Vec3f point);
	Vec3f getTrans(Vec3f point);
	float getIndexOfRefrac(Vec3f point);
	bool isReflect(Vec3f point);
	bool isTransparent(Vec3f point);
private:
	double caculateMarble(Vec3f point) const;
	Matrix *m;
	Material *mat1;
	Material *mat2;
	int octaves;
	float frequency;
	float amplitude;
};

class Wood : public Material{
public:
	Wood(Matrix *_m, Material *_mat1, Material *_mat2, int _octaves, float _frequency, float _amplitude);
	Vec3f Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const;
	Vec3f getDiffuse(Vec3f point);
	Vec3f getReflect(Vec3f point);
	Vec3f getTrans(Vec3f point);
	float getIndexOfRefrac(Vec3f point);
	bool isReflect(Vec3f point);
	bool isTransparent(Vec3f point);
private:
	bool isMat1(Vec3f point) const;
	Matrix *m;
	Material *mat1;
	Material *mat2;
	int octaves;
	float frequency;
	float amplitude;
	float ringWidth;
	float grainWidth;
};

#endif