#ifndef _GDBTASK_DATA_PARTY_H
#define _GDBTASK_DATA_PARTY_H


class GDBT_PARTY
{
public :
	GDBT_PARTY(const MUID& uidParty, int64 nAID, int64 nCID, uint8 nType)
		: m_uidParty(uidParty), m_nAID(nAID), m_nCID(nCID), m_nType(nType)
	{

	}

	// 나중에 적당한 곳으로 옮기자.
	enum PARTY_TYPE
	{
		NORMAL = 1,	  
	};

	MUID m_uidParty;
	int64 m_nAID;
	int64 m_nCID;
	uint8 m_nType;
};


#endif