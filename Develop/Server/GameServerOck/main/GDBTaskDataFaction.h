#ifndef _GDBTASK_DATA_FACTION_H
#define _GDBTASK_DATA_FACTION_H


class GDBT_DATA_FACTION // 자료구조 이름이 겹처서 _DATA추가.
{
public :
	GDBT_DATA_FACTION() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nFactID(0), m_nVal(0) {}
	GDBT_DATA_FACTION(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nFactID, const int nVal)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nFactID(nFactID), m_nVal(nVal) {}
	
	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nFactID;
	int		m_nVal;
};


#endif