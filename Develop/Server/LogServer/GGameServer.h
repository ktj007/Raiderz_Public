#ifndef _G_GAME_SERVER_H_
#define _G_GAME_SERVER_H_

#include <map>
#include "MUID.h"
#include "GLog.h"

using namespace std;

class GGameServerMgr;
class GServer;

struct TDL_LOG_ITEM;

enum LOG_CHAR_TYPE;
enum LOG_ITEM_ACQUIRE_TYPE;
enum LOG_ITEM_LOST_TYPE;
enum LOG_QUEST_TYPE;



/// 게임 서버
class GGameServer
{
	friend class GGameServerMgr;
private:	
	MUID	m_UID;						///< 게임서버 UID. Link UID와 같은 값이다.

	VEC_LOG	m_vecTopPriorityLog;		///< 최고 순위 로그
	VEC_LOG	m_vecSecondPriorityLog;		///< 두번째 순위 로그
	VEC_LOG	m_vecThirdPriorityLog;		///< 세번째 순위 로그

private:
	GGameServer(MUID& uid);	

public:
	virtual ~GGameServer();

	const MUID	GetUID(void) const { return m_UID; }
	int CalcLogPoint();
	void PickUpLog(int nQuantity, vector<GLog*>& outvecLog);	

	void InsertLog_Char(const string& strDate, int nCID, LOG_CHAR_TYPE nType, const string& strIP);

	void InsertLog_ItemAcquire(const string& strDate, int nCID, LOG_ITEM_ACQUIRE_TYPE nType, const int64& nIUID, int nQuantity, int nGold);
	void InsertLog_ItemLost(const string& strDate, int nCID, LOG_ITEM_LOST_TYPE nType, const int64& nIUID, int nQuantity, int nGold);
	void InsertLog_ItemTrade(const string& strDate,	int nCID1, const vector<TDL_LOG_ITEM>& vecTDItem1, int nGold1, 
													int nCID2, const vector<TDL_LOG_ITEM>& vecTDItem2, int nGold2);

	void InsertLog_Quest(const string& strDate, int nCID, LOG_QUEST_TYPE nType, int nQID);
};

#endif