#include "filter.h"
#include "film.h"
#include <math.h>

#define  E 2.71828183

Vec3f Filter::getColor(int i, int j, Film *film)
{
	int numSamplers = film->getNumSamples();
	int supportRadius = getSupportRadius();
	float sumWeight = 0;
	Vec3f sumColor(0,0,0);
	for(int k=i-sumWeight; k<=i+sumWeight; ++k)
	{
		for(int t=j-sumWeight; t<=j+sumWeight; ++t)
		{
			for(int m=0; m<numSamplers; ++m)
			{
				if(k<0 || k>=film->getWidth())
					continue;
				if(t<0 || t>=film->getHeight())
					continue;
				Sample tempSample = film->getSample(k,t,m);
				float tempX = tempSample.getPosition().x()-0.5-k+i;
				float tempY = tempSample.getPosition().y()-0.5-t+j;
				float tempWeight = getWeight(tempX,tempY);
				sumColor += tempSample.getColor()*tempWeight;
				sumWeight += tempWeight;
			}
		}
	}
	if(sumWeight!=0)
	{
		sumColor /= sumWeight;
		return sumColor;
	}
	else
		return Vec3f(0,0,0);
}

int BoxFilter::getSupportRadius()
{
	return (int)ceil(radius-0.5);
}

float BoxFilter::getWeight(float x, float y)
{
	if(fabs(x)<=radius && fabs(y)<=radius)
		return 1.0f;
	else
		return 0.0f;
}

int TentFilter::getSupportRadius()
{
	return (int)ceil(radius-0.5);
}

float TentFilter::getWeight(float x,float y)
{
	float dist = sqrt(x*x + y*y);
	if(dist>=radius)
		return 0.0f;
	else
		return (1.0f-dist/radius);
}

int GaussionFilter::getSupportRadius()
{
	return (int)ceil(2*sigma-0.5);
}

float GaussionFilter::getWeight(float x, float y)
{
	float dist2 = (x*x+y*y);
	float powUp = ((-1.0f)*dist2)/(2*sigma*sigma); 
	//if(sqrt(dist2)>getSupportRadius())
	//	return 0;
	//else
	return pow((float)E,powUp);
}