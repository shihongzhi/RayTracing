#include "sampler.h"

Vec2f UniformSampler::getSamplePosition(int n)
{
	int sqrtNum = (int)(sqrt((float)(numSamples)));
	int xNum = n%sqrtNum;
	int yNum = n/sqrtNum;
	float x = ((xNum+0.5)*1.0f)/(sqrtNum);
	float y = ((yNum+0.5)*1.0f)/(sqrtNum);
	return Vec2f(x,y);
}

Vec2f RandomSampler::getSamplePosition(int n)
{
	float x = (rand()%1000)*1.0f/1000.0;
	float y = (rand()%1000)*1.0f/1000.0;
	return Vec2f(x,y);
}

Vec2f JitteredSampler::getSamplePosition(int n)
{
	int sqrtNum = (int)(sqrt((float)(numSamples)));
	int xNum = n%sqrtNum;
	int yNum = n/sqrtNum;
	float x = ((xNum+(rand()%100)*1.0f/100.0)*1.0f)/(sqrtNum);
	float y = ((yNum+(rand()%100)*1.0f/100.0)*1.0f)/(sqrtNum);
	return Vec2f(x,y);
}