#ifndef _LDBTASK_DATA_CHAR_H
#define _LDBTASK_DATA_CHAR_H


class LDBT_CHAR_INSERT
{
public :
	LDBT_CHAR_INSERT() : m_nWorldID(0), m_nAID(0), m_nRace(0), m_nSex(0), m_nLevel(0)
	, m_nXP(0), m_nMoney(0), m_nTotalTP(0) {}
	LDBT_CHAR_INSERT(const MUID& uidPlayer, const int nWorldID, const int64 nAID, const wstring& strName
		, const uint8 nRace, const uint8 nSex, const uint8 nLevel, const int nXP, const int nMoney, const int nTotalTP)
		: m_uidPlayer(uidPlayer), m_nWorldID(nWorldID), m_nAID(nAID), m_strName(strName), m_nRace(nRace), m_nSex(nSex), m_nLevel(nLevel)
		, m_nXP(nXP), m_nMoney(nMoney), m_nTotalTP(nTotalTP) {}

	MUID	m_uidPlayer;
	int		m_nWorldID;
	int64	m_nAID;
	wstring	m_strName;
	uint8	m_nRace;
	uint8	m_nSex;
	uint8	m_nLevel;
	int		m_nXP;
	int		m_nMoney;
	int		m_nTotalTP;
};


#endif 