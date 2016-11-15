#pragma once
#include "XSwordTrailSamplingData.h"
#include "TSwordTrailEffectController.h"

class TSwordTrailEffect
{
public:
	vector<vec3>			m_vecStartSampling;
	vector<vec3>			m_vecEndSampling;

public:
	TSwordTrailEffect();
	~TSwordTrailEffect();
};

class TSwordTrailSampling : public XSwordTrailSamplingData
{
private:
	SWORDTRAILSAMPLING_DATA* m_pCurrentSwordTrail;
	TSwordTrailEffectController m_SwordTrailEffectController;

private:
	void					SwordTrailSampling(RAnimation* pAnimation, SWORDTRAILSAMPLING_DATA& sampling_data, string strModelName, string strParentsBoneName);
	string					GetWeaponParentDummyName(RAnimation* pAnimation);
	void					GetSamplingData(const char* pNodeName, int nStartFrame, int nEndFrame, int nSampleInterval, std::vector< RMatrix >& _rOUt, const RMatrix* pBaseCoordIfUse);

	void					SaveSwordTrailData(MXmlElement * pParentElement, SWORDTRAIL_DATA & swordTrail_data);
	void					SaveSwordTrailSamplingData(MXmlElement * pParentElement, SWORDTRAILSAMPLING_DATA & swordTrailSampling_data);

	bool					CalSwordTrailFrame(int nStartFrame, int nEndFrame, int nFrameGap, int nAnimationEndFrame, int& nCalEndFrame);

	void					CheckSwordTrailEvent(RAnimation* pAnimation, vector<SWORDTRAILEVENT_DATA>& vecData);

public:
	TSwordTrailSampling();
	~TSwordTrailSampling();

	void					DestroySwordTrailSampling();

	void					StartSwordTrailSampling(string strModelName, RAnimation* pAnimation);
	void					DeleteSwordTrailSampling(string strModelName, RAnimation* pAnimation);

	bool					Save();

	void					UpdateSwordTrailSampling(float fDelta);
	void					RenderSwordTrailSampling();
};