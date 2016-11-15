#pragma once

#include "XGameTransformControllerManager.h"

struct TestAniDummyLoc
{
	vec3		vPos;
	vec3		vDir;
};

class TestAniData
{
private:
	vector<TestAniDummyLoc>		vecLoc;		// 0.1초마다 샘플링해서 들어감
public:
	wstring					strName;
	double					dMaxFrameTime;
	AnimationLoopType		nLoopType;
	double					dSpeed;

	void AddDummyLocPer100ms(vec3 vPos, vec3 vDir)
	{
		TestAniDummyLoc data;
		data.vPos = vPos;
		data.vDir = vDir;

		vecLoc.push_back(data);
	}
	TestAniDummyLoc GetDummyLocData(float fTime)
	{
		TestAniDummyLoc failedData;
		failedData.vPos = vec3::ZERO;
		failedData.vDir = vec3(0.0f, -1.0f, 0.0f);

		int index = (int)(fTime * 10);
		if (index < 0 || index >= (int)vecLoc.size()) return failedData;

		return vecLoc[index];
	}
	bool IsExistDummyLoc() { return (!vecLoc.empty()); }
};

class TestAniDataSet
{
private:
	map<wstring, TestAniData*>		m_Data;
public:
	TestAniDataSet();
	~TestAniDataSet();
	void Clear();
	void AddAni(const wchar_t* szName, double fTime, bool bLoop, double dSpeed = 1);
	TestAniData* GetAniData(const wchar_t* szAniName);
};


class MockAnimationController : public XGameTransformControllerManager
{
private:
	TestAniData*		m_pCurrAni;
	TestAniDataSet*		m_pAniDataSet;
	double				m_dSpeed;

	double				m_dCurrFrameTime;
	bool				m_bPlayed;
	bool				m_bOncePlayDone;

	wstring				m_strLastAni;

	void UpdateDummyLoc( float fDelta );
public:
	MockAnimationController(XCharacter* pCharacter, MPxAniLookUpTC* pAniLookUpTransformController);
	virtual ~MockAnimationController();
	void SetAniDataSet(TestAniDataSet* pAniDataSet);

	virtual void Update(float fDelta) override;

	virtual bool SetAnimation(wstring& strAniName, bool bForce=false, bool bSameFrame = false) override;

	virtual float			GetAnimationSpeed() override;
	virtual void			SetAnimationSpeed(float fSpeed) override;
	virtual void			SetAnimationFrame(unsigned int nFrame) override;
	virtual float			GetMaxFrameToSecond() override;
	virtual float			GetCurFrameToSecond() override;

	virtual void			PauseAnimation() override;
	virtual void			StopAnimation() override;
	virtual void			PlayAnimation() override;

	virtual wstring			GetAnimationName() override;
	virtual bool			IsAnimationLoop() override;
	virtual int				GetAniFrame() override;
	virtual int				GetAniMaxFrame() override;

	virtual bool			IsPlayDone() override;
	virtual bool			IsOncePlayDone() override;
};
