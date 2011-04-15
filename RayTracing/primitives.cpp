#include "primitives.h"
#include "hit.h"
#include "ray.h"
#include "marching.h"
#include "object3dvector.h"
#include "raytracing_stats.h"
#include "utility.h"

void Object3D::interObjToGrid(Grid* grid)
{
	Vec3f tempMin;
	Vec3f tempMax;
	int iMin,iMax,jMin,jMax,kMin,kMax;

	tempMin = min;
	tempMax = max;
	//计算出来的i,j,k 肯定是属于0-nx,0-ny,0-nz;
	iMin = (int)((tempMin.x()-grid->getMin().x())/grid->GetDx());
	jMin = (int)((tempMin.y()-grid->getMin().y())/grid->GetDy());
	kMin = (int)((tempMin.z()-grid->getMin().z())/grid->GetDz());
	iMax = (int)((tempMax.x()-grid->getMin().x())/grid->GetDx());
	jMax = (int)((tempMax.y()-grid->getMin().y())/grid->GetDy());
	kMax = (int)((tempMax.z()-grid->getMin().z())/grid->GetDz());

	//其实这里可以不要的
	if(iMax==grid->GetNx()) iMax--;
	if(jMax==grid->GetNy()) jMax--;
	if(kMax==grid->GetNz()) kMax--;
	//printf("%d %d %d %d %d %d  %d\n",iMin,jMin,kMin,iMax,jMax,kMax,count);
	for(int i=iMin; i<=iMax; ++i)
	{
		for(int j=jMin; j<=jMax; ++j)
		{
			for(int k=kMin; k<=kMax; ++k)
			{
				grid->GetObject3d(i,j,k).addObject(this);
			}
		}
	}
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
	Vec3f v = center - r.getOrigin();
	float tp = v.Dot3(r.getDirection());
	float det = tp*tp - v.Dot3(v) + radius*radius;
	//intersect
	if(det > 0)
	{
		//t'
		det = sqrtf(det); 

		float t1 = tp - det;
		float t2 = tp + det;

		if(t1 > tmin && t1 < h.getT())
		{
			Vec3f normal = (r.pointAtParameter(t1) - center);
			normal /= radius;
			normal.Normalize();
			h.set(t1,material,normal,r);
			return 1;
		}
		else if(t2 > tmin && t2 < h.getT())
		{
			//sphere's normal
			Vec3f normal = (r.pointAtParameter(t2) - center);
			normal /= radius;
			normal.Normalize();
			h.set(t2,material,normal,r);
			return 1;
		}
	}
	return 0;
}


//normal.(r.origin + t*r.direction) +d = 0
bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
	float isParallel = normal.Dot3(r.getDirection());
	if(!Utility::isZero(isParallel))
	{
		float dist = -(normal.Dot3(r.getOrigin())-d)/isParallel;  //当时没有理解d的含义,构造是应该是ax+by+cz-d=0
		if(dist > tmin && dist < h.getT())
		{
			h.set(dist,material,normal,r);
			return 1;
		}
	}
	return 0;
}

void Plane::interObjToGrid(Grid* grid)
{
	grid->GetOthers().addObject(this);
}

Triangle::Triangle(Vec3f &_a, Vec3f &_b, Vec3f &_c, Material *m)
{
	a = _a; 
	b = _b; 
	c = _c; 
	material = m; 
	Vec3f::Cross3(normal,b-a,c-a); 
	normal.Normalize(); 
	d = -1.0 * normal.Dot3(a);
	Vec3f::GetThreeMax(max,a,b,c); 
	Vec3f::GetThreeMin(min,a,b,c);
	if(normal.x()>=normal.y() && normal.x()>=normal.z())
		i0 = 0;
	else if(normal.y()>=normal.x() && normal.y()>=normal.z())
		i0 = 1;
	else
		i0 = 2;
}

//用了彭老师书上介绍的求交方法，但是transform之后就有问题
bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
	Matrix A;
	A.SetToIdentity();
	A.Set(0,0,a.x()-b.x());
	A.Set(1,0,a.x()-c.x());
	A.Set(2,0,r.getDirection().x());

	A.Set(0,1,a.y()-b.y());
	A.Set(1,1,a.y()-c.y());
	A.Set(2,1,r.getDirection().y());

	A.Set(0,2,a.z()-b.z());
	A.Set(1,2,a.z()-c.z());
	A.Set(2,2,r.getDirection().z());

	Vec4f vr(a.x()-r.getOrigin().x(),a.y()-r.getOrigin().y(),a.z()-r.getOrigin().z(),1);

	if(fabs(r.getDirection().Dot3(normal))>0.0f)
	{
		A.Inverse();
		A.Transform(vr);
		if((vr.x()+vr.y())<=1 && vr.x()>=0 && vr.y()>=0 && vr.z()>tmin && vr.z()<h.getT())
		{
			h.set(vr.z(),material,normal,r);
			return 1;
		}
	}
	return 0;
	//为什么当使用transform之后，这里的求交函数就不能用了？  用没有加速的程序检测了一下，也无法求交，应该是这里的求交写错掉了
	//Vec3f origin = r.getOrigin();
	//Vec3f direct = r.getDirection();
	//float isParallel = normal.Dot3(direct);
	//Vec3f tempbeta0;
	//Vec3f tempbeta1;
	//Vec3f tempbeta2;
	//float beta0;
	//float beta1;
	//float beta2;
	//if(fabs(isParallel)>0.0f)
	//{
	//	float dist = -(normal.Dot3(origin)+d)/isParallel;
	//	Vec3f q = origin + direct*dist;
	//	Vec3f::Cross3(tempbeta0,(c-b),(q-b));
	//	Vec3f::Cross3(tempbeta1,(a-c),(q-c));
	//	Vec3f::Cross3(tempbeta2,(b-a),(q-a));
	//	if(i0 == 0)
	//	{
	//		beta0 = tempbeta0.x()/normal.x();	
	//		beta1 = tempbeta1.x()/normal.x();
	//		beta2 = tempbeta2.x()/normal.x();
	//	}
	//	else if(i0 == 1)
	//	{
	//		beta0 = tempbeta0.y()/normal.y();
	//		beta1 = tempbeta1.y()/normal.y();
	//		beta2 = tempbeta2.y()/normal.y();
	//	}
	//	else
	//	{
	//		beta0 = tempbeta0.z()/normal.z();
	//		beta1 = tempbeta1.z()/normal.z();
	//		beta2 = tempbeta2.z()/normal.z();
	//	}
	//	if(beta0>=0 && beta0<=1 && beta1>=0 && beta1<=1 && beta2>=0 && beta2<=1)
	//	{
	//		if(dist > tmin && dist < h.getT())
	//		{
	//			h.set(dist,material,normal,r);
	//			return true;	
	//		}
	//	}
	//}
	//return false;
}

Transform::Transform(Matrix &_m, Object3D *o)
{
	m = _m;
	instance = o;

	//还可以对三角形进行优化
	Vec3f *temp;
	temp = (Vec3f*)malloc(sizeof(Vec3f)*8);
	temp[0] = instance->getMin();
	temp[7] = instance->getMax();
	temp[1].Set(temp[0].x(),temp[0].y(),temp[7].z());
	temp[2].Set(temp[0].x(),temp[7].y(),temp[0].z());
	temp[3].Set(temp[0].x(),temp[7].y(),temp[7].z());
	temp[4].Set(temp[7].x(),temp[0].y(),temp[0].z());
	temp[5].Set(temp[7].x(),temp[0].y(),temp[7].z());
	temp[6].Set(temp[7].x(),temp[7].y(),temp[0].z());
	for(int i=0; i<8; ++i)
	{
		m.Transform(temp[i]);	
	}
	float xMax,xMin,yMax,yMin,zMax,zMin;
	xMax = temp[0].x();
	xMin = temp[0].x();
	yMax = temp[0].y();
	yMin = temp[0].y();
	zMax = temp[0].z();
	zMin = temp[0].z();
	for(int i=1; i<8; ++i)
	{
		if(temp[i].x() > xMax)
			xMax = temp[i].x();
		if(temp[i].x() < xMin)
			xMin = temp[i].x();
		if(temp[i].y() > yMax)
			yMax = temp[i].y();
		if(temp[i].y() < yMin)
			yMin = temp[i].y();
		if(temp[i].z() > zMax)
			zMax = temp[i].z();
		if(temp[i].z() < zMin)
			zMin = temp[i].z();
	}
	min.Set(xMin,yMin,zMin);
	max.Set(xMax,yMax,zMax);
}

bool Transform::intersect(const Ray &r, Hit &h, float tmin)
{
	Vec3f rTransOri = r.getOrigin();
	Vec3f rTransDir = r.getDirection();
	Matrix mInverse;
	m.Inverse(mInverse);
	mInverse.Transform(rTransOri);
	mInverse.TransformDirection(rTransDir);
	
	rTransDir.Normalize();
	Ray rTrans(rTransDir,rTransOri);
	Hit hTrans(10000,NULL,Vec3f(0,0,0));  //need a new hit,because the x-y-z had changed  就因为这里没有使用一个新的hit导致了自己debug了两天
	instance->intersect(rTrans,hTrans,tmin);
	if(hTrans.getT()<10000)
	{
		//world's t
		float t;
		//Vec3f hitPoint = rTransOri + rTransDir * hTrans.getT();
		Vec3f hitPoint = rTrans.pointAtParameter(hTrans.getT());
		m.Transform(hitPoint);
		Vec3f rOri = r.getOrigin();
		Vec3f rDir = r.getDirection();
		if((fabs(rDir[0])>=fabs(rDir[1]))&&(fabs(rDir[0])>=fabs(rDir[2]))){
			t = (hitPoint[0] - rOri[0]) / rDir[0]; 	
		}
		else if((fabs(rDir[1])>=fabs(rDir[0]))&&(fabs(rDir[1])>=fabs(rDir[2]))){
			t = (hitPoint[1] - rOri[1]) / rDir[1];
		}
		else if((fabs(rDir[2])>=fabs(rDir[0]))&&(fabs(rDir[2])>=fabs(rDir[1]))){
			t = (hitPoint[2] - rOri[2]) / rDir[2];
		}

		//world's normal
		mInverse.Transpose();
		Vec3f wNormal = hTrans.getNormal();
		mInverse.TransformDirection(wNormal);
		wNormal.Normalize();  //need normalize
		//h.setNormal(wNormal);

		if(t>=tmin && t<=h.getT())
		{
			h.set(t,hTrans.getMaterial(),wNormal,r);
			return 1;
		}
	}
	return 0;
}

Group::Group(int nu)
{
	number = nu;
	instances = (Object3D**)malloc(sizeof(Object3D*)*number);
}

bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
	int flag = 0;
	for(int i=0; i<number; i++)
	{
		if(instances[i]->intersect(r,h,tmin) == 1)
		{
			flag = 1;
		}	
	}
	if(flag == 1)
		return 1;
	else
		return 0;
}

void Group::addObject(int index, Object3D *obj)
{
	if(number > index)
	{
		instances[index] = obj;
	}
}

void Group::calutateMinMax()
{
	//是在这里吗？
	float max_x,max_y,max_z;
	float min_x,min_y,min_z;
	max_x = instances[0]->getMax().x();
	max_y = instances[0]->getMax().y();
	max_z = instances[0]->getMax().z();
	min_x = instances[0]->getMin().x();
	min_y = instances[0]->getMin().y();
	min_z = instances[0]->getMin().z();
	for(int i=1; i<number; i++)
	{
		if(max_x < instances[i]->getMax().x())
			max_x = instances[i]->getMax().x();
		if(max_y < instances[i]->getMax().y())
			max_y = instances[i]->getMax().y();
		if(max_z < instances[i]->getMax().z())
			max_z = instances[i]->getMax().z();
		if(min_x > instances[i]->getMin().x())
			min_x = instances[i]->getMin().x();
		if(min_y > instances[i]->getMin().y())
			min_y = instances[i]->getMin().y();
		if(min_z > instances[i]->getMin().z())
			min_z = instances[i]->getMin().z();
	}
	max.Set(max_x,max_y,max_z);
	min.Set(min_x,min_y,min_z);
}

void Group::interObjToGrid(Grid *grid)
{
	for(int i=0; i<number; i++)
	{
		instances[i]->interObjToGrid(grid);
	}
}

Grid::Grid(Vec3f &_min, Vec3f &_max, int _nx, int _ny, int _nz)
{
	min = _min;
	max = _max;
	nx = _nx;
	ny = _ny;
	nz = _nz;

	//申请三维数组 ，但是这样申请的对象是没有初始化过的
	//isOpaque = (Object3DVector ***)malloc(nx*sizeof(Object3DVector **));
	//for(int i=0; i<nx; i++)
	//{
	//	isOpaque[i] = (Object3DVector **)malloc(ny*sizeof(Object3DVector *));
	//	for(int j=0; j<ny; j++)
	//	{
	//		isOpaque[i][j] = (Object3DVector*)malloc(nz*sizeof(Object3DVector));
	//	}
	//}
	//material = _m;

	isOpaque = new Object3DVector**[nx];
	for(int i=0; i<nx; ++i)
	{
		isOpaque[i] = new Object3DVector*[ny];
		for(int j=0; j<ny; j++)
		{
			isOpaque[i][j] = new Object3DVector[nz];
		}
	}
	dx = (max.x()-min.x())/nx;
	dy = (max.y()-min.y())/ny;
	dz = (max.z()-min.z())/nz;
}

Grid::~Grid()
{
	//释放isOpaque内存空间
	for(int i=0; i<nx; i++)
	{
		for(int j=0; j<ny; j++)
			delete [] isOpaque[i][j];
		delete [] isOpaque[i];
	}
	delete [] isOpaque;
}


void Grid::initialRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const
{
	Vec3f rayOri = r.getOrigin();
	Vec3f rayDir = r.getDirection();

	float tbottom,ttop,tfront,tback,tleft,tright,tc;

	float dtx,dty,dtz;
	int celli,cellj,cellk;
	float tnext_x,tnext_y,tnext_z;
	int signx,signy,signz;
	signx = (rayDir.x()>=0) ? 1 : -1;
	signy = (rayDir.y()>=0) ? 1 : -1;
	signz = (rayDir.z()>=0) ? 1 : -1;
	
	//需要求绝对值  这里rayDir.x()等于0没有处理
	dtx = abs(dx / rayDir.x());
	dty = abs(dy / rayDir.y());
	dtz = abs(dz / rayDir.z());

	Vec3f normal;
	Vec3f rayOriTmin = rayOri + rayDir * tmin;
	//这里临界值要注意
	//inside;
	if(max.x()>rayOriTmin.x()&&min.x()<rayOriTmin.x()&&max.y()>rayOriTmin.y()&&min.y()<rayOriTmin.y()&&max.z()>rayOriTmin.z()&&min.z()<rayOriTmin.z())  //忽略了tmin的作用
	{
		celli = (int)((rayOriTmin.x()-min.x())/dx);
		cellj = (int)((rayOriTmin.y()-min.y())/dy);
		cellk = (int)((rayOriTmin.z()-min.z())/dz);

		if(Utility::isZero(rayDir.x()))
			tnext_x = 10000;
		else if(signx == -1)  //如果反方向的话，求tnext不需要加signx  谢谢大师提醒
			tnext_x = (min.x()+celli*dx-rayOri.x())/rayDir.x();
		else
			tnext_x = (min.x()+(celli+signx)*dx-rayOri.x())/rayDir.x();
		if(Utility::isZero(rayDir.y()))
			tnext_y = 10000;
		else if(signy == -1)
			tnext_y = (min.y()+cellj*dy-rayOri.y())/rayDir.y();
		else
			tnext_y = (min.y()+(cellj+signy)*dy-rayOri.y())/rayDir.y();
		if(Utility::isZero(rayDir.z()))
			tnext_z = 10000;
		else if(signz == -1)
			tnext_z = (min.z()+cellk*dz-rayOri.z())/rayDir.z();
		else
			tnext_z = (min.z()+(cellk+signz)*dz-rayOri.z())/rayDir.z();
		//这里tmin如何修改
		mi.Set(0,celli,cellj,cellk,tnext_x,tnext_y,tnext_z,dtx,dty,dtz,signx,signy,signz,normal);
	}
	else //outside
	{
		float t1x,t1y,t1z;
		float t2x,t2y,t2z;
		float tnear,tfar;
		
		if(Utility::isZero(rayDir.x()))
		{
			tleft = 0;
			tright = 10000;
		}
		else
		{
			tleft = (min.x()-rayOri.x())/rayDir.x();
			tright = (max.x()-rayOri.x())/rayDir.x();
		}
		if(Utility::isZero(rayDir.y()))
		{
			tbottom = 0;
			ttop = 10000;
		}
		else
		{
			tbottom = (min.y()-rayOri.y())/rayDir.y();
			ttop = (max.y()-rayOri.y())/rayDir.y();
		}
		if(Utility::isZero(rayDir.z()))
		{
			tback = 0;
			tfront = 10000;
		}
		else
		{
			tback = (min.z()-rayOri.z())/rayDir.z();
			tfront = (max.z()-rayOri.z())/rayDir.z();
		}
		t1x = (signx == 1) ? tleft : tright;
		t2x = (signx == 1) ? tright : tleft;
		t1y = (signy == 1) ? tbottom : ttop;
		t2y = (signy == 1) ? ttop : tbottom;
		t1z = (signz == 1) ? tback : tfront;
		t2z = (signz == 1) ? tfront : tback;

		tnear = Utility::getMax(t1x,t1y,t1z);
		tfar = Utility::getMin(t2x,t2y,t2z);

		if(tnear>tfar) //miss
		{}
		else if(tfar<tmin) //behind
		{}
		else  //hit
		{
			tc = tnear;

			//求i,j,k
			if(rayOri.x()+rayDir.x()*tc+tmin < min.x())   //要加上0.0001
				celli = -1;
			else
				celli = (int)((rayOri.x()+rayDir.x()*tc-min.x()-tmin)/dx);  //减去0.0001是为了使边界也算交
			if(rayOri.y()+rayDir.y()*tc+tmin < min.y())
				cellj = -1;
			else
				cellj = (int)((rayOri.y()+rayDir.y()*tc-min.y()-tmin)/dy);
			if(rayOri.z()+rayDir.z()*tc+tmin < min.z())
				cellk = -1;
			else
				cellk = (int)((rayOri.z()+rayDir.z()*tc-min.z()-tmin)/dz);


			//求tnext
			if(Utility::isZero(rayDir.x()))
				tnext_x = 10000;
			else if(signx == -1)  //如果反方向的话，求tnext不需要加signx  谢谢大师提醒
				tnext_x = (min.x()+celli*dx-rayOri.x())/rayDir.x();
			else
				tnext_x = (min.x()+(celli+signx)*dx-rayOri.x())/rayDir.x();
			if(Utility::isZero(rayDir.y()))
				tnext_y = 10000;
			else if(signy == -1)
				tnext_y = (min.y()+cellj*dy-rayOri.y())/rayDir.y();
			else
				tnext_y = (min.y()+(cellj+signy)*dy-rayOri.y())/rayDir.y();
			if(Utility::isZero(rayDir.z()))
				tnext_z = 10000;
			else if(signz == -1)
				tnext_z = (min.z()+cellk*dz-rayOri.z())/rayDir.z();
			else
				tnext_z = (min.z()+(cellk+signz)*dz-rayOri.z())/rayDir.z();


			if(Utility::isEqual(tc,tleft))
				normal.Set(-1,0,0);
			else if(Utility::isEqual(tc,tright))
				normal.Set(1,0,0);
			else if(Utility::isEqual(tc,tbottom))
				normal.Set(0,-1,0);
			else if(Utility::isEqual(tc,ttop))
				normal.Set(0,1,0);
			else if(Utility::isEqual(tc,tfront))
				normal.Set(0,0,1);
			else if(Utility::isEqual(tc,tback))
				normal.Set(0,0,-1);

			mi.Set(tc,celli,cellj,cellk,tnext_x,tnext_y,tnext_z,dtx,dty,dtz,signx,signy,signz,normal);
			//printf("celli: %d  cellj: %d  cellk: %d \n",celli,cellj,cellk);
		}
	}
}


bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
	MarchingInfo march;
	initialRayMarch(march,r,tmin);

	int currentI = march.GetI();
	int currentJ = march.GetJ();    
	int currentK = march.GetK();
	float currentT = march.GetT();
	int numObjects;
	//Object3DVector currentVector; 绝对不能新建一个Object3DVector，然后把isOpaque的值赋给这个Object3DVector，因为当这个函数结束时，会把Object3DVector销毁，对应的指针指向的对象也会被销毁。
	//当然还有一种方法是定义一个Object3DVector指针
	//printf("i:%d j:%d currentI:%d currentJ:%d currentK:%d\n",i,j,currentI,currentJ,currentK);

	Object3DVector isAlreadyIntersect;

	while(currentI>=0 && currentI<nx && currentJ>=0 && currentJ<ny && currentK>=0 && currentK<nz)
	{
		RayTracingStats::IncrementNumGridCellsTraversed();
		Object3DVector* currentVector = &isOpaque[currentI][currentJ][currentK];
		numObjects = currentVector->getNumObjects();
		//printf("%d %d %d\n",currentI,currentJ,currentK);
		for(int i=0; i<numObjects; ++i)
		{
			//already intersected object don't need intersect again
			if(isAlreadyIntersect.isInside(currentVector->getObject(i)))
				continue;
			RayTracingStats::IncrementNumIntersections();
			currentVector->getObject(i)->intersect(r,h,tmin);
			isAlreadyIntersect.addObject(currentVector->getObject(i));
		}
		if(h.getMaterial()!=NULL && h.getT()>=currentT && h.getT()<=march.GetNext_x() && h.getT()<=march.GetNext_y() && h.getT()<=march.GetNext_z())
		{
			return true;
		}

		march.nextCell();
		currentI = march.GetI();
		currentJ = march.GetJ();
		currentK = march.GetK();
		currentT = march.GetT();  //折射或反射，shadow时，光线圆点会在grid内
	}
	numObjects = others.getNumObjects();
	for(int i=0; i<numObjects; i++)
	{
		others.getObject(i)->intersect(r,h,tmin);
	}
	if(h.getMaterial()!=NULL)   //这里有问题，因为如果没有和平面相交的话，h的material也有可能不是null
		return true;

	return false;
}

bool Grid::shadowIntersect(const Ray &r, Hit &h, float tmin)
{
	MarchingInfo march;
	initialRayMarch(march,r,tmin);

	int currentI = march.GetI();
	int currentJ = march.GetJ();    
	int currentK = march.GetK();
	float currentT = march.GetT();
	int numObjects;

	Object3DVector isAlreadyIntersect;

	while(currentI>=0 && currentI<nx && currentJ>=0 && currentJ<ny && currentK>=0 && currentK<nz)
	{
		RayTracingStats::IncrementNumGridCellsTraversed();
		Object3DVector &currentVector = isOpaque[currentI][currentJ][currentK];
		numObjects = currentVector.getNumObjects();
		for(int i=0; i<numObjects; ++i)
		{
			if(isAlreadyIntersect.isInside(currentVector.getObject(i)))
				continue;
			RayTracingStats::IncrementNumIntersections();
			if(currentVector.getObject(i)->intersect(r,h,tmin))
				return true;
			isAlreadyIntersect.addObject(currentVector.getObject(i));
		}
		march.nextCell();
		currentI = march.GetI();
		currentJ = march.GetJ();
		currentK = march.GetK();
		currentT = march.GetT();  //折射或反射，shadow时，光线圆点会在grid内
	}
	numObjects = others.getNumObjects();
	for(int i=0; i<numObjects; i++)
	{
		if(others.getObject(i)->intersect(r,h,tmin))
			return true;
	}
	return false;
}