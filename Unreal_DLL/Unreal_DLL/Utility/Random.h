#pragma once
#include <random>
#include "../Common/DLL.h"
#ifndef UINT
typedef unsigned int UINT;
#endif
class DLL_CLASS Random
{
private:
	std::mt19937 eng;
	std::uniform_int_distribution<UINT> dist;// (eng.min(), eng.max());
public:
	Random();
	double GetNormFloat();
	UINT GetInt();
	double GetRangedFloat(double min, double max);
};