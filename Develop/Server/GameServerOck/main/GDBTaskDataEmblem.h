#ifndef _GDBTASK_DATA_EMBLEM_H
#define _GDBTASK_DATA_EMBLEM_H


class GDBT_EMBLEM
{
public :
	GDBT_EMBLEM() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nEmblemID(0) {}
	GDBT_EMBLEM(const int64 nAID, const MUID uidPlayer, const int64 nCID, const int nEmblemID)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nEmblemID(nEmblemID) {}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nEmblemID;
};


#endif