#include "marching.h"

void MarchingInfo::nextCell()
{
	if(tnext_x<tnext_y && tnext_x<tnext_z)
	{
		i += signx;
		tmin = tnext_x;
		tnext_x += dtx;
		normal = (signx==1) ? Vec3f(-1,0,0) : Vec3f(1,0,0);
	}
	else if(tnext_y<tnext_x && tnext_y<tnext_z)
	{
		j += signy;
		tmin = tnext_y;
		tnext_y += dty;
		normal = (signy==1) ? Vec3f(0,-1,0) : Vec3f(0,1,0);
	}
	else
	{
		k += signz;
		tmin = tnext_z;
		tnext_z += dtz;
		normal = (signz==1) ? Vec3f(0,0,-1) : Vec3f(0,0,1);
	}
}