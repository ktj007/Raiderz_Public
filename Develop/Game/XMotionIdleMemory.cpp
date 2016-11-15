#include "stdafx.h"
#include "XMotionIdleMemory.h"
#include "XMotionLogic.h"

void XMotionIdleMemory::Init()
{
}

void XMotionIdleMemory::SetIdleMemoryMotion( int nBuffID, wstring strMotion, XMotionTypes nType/*=MT_DEFAULT*/, bool bForced/*=false*/ )
{
	stBuffAniMemory stBuffAni;
	stBuffAni.m_nBuffID = nBuffID;
	stBuffAni.m_strMotionName = strMotion;
	stBuffAni.m_nType = nType;
	stBuffAni.m_bForced = bForced;

	m_vecBuffAniMemory.push_back(stBuffAni);

	m_bMemory = true;
}


void XMotionIdleMemory::SetIdleMemoryCustomMotion( int nBuffID, wstring strMotion, wstring strAnimationName, bool bForced/*=false*/ )
{
	stBuffAniMemory stBuffAni;
	stBuffAni.m_nBuffID = nBuffID;
	stBuffAni.m_strMotionName = strMotion;
	stBuffAni.m_strAnimationName = strAnimationName;
	stBuffAni.m_bForced = bForced;

	m_vecBuffAniMemory.push_back(stBuffAni);

	m_bMemory = true;
}

bool XMotionIdleMemory::CheckChangeMotion( wstring strMotion, XMotionLogic* pLogic, bool bForceIdle )
{
	if(m_bMemory == false)
		return false;

	if(strMotion == MOTION_NAME_IDLE ||
		strMotion == MOTION_NAME_NPC_IDLE)
	{
		if(bForceIdle)
			return false;

		return ChangeMotion(pLogic);
	}

	return false;
}


bool XMotionIdleMemory::CheckPushMotion( wstring strMotion, XMotionLogic* pLogic, bool bForceIdle )
{
	if(m_bMemory == false)
		return false;

	if(strMotion == MOTION_NAME_IDLE ||
		strMotion == MOTION_NAME_NPC_IDLE)
	{
		if(bForceIdle)
			return false;

		return PushMotion(pLogic);
	}

	return false;
}

bool XMotionIdleMemory::ChangeMotion( XMotionLogic* pLogic )
{
	stBuffAniMemory* pData = GetBuffAniLastData();

	if(pData == NULL)
		return false;

	if(pData->m_strAnimationName.empty())
		pLogic->ChangeMotion(pData->m_strMotionName.c_str(), pData->m_nType, pData->m_bForced);
	else
		pLogic->ChangeToCustomMotion(pData->m_strMotionName.c_str(), pData->m_strAnimationName.c_str(), pData->m_bForced);

	return true;
}

bool XMotionIdleMemory::PushMotion( XMotionLogic* pLogic )
{
	stBuffAniMemory* pData = GetBuffAniLastData();

	if(pData == NULL)
		return false;

	if(pData->m_strAnimationName.empty())
		pLogic->PushMotion(pData->m_strMotionName.c_str());
	else
		pLogic->PushCustomMotion(pData->m_strMotionName.c_str(), pData->m_strAnimationName.c_str());

	return true;
}

void XMotionIdleMemory::End( int nBuffID, XMotionLogic* pLogic, bool bNpc )
{
	stBuffAniMemory stDelAniData;
	if(DeleteBuffAni(nBuffID, stDelAniData) == false)
		return;

	stBuffAniMemory* pLastAniData = GetBuffAniLastData();

	wstring strChangeMotion;
	wstring strChangeAnimation = L"";
	bool bForce = false;
	if(pLastAniData == NULL)
	{
		// 기억된 모션을 Idle로 변경한다.
		strChangeMotion = MOTION_NAME_IDLE;
		if(bNpc)
			strChangeMotion = MOTION_NAME_NPC_IDLE;
	}
	else
	{
		strChangeMotion = pLastAniData->m_strMotionName;
		strChangeAnimation = pLastAniData->m_strAnimationName;
		bForce = pLastAniData->m_bForced;
	}

	stBuffAniMemory* pData = GetBuffAniData(nBuffID);

	if(pLogic->ChangeMotionQueueInfo(stDelAniData.m_strMotionName, stDelAniData.m_strAnimationName, strChangeMotion, strChangeAnimation) == false)
	{
		// 대기중인 모션이 없다.
		// 현재 기억된 모션중인가?
		if(pLogic->GetCurrMotion() == stDelAniData.m_strMotionName)
		{
			if(stDelAniData.m_strAnimationName.empty() || pLogic->GetCustomMotionAniName(stDelAniData.m_strMotionName) == stDelAniData.m_strAnimationName)
			{
				if(strChangeAnimation.empty())
					pLogic->ChangeMotion(strChangeMotion.c_str(), MT_DEFAULT, true);
				else
					pLogic->ChangeToCustomMotion(strChangeMotion.c_str(), strChangeAnimation.c_str(), bForce);
			}
		}
	}

	// 남은 버프 애니메이션 체크
	if(m_vecBuffAniMemory.empty())
		m_bMemory = false;
}

XMotionIdleMemory::stBuffAniMemory* XMotionIdleMemory::GetBuffAniLastData()
{
	int nSize = m_vecBuffAniMemory.size();

	if(nSize == 0)
		return NULL;

	return &(m_vecBuffAniMemory[nSize - 1]);
}

XMotionIdleMemory::stBuffAniMemory* XMotionIdleMemory::GetBuffAniData( int nBuffID )
{
	for(vector<stBuffAniMemory>::iterator it = m_vecBuffAniMemory.begin(); it != m_vecBuffAniMemory.end(); ++it)
	{
		if(it->m_nBuffID == nBuffID)
			return &(*it);
	}

	return NULL;
}

bool XMotionIdleMemory::DeleteBuffAni( int nBuffID, stBuffAniMemory& stDelData )
{
	for(vector<stBuffAniMemory>::iterator it = m_vecBuffAniMemory.begin(); it != m_vecBuffAniMemory.end(); ++it)
	{
		if(it->m_nBuffID == nBuffID)
		{
			stDelData = *it;
			m_vecBuffAniMemory.erase(it);
			return true;
		}
	}

	return false;
}

void XMotionIdleMemory::AllDel()
{
	m_vecBuffAniMemory.clear();

	m_bMemory = false;
}
