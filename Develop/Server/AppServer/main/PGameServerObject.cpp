#include "stdafx.h"
#include "PGameServerObject.h"
#include "PCommandTable.h"
#include "PCommandCenter.h"
#include "PDBManager.h"
#include "Log/PLog_Char.h"
#include "Log/PLog_ItemAcquire.h"
#include "Log/PLog_ItemLost.h"
#include "Log/PLog_ItemTrade.h"
#include "Log/PLog_Quest.h"
#include "PDef.h"

PGameServerObject::PGameServerObject(MUID& uidObject, int nServerID)
: SCommObject(uidObject)
, m_nServerID(nServerID)
{
	;
}

PGameServerObject::~PGameServerObject()
{
	;
}


int PGameServerObject::CalcLogPoint()
{
	return (int)(m_vecTopPriorityLog.size() * LOG_POINT_TOP_PRIORITY + 
		m_vecSecondPriorityLog.size() * LOG_POINT_SECOND_PRIORITY +
		m_vecThirdPriorityLog.size() * LOG_POINT_THIRD_PRIORITY);
}

void PGameServerObject::PickUpLog(int nQuantity, vector<PLog*>& outvecLog)
{
	for (VEC_LOG::iterator itor = m_vecTopPriorityLog.begin(); itor != m_vecTopPriorityLog.end();)
	{
		if (nQuantity <= (int) outvecLog.size()) return;

		PLog* pLog = (*itor);		
		outvecLog.push_back(pLog);
		itor = m_vecTopPriorityLog.erase(itor);
	}	

	for (VEC_LOG::iterator itor = m_vecSecondPriorityLog.begin(); itor != m_vecSecondPriorityLog.end();)
	{
		if (nQuantity <= (int) outvecLog.size()) return;

		PLog* pLog = (*itor);		
		outvecLog.push_back(pLog);
		itor = m_vecSecondPriorityLog.erase(itor);
	}	

	for (VEC_LOG::iterator itor = m_vecThirdPriorityLog.begin(); itor != m_vecThirdPriorityLog.end();)
	{
		if (nQuantity <= (int) outvecLog.size()) return;

		PLog* pLog = (*itor);		
		outvecLog.push_back(pLog);
		itor = m_vecThirdPriorityLog.erase(itor);
	}
}

void PGameServerObject::InsertLog_Char(const wstring& strDate, int nCID, LOG_CHAR_TYPE nType, const wstring& strIP)
{
	PLog_Char* pNewLog = new PLog_Char(strDate, nCID, nType, strIP);
	m_vecTopPriorityLog.push_back(pNewLog);
}

void PGameServerObject::InsertLog_ItemAcquire(const wstring& strDate, int nCID, LOG_ITEM_ACQUIRE_TYPE nType, const int64& nIUID, int nQuantity, int nGold)
{
	PLog_ItemAcquire* pNewLog = new PLog_ItemAcquire(strDate, nCID, nType, nIUID, nQuantity, nGold);
	m_vecTopPriorityLog.push_back(pNewLog);
}

void PGameServerObject::InsertLog_ItemLost(const wstring& strDate, int nCID, LOG_ITEM_LOST_TYPE nType, const int64& nIUID, int nQuantity, int nGold)
{
	PLog_ItemLost* pNewLog = new PLog_ItemLost(strDate, nCID, nType, nIUID, nQuantity, nGold);
	m_vecTopPriorityLog.push_back(pNewLog);
}

void PGameServerObject::InsertLog_ItemTrade(const wstring& strDate,	int nCID1, const vector<TDL_LOG_ITEM>& vecTDItem1, int nGold1, 
									  int nCID2, const vector<TDL_LOG_ITEM>& vecTDItem2, int nGold2)
{
	PLog_ItemTrade* pNewLog = new PLog_ItemTrade(strDate,	nCID1, vecTDItem1, nGold1,
		nCID2, vecTDItem2, nGold2);
	m_vecTopPriorityLog.push_back(pNewLog);
}

void PGameServerObject::InsertLog_Quest(const wstring& strDate, int nCID, LOG_QUEST_TYPE nType, int nQID)
{
	PLog_Quest* pNewLog = new PLog_Quest(strDate, nCID, nType, nQID);
	m_vecTopPriorityLog.push_back(pNewLog);
}