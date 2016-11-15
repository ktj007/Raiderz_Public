#ifndef _XTARGET_H
#define _XTARGET_H

/************************************************************************/
/* 타겟 정보와 이펙트 셋팅까지 한다.                                    */
/************************************************************************/
#include "XModuleActorControl.h"
#include "XModuleCollision.h"
#include "XMouseBase.h"

class XObject;
class XNonPlayer;

#define TARGET_EFFECT_FILE_NAME		L"lock"
#define TARGET_CHECK_OBJECT_DIST	1.0f
#define TARGET_CHECK_LINE_DIST		35.0f
#define ICON_EFFECT_MARKER			L"ef_head_icon_marker"

enum TARGET_TYPE
{
	XTT_BASE,
	XTT_LINE,
	XTT_STICKY,
	XTT_GROUND,
	XTT_COLLISIONBOX,
};

//------------------------------------------------------------------------
struct stTargetID
{
	MUID		uidTarget;			// 타겟 오브젝트 UID
	vec3		vTargetPos;			// 타겟을 잡은 중심 좌표(즉, 타겟을 잡게 된 핵심 좌표이다.)
	int			nTargetGroupID;		// 타겟 그룹 ID
	int			nTargetCapsuleID;	// 타겟 캡슐 ID
	MUID		uidEffect;			// 타겟효과 uid

	bool		bHitParts;			// 부분 판정
	unsigned long	time;

	stTargetID()
	{
		Init();
	};

	void Init()
	{
		uidTarget			= MUID::ZERO;
		vTargetPos			= vec3::ZERO;
		nTargetGroupID		= -1;
		nTargetCapsuleID	= -1;
		uidEffect			= MUID::ZERO;
		bHitParts			= false;
		time				= 0;
	}

	stTargetID(MUID uidTarget, MUID e) : uidTarget(uidTarget), uidEffect(e) {}
};

//------------------------------------------------------------------------
class XTargetRenderer
{
private:
public:
	XTargetRenderer()	{}
	MUID SetIndicatorEffect(XObject* pObject);
	MUID SetTargetBoneEffect(XObject* pObject, RActorNode * pPartsTargetNode, vec3& pos);
	MUID SetTargetHitPartsEffect(XObject* pObject, vec3& centerPos, vec3& pos);
	void DelTargetEffect(MUID uidEffect);
};

// [1/23/2008 isnara] XTarget 리펙토링
class XTarget : public XMouseBase
{
protected:
	CSEffectInfo::RELATION	m_nCurrentTargetAllowed;

	XTargetRenderer				m_TargetRenderer;

	float						m_fTargetRange;
	int							m_nOneTargetEn;

	bool						m_bTargetUI;

protected:
	// 여러가지 검사
	bool						CheckMe(XObject* pObject);
	bool						CheckNPC(XObject* pObject);											// NPC냐 몬스터냐 구별
	virtual bool				CheckTargetAllowed(XObject* pObject);
	bool						CheckExistTargetObjOnScreen(XObject* pObject);
	bool						CheckColTerrain(vec3& vStart, vec3& vTarget, float& fOutDistance, vec3* vOutTargetPos = NULL);
	bool						CheckTotalEN(int nTargetCount);
	bool						CheckEqualFaction(XPlayer* pObject);
	bool						CheckTargetHitCapsule(XObject* pObject);

	// 기타
	virtual void				GetCameraInfo(vec3& vPos, vec3& vDir);

public:
	XTarget(XObject* pObject);
	virtual ~XTarget();

	virtual void				InitTarget(bool bTargetUI);
	virtual void				ExitTarget();
	virtual void				UpdateTarget()									{}
	virtual void				DelTargetAll()									{}

	void						StartTargetUI();
	void						EndTargetUI();

	virtual void				SetTargetInfo(float fRange, int nEn, CSEffectInfo::RELATION nAllowed = CSEffectInfo::RELATION_ENEMY);
	void						UseTargetUI(bool bUse)		{ m_bTargetUI = bUse; }

	virtual stTargetID*			GetTargetInfo()									{ return NULL; }
	virtual bool				GetTargetIDByUID(MUID uid, int& nOutGroupID, int& nOutDummyID)	{ return false; }

	virtual TARGET_TYPE			GetTargetType()		{ return XTT_BASE; }
};

#endif // _XTARGET_H