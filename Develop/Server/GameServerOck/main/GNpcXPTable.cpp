#include "stdafx.h"
#include "GNpcXPTable.h"


GNpcXPTable::GNpcXPTable()
{
	MakeNPCGradeFactor();
}

void GNpcXPTable::MakeNPCGradeFactor()
{
	// http://iworks2.maiet.net/wiki/sh:formulas:xp
	// 등급별 경험치 수정자

	m_FactorGrade[0]		= 0.0f;
	m_FactorGrade[1]		= 0.3f;
	m_FactorGrade[2]		= 0.6f;
	m_FactorGrade[3]		= 1.0f;
	m_FactorGrade[4]		= 5.0f;
	m_FactorGrade[5]		= 10.0f;
	m_FactorGrade[6]		= 15.0f;
	m_FactorGrade[7]		= 20.0f;
	m_FactorGrade[8]		= 25.0f;
	m_FactorGrade[9]		= 35.0f;
	m_FactorGrade[10]		= 45.0f;
}

float GNpcXPTable::GetFactorGrade( int nGrade )
{
	if (nGrade < 0) nGrade = 0;
	if (nGrade > MAX_NPC_GRADE) nGrade = MAX_NPC_GRADE;

	return m_FactorGrade[nGrade];
}