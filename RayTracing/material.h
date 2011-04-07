#include "vectors.h"

class Ray;
class Hit;

class Material{
public:
	Material(){}
	Material(const Vec3f &_diffuseColor,
		const Vec3f &_transparentColor,
		const Vec3f &_reflectiveColor,
		float _indexOfRefraction):diffuseColor(_diffuseColor),transparentColor(_transparentColor),reflectiveColor(_reflectiveColor),indexOfRefraction(_indexOfRefraction){}
	~Material(){}
	virtual Vec3f Shade(const Ray &ray,const Hit &hit,const Vec3f &dirToLight,const Vec3f &lightColor) const = 0;
	Vec3f getDiffuse(){ return diffuseColor;}
	Vec3f getReflect(){ return reflectiveColor;}
	Vec3f getTrans(){ return transparentColor;}
	float getIndexOfRefrac(){ return indexOfRefraction;}

	bool isReflect(){return (reflectiveColor.x()>0)||(reflectiveColor.y()>0)||(reflectiveColor.z()>0);}
	bool isTransparent(){return (transparentColor.x()>0)||(transparentColor.y()>0)||(transparentColor.z()>0);}
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