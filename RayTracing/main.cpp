#include "vectors.h"
#include "image.h"
#include "primitives.h"
#include "hit.h"
#include "scene_parser.h"
#include "camera.h"
#include "light.h"
#include "RayTracer.h"
#include "marching.h"
#include "material.h"
#include "raytracing_stats.h"

#define EPSILON 0.0001

int main(int argc,char **argv)
{
	char *input_file = "assignment6/scene6_14_glass_sphere.txt";
	int width = 600;
	int height = 600;
	char *output_file = "test.tga";
	float depth_min = 0;
	float depth_max = 1;
	char *depth_file = NULL;
	int nx = 50;
	int ny = 50;
	int nz = 50;

	SceneParser parser(input_file);
	Camera* camera = parser.getCamera();
	RayTracer tracer(&parser,5);
	Image image(width,height);
	Ray r;

	Grid* grid = new Grid(parser.getGroup()->getMin(),parser.getGroup()->getMax(),nx,ny,nz);
	RayTracingStats::Initialize(width,height,nx,ny,nz,parser.getGroup()->getMin(),parser.getGroup()->getMax());
	parser.getGroup()->interObjToGrid(grid);

	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			//printf("%d %d\n",i,j);
			Vec2f point((float)i/(height*1.0), (float)j/(width*1.0));  //这里没有包含（1，1），以后要加上去

			r =camera->generateRay(point);
			Hit h(1000,NULL,Vec3f(1,1,1));
			Vec3f finalColor = tracer.traceRay(r,EPSILON,0,1,h,grid);

			//noshadow
			//Vec3f finalColor;
			//if(grid->intersect(r,h,EPSILON))
			//{
			//	finalColor = Vec3f(1,1,1);
			//}
			//else 
			//	finalColor = Vec3f(0,1,0);
			image.SetPixel(j,height-1-i,finalColor);
		}
	}
	//RayTracingStats::PrintStatistics();
	RayTracingStats::PrintStatistics();
  	image.SaveTGA(output_file);
	return 0;
}