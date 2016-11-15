#ifndef _GNPC_XP_TABLE_H
#define _GNPC_XP_TABLE_H

#include "GDef.h"
#include "GMath.h"
#include "GLevelFactorTable.h"
#include "CSChrInfo.h"

class GNpcXPTable
{
protected:
	float m_FactorGrade[MAX_NPC_GRADE+1];

	void MakeNPCGradeFactor();
public:
	GNpcXPTable();
	float GetFactorGrade(int nGrade);
};


#endif