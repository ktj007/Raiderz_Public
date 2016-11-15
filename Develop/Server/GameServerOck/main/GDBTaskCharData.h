#ifndef _GDBTASK_CHAR_DATA_H
#define _GDBTASK_CHAR_DATA_H


#include "STypes.h"


class GDBT_CHAR_XP_DATA
{
public :
	GDBT_CHAR_XP_DATA() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nCode(0), m_strRegDate(L"NULL"), m_nXP(0), m_nModXP(0), m_nLevel(0), m_nMoney(0), m_nCharPtm(0), m_nNpcID(0) {}
	GDBT_CHAR_XP_DATA(const int64 nAID, const MUID& uidPlayer, const int64 nCID, int nCode, const wstring& strRegDate, const int nXP, const int nModXP, const uint8 nLevel, const int nMoney, const int nCharPtm, int nDeltaCharPtm, int nNpcID)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCode(nCode), m_strRegDate(strRegDate), m_nXP(nXP), m_nModXP(nModXP), m_nLevel(nLevel), m_nMoney(nMoney), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nNpcID(nNpcID) {}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nCode;
	wstring	m_strRegDate;
	int		m_nXP;
	int		m_nModXP;
	uint8	m_nLevel;
	int		m_nMoney;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	int		m_nNpcID;
};

class GDBT_CHAR_LEVEL_UP_DATA : public GDBT_CHAR_XP_DATA
{
public :
	GDBT_CHAR_LEVEL_UP_DATA() : m_nRemainTP(0) {}
	GDBT_CHAR_LEVEL_UP_DATA(const int64 nAID, const MUID& uidPlayer, const int64 nCID, int nCode, const wstring& strRegDate, const int nXP, const int nModXP, const uint8 nLevel, const int nMoney
		, const int16 nRemainTP, const int nCharPtm, int nDeltaCharPtm, int nNpcID, int nFieldID, const vec3& vPos)
		: GDBT_CHAR_XP_DATA(nAID, uidPlayer, nCID, nCode, strRegDate, nXP, nModXP, nLevel, nMoney, nCharPtm, nDeltaCharPtm, nNpcID), m_nRemainTP(nRemainTP), m_nFieldID(nFieldID), m_vPos(vPos) {}

	int16	m_nRemainTP;	
	int		m_nFieldID;
	vec3	m_vPos;
};


class GDBT_CHAR_ENTER_FILED
{
public :
	GDBT_CHAR_ENTER_FILED(const int64 nAID, const int64 nCID, const int nXP, const uint8 nLevel, const int nMoney, const int nCharPtm
		, const int nSharedFieldID, const float fPosX, const float fPosY, const float fPosZ)
		: m_nAID(nAID), m_nCID(nCID), m_nXP(nXP), m_nLevel(nLevel), m_nMoney(nMoney), m_nCharPtm(nCharPtm)
	, m_nSharedFieldID(nSharedFieldID), m_fPosX(fPosX), m_fPosY(fPosY), m_fPosZ(fPosZ) {}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nXP;
	uint8	m_nLevel;
	int		m_nMoney;
	int		m_nCharPtm;
	int		m_nSharedFieldID;
	float	m_fPosX;
	float	m_fPosY;
	float	m_fPosZ;
};

class GDBT_CHAR_DELETE
{
public :
	GDBT_CHAR_DELETE() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nIndex(-1) {}
	GDBT_CHAR_DELETE(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nIndex)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nIndex(nIndex)
	{

	}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nIndex;
};

class GDBT_CHAR_SERIALIZE
{
public :
	GDBT_CHAR_SERIALIZE() : m_nAID(0), m_uidPlayer(0), m_nUIID(0), m_nCID(0), m_bReload(false) {}
	GDBT_CHAR_SERIALIZE(const int64 nAID, const MUID& uidPlayer, const UIID nUIID, const int64 nCID, bool bReload)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nUIID(nUIID), m_nCID(nCID), m_bReload(bReload) {}

	int64	m_nAID;
	MUID	m_uidPlayer;
	UIID	m_nUIID;
	int64	m_nCID;
	bool	m_bReload;
};

class GDBT_CHAR_SAVE_SOUL_BINDING
{
public :
	GDBT_CHAR_SAVE_SOUL_BINDING() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nSoulBindingID(0), m_nCharPtm(0) {}
	GDBT_CHAR_SAVE_SOUL_BINDING(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nSoulBindingID, const int nCharPtm)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nSoulBindingID(nSoulBindingID), m_nCharPtm(nCharPtm) {}
	
	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nSoulBindingID;
	int		m_nCharPtm;
};

class GDBT_CHAR_REGIST_INN_ROOM
{
public :
	GDBT_CHAR_REGIST_INN_ROOM() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nRoomID(0), m_nCharPtm(0) {}
	GDBT_CHAR_REGIST_INN_ROOM(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nRoomID, const int nCharPtm)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nRoomID(nRoomID), m_nCharPtm(nCharPtm) {}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nRoomID;
	int		m_nCharPtm;
};

class GDBT_CHAR_MONEY_INC_DEC
{
public :
	GDBT_CHAR_MONEY_INC_DEC() : m_uidPlayer(0), m_nAID(0), m_nCID(0), m_nCode(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nLevel(0), m_nMoney(0), m_nDeltaMoney(0), m_nXP(0), m_bRouteParty(0), m_nNpcID(0) {}
	GDBT_CHAR_MONEY_INC_DEC(const MUID& uidPlayer, int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, int nLevel, int nXP, int nMoney, int nDeltaMoney, bool bRouteParty, int nNpcID)
		: m_uidPlayer(uidPlayer), m_nAID(nAID), m_nCID(nCID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nXP(nXP), m_nMoney(nMoney), m_nDeltaMoney(nDeltaMoney)
		, m_bRouteParty(bRouteParty), m_nNpcID(nNpcID) {}

	MUID	m_uidPlayer;
	int64	m_nAID;
	int64	m_nCID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	int		m_nLevel;
	int		m_nMoney;
	int		m_nDeltaMoney;
	int		m_nXP;
	bool	m_bRouteParty;
	int		m_nNpcID;
};

class GDBT_CHAR_KILL
{
public :
	GDBT_CHAR_KILL() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nCode(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nLevel(0), m_nXP(0), m_nFieldID(0) {}

	GDBT_CHAR_KILL(int64 nAID, const MUID& uidPlayer, int nCID, int nCode, int nCharPtm, int nDeltaCharPtm, int nLevel, int nXP, int nFieldID, const vec3& vPos) 
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel)
		, m_nXP(nXP), m_nFieldID(nFieldID), m_vPos(vPos)
	{
	}
	
	int64	m_nAID;
	MUID	m_uidPlayer; 
	int64	m_nCID; 
	int		m_nCode; 
	int		m_nCharPtm; 
	int		m_nDeltaCharPtm;
	int		m_nLevel; 
	int		m_nXP; 
	int		m_nFieldID;
	vec3	m_vPos;
};



#endif