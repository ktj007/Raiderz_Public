#pragma once

#include "XMotion.h"
#include "TCharacter.h"

#define TOOL_PLAY_MOTION "PlayMotion"

/// 클라이언트 XModuleMotion()와 같은 역할 ??
struct PlayMotionList
{
	string			strMotion;
	string			strAni;

	bool			bCustom;
	XMotionTypes	nMotionType;

	int				nAniSequenceCount;
};

class TPlayMotion
{
	struct SCustomAniName
	{
		string	m_strMotion;
		string	m_strAni;

		SCustomAniName(const string& motion, const string& ani) : m_strMotion(motion), m_strAni(ani) {}
	};

	struct stEndTime
	{
		bool bUse;
		float fEndTime;

		stEndTime() : bUse(false), fEndTime(0.0f) {}
	};

protected:
	XMotionMgr*				m_pMotionMgr;
	XMotion*				m_pCurrMotion;
	XMotionTypes			m_nCurrType;

	TCharacter*				m_pActor;

	bool					m_bChanged;
	bool					m_bPlayDone;
	bool					m_bForceNoWeaponAni;
	bool					m_bPlayAnimationList;
	int						m_nWeaponType;
	int						m_nCurrAniSequence;

	float					m_fPlayAnimationListSpeed;

	string					m_strCustomAnimationName;
	string					m_strLastEvent;

	vector<SCustomAniName>	m_vCustomMotionAniName;
	vector<XMotionAniSequence*> m_vCustomAnimationSequence;

	vector<stEndTime>		m_vecCustomAnimationListEndTime;

	bool					m_bIgnoreLoop;

	string					GetCustomMotionAniName(const string& szMotionName);
	string					GetAniNameWithMode(string strAniName);

	bool					IsAnimationLoop(TCharacter* pActor);

	string					MakeFullPlayerTalentAniName(string& szAniName, WEAPON_TYPE nCurrWeaponType);

	void					ClearAnimationSequence();
	void					SetAnimationSequence(const char* szMotionName);
	void					SetCustomAnimaitonSequence(const char* szAnionName);

	bool					CheckWeaponAni();
	bool					CheckNextSequenceAnimation(int nNextSequence);

public:
	TPlayMotion(void);
	virtual ~TPlayMotion(void);

	bool					Init();
	void					InitMotion();
	void					CurMotionClear();

	void					OnUpdate(float fDelta);
	void					OnEvent(const char* szEvent);

	bool					ChangeMotion(const char* szMotionName, XMotionTypes nType=MT_DEFAULT, bool bIgnoreLoop = false);
	bool					ChangeMotion(const char* szMotionName, const char* szNextMotionName, XMotionTypes nType=MT_DEFAULT, bool bIgnoreLoop = false);
	bool					ChangeCustomMotion(const char* szMotionName, const char* szAnimationName, XMotionTypes nType = MT_DEFAULT, bool bIgnoreLoop = false, bool bNormalStance = false);
	bool					_ChangeMotion(const char* szMotionName, XMotionTypes nType, bool bIgnoreLoop);

	void					PushMotion(const char* szMotionName, bool bIgnoreLoop = false);
	void					PushCustomMotion(const char* szMotionName, const char* szAnimationName, bool bIgnoreLoop = false, bool bNormalStance = false);

	bool					SetAnimation(bool bForce = false, bool bTestPlay = false);
	void					SetSequence(int nSequence, bool bForce = false);
	void					ResetAniTime();

	void					GetCurMotionAnimationList(vector<string>& vecList);

	int						GetWeaponType() { return m_nWeaponType; }
	void					SetWeaponType(WEAPON_TYPE weapontype);

	XMotionMgr*				GetMotionMgr() { return m_pMotionMgr; }
	XMotion*				GetMotion()	{ return m_pCurrMotion;}
	void					SetMotion(XMotion* pMotion)	{ m_pCurrMotion = pMotion;}
	XMotionTypes			GetCurMotionType() { return m_nCurrType; }

	int						GetCurTalentFrame(int AniSequenceindex);
	int						GetCurTalentFrame();
	int						GetCurTalentFrameMax();
	int						GetCurTalentCurFrame();
	int						GetTalentFrame(int nAniSequenceindex);

	float					GetCurTalentCurFrametime();
	float					GetCurTalentMaxFrametime();
	float					GetCurTalentFrametime(int nSeqIndex);

	int						GetCurAniMaxSequence() { return m_pCurrMotion->m_vecAniSequence[m_nCurrType].size(); }
	int						GetCurAniSequence() { return m_nCurrAniSequence; }

	void					SetFrame(int frame);

	void					Clear();


	RMatrix					GetAnimationDummyLocLastMatrix(int nAniSequence, vector<RMatrix>& vecMatrixList);
	void					CalAnimationDummyLocLastMatrixList(TCharacter* pActor, vector<RMatrix>& vecOutMatrixList);

	void					ChangeCustomAnimationList(vector<string>& vecAnimaionList);
	void					ChangeCustomAnimationListSpeed(float fSpeed);

	bool					CheckExistCurAnimation();
	bool					CheckMotionInfoModify();

	void					NextSequence( TCharacter* pAniActor );

	void					SetActor(TCharacter* pActor) { m_pActor = pActor; }

	void					SetCustomAnimationListEndTime(int nIndex, float fEndTime);
};
