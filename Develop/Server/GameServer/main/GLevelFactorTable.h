#ifndef _GLEVEL_FACTOR_TABLE_H
#define _GLEVEL_FACTOR_TABLE_H

#include "GDef.h"
#include "GMath.h"

#define LEVELFACTOR_DIFF_MIN		(-10)
#define LEVELFACTOR_DIFF_MAX		10
#define LEVELFACTOR_COUNT			(LEVELFACTOR_DIFF_MAX-LEVELFACTOR_DIFF_MIN+1)		// 21

/// ∑π∫ß ∆—≈Õ
class GLevelFactorTable
{
private:
	int m_nValues[LEVELFACTOR_COUNT];
public:
	GLevelFactorTable();
	int GetFactor(int nActorLevel, int nTargetLevel);
	int GetFactor(int nLevelDiff);
};



#endif