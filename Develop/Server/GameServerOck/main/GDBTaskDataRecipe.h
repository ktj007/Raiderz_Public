#ifndef _GDBTASK_DATA_RECIPE_H
#define _GDBTASK_DATA_RECIPE_H


class GDBT_RECIPE
{
public :
	GDBT_RECIPE() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nRecpID(0) {}
	GDBT_RECIPE(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nRecpID)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nRecpID(nRecpID) {}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nRecpID;
};


#endif