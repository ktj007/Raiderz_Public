#pragma once

#include "MUID.h"
#include "MReferenceCounted.h"

#include "PmDef.h"

/// 플레이어 상태
enum PLAYER_STATE
{
	PS_SELECT_CHAR		= 0,	/// 캐릭터 고르는 중
	PS_READY_LOGIN_GAMESERVER,	/// 게임서버로 첫 진입 중 (LS->GS)
	PS_IN_WORLD,				/// 게임 중
	PS_READY_MOVE_GAMESERVER,	/// 게임서버로 이동 준비 중 (GS->GS)
	PS_MOVE_GAMESERVER,			/// 게임서버로 이동 (LS->GS, GS->GS)
	PS_READY_MOVE_LOGINSERVER,	/// 로그인서버로 이동 중비 중 (GS->LS)
	PS_MOVE_LOGINSERVER,		/// 로그인서버로 이동 (GS->LS)
	PS_KICK_BY_DUPLOGIN,		/// 중복접속으로 퇴장처리 중
	PS_END,
};

class ZPlayer : public MReferenceCounted
{
protected:
	AID				m_AID;
	CID				m_CID;
	MUID			m_UID;
	wstring			m_strUserID;
	int				m_nGameServerID;
	MUID			m_uidField;
	MUID			m_uidParty;
	int				m_nGuildID;
	wstring			m_strPlayerName;
	int				m_nFieldID;
	int				m_nChannelID;
	int				m_nReservedGameServerID;
	MUID			m_uidReservedField;
	MUID			m_uidReservedConnectionKey;

	PLAYER_STATE	m_eState;

	// 피망 정보
	PmUserData	m_pmangUserData;

public:
	ZPlayer(MUID uidPlayer, AID nAID, const wstring& strUserID);
	virtual ~ZPlayer() {}

	void			SetWorldInfo(CID nCID, int nGameServerID, MUID uidPlayer, const std::wstring& strPlayerName, int nGID);
	void			ClearWorldInfo();

	void			AddPmangUserData(PmUserData* pPmangUserData);
	PmUserData*		GetUserPmangData();
	bool			IsExistPmangUserData() const;

	void			SetState(PLAYER_STATE eState)	{ m_eState = eState; }
	void			SetUID(MUID uidPlayer);
	void			SetFieldID(int nFieldID);
	void			SetChannelID(int nChannelID);
	void			SetFieldUID(MUID uidField);
	void			SetPartyUID(MUID uidParty);
	void			SetGuildID(int nGuildID);
	void			SetReservedGameServerID(int nReservedID) { m_nReservedGameServerID = nReservedID; }
	void			SetReservedFieldUID(MUID uidReservedField) { m_uidReservedField = uidReservedField; }
	void			ClearReservedFieldID() { m_uidReservedField.SetZero(); }
	void			SetReservedConnectionKey(MUID uidConnectionKey)	{ m_uidReservedConnectionKey = uidConnectionKey;	}
	void			SetGameServerID(int nID)	{ m_nGameServerID = nID; }

	PLAYER_STATE	GetState() const			{ return m_eState; }
	AID				GetAID() const				{ return m_AID; }
	wstring&		GetUserID()					{ return m_strUserID; }
	CID				GetCID() const				{ return m_CID; }
	MUID			GetUID() const				{ return m_UID;	}
	MUID			GetFieldUID() const			{ return m_uidField; }
	MUID			GetPartyUID() const			{ return m_uidParty; }
	int				GetGuildID() const			{ return m_nGuildID; }
	int				GetFieldID() const			{ return m_nFieldID; }
	int				GetChannelID() const		{ return m_nChannelID; }
	int				GetReservedGameServerID() const	{ return m_nReservedGameServerID; }
	MUID			GetReservedFieldUID() const	{ return m_uidReservedField; }
	MUID			GetReservedConnectionKey() const { return m_uidReservedConnectionKey;	}

	MUID			GetFieldGroupUID() const;
	
	int				GetGameServerID() const		{ return m_nGameServerID; }
	MUID			GetGameServerUID();
	wstring&		GetPlayerName()				{ return m_strPlayerName; }
	
	bool			IsInWorld()					{ return PS_IN_WORLD == GetState(); }
	bool			IsReadyMoveGameServer()		{ return PS_READY_MOVE_GAMESERVER == GetState(); }
	bool			IsMoveGameServer()			{ return PS_MOVE_GAMESERVER == GetState(); }

};

typedef tr1::unordered_map<AID, ZPlayer*>			ZPlayerAIDMap;
typedef tr1::unordered_map<CID, ZPlayer*>			ZPlayerCIDMap;
typedef tr1::unordered_map<uint64, ZPlayer*>		ZPlayerUIDMap;
