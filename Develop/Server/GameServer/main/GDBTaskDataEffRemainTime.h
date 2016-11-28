#ifndef _GDBTASK_DATA_EFF_REMAIN_TIME_H
#define _GDBTASK_DATA_EFF_REMAIN_TIME_H


class GDBT_EFF_REMAIN_TIME_INSERT
{
public :
	GDBT_EFF_REMAIN_TIME_INSERT(const AID nAID, const MUID& uidPlayer, const CID nCID, const int nCharPtm
		, vector<REMAIN_BUFF_TIME>& vecBuffRemainTime, vector<pair<int, float>>& vecTalentCoolTime)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCharPtm(nCharPtm)
	{
		m_vecBuffRemainTime.swap(vecBuffRemainTime);
		m_vecTalentCoolTime.swap(vecTalentCoolTime);
	}

	AID							m_nAID;
	MUID						m_uidPlayer;
	CID							m_nCID;
	int							m_nCharPtm;
	vector<REMAIN_BUFF_TIME>	m_vecBuffRemainTime;
	vector<pair<int, float>>	m_vecTalentCoolTime;
};


#endif