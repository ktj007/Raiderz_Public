#pragma once

struct TD_BUFF_REMAIN_TIME;
struct TD_TALENT_REMAIN_COOLTIME;

class GClientCharRouter
{
public:
	GClientCharRouter(void);
	~GClientCharRouter(void);

	void CharMyTimeInfo(MUID uidPlayer
						, vector<TD_BUFF_REMAIN_TIME>& rvecTDBuffRemainTime
						, vector<TD_TALENT_REMAIN_COOLTIME>& rvecTDTalentRemainCoolTime
						, float fAutoRebirthRemainTime);
};
