#ifndef _GDBTASK_DATA_EMBLEM_H
#define _GDBTASK_DATA_EMBLEM_H


class GDBT_EMBLEM
{
public :
	GDBT_EMBLEM() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nEmblemID(0) {}
	GDBT_EMBLEM(const AID nAID, const MUID uidPlayer, const CID nCID, const int nEmblemID)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nEmblemID(nEmblemID) {}

	AID		m_nAID;
	MUID	m_uidPlayer;
	CID		m_nCID;
	int		m_nEmblemID;
};


#endif