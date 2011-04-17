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
#include "film.h"
#include "sampler.h"
#include "filter.h"
#include <time.h>

#define EPSILON 0.0001

int main(int argc,char **argv)
{
	char *input_file = "assignment7/scene7_06_faceted_gem.txt";
	int width = 200;
	int height = 200;
	int numSamples = 9;
	char *output_file = "test.tga";
	float depth_min = 0;
	float depth_max = 1;
	char *depth_file = NULL;
	int nx = 20;
	int ny = 20;
	int nz = 20;

	SceneParser parser(input_file);
	Camera* camera = parser.getCamera();
	RayTracer tracer(&parser,5);
	Image image(width,height);
	Ray r;

	Grid* grid = new Grid(parser.getGroup()->getMin(),parser.getGroup()->getMax(),nx,ny,nz);
	RayTracingStats::Initialize(width,height,nx,ny,nz,parser.getGroup()->getMin(),parser.getGroup()->getMax());
	parser.getGroup()->interObjToGrid(grid);

	Film film(width,height,numSamples);
	//UniformSampler uniformSampler(numSamples);
	//RandomSampler randomSampler(numSamples);
	RandomSampler randomSampler(numSamples);
	srand((unsigned)time(NULL));  //不能放在for循环中
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			for(int ns=0; ns<numSamples; ++ns)
			{
				Vec2f moveOffset = randomSampler.getSamplePosition(ns);
				Vec2f point((float)(i+moveOffset.x())/height, (float)(j+moveOffset.y())/width);
				r =camera->generateRay(point);
				Hit h(1000,NULL,Vec3f(1,1,1));
				Vec3f finalColor = tracer.traceRay(r,EPSILON,0,1,h,grid);
				film.setSample(j,i,ns,moveOffset,finalColor);
			}
			//Vec2f point((float)i/(height*1.0), (float)j/(width*1.0));  //这里没有包含（1，1），以后要加上去
			//r =camera->generateRay(point);
			//Hit h(1000,NULL,Vec3f(1,1,1));
			//Vec3f finalColor = tracer.traceRay(r,EPSILON,0,1,h,grid);
			//image.SetPixel(j,height-1-i,finalColor);
		}
	}

	GaussionFilter *filter = new GaussionFilter(0.4);
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			Vec3f finalImageColor = filter->getColor(j,i,&film);
			image.SetPixel(j,i,finalImageColor);
		}
	}
	RayTracingStats::PrintStatistics();
  	image.SaveTGA(output_file);

	return 0;
}