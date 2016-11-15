#pragma once

struct TD_TARGET_INFO;

class GPlayerTargetInfoSender : public MTestMemPool<GPlayerTargetInfoSender>
{
public:
	GPlayerTargetInfoSender(GEntityPlayer* pOwner);
	~GPlayerTargetInfoSender();

	void Update(float fDelta);
	void Send(const vector<UIID>& vecUIID);
	void ClearSendTime();

private:
	vector<TD_TARGET_INFO> MakeTDTargetInfo(const vector<UIID>& vecUIID);

	bool IsJustNowSend(const UIID& nUIID);
	void RecordSendTime(vector<TD_TARGET_INFO>& vecTDTargetInfo);
	void Route(const vector<TD_TARGET_INFO>& vecTDTargetInfo);

	void ClearInvalidSendTimeRecord();

private:
	GEntityPlayer*		m_pOwner;

	map<UIID, uint32>	m_mapSendTime;		// 정보보낸 시간

	MRegulator			m_rgrTick;
};
