#pragma once

#include "MUID.h"
#include "SCommObject.h"
#include "STypes.h"
#include "SAccountInfo.h"
#include "LSelMyCharTimeoutHandler.h"
#include "PmDef.h"
#include <map>
using namespace std;

class LPlayerObjectManager;
class LServer;
class LMasterServer;

/// 플레이어 오브젝트의 상태
enum PlayerObjectState
{
	POS_INVALID				= -1,
	POS_WAITING_FOR_LOGIN	= 0,	///< 로그인 대기중
	POS_SELECT_CHAR,				///< 캐릭터 고르는 중
	POS_LOGIN_GAME_SERVER,			///< 게임서버 로그인 요청 중
	POS_MOVE_SERVER,				///< 게임서버로 이동 중
};


/// 플레이어 오브젝트
class LPlayerObject : public SCommObject
{	
protected:
	friend class LPlayerObjectManager;

	ACCOUNT_INFO					m_AccountInfo;		///< 계정 정보
	PlayerObjectState				m_nState;			///< 플레이어 오브젝트의 상태
		
	vector<SAccountCharFieldInfo>	m_vecAccountCharFieldInfo;
	LMasterServer*					m_pConnectingMasterServer;	

	SServerType						m_nServerType;
	int								m_nServerID;	
	
	int								m_nSelectedCharIndex;
	LSelMyCharTimeoutHandler		m_SelMyCharTimeoutHandler;

	/// PmUserData
	PmStatIndex						m_statIndex;
	int								m_nPCCafeID;
	bool							m_isSetStatIndex;

public:
	LPlayerObject(MUID& uidObject);
	virtual ~LPlayerObject();
	virtual void Create() override;							///< LPlayerObjectManager에서 호출
	virtual void Destroy() override;						///< LPlayerObjectManager에서 호출

	void InitAccountInfo(ACCOUNT_INFO& info);	
	void StartWatchingSelMyCharTimeout();

	void ClearCharFieldInfo();
	void AddCharFieldInfo(const SAccountCharFieldInfo& accountCharFieldInfo);
	void RemoveCharFieldInfo(size_t nIndex);
	int GetCharFieldInfoCount();
	SAccountCharFieldInfo* GetCharFieldInfo(size_t nIndex);	
	
	// 상태변경 처리 메소드.
	void OnSelectChar();
	void OnLoginGameServer(int nCharIndex);
	void OnMoveServer();
	
	void AddStatIndex(const PmStatIndex* statIndex, int nPCCafeID);
	PmStatIndex* GetStatIndex();
	int GetPCCafeID();
	bool IsExistStatIndex();

	const MUID			GetUID(void) const		{ return m_UID; }
	ACCOUNT_INFO&		GetAccountInfo()		{ return m_AccountInfo; }
	PlayerObjectState	GetState()				{ return m_nState; }
	LMasterServer*		GetMasterServer()		{ return m_pConnectingMasterServer; }
	int					GetSelectedCharIndex()	{ return m_nSelectedCharIndex; }
};
