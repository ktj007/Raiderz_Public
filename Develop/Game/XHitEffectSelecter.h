#pragma once

class XTalentHitParam;

#define HIT_INFO_AUTO_DELETE_TIME 1.5f

class XHitEffectSelecter
{
protected:
	enum HIT_EFFECT_SELECT_TYPE
	{
		HEST_DISTANCE_SORT,
		HEST_ORDER
	};

	struct stHitInfo
	{
		UIID					uiidAttacker;
		int						nTalentID;
		HIT_EFFECT_SELECT_TYPE	eType;

		float					fRemainedTime;

		vector<XTalentHitParam*>	vecHitParam;

		stHitInfo()
		{
			nTalentID			= -1;
			fRemainedTime		= HIT_INFO_AUTO_DELETE_TIME;
			eType				= HEST_DISTANCE_SORT;
		}
	};

	vector<stHitInfo>			m_vecHitbyTalentInfo;

private:
	stHitInfo*					CreateHitInfo(UIID uiidAttacker, int nTalentID, bool& bNew); 
	void						DelHitInfobyAttacker(UIID uiidAttacker, int nTalentID);

	bool						CheckForceFirtstHitEffectByGrade(UIID uiidVictim);

	stHitInfo*					GetHitInfo(UIID uiidAttacker, int nTalentID);

public:
	XHitEffectSelecter();
	virtual ~XHitEffectSelecter() {}

	void						AddHitInfo(XTalentHitParam* pHitParam, unsigned int nDelayTime);
	void						Update(float fDelta);

	void						StartHitEffectSelect(XTalentHitParam* pHitParam);
};