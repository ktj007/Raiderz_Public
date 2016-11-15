#ifndef _XMODULE_MOTION_H
#define _XMODULE_MOTION_H

#include "XModule.h"
#include "XMotion.h"
#include "CSItemData.h"
#include "RSkeleton.h"
#include "XMotionIdleMemory.h"

class XMotionLogic;
class XUpperMotionLogic;



/// @brief 오브젝트에서 애니메이션을 관리하는 모듈
/// @author 남기룡
class XModuleMotion : public XModule, public MMemPool<XModuleMotion>
{

	DECLARE_ID(XMID_MOTION);
private:
	XModuleEntity*			m_pModuleEntity;
	XMotionIdleMemory		m_MotionIdleMemory;

protected:
	XMotionLogic*			m_pMotionLogic;
	XUpperMotionLogic*		m_pUpperMotionLogic;


private:
	virtual void			OnUpdate(float fDelta);
	virtual XEventResult	OnEvent(XEvent& msg);
	virtual void			OnSubscribeEvent();
	virtual void			OnInitialized();

public:
	bool ChangeMotion(const wchar_t* szMotionName, XMotionTypes nType=MT_DEFAULT, bool bForced=false, bool bForceIdle=false );	///< 모션 변경
	bool ChangeMotion(const wchar_t* szMotionName, const wchar_t* szNextMotionName, XMotionTypes nType=MT_DEFAULT, bool bForced=false );	///< 모션 변경
	bool ChangeToCustomMotion(const wchar_t* szMotionName, const wchar_t* szAnimationName, bool bForced=false);	///< 특수한 모션으로 변경(스킬 등)
	void RefreshThisMotion();

	XMotionTypes GetCurrMotionType();


	// 정리됨 --
	XModuleMotion(XObject* pOwner=NULL);		///< 생성자
	virtual ~XModuleMotion(void);				///< 소멸자

	void PushMotion(const wchar_t* szMotionName, float fMotionSpeed =1.0f, float fMotionSppedRatio = 1.0f, bool bForceIdle=false);									///< 모션 예약. 모션을 큐에 넣는다.
	void PushCustomMotion(const wchar_t* szMotionName, const wchar_t* szAnimationName, float fMotionSpeed =1.0f, float fMotionSppedRatio = 1.0f);	///< 특수한 모션으로 변경(스킬 등)

	void SetPause(bool bPause);
	void ChangeWeaponAniType(WEAPON_TYPE nWeaponType);	///< 무기 모션 타입은 외부에서 직접 설정해줘야 한다.	//--
	void ChangeStanceType(); // 필요할 때만 쓰세요.
	void SetAnimationTime(int nAnimationIndex, unsigned int nPlayTime);		///< 해당 인덱스 애니메이션의 속도를 설정한다.
	void SetMotionTime(unsigned int nPlayTime);		///< 전체 모션 플레이타임
	float GetMotionTime();
	void SetSpeed(float fSpeed);		///< 모션 스피드 설정
	float GetSpeed();					///< 모션 스피드 구함
	float GetOrgSpeed();

	wstring GetCurrMotion();
	wstring GetCurrSequence();
	wstring GetCustomMotionAniName(const wstring& szMotionName);

	bool IsCameraLook();
	bool IsCurrMotionLoop();
	bool IsCurrMotionPlayDone();
	bool IsCurrentMotion(wstring szMotionName);
	bool IsPastDeadPostTrigger();

	void HoldSequence( bool bHold);
	bool IsHoldingSequence();

	void SetAnimationNamePostfix(const wchar_t* szPostfix);
	void ReleaseAnimationNamePostfix();

	void ReserveNextMotionSpeed(float fSpeed);

	void Reload();

	//////////////////////////////////////////////////////////////////////////
	// 상체 애니메이션
	bool ChangeUpperMotion(const wchar_t* szMotionName, XMotionTypes nType=MT_DEFAULT, bool bForced=false );	///< 모션 변경
	bool ChangeUpperMotion(const wchar_t* szMotionName, const wchar_t* szNextMotionName, XMotionTypes nType=MT_DEFAULT, bool bForced=false );	///< 모션 변경
	bool ChangeToCustomUpperMotion(const wchar_t* szMotionName, const wchar_t* szAnimationName, bool bForced=false);	///< 특수한 모션으로 변경(스킬 등)
	
	void SetUpperAnimationTime(int nAnimationIndex, unsigned int nPlayTime);		///< 해당 인덱스 애니메이션의 속도를 설정한다.

	void StopUpperMotion();

	wstring GetCurrUpperMotion();
	bool IsCurrentUpperMotion(wstring szMotionName);

	void SetUpperSpeed(float fSpeed);		///< 모션 스피드 설정

	//////////////////////////////////////////////////////////////////////////
	// Memory
	void SetIdleMemoryMotion(int nBuffID, wstring strMotion, XMotionTypes nType=MT_DEFAULT, bool bForced=false);
	void SetIdleMemoryCustomMotion(int nBuffID, wstring strMotion, wstring strAnimationName, bool bForced=false);
	void RemoveIdleMemory(int nBuffID);
	void RemoveAllIdleMemory();

};


#endif // _XMODULE_MOTION_H