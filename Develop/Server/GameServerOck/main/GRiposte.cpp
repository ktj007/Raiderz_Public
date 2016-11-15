#include "StdAfx.h"
#include "GRiposte.h"
#include "GDef.h"

GRiposte::GRiposte(void)
{
	Clear();
}

GRiposte::~GRiposte(void)
{
}

void GRiposte::AddRiposte( TALENT_CATEGORY nTalentCategory, float fRate )
{
	VALID(nTalentCategory >= 0);
	VALID(nTalentCategory < TC_MAX);
	m_Rate[nTalentCategory] += fRate;
}

void GRiposte::EraseRiposte( TALENT_CATEGORY nTalentCategory, float fRate )
{
	VALID(nTalentCategory >= 0);
	VALID(nTalentCategory < TC_MAX);
	m_Rate[nTalentCategory] -= fRate;
	max(m_Rate[nTalentCategory], 0.0f);	// 보정치가 음수가 되지는 않음
}

bool GRiposte::HasRiposte( TALENT_CATEGORY nTalentCategory )
{
	VALID_RET(nTalentCategory >= 0, false);
	VALID_RET(nTalentCategory < TC_MAX, false);
	return (m_Rate[nTalentCategory] > 0.0f);	// 보정치가 있다면 되받아치기 속성이 있다고 구분
}

float GRiposte::GetRiposteRate( TALENT_CATEGORY nTalentCategory )
{
	VALID_RET(nTalentCategory >= 0, 0.0f);
	VALID_RET(nTalentCategory < TC_MAX, 0.0f);
	return m_Rate[nTalentCategory];
}

void GRiposte::Clear()
{
	for (int i = 0; i < TC_MAX; i++)
	{
		m_Rate[i] = 0.0f;
	}
}