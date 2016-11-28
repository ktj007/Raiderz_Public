#ifndef _GDBTASK_DATA_RECIPE_H
#define _GDBTASK_DATA_RECIPE_H


class GDBT_RECIPE
{
public :
	GDBT_RECIPE() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nRecpID(0) {}
	GDBT_RECIPE(const AID nAID, const MUID& uidPlayer, const CID nCID, const int nRecpID)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nRecpID(nRecpID) {}

	AID		m_nAID;
	MUID	m_uidPlayer;
	CID		m_nCID;
	int		m_nRecpID;
};


#endif