#ifndef _SceneParser_H_
#define _SceneParser_H_

#include "vectors.h"

class Camera;
class Light;
class Object3D;
class Group;
class Sphere;
class Plane;
class Triangle;
class Transform;
class Material;
class Grid;
//class Box;

#define MAX_PARSER_TOKEN_LENGTH 100

// ====================================================================
// ====================================================================

class SceneParser {

public:

  // CONSTRUCTORS, DESTRUCTOR & INITIALIZE
  SceneParser();
  SceneParser(const char *filename);
  ~SceneParser();

  // ACCESSORS
  Camera* getCamera() { return camera; }
  Vec3f getBackgroundColor() { return background_color; }
  Vec3f getAmbientLight() { return ambient_light; }
  int getNumLights() { return num_lights; }
  Light* getLight(int i) { 
    assert(i >= 0 && i < num_lights); 
    return lights[i]; }  
  int getNumMaterials() { return num_materials; }
  Material* getMaterial(int i) { 
    assert(i >= 0 && i < num_materials); 
    return materials[i]; }  
  Group* getGroup() { return group; }

private:

  // HELPER FUNCTIONS
  void initialize();
  int getToken(char token[MAX_PARSER_TOKEN_LENGTH]);
  Vec3f readVec3f();
  float readFloat();
  int readInt();

  // PARSING
  void parseFile();
  void parseOrthographicCamera();
  void parsePerspectiveCamera();
  void parseBackground();
  void parseLights();
  void parseMaterials();
  void parseMaterialIndex();

  Object3D* parseObject(char token[MAX_PARSER_TOKEN_LENGTH]);
  Group* parseGroup();
  Sphere* parseSphere();
  Plane* parsePlane();
  Triangle* parseTriangle();
  Transform* parseTransform();
  Grid* parseGrid();
  //Box* parseBox();
  Group* parseTriangleMesh();

  // REPRESENTATION
  Group *group;
  Camera *camera;
  Vec3f background_color;
  Vec3f ambient_light;
  FILE *file;
  Light **lights;
  Material **materials;
  int num_lights;
  int num_materials;
  Material *current_material;

};

// ====================================================================
// ====================================================================

#endif
