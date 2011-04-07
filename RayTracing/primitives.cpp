#include "primitives.h"
#include "hit.h"
#include "ray.h"
#include "marching.h"
#include "object3dvector.h"
#include "raytracing_stats.h"
#include "utility.h"

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
		float dist = -(normal.Dot3(r.getOrigin())-d)/isParallel;  //��ʱû������d�ĺ���,������Ӧ����ax+by+cz-d=0
		if(dist > tmin && dist < h.getT())
		{
			h.set(dist,material,normal,r);
			return 1;
		}
	}
	return 0;
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

//rayCast2 P48
bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{

	//float A = Matrix::det3x3(a.x()-b.x(),a.x()-c.x(),r.getDirection().x(),
	//						 a.y()-b.y(),a.y()-c.y(),r.getDirection().y(),
	//						 a.z()-b.z(),a.z()-c.z(),r.getDirection().z());
	//float betaUp = Matrix::det3x3(a.x()-r.getOrigin().x(),a.x()-c.x(),r.getDirection().x(),
	//							  a.y()-r.getOrigin().y(),a.y()-c.y(),r.getDirection().y(),
	//							  a.z()-r.getOrigin().z(),a.z()-c.z(),r.getDirection().z());
	//float alphaUp = Matrix::det3x3(a.x()-b.x(),a.x()-r.getOrigin().x(),r.getDirection().x(),
	//							   a.y()-b.y(),a.y()-r.getOrigin().y(),r.getDirection().y(),
	//							   a.z()-b.z(),a.z()-r.getOrigin().z(),r.getDirection().z());
	//float tUp = Matrix::det3x3(a.x()-b.x(),a.x()-c.x(),a.x()-r.getOrigin().x(),
	//						   a.y()-b.y(),b.y()-c.y(),a.y()-r.getOrigin().y(),
	//						   a.z()-b.z(),b.z()-c.z(),a.z()-r.getOrigin().z());
	//float beta = betaUp / A;
	//float alpha = alphaUp / A;
	//float t = tUp / A;
	//if((beta+alpha)<=1 && (beta>=0) && (alpha>=0) && (t>tmin) && (t<h.getT()))
	//{
	//	Vec3f normal;
	//	Vec3f::Cross3(normal,b-a,c-a);
	//	normal.Normalize();
	//	h.set(t,material,normal,r);
	//	return 1;
	//}
	//return 0;
	/*Matrix A;
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
	return 0;*/
	Vec3f origin = r.getOrigin();
	Vec3f direct = r.getDirection();
	float isParallel = normal.Dot3(direct);
	Vec3f tempbeta0;
	Vec3f tempbeta1;
	Vec3f tempbeta2;
	float beta0;
	float beta1;
	float beta2;
	if(!Utility::isZero(isParallel))
	{
		float dist = -(normal.Dot3(origin)+d)/isParallel;
		Vec3f q = origin + direct*dist;
		Vec3f::Cross3(tempbeta0,(c-b),(q-b));
		Vec3f::Cross3(tempbeta1,(a-c),(q-c));
		Vec3f::Cross3(tempbeta2,(b-a),(q-a));
		if(i0 == 0)
		{
			beta0 = tempbeta0.x()/normal.x();	
			beta1 = tempbeta1.x()/normal.x();
			beta2 = tempbeta2.x()/normal.x();
		}
		else if(i0 == 1)
		{
			beta0 = tempbeta0.y()/normal.y();
			beta1 = tempbeta1.y()/normal.y();
			beta2 = tempbeta2.y()/normal.y();
		}
		else
		{
			beta0 = tempbeta0.z()/normal.z();
			beta1 = tempbeta1.z()/normal.z();
			beta2 = tempbeta2.z()/normal.z();
		}
		if(beta0>=0 && beta0<=1 && beta1>=0 && beta1<=1 && beta2>=0 && beta2<=1)
		{
			if(dist > tmin && dist < h.getT())
			{
				h.set(dist,material,normal,r);
				return true;	
			}
		}
	}
	return false;

}
Transform::Transform(Matrix &_m, Object3D *o)
{
	m = _m;
	instance = o;

	//����ֱ���ж�ΪTriangle��
	Vec3f aTrans = ((Triangle*)instance)->getA();
	Vec3f bTrans = ((Triangle*)instance)->getB();
	Vec3f cTrans = ((Triangle*)instance)->getC();
	m.Transform(aTrans);
	m.Transform(bTrans);
	m.Transform(cTrans);
	Vec3f::GetThreeMax(max,aTrans,bTrans,cTrans);
	Vec3f::GetThreeMin(min,aTrans,bTrans,cTrans);
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
	Hit hTrans(10000,NULL,Vec3f(0,0,0));  //need a new hit,because the x-y-z had changed  ����Ϊ����û��ʹ��һ���µ�hit�������Լ�debug������
	if(instance->intersect(rTrans,hTrans,tmin) == 1)
	{

		//world's normal
		Matrix mInTr;
		mInverse.Transpose(mInTr);
		Vec3f wNormal = hTrans.getNormal();
		mInTr.TransformDirection(wNormal);
		wNormal.Normalize();  //need normalize
		//h.setNormal(wNormal);

		//world's t
		float t;
		Vec3f hitPoint = rTransOri + rTransDir * hTrans.getT();
		m.Transform(hitPoint);
		Vec3f rOri = r.getOrigin();
		Vec3f rDir = r.getDirection();
		if((rDir[0]>=rDir[1])&&(rDir[0]>=rDir[2])){
			t = (hitPoint[0] - rOri[0]) / rDir[0]; 	
		}
		else if((rDir[1]>=rDir[0])&&(rDir[1]>=rDir[2])){
			t = (hitPoint[1] - rOri[1]) / rDir[1];
		}
		else if((rDir[2]>=rDir[0])&&(rDir[2]>=rDir[1])){
			t = (hitPoint[2] - rOri[2]) / rDir[2];
		}
		if(t>tmin && t<h.getT())
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


//����������
void Group::addObject(int index, Object3D *obj)
{
	if(number > index)
	{
		instances[index] = obj;
	}
}

void Group::calutateMinMax()
{
	//����������
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
		if(max_z < instances[i]->getMax().y())
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
	Vec3f tempMin;
	Vec3f tempMax;
	int iMin,iMax,jMin,jMax,kMin,kMax;
	Group* tempGroup = (Group*)instances[0];
	int tempNumber = tempGroup->getNumber();

	for(int count=0; count<tempNumber; ++count)
	{
		tempMin = tempGroup->getObject(count)->getMin();
		tempMax = tempGroup->getObject(count)->getMax();
		//���������i,j,k �϶�������0-nx,0-ny,0-nz;
		iMin = (int)((tempMin.x()-grid->getMin().x())/grid->GetDx());
		jMin = (int)((tempMin.y()-grid->getMin().y())/grid->GetDy());
		kMin = (int)((tempMin.z()-grid->getMin().z())/grid->GetDz());
		iMax = (int)((tempMax.x()-grid->getMin().x())/grid->GetDx());
		jMax = (int)((tempMax.y()-grid->getMin().y())/grid->GetDy());
		kMax = (int)((tempMax.z()-grid->getMin().z())/grid->GetDz());
		
		//��ʵ������Բ�Ҫ��
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
					grid->GetObject3d(i,j,k).addObject(tempGroup->getObject(count));
				}
			}
		}
	}
}

Grid::Grid(Vec3f &_min, Vec3f &_max, int _nx, int _ny, int _nz)
{
	min = _min;
	max = _max;
	nx = _nx;
	ny = _ny;
	nz = _nz;

	//������ά���� ��������������Ķ�����û�г�ʼ������
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
	//�ͷ�isOpaque�ڴ�ռ�
	for(int i=0; i<nx; i++)
	{
		for(int j=0; j<ny; j++)
			delete [] isOpaque[i][j];
		delete [] isOpaque[i];
	}
	delete [] isOpaque;
}

//void Grid::rasterizeSphere(Vec3f center, float radius)
//{
//	float sizeX = (max.x()-min.x()) / nx;
//	float sizeY = (max.y()-min.y()) / ny;
//	float sizeZ = (max.z()-min.z()) / nz;
//	for(int i=0; i<nx; i++)
//	{
//		for(int j=0; j<ny; j++)
//		{
//			for(int k=0; k<nz; k++)
//			{
//				Vec3f temp(min.x()+sizeX*i+sizeX/2,min.y()+sizeY*j+sizeY/2,min.z()+sizeZ*k+sizeZ/2);
//				Vec3f disVec = center - temp;
//				float len = disVec.Length();
//				if(len<=radius)
//					isOpaque[i][j][k] = true;
//				else
//					isOpaque[i][j][k] = false;
//			}
//		}
//	}
//}

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
	
	//��Ҫ�����ֵ  ����rayDir.x()����0û�д���
	dtx = abs(dx / rayDir.x());
	dty = abs(dy / rayDir.y());
	dtz = abs(dz / rayDir.z());

	Vec3f normal;
	Vec3f rayOriTmin = rayOri + rayDir * tmin;
	//�����ٽ�ֵҪע��
	//inside;
	if(max.x()>rayOriTmin.x()&&min.x()<rayOriTmin.x()&&max.y()>rayOriTmin.y()&&min.y()<rayOriTmin.y()&&max.z()>rayOriTmin.z()&&min.z()<rayOriTmin.z())  //������tmin������
	{
		celli = (int)((rayOriTmin.x()-min.x())/dx);
		cellj = (int)((rayOriTmin.y()-min.y())/dy);
		cellk = (int)((rayOriTmin.z()-min.z())/dz);

		if(Utility::isZero(rayDir.x()))
			tnext_x = 10000;
		else if(signx == -1)  //���������Ļ�����tnext����Ҫ��signx  лл��ʦ����
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
		//����tmin����޸�
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

			//��i,j,k
			if(rayOri.x()+rayDir.x()*tc+tmin < min.x())   //Ҫ����0.0001
				celli = -1;
			else
				celli = (int)((rayOri.x()+rayDir.x()*tc-min.x()-tmin)/dx);  //��ȥ0.0001��Ϊ��ʹ�߽�Ҳ�㽻
			if(rayOri.y()+rayDir.y()*tc+tmin < min.y())
				cellj = -1;
			else
				cellj = (int)((rayOri.y()+rayDir.y()*tc-min.y()-tmin)/dy);
			if(rayOri.z()+rayDir.z()*tc+tmin < min.z())
				cellk = -1;
			else
				cellk = (int)((rayOri.z()+rayDir.z()*tc-min.z()-tmin)/dz);


			//��tnext
			if(Utility::isZero(rayDir.x()))
				tnext_x = 10000;
			else if(signx == -1)  //���������Ļ�����tnext����Ҫ��signx  лл��ʦ����
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
	//Object3DVector currentVector; ���Բ����½�һ��Object3DVector��Ȼ���isOpaque��ֵ�������Object3DVector����Ϊ�������������ʱ�����Object3DVector���٣���Ӧ��ָ��ָ��Ķ���Ҳ�ᱻ���١�
	//��Ȼ����һ�ַ����Ƕ���һ��Object3DVectorָ��
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
		currentT = march.GetT();  //������䣬shadowʱ������Բ�����grid��
	}
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
		currentT = march.GetT();  //������䣬shadowʱ������Բ�����grid��
	}
	return false;
}