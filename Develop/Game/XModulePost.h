#pragma once

#include "XModule.h"

class XModuleEntity;

class XMovementPostChecker
{
public:
	enum RESULT
	{
		RESULT_NONE = 0,
		RESULT_PostMove,
		RESULT_PostStop,
		RESULT_PostMoveStop,
		RESULT_PostPosSync,
		RESULT_PostRotate
	};

	struct CheckArgs
	{
		vec3 Pos;
		vec3 Dir;

		bool bCurrentGoing;
		bool bCurrentSliding;
		bool bKeyAction;
		bool bCurrentTalentActive;
		bool bCurrentPosSync;
		bool bSendablePositionPost;

		CheckArgs() {}
		CheckArgs(vec3& pos, vec3& dir, bool currentGoing, bool currentSliding, bool keyAction, bool currentTalentActive, bool currentPosSync, bool sendablePositionPost)
			: Pos(pos), 
			  Dir(dir), 
			  bCurrentGoing(currentGoing), 
			  bCurrentSliding(currentSliding),
			  bKeyAction(keyAction),
			  bCurrentTalentActive(currentTalentActive),
			  bCurrentPosSync(currentPosSync),
			  bSendablePositionPost(sendablePositionPost) {}
	};
private:
	bool					m_bStopPosted;
	vec3					m_vLastPostedPosition;
	vec3					m_vLastPostedSyncPosition;
	vec3					m_vLastPostedDir;
	float					m_fPosSyncElapsedTime;

	void ApplyCheckResult(RESULT nResult, const vec3& pos, const vec3& dir);
	RESULT _Check(float fDelta, const CheckArgs& args);
public:

	XMovementPostChecker();
	RESULT Check(float fDelta, const CheckArgs& args);
	void SetLastPostedPosition(vec3& vPos) { m_vLastPostedPosition = vPos; }
	void SetLastPostedDir(vec3& vDir) { m_vLastPostedDir = vDir; }
};

//////////////////////////////////////////////////////////////////////////
/// 커맨드 전송 관리(MyPlayer만 사용함)
class XModulePost : public XModule
{
	DECLARE_ID(XMID_POST);
protected:
	virtual void			OnUpdate(float fDelta);

	XMovementPostChecker	m_PostChecker;
	void _UpdateMovementPost(float fDelta, vec3& pos, vec3& dir, bool bKeyAction, uint32 nMovementFlags);

	void PostMove(vec3& vPos, vec3& vDir, uint32 nMovementFlags);
	void PostPosSync(vec3& vPos);
	void PostRotate(vec3& vDir);
public:
	XModulePost(XObject* pOwner=NULL);
	virtual ~XModulePost(void);
	void Init(vec3& vPos, vec3& vDir);

	void PostStop(vec3& vPos, vec3& vDir);
	void PostMoveStop(vec3& vPos);
	void PostPosSyncWhenFinishMovingMotionFactor(vec3& vPos);

	void PostEquipItem(SH_ITEM_SLOT nItemSlot, MUID uidItem);

	void UpdateMovement(float fDelta, vec3 pos, vec3 dir, bool bKeyAction, uint32 nMovementFlags);
};

