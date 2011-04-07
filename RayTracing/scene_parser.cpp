#include <stdio.h>

#include <string.h>

#include "vectors.h"
#include "matrix.h"
#include "scene_parser.h"

#include "camera.h" 
#include "light.h"
#include "primitives.h"
#include "material.h"

//#include "box.h"

#define M_PI 3.1415926
#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

// ====================================================================
// ====================================================================
// CONSTRUCTORS, DESTRUCTOR & INITIALIZE

SceneParser::SceneParser() {
  initialize();
}

SceneParser::SceneParser(const char* filename) {
  initialize();

  // open the file
  assert(filename != NULL);
  const char *ext = &filename[strlen(filename)-4];
  assert(!strcmp(ext,".txt"));
  file = fopen(filename,"r");
  assert (file != NULL);

  // parse the scene
  parseFile();

  // close the file
  fclose(file); 
  file = NULL;
}

SceneParser::~SceneParser() {
  if (group != NULL) 
    delete group;
  if (camera != NULL) 
    delete camera;
}

void SceneParser::initialize() {
  // initialize some reasonable default values
  group = NULL;
  camera = NULL;
  background_color = Vec3f(0.5,0.5,0.5);
  lights = NULL;
  num_lights = 0;
  materials = NULL;
  num_materials = 0;
  current_material = NULL;
  file = NULL;
}

// ====================================================================
// ====================================================================

void SceneParser::parseFile() {
  //
  // at the top level, the scene can have a camera, 
  // background color and a group of objects
  // (we will add lights and other things in future assignments)
  //
  char token[MAX_PARSER_TOKEN_LENGTH];    
  while (getToken(token)) { 
    if (!strcmp(token, "OrthographicCamera")) {
      parseOrthographicCamera();
    } else if (!strcmp(token, "PerspectiveCamera")) {
      parsePerspectiveCamera();
    } else if (!strcmp(token, "Lights")) {
      parseLights();
    } else if (!strcmp(token, "Materials")) {
      parseMaterials();
    } else if (!strcmp(token, "Background")) {
      parseBackground();
    } else if (!strcmp(token, "Group")) {
      group = parseGroup();
    } else {
      printf ("Unknown token in parseFile: '%s'\n", token);
      exit(0);
    }
  }
}

Group* SceneParser::parseGroup() {
  //
  // each group starts with an integer that specifies
  // the number of objects in the group
  //
  // the material node sets the color of all objects which 
  // follow, until the next material node
  //
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token); assert (!strcmp(token, "{"));

  // read in the number of objects
  getToken(token); assert (!strcmp(token, "numObjects"));
  int num_objects = readInt();

  Group *answer = new Group(num_objects);

  // read in the objects
  int count = 0;
  while (num_objects > count) {
    getToken(token); 
    if (!strcmp(token, "MaterialIndex")) {
      // materials don't count
      int index = readInt();
      assert (index >= 0 && index <= getNumMaterials());
      // change the current material
      // scoping for the materials is very simplistic,
      // and essentially ignores any tree hierarchy
      current_material = getMaterial(index);
    } else {
      // everything else does count
      assert (current_material != NULL);
      Object3D *object = parseObject(token);
      assert (object != NULL);
      answer->addObject(count,object);
      count++;
    }
  }
  getToken(token); assert (!strcmp(token, "}"));
  
  //计算 min和max
  answer->calutateMinMax();
  // return the group
  return answer;
}


Object3D* SceneParser::parseObject(char token[MAX_PARSER_TOKEN_LENGTH]) {
  //printf ("PARSE OBJECT %s\n", token);
  Object3D *answer = NULL;
  if (!strcmp(token, "Group")) {      
    answer = (Object3D*)parseGroup();
  } else if (!strcmp(token, "Sphere")) {      
    answer = (Object3D*)parseSphere();
  } else if (!strcmp(token, "Box")) {      
    //answer = (Object3D*)parseBox();
  } else if (!strcmp(token, "Plane")) {      
    answer = (Object3D*)parsePlane();
  } else if (!strcmp(token, "Triangle")) {      
    answer = (Object3D*)parseTriangle();
  } else if (!strcmp(token, "TriangleMesh")) {      
    answer = (Object3D*)parseTriangleMesh();
  } else if (!strcmp(token, "Transform")) {      
    answer = (Object3D*)parseTransform();
  } else if (!strcmp(token, "Grid")) {      
    answer = (Object3D*)parseGrid();
  } else {
    printf ("Unknown token in parseObject: '%s'\n", token);
    exit(0);
  }
  return answer;
}


// ====================================================================
// ====================================================================

void SceneParser::parseLights() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token); assert (!strcmp(token, "{"));

  // read in the number of objects
  getToken(token); assert (!strcmp(token, "numLights"));
  num_lights = readInt();
  lights = new Light*[num_lights];

  // read in the objects
  int count = 0;
  while (num_lights > count) {
    getToken(token); 
    if (!strcmp(token, "DirectionalLight")) {
      getToken(token); assert (!strcmp(token, "{"));
      getToken(token); assert (!strcmp(token, "direction"));
      Vec3f direction = readVec3f();
      getToken(token); assert (!strcmp(token, "color"));
      Vec3f color = readVec3f();
      getToken(token); assert (!strcmp(token, "}"));
      lights[count] = new DirectionalLight(direction,color);
      count++;
    } else if (!strcmp(token, "PointLight")) {
      getToken(token); assert (!strcmp(token, "{"));
      getToken(token); assert (!strcmp(token, "position"));
      Vec3f position = readVec3f();
      getToken(token); assert (!strcmp(token, "color"));
      Vec3f color = readVec3f();
      float attenuation_1 = 1;
      float attenuation_2 = 0;
      float attenuation_3 = 0;
      while(1) {
	getToken(token); 
	if (!strcmp(token,"attenuation")) {
	  attenuation_1 = readFloat();
	  attenuation_2 = readFloat();
	  attenuation_3 = readFloat();
	} else if (!strcmp(token, "}")) {
	  break; 
	} else {
	  printf ("Unknown token in parsePointLight: '%s'\n", token);
	  exit(0);
	}
      }
      lights[count] = new PointLight(position,color,attenuation_1,attenuation_2,attenuation_3);
      count++;
    } else {
      printf ("Unknown token in parseGroup: '%s'\n", token);
      exit(0);
    }   
  }
  getToken(token); assert (!strcmp(token, "}"));
}

void SceneParser::parseMaterials() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token); assert (!strcmp(token, "{"));

  // read in the number of objects
  getToken(token); assert (!strcmp(token, "numMaterials"));
  num_materials = readInt();
  materials = new Material*[num_materials];
  
  // read in the objects
  int count = 0;
  while (num_materials > count) {
    getToken(token); 
    if (!strcmp(token, "PhongMaterial")) {
      getToken(token); assert (!strcmp(token, "{"));
      Vec3f diffuseColor(1,1,1);
      Vec3f specularColor(0,0,0);
      float exponent = 1;
      Vec3f transparentColor(0,0,0);
      Vec3f reflectiveColor(0,0,0);
      float indexOfRefraction = 1;
      while (1) {
	getToken(token); 
	if (!strcmp(token, "diffuseColor")) {
	  diffuseColor = readVec3f();
	} else if (!strcmp(token, "specularColor")) {
	  specularColor = readVec3f();
	} else if  (!strcmp(token, "exponent")) {
	  exponent = readFloat();
	} else if (!strcmp(token, "transparentColor")) {
	  transparentColor = readVec3f();
	} else if (!strcmp(token, "reflectiveColor")) {
	  reflectiveColor = readVec3f();
	} else if (!strcmp(token, "indexOfRefraction")) {
	  indexOfRefraction = readFloat();
	} else {
	  assert (!strcmp(token, "}"));
	  break;
	}
      }
      materials[count] = new PhongMaterial
	(diffuseColor,specularColor,exponent,
	 transparentColor,reflectiveColor,indexOfRefraction);
      count++;
    }
  }
  getToken(token); assert (!strcmp(token, "}"));
}  

// ====================================================================
// ====================================================================

void SceneParser::parseOrthographicCamera() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the camera parameters
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "center"));
  Vec3f center = readVec3f();
  getToken(token); assert (!strcmp(token, "direction"));
  Vec3f direction = readVec3f();
  getToken(token); assert (!strcmp(token, "up"));
  Vec3f up = readVec3f();
  getToken(token); assert (!strcmp(token, "size"));
  float size = readFloat();
  getToken(token); assert (!strcmp(token, "}"));
  camera = new OrthographicCamera(center,direction,up,size);
}


void SceneParser::parsePerspectiveCamera() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the camera parameters
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "center"));
  Vec3f center = readVec3f();
  getToken(token); assert (!strcmp(token, "direction"));
  Vec3f direction = readVec3f();
  getToken(token); assert (!strcmp(token, "up"));
  Vec3f up = readVec3f();
  getToken(token); assert (!strcmp(token, "angle"));
  float angle_degrees = readFloat();
  float angle_radians = DegreesToRadians(angle_degrees);
  getToken(token); assert (!strcmp(token, "}"));
  camera = new PerspectiveCamera(center,direction,up,angle_radians);
}


void SceneParser::parseBackground() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the background color
  getToken(token); assert (!strcmp(token, "{"));  
  while (1) {
    getToken(token); 
    if (!strcmp(token, "}")) { 
      break;  
    } else if (!strcmp(token, "color")) {
      background_color = readVec3f();
    } else if (!strcmp(token, "ambientLight")) {
      ambient_light = readVec3f();
    } else {
      printf ("Unknown token in parseBackground: '%s'\n", token);
      assert(0);
    }
  }
}

// ====================================================================
// ====================================================================

Sphere* SceneParser::parseSphere() {
  char token[MAX_PARSER_TOKEN_LENGTH];

  // read in the sphere parameters
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "center"));
  Vec3f center = readVec3f();
  getToken(token); assert (!strcmp(token, "radius"));
  float radius = readFloat();
  getToken(token); assert (!strcmp(token, "}"));

  return new Sphere(center,radius,current_material);
}


/*

// OPTIONAL, NOT REQUIRED TO HANDLE BOXES

Box* SceneParser::parseBox() {
  char token[MAX_PARSER_TOKEN_LENGTH];

  // read in the sphere parameters
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "min"));
  Vec3f min = readVec3f();
  getToken(token); assert (!strcmp(token, "max"));
  Vec3f max = readVec3f();
  getToken(token); assert (!strcmp(token, "}"));

  // create a new box object and return it
  return new Box(min,max,current_material);
}

*/

Plane* SceneParser::parsePlane() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the sphere parameters
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "normal"));
  Vec3f normal = readVec3f();
  getToken(token); assert (!strcmp(token, "offset"));
  float offset = readFloat();
  getToken(token); assert (!strcmp(token, "}"));
  return new Plane(normal,offset,current_material);
}

Triangle* SceneParser::parseTriangle() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the sphere parameters
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "vertex0"));
  Vec3f v0 = readVec3f();
  getToken(token); assert (!strcmp(token, "vertex1"));
  Vec3f v1 = readVec3f();
  getToken(token); assert (!strcmp(token, "vertex2"));
  Vec3f v2 = readVec3f();
  getToken(token); assert (!strcmp(token, "}"));
  return new Triangle(v0,v1,v2,current_material);
}


Grid* SceneParser::parseGrid() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the sphere parameters
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "min"));
  Vec3f min = readVec3f();
  getToken(token); assert (!strcmp(token, "max"));
  Vec3f max = readVec3f();
  getToken(token); assert (!strcmp(token, "numCells"));
  int nx = readInt();
  int ny = readInt();
  int nz = readInt();
  getToken(token); assert (!strcmp(token, "rasterizeSphere"));
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "center"));
  Vec3f center = readVec3f();
  getToken(token); assert (!strcmp(token, "radius"));
  float radius = readFloat();
  getToken(token); assert (!strcmp(token, "}"));
  getToken(token); assert (!strcmp(token, "}"));

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  // CALLING ASSIGNMENT 3 CODE
  Grid *answer = new Grid(min,max,nx,ny,nz);  //修改过了！！！
  answer->rasterizeSphere(center,radius);
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  return answer;
}

Group* SceneParser::parseTriangleMesh() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  char filename[MAX_PARSER_TOKEN_LENGTH];

  // get the filename
  getToken(token); assert (!strcmp(token, "{"));
  getToken(token); assert (!strcmp(token, "obj_file"));
  getToken(filename); 
  getToken(token); assert (!strcmp(token, "}"));
  const char *ext = &filename[strlen(filename)-4];
  assert(!strcmp(ext,".obj"));

  // read it once, get counts
  FILE *file = fopen(filename,"r");
  assert (file != NULL);
  int vcount = 0; int fcount = 0;
  while (1) {
    int c = fgetc(file);
    if (c == EOF) { break;
    } else if (c == 'v') { 
      assert(fcount == 0); float v0,v1,v2;
      fscanf (file,"%f %f %f",&v0,&v1,&v2);
      vcount++; 
    } else if (c == 'f') {
      int f0,f1,f2;
      fscanf (file,"%d %d %d",&f0,&f1,&f2);
      fcount++; 
    } // otherwise, must be whitespace
    
  }
  //printf ("verts %d faces %d\n", vcount,fcount);
  fclose(file);

  // make arrays
  Vec3f *verts = new Vec3f[vcount];
  Group *answer = new Group(fcount);

  // read it again, save it
  file = fopen(filename,"r");
  assert (file != NULL);
  int new_vcount = 0; int new_fcount = 0;
  while (1) {
    int c = fgetc(file);
    if (c == EOF) { break;
    } else if (c == 'v') { 
      assert(new_fcount == 0); float v0,v1,v2;
      fscanf (file,"%f %f %f",&v0,&v1,&v2);
      verts[new_vcount] = Vec3f(v0,v1,v2);
      new_vcount++; 
    } else if (c == 'f') {
      assert (vcount == new_vcount);
      int f0,f1,f2;
      fscanf (file,"%d %d %d",&f0,&f1,&f2);
      // indexed starting at 1...
      assert (f0 > 0 && f0 <= vcount);
      assert (f1 > 0 && f1 <= vcount);
      assert (f2 > 0 && f2 <= vcount);
      Triangle *t = new Triangle(verts[f0-1],verts[f1-1],verts[f2-1],
				 current_material);
      answer->addObject(new_fcount,t);
      new_fcount++; 
    } // otherwise, must be whitespace
  }
  delete [] verts;
  assert (fcount == new_fcount);
  assert (vcount == new_vcount);
  fclose(file);

  answer->calutateMinMax(); //计算min和max
  return answer;
}


Transform* SceneParser::parseTransform() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  Matrix matrix; matrix.SetToIdentity();
  Object3D *object = NULL;
  getToken(token); assert (!strcmp(token, "{"));

  // read in transformations: 
  // apply to the LEFT side of the current matrix (so the first
  // transform in the list is the last applied to the object)
  getToken(token);
  while (1) {
    if (!strcmp(token,"Scale")) {
      getToken(token); assert (!strcmp(token, "{"));
      matrix *= Matrix::MakeScale(readVec3f());
      getToken(token); assert (!strcmp(token, "}"));
    } else if (!strcmp(token,"XRotate")) {
      getToken(token); assert (!strcmp(token, "{"));
      matrix *= Matrix::MakeXRotation(DegreesToRadians(readFloat()));
      getToken(token); assert (!strcmp(token, "}"));
    } else if (!strcmp(token,"YRotate")) {
      getToken(token); assert (!strcmp(token, "{"));
      matrix *= Matrix::MakeYRotation(DegreesToRadians(readFloat()));
      getToken(token); assert (!strcmp(token, "}"));
    } else if (!strcmp(token,"ZRotate")) {
      getToken(token); assert (!strcmp(token, "{"));
      matrix *= Matrix::MakeZRotation(DegreesToRadians(readFloat()));
      getToken(token); assert (!strcmp(token, "}"));
    } else if (!strcmp(token,"Rotate")) {
      getToken(token); assert (!strcmp(token, "{"));
      Vec3f axis = readVec3f();
      float degrees = readFloat();
      matrix *= Matrix::MakeAxisRotation(axis,DegreesToRadians(degrees));
      getToken(token); assert (!strcmp(token, "}"));
    } else if (!strcmp(token,"Translate")) {
      getToken(token); assert (!strcmp(token, "{"));
      matrix *= Matrix::MakeTranslation(readVec3f());
      getToken(token); assert (!strcmp(token, "}"));
    } else if (!strcmp(token,"Matrix")) {
      Matrix matrix2; matrix2.SetToIdentity();
      getToken(token); assert (!strcmp(token, "{"));
      for (int j = 0; j < 4; j++) {
	for (int i = 0; i < 4; i++) {
	  float v = readFloat();
	  matrix2.Set(i,j,v); } }
      getToken(token); assert (!strcmp(token, "}"));
      matrix = matrix2 * matrix;
    } else {
      // otherwise this must be an object,
      // and there are no more transformations
      object = parseObject(token);
      break;
    }
    getToken(token);
  }
  assert(object != NULL);
  getToken(token); assert (!strcmp(token, "}"));
  return new Transform(matrix, object);
}


// ====================================================================
// ====================================================================

int SceneParser::getToken(char token[MAX_PARSER_TOKEN_LENGTH]) {
  // for simplicity, tokens must be separated by whitespace
  assert (file != NULL);
  int success = fscanf(file,"%s ",token);
  if (success == EOF) {
    token[0] = '\0';
    return 0;
  }
  return 1;
}

Vec3f SceneParser::readVec3f() {
  float x,y,z;
  int count = fscanf(file,"%f %f %f",&x,&y,&z);
  if (count != 3) {
    printf ("Error trying to read 3 floats to make a Vec3f\n");
    assert (0);
  }
  return Vec3f(x,y,z);
}

float SceneParser::readFloat() {
  float answer;
  int count = fscanf(file,"%f",&answer);
  if (count != 1) {
    printf ("Error trying to read 1 float\n");
    assert (0);
  }
  return answer;
}

int SceneParser::readInt() {
  int answer;
  int count = fscanf(file,"%d",&answer);
  if (count != 1) {
    printf ("Error trying to read 1 int\n");
    assert (0);
  }
  return answer;
}


// ====================================================================
// ====================================================================

