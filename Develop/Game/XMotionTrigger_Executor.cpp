#include "stdafx.h"
#include "XMotionTrigger_Executor.h"


XMotionTrigger_Executor::XMotionTrigger_Executor()
{
	m_vecExecute.resize(MTRIGGER_THEN_MAX);

	for (int i = 0; i < MTRIGGER_THEN_MAX; i++)
	{
		m_vecExecute[i] = NULL;
	}

	m_vecExecute[MTRIGGER_THEN_CHANGE_MOTION] = new XMotionTrigger_ExecuteChangeMotion();
	m_vecExecute[MTRIGGER_THEN_HOLD] = new XMotionTrigger_ExecuteHold();
}

XMotionTrigger_Executor::~XMotionTrigger_Executor()
{
	for (size_t i = 0; i < m_vecExecute.size(); i++)
	{
		SAFE_DELETE(m_vecExecute[i]);
	}
	m_vecExecute.clear();
}

void XMotionTrigger_Executor::Execute( XMotionTrigger_ThenType nThenType, const wstring& strParam1, const wstring& strParam2, XMotionLogic* pMotionLogic )
{
	if (nThenType < 0 || nThenType > MTRIGGER_THEN_MAX) return;

	_ASSERT(m_vecExecute[nThenType] != NULL);
	m_vecExecute[nThenType]->Execute(pMotionLogic, strParam1, strParam2);
}