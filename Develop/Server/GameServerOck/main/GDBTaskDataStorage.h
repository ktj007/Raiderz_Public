#pragma once

#include "GDBTaskItem.h"

class GDBT_STORAGE_SERIALIZE
{
public:
	GDBT_STORAGE_SERIALIZE()
	: m_uidPlayer(MUID::ZERO), m_nCID(0) {}
	GDBT_STORAGE_SERIALIZE(MUID uidPlayer, int64 nCID)
	: m_uidPlayer(uidPlayer), m_nCID(nCID) {}
	
	MUID	m_uidPlayer;
	int64	m_nCID;
	GDBT_ITEMINSTANCEQ m_itemInstanceQ;		
};


class GDBT_STORAGE_UPDATE_MONEY
{
public:
	enum TYPE
	{
		DEPOSIT,
		WITHDRAW,
	};

public:
	GDBT_STORAGE_UPDATE_MONEY()
	: m_nType(DEPOSIT), m_uidPlayer(MUID::ZERO), m_nAID(0), m_nCID(0), m_nMoney(0), m_nStorageMoney(0) {}
	GDBT_STORAGE_UPDATE_MONEY(TYPE nType, MUID uidPlayer, int64 nAID, int64 nCID, int nMoney, int nStorageMoney)
	: m_nType(nType), m_uidPlayer(uidPlayer), m_nAID(nAID), m_nCID(nCID), m_nMoney(nMoney), m_nStorageMoney(nStorageMoney) {}

	TYPE	m_nType;
	MUID	m_uidPlayer;
	int64	m_nAID;	
	int64	m_nCID;
	int		m_nMoney;
	int		m_nStorageMoney;
};