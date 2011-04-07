#ifndef _UTILITY_H_
#define _UTILITY_H_

class Utility{
public:
	static float getMax(float x, float y, float z){
		if(x>=y && x>= z) return x;
		else if(y>=x && y>=z) return y;
		else return z;}
	static float getMin(float x, float y, float z){
		if(x<=y && x<=z) return x;
		else if(y<=x && y<=z) return y;
		else return z;}
	static bool isZero(float x){
		if(abs(abs(x)-0.000001)<=0.00001)  //×¢ÒâÕâÀï
			return true;
		else
			return false;}
	static bool isEqual(float a, float b){
		if(abs(a-b) <= 0.000001)
			return true;
		else
			return false;}
};

#endif