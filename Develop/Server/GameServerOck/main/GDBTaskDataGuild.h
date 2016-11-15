#ifndef _GDBTASK_DATA_GUILD_H
#define _GDBTASK_DATA_GUILD_H


class GDBT_GUILD
{
public :
	GDBT_GUILD() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nCODE(0), m_nGID(0) {}
	GDBT_GUILD(const int64 nAID, const MUID& uidPlayer, const int64 nCID, int nCharPtm, int nDeltaCharPtm, const int nMoney, const int nDeltaMoney, uint8 nLevel, const int nCODE, const int64 nGID, const wstring strName = L"") :
	m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nMoney(nMoney), m_nDeltaMoney(nDeltaMoney), m_nLevel(nLevel), m_nCODE(nCODE), m_nGID(nGID), m_strName(strName) {}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nCharPtm;			// 길드 생성때만 사용됨. 그 외에는 0으로 넘겨주면 됩니다.
	int		m_nDeltaCharPtm;	// 길드 생성때만 사용됨. 그 외에는 0으로 넘겨주면 됩니다.
	int		m_nMoney;			// 길드 생성때만 사용됨. 그 외에는 0으로 넘겨주면 됩니다.
	int		m_nDeltaMoney;		// 길드 생성때만 사용됨. 그 외에는 0으로 넘겨주면 됩니다.
	uint8	m_nLevel;			// 길드 생성때만 사용됨. 그 외에는 0으로 넘겨주면 됩니다.
	int		m_nCODE;
	int64	m_nGID;	
	wstring	m_strName;
};

class GDBT_GUILD_CHANGE_MASTER
{
public :
	GDBT_GUILD_CHANGE_MASTER() : m_uidPreMaster(0), m_nPreMasterCID(0) {}
	GDBT_GUILD_CHANGE_MASTER(const MUID uidPreMaster, const int64 nCID) : m_uidPreMaster(uidPreMaster), m_nPreMasterCID(nCID) {}

	void SetNewMaster(const int64 nAID, const int64 nCID, const int nCODE, const int64 nGID)
	{
		m_NewMaster.m_nAID		= nAID;
		m_NewMaster.m_nCID		= nCID;
		m_NewMaster.m_nCODE		= nCODE;
		m_NewMaster.m_nGID		= nGID;
	}

	MUID		m_uidPreMaster;
	int64		m_nPreMasterCID;
	GDBT_GUILD	m_NewMaster;
};

class GDBT_GUILD_UPDATE_STORAGEMONEY
{
public:
	enum TYPE
	{
		DEPOSIT,
		WITHDRAW,
	};

public:
	GDBT_GUILD_UPDATE_STORAGEMONEY() : m_nType(DEPOSIT), m_nAID(0), m_uidPlayer(MUID::ZERO), m_nCID(0), m_nMoney(0), m_nGID(0), m_nStorageMoney(0), m_nModMoney(0) {}
	GDBT_GUILD_UPDATE_STORAGEMONEY(TYPE nType, int64 nAID, MUID uidPlayer, int64 nCID, int nMoney, int64 nGID, int nStorageMoney, int nModMoney) : m_nType(nType), m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nMoney(nMoney), m_nGID(nGID), m_nStorageMoney(nStorageMoney), m_nModMoney(nModMoney) {}

	TYPE	m_nType;
	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;	
	int		m_nMoney;
	int64	m_nGID;
	int		m_nStorageMoney;
	int		m_nModMoney;
};


#endif