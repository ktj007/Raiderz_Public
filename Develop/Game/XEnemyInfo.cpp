#include "StdAfx.h"
#include "XEnemyInfo.h"
#include "XNonPlayer.h"
#include "XModuleActorControl.h"
#include "XScript.h"

void XEnemyInfo::Clear()
{
	m_vecEnemies.clear();
}

void XEnemyInfo::ClearEnemyInfo(UIID nUIID)
{
	for(vector<TD_ENEMY_INFO>::iterator itor = m_vecEnemies.begin(); itor != m_vecEnemies.end(); ++itor)
	{
		if(itor->nUIID == nUIID)
		{
			m_vecEnemies.erase(itor);
			return;
		}
	}
}

TD_ENEMY_INFO* XEnemyInfo::GetEnemyInfo( int nIndex )
{
	if( nIndex < 0 || (int)m_vecEnemies.size() <= nIndex) return NULL;

	return &(m_vecEnemies[nIndex]);
}

TD_ENEMY_INFO* XEnemyInfo::GetEnemyInfoByUIID( UIID nUIID )
{
	XActor* pActor = gg.omgr->FindActor_UIID(nUIID);
	if(pActor == NULL)	return NULL;

	for(vector<TD_ENEMY_INFO>::iterator itor = m_vecEnemies.begin(); itor != m_vecEnemies.end(); ++itor)
	{
		if(itor->nUIID == nUIID)
			return &(*itor);
	}

	return NULL;
}

void XEnemyInfo::CheckDamageInfo()
{
	for(vector<TD_ENEMY_INFO>::iterator itor = m_vecEnemies.begin(); itor != m_vecEnemies.end(); ++itor)
	{
		XActor* pActor = gg.omgr->FindActor_UIID(itor->nUIID);
		if(pActor == NULL)	continue;

		if(pActor->GetType() == XOBJ_NPC)
		{
			XModuleActorControl* pActorControl = pActor->GetModuleActorControl();
			if(pActorControl)
				pActorControl->SetApplicationDamage();
		}
	}
}

void XEnemyInfo::NPCGradeSort()
{
	if(m_vecEnemies.size() < 2) return;

	struct GreateGrade
	{
		bool operator()(const TD_ENEMY_INFO& info1,const TD_ENEMY_INFO& info2) const
		{
			_ASSERT(info1.nHPPercent);
			_ASSERT(info2.nHPPercent);

			XNonPlayer* pNPC1 = gg.omgr->FindNPC_UIID(info1.nUIID);
			XNonPlayer* pNPC2 = gg.omgr->FindNPC_UIID(info2.nUIID);
			if (pNPC1 && pNPC2 )
			{
				XNPCInfo* pNPCInfo1 = pNPC1->GetInfo();
				XNPCInfo* pNPCInfo2 = pNPC2->GetInfo();

				return pNPCInfo1->nGrade > pNPCInfo2->nGrade;
			}

			return false;
		}
	};

	std::sort(m_vecEnemies.begin(), m_vecEnemies.end(), GreateGrade());
}

// 오브젝트 리스트에 없는 NPC이면 삭제
void XEnemyInfo::CheckEnemyInfoList()
{
	for(vector<TD_ENEMY_INFO>::iterator itor = m_vecEnemies.begin();
		itor != m_vecEnemies.end();)
	{
		XActor* pActor = gg.omgr->FindActor_UIID(itor->nUIID);
		if(pActor == NULL)	
		{
			itor = m_vecEnemies.erase(itor);
			continue;
		}

		 ++itor;
	}
}