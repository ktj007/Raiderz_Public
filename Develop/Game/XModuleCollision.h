#pragma once
#include "XModule.h"
#include "RActorNode.h"
#include "XCollision.h"
#include "RCollisionTree.h"
#include "CSMeshInfo.h"
#include "CSHitCapsuleIndexMgr.h"
#include "XWorld.h"
#include "XHitEffectPosCalculator.h"
#include "XMoveCollision.h"
#include "XObjectCollisionDebug.h"

class CSTalentHitInfo;
class CSMeshHitInfo;

enum INTERACTIONTYPE
{
	FIND_ALL = 1,
	FIND_EXCEPTDEAD,
	FIND_NETPLAYER,
	FIND_NPC,
	FIND_DEADNPC,
};

#define COLBOX_BONE			L"Bip01 Spine1"
#define OVER_MAP_HEIGHT		50.0f	// 넘어 갈 수있는 지형 높이


/// Actor 의 충돌처리및 외부에서 오는 충격을 처리하는 모듈
class XModuleCollision : public XModule, public MMemPool<XModuleCollision>
{
	DECLARE_ID(XMID_COLLISION);

	friend class XHitEffectPosCalculator;

	XMoveCollision			m_MoveCollision;
	CSHitCapsuleIndexMgr	m_HitCapsuleIndex;				// NPC만쓴다.
	XHitEffectPosCalculator	m_HitEffectPosCalculator;
	XObjectCollisionDebug	m_ObjectCollisionDebug;

	CSMeshInfo*				m_pMeshInfo;
private:
	bool					CheckErrorGroupID(int nGroupID);

protected:
	XModuleEntity*			m_pModuleEntity;
	virtual void			OnInitialized();
	virtual void			OnFinalized()	{}

	bool					ColCapsuleToCapsule(MCapsule& end_Capsule, MCapsule& target_Capsule, vec3& vImpactPos, vec3& vImpactDir, float& fOutDist, vec3* vNearFrom_Capsule = NULL, vec3* vNearTarget_Capsule = NULL);
	bool					ColVolumCapsuleToCapsule(MCapsule& start_Capsule, MCapsule& end_Capsule, vec3& vTo, vector<MCapsule>& vecVolumCapsule, vec3& vImpactPos, vec3& vImpactDir, vec3 vCurPos, float& fOutDist, bool bRub = true, XObjectCollisionDebug* pDebug = NULL);
	bool					GetCompareCollisionCapsule(XObject* pObejct, MCapsule& compare_Capsule, MCapsule& out_Capsule, float fDist);

	// 비비지 않기 위한 함수들...
	bool					GetColCapsuleToObjectNotRub(MCapsule& capsuleColTest, MCapsule& capsuleBase, vector<MCapsule>& vecTestColCapsul, MCapsule& ColOutOrgCapsule, MCapsule& ColOutCapsule, vec3& vNearestTargetPos, float& fOutDist);
	float					ColCapsuleToLineForNotRub(const vec3& vStart, const vec3& vEnd, const vec3& vNearestTargetPos, float fMaxLen, vec3& vOutNearest);
	float					ColCapsuleToLineForNotRub(const vec3& vStart, const vec3& vEnd, MCapsule& cpauleNearestTarget, float fMaxLen, vec3& vOutNearest);

	CSMeshInfo *			GetMyObjectMeshInfo();
	CSMeshInfo *			GetOtherObjectMeshInfo(XObject * pObejct);
	MCapsule				GetMyObjectCollisionCapsule(int n);
	MCapsule				GetNearColCapsuleFromPos(vec3 vPos);
	int						GetCollisionCapsuleCount();
	int						GetHitEffectCapsuleCount();

	float					CalLineTarget(vec3& vCenterPos, vec3& pos, vec3& dir, float& fRadius);
	RActorNode*				CheckLineTargetDummy(vec3& pos, vec3& dir, wchar_t* pDummyName);
	void					MakeVolumCapsule(MCapsule& start_Capsule, MCapsule& end_Capsule, vector<MCapsule>& vecOutVolumCapsule);

public:
	XModuleCollision(XObject* pOwner=NULL);
	virtual ~XModuleCollision(void);
	void					InitCol(CSMeshInfo* pMeshInfo);

	//----------------------------------------------------------------------------
	// 지형 충돌
	bool					GroundMoveTestMap(const RVector &from, const RVector &to, RVector& vOut, RVector& vOutNor, bool bJump, bool *pSliding, bool *pFontCol, vector<MUID>* pvecTerrainColObjectList);	// from->to 로 이동할때 가능한만큼을 pOut으로 리턴
	bool					WaterMoveTestMap(const RVector &from, const RVector &to, float fWaterHeightLimit, RVector& vOut, RVector& vOutNor, vector<MUID>* pvecTerrainColObjectList);

	bool					PickRayToTerrain(vec3& vstart, vec3& vend, vec3& voutPos, vec3& voutNor, DWORD dwCollisionAttribue = ~CollisionAttribute::WATER);
	bool					PickTerrainWater(const vec3 &pos, vec3& vOut);

	//----------------------------------------------------------------------------
	// 오브젝트 충돌
	bool					ColObject(vec3& vTo, vec3 vPos, vec3& vImpactPos, vec3& vImpactDir, float& fOutDist, vector<MUID>& vecTerrainColObjectList, bool bRub = true, bool bRadiusOffset = false);

	// 선분 충돌
	bool					Pick(vec3& old_pos, vec3& pos, const vec3& dir);
	bool					Pick(vec3& old_pos, vec3& pos, const vec3& dir, float& fDistance);

	// 어느점부터 직선 충돌
	bool					PickRayToObject(vec3& pos, const vec3& dir, float& fMaxDist, float& fOutDist);
	bool					PickRayToColCapsule(const vec3& pos, const vec3& dir, float fMaxDist, MCapsule& outCapsule, vec3& vOutPos, float& fOutDist);
	bool					PickRayFindNearBone(vec3& pos, RActorNode** ppOutBone = NULL);
	bool					PickRayToHitEffectCapsule(const vec3& pos, const vec3& dir, float fMaxDist, MCapsule& outCapsule, vec3& vOutPos, float& fOutDist);
	bool					PickRayToHitEffectObject(vec3& pos, const vec3& dir, float& fMaxDist, vec3& vOutPos, float& fOutDist, int& nOutGroupID, int& nOutCapsuleID);

	// 원뿔 충돌
	bool					LineRayToObject(vec3& pos, vec3& dir, float& fMaxDist, vec3& vOutPos, float& fOutDist);
	bool					LineRayToHitEffectObject(vec3& pos, vec3& dir, float& fMaxDist, vec3& vOutPos, float& fOutDist, int& nOutGroupID, int& nOutCapsuleID);

	// 캡슐 충돌
	bool					ColCapsuleToObject(MCapsule& start_Capsule, MCapsule& end_Capsule, vec3& vTo, vec3& vImpactPos, vec3& vImpactDir, float& fOutDist, bool bRub = true, XObjectCollisionDebug* pDebug = NULL);
	// 히트캡슐관리 함수
	void					InitHitCapsuleIndex();
	void					SetHitCapsuleByTrigger(int nGroupID)		{ m_HitCapsuleIndex.Change_ByTrigger(nGroupID); }
	void					SetHitCapsuleByTalent(int nGroupID)			{ m_HitCapsuleIndex.Change_ByTalent(nGroupID); }
	void					SetHitCapsuleByNPCMode(int nMode)			{ m_HitCapsuleIndex.ChangeNPCMode(nMode); }
	void					SetHitTalentComplete()						{ m_HitCapsuleIndex.Change_ByTalentComplete(); }
	int						GetHitCapsuleGroupIndex()					{ return m_HitCapsuleIndex.GetGroupIndex(); }

	void					CheckXYTerrainCollision( const vec3& vEntityPosition, vec3 &vec, vec3 &out, float fPickAddOriginZ = 100.0f );
	bool					CheckTerrainCollisionUsable();

	vec3					GetMeleeHitEffectPosition(XObject* pAttacker, int nTalentID);
	vec3					GetColObjectCenter();
	vec3					GetNotScaleColObjectCenter();
	vec3					GetColProjectileTargetPos(int nGroupID = -1 , int nIndexID = -1);
	vec3					GetHitEffectProjectileTargetPos(int nGroupID = -1 , int nIndexID = -1);

	MCapsule				GetCollisionCapsuleByIndex(int n);
	MCapsule				GetHitCapsule(int nHitGroupID, int nHitCapsuleID);

	CSMeshInfo *			GetMeshInfo();

	bool					GetObjectColBox( INTERACTIONTYPE FindType, vec3& vPos, vec3& vDir, RBoundingBox& ColBox, float& fDist );
	XObject*				GetInteractionObjectColBox(INTERACTIONTYPE FindType, vec3& vPos, vec3& vDir, RBoundingBox& ColBox, float& fDist );
	void					GetInteractionObjectListColBox(vector<XObject*>& outvecObjectList, INTERACTIONTYPE FindType, vec3& vPos, vec3& vDir, RBoundingBox& ColBox );

	float					GetRadius();
	float					GetDiameter();
	int						GetOpenHitGroupID();					

	MCapsule				CalCollisionCapsuleAnimation(CSColCapsule * pColCapsule, XObject * pTargetObj);
	void					BuildTerrainColObjectPolygon(vector<MUID>* pVec, vector<RCollisionPolygon>& buildPolygon);

	bool					IsHaveHitCapsuleCount(int nGroupID);
	bool					IsHaveCollisionCapsuleCount();
	bool					IsHaveHitEffectCount();

	bool					IsCheckXYFieldType( vec3& vEntityPosition, RCollisionTriangle::ATTRIBUTE attr);

	bool					IsBaseColliable();

	void					SetRadius_ForTest(float fRadius);

	wstring					GetMaterialName() { return m_MoveCollision.GetMoveTestDownMaterialName(); }

	void					ObjColDebugRender();
};

