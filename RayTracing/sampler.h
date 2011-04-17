#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include "vectors.h"

class Sampler
{
public:
	Sampler():numSamples(0){}
	Sampler(int _numSamples):numSamples(_numSamples){}
	virtual Vec2f getSamplePosition(int n) = 0;
protected:
	int numSamples;
};

class RandomSampler : public Sampler
{
public:
	RandomSampler(int _numSamples){ numSamples = _numSamples;}
	Vec2f getSamplePosition(int n);
};

class UniformSampler : public Sampler
{
public:
	UniformSampler(int _numSamples){ numSamples = _numSamples;}
	Vec2f getSamplePosition(int n);
};

class JitteredSampler : public Sampler
{
public:
	JitteredSampler(int _numSamples){ numSamples = _numSamples;}
	Vec2f getSamplePosition(int n);
};

#endif