#ifndef _GNPC_KILLER_DATA_H
#define _GNPC_KILLER_DATA_H


class GNPC_KILLER
{
public :
	GNPC_KILLER() { Reset(); }
	GNPC_KILLER(AID nAID, CID nCID, int nCharPtm, int nDeltaCharPtm, uint8 nLevel)
		: m_nAID(nAID), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel)
	{

	}

	void Reset()
	{
		m_nAID			= 0;
		m_nCID			= 0;
		m_nCharPtm		= 0;
		m_nDeltaCharPtm = 0;
		m_nLevel		= 0;
	}

	enum MAX_ONCE_CHAR_COUNT
	{
		MAX_COUNT = 5,
	};

	AID		m_nAID;
	CID		m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
};


#endif 