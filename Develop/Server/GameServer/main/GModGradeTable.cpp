#include "stdafx.h"
#include "GModGradeTable.h"

GModGradeTable::GModGradeTable()
{
	for (int grade = NPC_GRADE_MIN; grade <= NPC_GRADE_MAX; grade++)
	{
		double a = ((grade*grade) + (12 * grade)) / 45.0f;
		m_Tables[grade] = float(GMath::Round(a * 10.0) / 10.0);
	}
}

float GModGradeTable::GetMODGrade( int nNPCGrade )
{
	if (nNPCGrade < NPC_GRADE_MIN) nNPCGrade = NPC_GRADE_MIN;
	else if (nNPCGrade > NPC_GRADE_MAX) nNPCGrade = NPC_GRADE_MAX;

	return m_Tables[nNPCGrade];
}
