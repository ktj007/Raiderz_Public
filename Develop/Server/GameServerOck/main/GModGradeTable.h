#ifndef _GMOD_GRADE_TABLE_H
#define _GMOD_GRADE_TABLE_H

#include "GDef.h"
#include "GMath.h"

#define NPC_GRADE_MIN		0	// (0~10)
#define NPC_GRADE_MAX		10
#define NPC_GRADE_COUNT		(NPC_GRADE_MAX-NPC_GRADE_MIN+1)

class GModGradeTable
{
private:
	float m_Tables[NPC_GRADE_COUNT];
public:
	GModGradeTable();
	float GetMODGrade(int nNPCGrade);
};


#endif