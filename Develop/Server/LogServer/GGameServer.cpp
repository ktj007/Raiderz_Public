#include "stdafx.h"
#include "GGameServer.h"
#include "GCommandTable.h"
#include "GCommandCenter.h"
#include "GDBManager.h"
#include "GLog_Char.h"
#include "GLog_ItemAcquire.h"
#include "GLog_ItemLost.h"
#include "GLog_ItemTrade.h"
#include "GLog_Quest.h"

GGameServer::GGameServer(MUID& uid)
: m_UID(uid)
{
	;
}

GGameServer::~GGameServer()
{
	;
}

int GGameServer::CalcLogPoint()
{
	return (int)(m_vecTopPriorityLog.size() * LOG_POINT_TOP_PRIORITY + 
				m_vecSecondPriorityLog.size() * LOG_POINT_SECOND_PRIORITY +
				m_vecThirdPriorityLog.size() * LOG_POINT_THIRD_PRIORITY);
}

void GGameServer::PickUpLog(int nQuantity, vector<GLog*>& outvecLog)
{
	for (VEC_LOG::iterator itor = m_vecTopPriorityLog.begin(); itor != m_vecTopPriorityLog.end();)
	{
		if (nQuantity <= (int) outvecLog.size()) return;

		GLog* pLog = (*itor);		
		outvecLog.push_back(pLog);
		itor = m_vecTopPriorityLog.erase(itor);
	}	

	for (VEC_LOG::iterator itor = m_vecSecondPriorityLog.begin(); itor != m_vecSecondPriorityLog.end();)
	{
		if (nQuantity <= (int) outvecLog.size()) return;

		GLog* pLog = (*itor);		
		outvecLog.push_back(pLog);
		itor = m_vecSecondPriorityLog.erase(itor);
	}	

	for (VEC_LOG::iterator itor = m_vecThirdPriorityLog.begin(); itor != m_vecThirdPriorityLog.end();)
	{
		if (nQuantity <= (int) outvecLog.size()) return;

		GLog* pLog = (*itor);		
		outvecLog.push_back(pLog);
		itor = m_vecThirdPriorityLog.erase(itor);
	}
}

void GGameServer::InsertLog_Char(const string& strDate, int nCID, LOG_CHAR_TYPE nType, const string& strIP)
{
	GLog_Char* pNewLog = new GLog_Char(strDate, nCID, nType, strIP);
	m_vecTopPriorityLog.push_back(pNewLog);
}

void GGameServer::InsertLog_ItemAcquire(const string& strDate, int nCID, LOG_ITEM_ACQUIRE_TYPE nType, const int64& nIUID, int nQuantity, int nGold)
{
	GLog_ItemAcquire* pNewLog = new GLog_ItemAcquire(strDate, nCID, nType, nIUID, nQuantity, nGold);
	m_vecTopPriorityLog.push_back(pNewLog);
}

void GGameServer::InsertLog_ItemLost(const string& strDate, int nCID, LOG_ITEM_LOST_TYPE nType, const int64& nIUID, int nQuantity, int nGold)
{
	GLog_ItemLost* pNewLog = new GLog_ItemLost(strDate, nCID, nType, nIUID, nQuantity, nGold);
	m_vecTopPriorityLog.push_back(pNewLog);
}

void GGameServer::InsertLog_ItemTrade(const string& strDate,	int nCID1, const vector<TDL_LOG_ITEM>& vecTDItem1, int nGold1, 
																int nCID2, const vector<TDL_LOG_ITEM>& vecTDItem2, int nGold2)
{
	GLog_ItemTrade* pNewLog = new GLog_ItemTrade(strDate,	nCID1, vecTDItem1, nGold1,
															nCID2, vecTDItem2, nGold2);
	m_vecTopPriorityLog.push_back(pNewLog);
}

void GGameServer::InsertLog_Quest(const string& strDate, int nCID, LOG_QUEST_TYPE nType, int nQID)
{
	GLog_Quest* pNewLog = new GLog_Quest(strDate, nCID, nType, nQID);
	m_vecTopPriorityLog.push_back(pNewLog);
}