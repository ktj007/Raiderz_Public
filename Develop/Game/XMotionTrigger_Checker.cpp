#include "stdafx.h"
#include "XMotionTrigger_Checker.h"

XMotionTrigger_Checker::XMotionTrigger_Checker()
{
	m_vecIf.resize(MTRIGGER_IF_MAX);

	for (int i = 0; i < MTRIGGER_IF_MAX; i++)
	{
		m_vecIf[i] = NULL;
	}

	m_vecIf[MTRIGGER_IF_TRUE] = new XMotionTrigger_IfTrue();
	m_vecIf[MTRIGGER_IF_DEAD] = new XMotionTrigger_IfDead();
}

XMotionTrigger_Checker::~XMotionTrigger_Checker()
{
	for (size_t i = 0; i < m_vecIf.size(); i++)
	{
		SAFE_DELETE(m_vecIf[i]);
	}
	m_vecIf.clear();
}

bool XMotionTrigger_Checker::Check( XMotionTrigger_IfType nIfType, XActor* pActor )
{
	if (nIfType < 0 || nIfType > MTRIGGER_IF_MAX) return false;

	_ASSERT(m_vecIf[nIfType] != NULL);
	return m_vecIf[nIfType]->Check(pActor);
}