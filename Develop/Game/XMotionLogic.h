#pragma once

#include "XMotion.h"
#include "CSItemData.h"
#include "RSkeleton.h"
#include "XEventID.h"
#include "XMotionTrigger_Checker.h"
#include "XMotionTrigger_Executor.h"

class XGameTransformControllerManager;
class XMotionTimeModifier;
class XNPCInfo;
class XMotionIdleMemory;

class XMotionAniNameMaker
{
private:
public:
	void MakePlayerAniName(wstring& outName, XMotion* pMotion, XMotionTypes curr, int nAniSequence, WEAPON_TYPE nWeaponType, CHAR_STANCE nStance);
	void MakeNPCAniName(wstring& outName, XMotion* pMotion, XMotionTypes curr, int nAniSequence, int nCurrNPCMode, XNPCInfo* pNPCInfo);

	// 탈것
	void MakeRideAniName( wstring& outName, XMotion* pMotion, XMotionTypes curr, int nAniSequence, int nCurrNPCMode, XNPCInfo* pNPCInfo );
	void MakeRiderAniName( wstring& outName, XMotion* pMotion, XMotionTypes curr, int nAniSequence, WEAPON_TYPE nWeaponType, CHAR_STANCE nStance );
};


/// Motion 로직 처리 담당 클래스
class XMotionLogic
{
	struct SCustomAniName
	{
		wstring	m_strMotion;
		wstring	m_strAni;

		SCustomAniName(const wstring& motion, const wstring& ani) : m_strMotion(motion), m_strAni(ani) {}
	};

	struct SMotionQueueNode
	{
		wstring	m_strMotionName;
		float	m_fSpeed;
		float	m_fSpeedRatio;
	};

	class XMotionError
	{
	private:
		wstring					m_strMotionError;
		wstring					m_strAnimationError;
	public:
		void Clear();
		void ClearMotionError();
		void ClearAnimationError();
		void SetMotionError(const wchar_t* szMsg);
		void SetMotionError(wstring& strMsg);
		void SetAnimationError(const wchar_t* szMsg);
		void SetAnimationError(wstring& strMsg);
		wstring GetAnimaionError() { return m_strAnimationError; }
		bool IsExistMotionError();
		bool IsExistAnimationError();
	};
protected:
	XActor*					m_pOwner;
	XGameTransformControllerManager*	m_pAnimationController;
	XMotionTimeModifier*	m_pMotionTimeModifier;
	wstring					m_strAniPostfix;
	bool					m_bUseAniPostfix;
	float					m_fReservedSpeed;
	XMotionError			m_Error;

	XMotionIdleMemory*		m_pMotionIdleMemory;

	static XMotionTrigger_Checker		m_TriggerChecker;
	static XMotionTrigger_Executor		m_TriggerExecutor;

	// event
	XMotion*				OnPreEnter(XMotion* pMotion);
	void					OnEnter(XMotion* pMotion);
	void					OnLeave(XMotion* pMotion);
	void					OnUpdate(XMotion* pMotion);
protected:
	XMotion*				m_pCurrMotion;
	int						m_nCurrAniSequence;
	XMotionTypes			m_nCurrType;
	XMotionTypes			m_nForcedType;			///< 이 값이 MT_DEFAULT가 아닌 값으로 설정되어 있으면 CurrType에 상관없이 이 모션 타입으로 모션을 바꾼다.
	wstring					m_strLastEvent;
	bool					m_bUseWeaponAni;		///< 플레이어는 무기별 애니메이션이 다르기 때문에
	bool					m_bChanged;
	WEAPON_TYPE				m_nWeaponType;


	bool					m_bPlayDone;
	bool					m_bHoldSequence;

	vector<SCustomAniName>	m_vCustomMotionAniName;
	list<SMotionQueueNode>	m_MotionQueue;

	void AddCustomMotionAniName(const wchar_t* szMotionName, const wchar_t* szAnimationName);
	void MakeCurrAniString( wstring& strOut, XMotionTypes curr );
	void MakeCurrAniStringNoWeapon(wstring& strOut, XMotionTypes curr);

	virtual bool _ChangeMotion(const wchar_t* szMotionName, XMotionTypes nType, bool bForced, bool bPreFrame = false );
	virtual bool SetAnimation(bool bForced=false, bool bPreFrame = false);

	bool _SetRideAnimation( bool bForced=false, bool bPreFrame = false );
	bool _SetAnimation( bool bForced=false, bool bPreFrame = false );
	void MakeRideAniString( wstring& strRideOut, wstring& strCharOut );

	void ResetInfo();
	void ResetAniTime();
	void SetSequence(bool bForce);
	virtual void PlayDone();

	void PlayNextMotion();
	void AnimationError();
	void SendEvent(XEventID nEventID);
	bool CheckPostTrigger(int nSeqIndex);

	void NextSequence( int nNextSeqIndex );

	bool CheckDuplicationNextSequenceAnimation(int nNextSeqIndex);
public:
	XMotionLogic(XActor* pOwner, XGameTransformControllerManager* pAnimationController, XMotionTimeModifier* pMotionTimeModifier, XMotionIdleMemory* pMotionIdleMemory);
	virtual ~XMotionLogic();

	static void BindMotionEvents();

	virtual void Update(float fDelta);

	void OnEvent(const wchar_t* szEvent);

	bool ChangeMotion(const wchar_t* szMotionName, XMotionTypes nType=MT_DEFAULT, bool bForced=false, bool bPreFrame = false);
	bool ChangeMotion(const wchar_t* szMotionName, const wchar_t* szNextMotionName, XMotionTypes nType=MT_DEFAULT, bool bForced=false, bool bPreFrame = false);
	bool ChangeToCustomMotion(const wchar_t* szMotionName, const wchar_t* szAnimationName, bool bForced=false, bool bPreFrame = false);

	void PushMotion(const wchar_t* szMotionName, float fMotionSpeed =1.0f, float fMotionSppedRatio = 1.0f);
	void PushCustomMotion(const wchar_t* szMotionName, const wchar_t* szAnimationName, float fMotionSpeed =1.0f, float fMotionSppedRatio = 1.0f);

	void Reload();
	void RefreshThisMotion();

	void SetAnimationTime(int nAnimationIndex, unsigned int nPlayTime);
	void SetSpeed(float fSpeed);
	float GetSpeed();
	float GetOrgSpeed();
	void SetMotionTime(unsigned int nPlayTime);
	float GetMotionTime();
	void ChangeWeaponAniType(WEAPON_TYPE nWeaponType);	///< 무기 모션 타입은 외부에서 직접 설정해줘야 한다.
	void ChangeStanceType();	// 필요 할때에만 쓰세요

	void HoldSequence();
	void HoldRelease();
	bool IsHoldingSequence() { return m_bHoldSequence; }

	void SetAnimationNamePostfix(const wchar_t* szPostfix);
	void ReleaseAnimationNamePostfix();

	void ReserveNextMotionSpeed(float fSpeed);		// 모션의 첫번째 Sequence만 적용된다.

	wstring GetCurrMotion();
	wstring GetCurrSequence();
	XMotionTypes GetCurrMotionType();
	wstring GetCustomMotionAniName(const wstring& szMotionName);

	bool IsCameraLook();
	bool IsCurrMotionLoop();
	bool IsCurrMotionPlayDone();
	virtual bool IsCurrentMotion(wstring szMotionName);
	bool IsPastDeadPostTrigger();

	bool ChangeMotionQueueInfo(wstring strMotionName, wstring strAnimationName, wstring strChangeMotionName, wstring strChangeAnimationName);
};
