#pragma once

#include "CSDef.h"
#include "CSStrings.h"
#include "MXml.h"
#include "XAnim.h"
#include <vector>
#include <string>
#if (_MSC_VER >= 1900)
#include <unordered_map>
#else
#include <hash_map>
#endif
using namespace std;

#include "CSItemData.h"
#include "XMotionTrigger.h"

class XMotion;
class XMotionMgr;
class XModuleMotion;


enum XMotionTypes
{
	MT_DEFAULT = 0,
	MT_FORWARD,
	MT_BACKWARD,
	MT_LEFT,
	MT_RIGHT,
	MT_COUNT
};

class XMotionAniSequence
{
	friend class XMotionMgr;
	friend class XMotion;
private:
	bool			m_bWeaponAni;
	vector<tstring>	m_WeaponAniName;		// 조금이라도 빠르게 하기위해 미리 애니메이션 이름을 저장해 넣는다.
	TCHAR			m_szAniName[128];
	TCHAR			szEvent[64];
	bool			bExistEvent;
	float			m_fSpeed;
	float			m_fTimeOut;			///< 이 값의 시간이 넘으면 다음 씨퀀스로 넘어간다.
	int				m_nFrameOver;		///< 이 값의 프레임이 넘으면 다은 씨퀀스로 넘어간다.

	XMotionTrigger	m_PostTrigger;
public:
	XMotionAniSequence();
	void SetAniName(const TCHAR* szAniName, bool bWeaponAni);
	bool IsExistEvent()			{ return bExistEvent; }
	const TCHAR* GetEventName()	{ return szEvent; }
	const TCHAR* GetAniName() { return m_szAniName; }
	const TCHAR* GetAniName(WEAPON_TYPE nWeaponType);
	int GetFrameOver() const { return m_nFrameOver; }
	float GetSpeed() const		{ return m_fSpeed; }
	float GetTimeOut() const	{ return m_fTimeOut; }
	XMotionTrigger* GetPostTrigger() { return &m_PostTrigger; }
};

class XBaseMotionEventLister
{
public:
	XBaseMotionEventLister() {}
	virtual ~XBaseMotionEventLister() {}
};

/// 모션 - 애니메이션의 상위 개념(한 동작을 이루는 애니메이션의 모음)
class XMotion
{
	friend class XMotionMgr;
public:
	enum XMotionStance
	{
		MS_NORMAL = 0,	// normal 스탠스만
		MS_BATTLE,		// combat 스탠스만
		MS_ALL,			// 모든 스탠스 
		MS_MAX
	};
private:
	tstring							m_strName;
	XMotionStance					m_nStanceType;

	bool							m_bLoop;
	bool							m_bCancelEnabled;		///< 캔슬 가능 여부
//	bool							m_bCameraLook;			///< 이값이 true이면 이 모션중일때는 무조건 카메라 방향으로 내캐릭터 방향을 설정한다.

	bool							m_bCameraLook[MS_MAX];	///< 이값이 true이면 이 모션중일때는 무조건 카메라 방향으로 내캐릭터 방향을 설정한다.

	bool							m_bCustomAni;			///< 씨퀀스대로 동작안하고 다른 방식으로 애니메이션 되는 모션
	bool							m_bWeaponAni;			//< 무기따라 애니메이션을 할것인가?

	bool							m_bExistTrigger;		///< 트리거가 있는지 유무

	bool							m_bAddPostfix;			///< 애니메이션 이름에 상황에 따른 접미어를 붙일지 여부
	bool							m_bAutoPrefix;			///< 애니메이션 이름에 상황에 따른 접두어 붙일지 여부

	void CheckTrigger();
	void SetCameraLook(bool b);

	XBaseMotionEventLister*			m_pEventLister;
public:
	vector<XMotionAniSequence*>		m_vecAniSequence[MT_COUNT];

	XMotion();
	virtual ~XMotion();
	void Cook();
	tstring& GetName()				{ return m_strName; }

	XMotionStance GetStanceType()	{ return m_nStanceType; }
	XMotionAniSequence*	GetAniSequence(XMotionTypes nMotionType, int index);
	XBaseMotionEventLister* GetEventListener() { return m_pEventLister; }
	void SetEventListener(XBaseMotionEventLister* pListener) { m_pEventLister = pListener; }

	bool IsCameraLook()									{ return m_bCameraLook[MS_ALL]; }
	bool IsCameraLook(XMotionStance nStance)			{ return m_bCameraLook[nStance]; }
	bool IsCancelEnabled()			{ return m_bCancelEnabled; }
	bool IsLoop()					{ return m_bLoop; }
	bool IsCustom()					{ return m_bCustomAni; }
	bool IsWeaponAni()				{ return m_bWeaponAni; }
	bool IsExistTrigger()			{ return m_bExistTrigger; }
	bool IsAddPostfix()				{ return m_bAddPostfix; }
	bool IsAutoPrefix()				{ return m_bAutoPrefix; }
};

