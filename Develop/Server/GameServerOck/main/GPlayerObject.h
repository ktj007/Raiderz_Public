#ifndef _GPLAYER_OBJECT_H
#define _GPLAYER_OBJECT_H

#include "MMemPool.h"
#include "MUID.h"
#include "GDef.h"
#include "GEntityPlayer.h"
#include "SAccountInfo.h"
#include "PmDef.h"

#include <map>
using namespace std;

class GPlayerObjectManager;
class GServer;
class GEntitySync;
class GField;
class GInventory;
class MockPlayerObject;
class GPlayerGameGuard;
class GPlayerSerializer;

/// 플레이어 오브젝트의 상태
enum PlayerObjectState
{
	POS_SELECT_CHAR	 = 0,			///< 캐릭터 고르는 중
	POS_INWORLD,					///< 게임중
	POS_READY_MOVE_GAME_SERVER,		///< 게임 서버 이동 준비
	POS_MOVE_GAME_SERVER,			///< 게임 서버 이동
	POS_READY_MOVE_LOGIN_SERVER,	///< 로그인서버로 이동 중비
	POS_MOVE_LOGIN_SERVER,			///< 로그인서버로 이동
	POS_LOGOUT,						///< 로그아웃 처리 중
	POS_DOING_LOGIN					///< Fake:로그인 처리 중
};

/// 플레이어 오브젝트
class GPlayerObject
{
protected:
	friend class GPlayerObjectManager;
	friend class MockPlayerObject;
	friend class GPlayerSerializer;

	bool				m_bCreated;

	MUID				m_UID;				///< 플레이어 UID. Link UID와 같은 값이다.
	UIID				m_nUIID;			///< m_UID의 인덱스 ID

	ACCOUNT_INFO		m_AccountInfo;		///< 계정 정보
	PlayerObjectState	m_nState;			///< 플레이어 오브젝트의 상태
	GEntityPlayer*		m_pMyEntity;		///< 플레이어 엔티티

	vector<SAccountCharFieldInfo> m_vecCharFieldInfo;
	
	PmStatIndex			m_statIndex;
	int					m_nPCCafeID;
	bool				m_isSetStatIndex;
	
	GPlayerGameGuard*	m_pGameGuard;

	// 서버이동 관련 변수들
	MUID				m_uidMoveGameServerTask;
	MUID				m_uidMoveLoginServerTask;
	MUID				m_uidMoveLoginServerConnectionKey;

	bool				m_bForceDisconnect;	///< Disconnect 디버깅용 플레그 - aibeast
	
public:
	GPlayerObject() {}
	GPlayerObject(const MUID& uid);
	virtual ~GPlayerObject();
	virtual void Create();							///< GPlayerObjectManager에서 호출
	virtual void Destroy();							///< GPlayerObjectManager에서 호출

	void InitAccountInfo(const unsigned int nAID, const wstring& strUserID, const bool bNewAcc);
	
	void OnStartGame()				{ m_nState = POS_INWORLD; }
	void OnBeginLogin()				{ m_nState = POS_DOING_LOGIN; }
	void OnEndLogin()				{ m_nState = POS_SELECT_CHAR; }
	void OnLogout()					{ m_nState = POS_LOGOUT; }
	void OnReadyMoveGameServer()	{ m_nState = POS_READY_MOVE_GAME_SERVER; }
	void OnMoveGameServer()			{ m_nState = POS_MOVE_GAME_SERVER; }
	void OnReadyMoveLoginServer()	{ m_nState = POS_READY_MOVE_LOGIN_SERVER; }
	void OnMoveLoginServer()		{ m_nState = POS_MOVE_LOGIN_SERVER; }

	void Update(float fDelta);

	bool SerializeSelectCharacter(int nCID, bool bReload = false);

	void AddStatIndex(const PmStatIndex* statIndex, int nPCCafeID);
	PmStatIndex* GetStatIndex();
	int GetPCCafeID();
	bool IsExistStatIndex();

	void ClearCharFieldInfo();
	void AddCharFieldInfo(const SAccountCharFieldInfo& accountCharFieldInfo);
	void RemoveCharFieldInfo(size_t nIndex);
	int GetCharFieldInfoCount();
	SAccountCharFieldInfo* GetCharFieldInfo(size_t nIndex);

	// Move to GameServer
	virtual void ReadyMoveGameServer(int nMoveTargetGameServerID, GGateInfo& refGateInfo);
	virtual void GetSetMoveGameServer(TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo);
	virtual void CancelMoveGameServer(void);
	void GoMoveGameServer(void);

	// Move to LoginServer
	void ReadyMoveLoginServer();
	void SetMoveLoginServerConnectionKey(MUID uidConnectionKey);
	void CancelMoveLoginServer(int nResult);
	void GoMoveLoginServer(int nResult);

	void SetMoveGameServerAsyncTaskUID(MUID uidTask)	{ m_uidMoveGameServerTask = uidTask; }
	void SetMoveLoginServerAsyncTaskUID(MUID uidTask)	{ m_uidMoveLoginServerTask = uidTask; }
	MUID GetMoveGameServerAsyncTaskUID()	{ return m_uidMoveGameServerTask; }
	MUID GetMoveLoginServerAsyncTaskUID()	{ return m_uidMoveLoginServerTask; }

	bool IsGetSetMoveGameServer(void)	{ return m_nState == POS_MOVE_GAME_SERVER;	}
	bool IsMoveGameServer(void)			{ return IsGetSetMoveGameServer();		}
	bool IsDoingLogin(void)				{ return m_nState == POS_DOING_LOGIN; }
	bool IsInWorld(void)				{ return m_nState == POS_INWORLD;	}
	bool IsReadyMoveLoginServer()		{ return m_nState == POS_READY_MOVE_LOGIN_SERVER; }
	bool IsMoveLoginServer(void)		{ return m_nState == POS_MOVE_LOGIN_SERVER; }
	bool IsLogout(void)					{ return m_nState == POS_LOGOUT; }
	bool IsMovingServer(void);
	
	// Gate
	virtual void Gate(RESPONSE_GATE_TYPE eResponseGateType, const GGateInfo* pGateInfo);
			
	const MUID	GetUID(void) const	{ return m_UID; }
	const UIID	GetUIID(void) const	{ return m_nUIID; }

	void RegUIID(UIID nUIID);

	ACCOUNT_INFO&			GetAccountInfo()			{ return m_AccountInfo; }	
	PlayerObjectState		GetState()				{ return m_nState; }
	virtual GEntityPlayer*	GetEntity()			{ return m_pMyEntity; }

	GPlayerGameGuard*		GetGameGuard();

	void					SetForceDisconnect()	{ m_bForceDisconnect = true; }	///< Disconnect 처리 디버깅용 메소드 - aibeast
};

inline bool IsNotEnabledPlayerEntity(GPlayerObject* pPlayerObject)
{
	if ((pPlayerObject == NULL) || (pPlayerObject->GetEntity() == NULL)) return true;
	return false;
}

class GPlayerObject_Leaf : public GPlayerObject, public MMemPool<GPlayerObject_Leaf>
{
public:
	GPlayerObject_Leaf() : GPlayerObject() {}
	GPlayerObject_Leaf(const MUID& uid) : GPlayerObject(uid) {}
	virtual ~GPlayerObject_Leaf() {}

};

#endif