#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "vectors.h"

#define INFINITY 10000
// ====================================================================
// ====================================================================

class Light {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Light() {}
  virtual ~Light() {}

  // VIRTUAL METHOD
  virtual void getIllumination (const Vec3f &p, Vec3f &dir, Vec3f &col, 
				float &distanceToLight) const = 0;

};

// ====================================================================
// ====================================================================

class DirectionalLight : public Light {

public:

  // CONSTRUCTOR & DESTRUCTOR
  DirectionalLight() { 
    direction = Vec3f(0,0,0);
    color = Vec3f(1,1,1); }
  DirectionalLight(const Vec3f &d, const Vec3f &c) {
    direction = d; direction.Normalize();
    color = c; }
  ~DirectionalLight() {}

  // VIRTUAL METHOD
  void getIllumination (const Vec3f &p, Vec3f &dir, Vec3f &col, 
			float &distanceToLight) const {
    // the direction to the light is the opposite of the
    // direction of the directional light source
    dir = direction * (-1.0f);
    col = color;
    // the distance is INFINITY
    distanceToLight = INFINITY; }

private:

  // REPRESENTATION
  Vec3f direction;
  Vec3f color;

};

// ====================================================================
// ====================================================================

class PointLight : public Light {

public:

  // CONSTRUCTOR & DESTRUCTOR
  PointLight() { 
    position = Vec3f(0,0,0);
    color = Vec3f(1,1,1); }
  PointLight(const Vec3f &p, const Vec3f &c, float a1, float a2, float a3) {
    position = p; 
    color = c; 
    attenuation_1 = a1;
    attenuation_2 = a2;
    attenuation_3 = a3;
    printf ("LIGHT  %f %f %f\n", attenuation_1,attenuation_2,attenuation_3);
 }
  ~PointLight() {}

  // VIRTUAL METHOD
  void getIllumination (const Vec3f &p, Vec3f &dir, Vec3f &col, float &distanceToLight) const {
    dir = position - p;
    // grab the length before the direction is normalized
    distanceToLight = dir.Length(); 
    dir.Normalize(); 
    float attenuation = 1 / (attenuation_1 + 
			     attenuation_2*distanceToLight + 
			     attenuation_3*distanceToLight*distanceToLight);
    //if (attenuation > 1) attenuation = 1;
    if (attenuation < 0) attenuation = 0;
    //printf ("attenuation (%f) %f %f %f   %f\n", distanceToLight, attenuation_1,attenuation_2,attenuation_3,attenuation);
    col = color * attenuation;
  }

private:

  // REPRESENTATION
  Vec3f position;
  Vec3f color;

  // attenuation of the light
  float attenuation_1;
  float attenuation_2;
  float attenuation_3;

};

// ====================================================================
// ====================================================================


#endif
