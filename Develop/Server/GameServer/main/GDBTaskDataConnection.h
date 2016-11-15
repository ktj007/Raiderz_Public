#ifndef _GDBTASK_DATA_CONNECTION_H
#define _GDBTASK_DATA_CONNECTION_H


class GDBT_CHAR_SELECT_LOG
{
public :
	GDBT_CHAR_SELECT_LOG(int64 nAID, int64 nCID, int nCharPtm, int nMoney, int nFieldID)
		: m_nAID(nAID), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nMoney(nMoney), m_nFieldID(nFieldID)
	{

	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nMoney;
	int		m_nFieldID;
};

class GDBT_DISCONN_LOG
{
public :
	GDBT_DISCONN_LOG(int64 nAID, int64 nCID, int nCharPtm, int nDeltaCharPtm, int nMoney, int nSumDeltaMoney, int nFieldID)
		: m_nAID(nAID), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nMoney(nMoney), m_nSumDeltaMoney(nSumDeltaMoney), m_nFieldID(nFieldID)
	{

	}

	bool operator == (const GDBT_DISCONN_LOG& in_data)
	{
		if (m_nAID != in_data.m_nAID) return false;
		if (m_nCID != in_data.m_nCID) return false;
		if (m_nCharPtm != in_data.m_nCharPtm) return false;
		if (m_nDeltaCharPtm != in_data.m_nDeltaCharPtm) return false;
		if (m_nMoney != in_data.m_nMoney) return false;
		if (m_nSumDeltaMoney != in_data.m_nSumDeltaMoney) return false;
		if (m_nFieldID != in_data.m_nFieldID) return false;

		return true;
	}

	bool operator == (const GDBT_DISCONN_LOG& in_data) const
	{
		if (m_nAID != in_data.m_nAID) return false;
		if (m_nCID != in_data.m_nCID) return false;
		if (m_nCharPtm != in_data.m_nCharPtm) return false;
		if (m_nDeltaCharPtm != in_data.m_nDeltaCharPtm) return false;
		if (m_nMoney != in_data.m_nMoney) return false;
		if (m_nSumDeltaMoney != in_data.m_nSumDeltaMoney) return false;
		if (m_nFieldID != in_data.m_nFieldID) return false;

		return true;
	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	int		m_nMoney;
	int		m_nSumDeltaMoney;
	int		m_nFieldID;
};


#endif