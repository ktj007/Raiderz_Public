#pragma once

#include "MUID.h"
#include "SCommObject.h"
#include "STypes.h"
#include <map>
using namespace std;

#include "Log/PLog.h"

class PGameServerObjectManager;
class PServer;

struct TDL_LOG_ITEM;

enum LOG_CHAR_TYPE;
enum LOG_ITEM_ACQUIRE_TYPE;
enum LOG_ITEM_LOST_TYPE;
enum LOG_QUEST_TYPE;


/// 게임 서버 오브젝트
class PGameServerObject : public SCommObject
{	
protected:
	SServerType				m_nServerType;
	int						m_nServerID;

	VEC_LOG	m_vecTopPriorityLog;		///< 최고 순위 로그
	VEC_LOG	m_vecSecondPriorityLog;		///< 두번째 순위 로그
	VEC_LOG	m_vecThirdPriorityLog;		///< 세번째 순위 로그

	PGameServerObject() : SCommObject(), m_nServerID(0), m_nServerType(SERVER_GAME) {}
public:
	PGameServerObject(MUID& uidObject, int nServerID);
	virtual ~PGameServerObject();

	int GetServerID() { return m_nServerID; }

	int CalcLogPoint();
	void PickUpLog(int nQuantity, vector<PLog*>& outvecLog);	

	void InsertLog_Char(const wstring& strDate, int nCID, LOG_CHAR_TYPE nType, const wstring& strIP);

	void InsertLog_ItemAcquire(const wstring& strDate, int nCID, LOG_ITEM_ACQUIRE_TYPE nType, const int64& nIUID, int nQuantity, int nGold);
	void InsertLog_ItemLost(const wstring& strDate, int nCID, LOG_ITEM_LOST_TYPE nType, const int64& nIUID, int nQuantity, int nGold);
	void InsertLog_ItemTrade(const wstring& strDate,	int nCID1, const vector<TDL_LOG_ITEM>& vecTDItem1, int nGold1, 
		int nCID2, const vector<TDL_LOG_ITEM>& vecTDItem2, int nGold2);

	void InsertLog_Quest(const wstring& strDate, int nCID, LOG_QUEST_TYPE nType, int nQID);

};

