#ifndef _GDBTASK_DATA_PARTY_H
#define _GDBTASK_DATA_PARTY_H


class GDBT_PARTY
{
public :
	GDBT_PARTY(const MUID& uidParty, AID nAID, CID nCID, uint8 nType)
		: m_uidParty(uidParty), m_nAID(nAID), m_nCID(nCID), m_nType(nType)
	{

	}

	// 나중에 적당한 곳으로 옮기자.
	enum PARTY_TYPE
	{
		NORMAL = 1,	  
	};

	MUID	m_uidParty;
	AID		m_nAID;
	CID		m_nCID;
	uint8	m_nType;
};


#endif