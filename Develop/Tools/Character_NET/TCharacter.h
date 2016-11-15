#pragma once

#include "RActor.h"
#include "RActorController.h"
#include "CSItemData.h"
#include "XCharacter.h"
#include "TTransformControllerManager.h"
#include "TBaseAnimationTC.h"
#include "TAnimationInfoMgr.h"

using namespace rs3;

/// SoulHunt 의 캐릭터 메쉬, RActor의 래퍼클래스
// item장착및 무기들의 장착등의 캐릭터의 규칙은 여기에 모두 정의된다.
// Character_Net 툴에서 같이 써야 하므로 *** 절대*** 아무거나 include 하지 말것

struct TCharacterEquipment
{
	//string			strDefaultPartsName;	// 기본 파츠(다른 메쉬일 수 있음)
	RMeshNode*			pBaseMeshNode;	// 아무것도 없을때 기본모습
	RActorNode*			pActorNode;		// 현재 장착된 노드
	RActorNode*			pActorNode_b;	// 무기에 대해 추가된 parent가 body 인 노드 
	RActorNode*			pActorNode_w;	// 무기에 대해 추가된 parent가 world 인 노드 
	const CSItemData*	pItemData;		// 현재 장착된 아이템

	TCharacterEquipment()
	{
		Clear();
	}

	void Clear()
	{
		pBaseMeshNode = NULL;
		pActorNode = NULL;
		pActorNode_b = NULL;
		pActorNode_w = NULL;
		pItemData = NULL;
	}
};

class TCharacter : public XCharacter
{
	MDeclareRTTI;
private:
	TCharacterEquipment	m_equipments[ITEMSLOT_MAX];
	RActorNode*			m_pQuiverNode;
	RActorController*	m_pSpineController;

	TTransformControllerManager* m_pTC;
	TBaseAnimationTC *	m_pBaseAnimationTC;

	TAnimationInfoMgr			m_toolAnimationInfoMgr;

protected:
	void AddQuiver();			// 화살통
	void RemoveQuiver();

public:
	TCharacter(void);
	~TCharacter(void);

	virtual bool Create(const char* szMeshName, bool bBackgroundCreation = false);
	virtual bool Create(RMesh* pMesh, bool bBackgroundCreation);
	virtual void Destroy();

	// Equip / UnEquip item
	// UnEquip은 SyncLoad로, Equip은 MeshLoading을 AsyncLoading으로
	//bool EquipItem(const CSItemData& itemData, int nSlot);
	bool EquipItem(const CSItemData& itemData, SH_ITEM_SLOT nEquipedSlot);
	void EquipItem( RMesh* _pMesh, const CSItemData& itemData);
	void EquipItem( RMesh* _pMesh, const CSItemData& itemData, SH_ITEM_SLOT nEquipedSlot);

	void XCharacterSerializeStrategy_CreatePartsNodeAsync( SH_ITEM_SLOT nEquipedSlot, string &strItemMeshName, const CSItemData &itemData );
	void UnequipItem(int nSlot, bool bResetToDefault = true);
	void SetToDefaultPartsMesh(int nSlot);		// 기본 메시로 파츠 변경

	TCharacterEquipment* GetEquipItems() { return m_equipments;}

	void	FindDummyHook(int nGroupID, vector<RActorNode*> &DummyHookNode);
	vec3	GetDummyHookPos(int nGroupID, int nSlotID);

	string GetPartsMeshName(const string& strMeshPath, const string& strMeshName, bool bMeshNameBySex);
	string GetWeaponMeshName(const string& strMeshName);

	WEAPON_TYPE	GetCurrentWeaponType();
	CSItemData* GetItem(SH_ITEM_SLOT nSlot);

	XPlayerPartsSlotType Slot2LookSlot( SH_ITEM_SLOT nSlot );

	string	CurrentSexType()
	{
		return GetPureFileName(m_pMesh->GetName());
	}
	bool IsEquipShield();

	//////////////////////////////////////////////////////////////////////////
	// 애니메이션
	void SetBaseAnimationTC(TBaseAnimationTC* pTC) { m_pBaseAnimationTC = pTC; }
	TTransformControllerManager* GetAnimationTC() { return m_pTC; }
	void UpdateAnimationTC(float fDela);
	void ForceUpdateBaseAnimationTC();

	bool SetAnimation(string& strAniName, bool bForce=false, bool bSameFrame = false, bool bTestPlay = false);
	bool ReSetAnimation();
	RAnimation* GetCurAnimation();

	void SetAnimationSpeed(float fSpeed);
	float GetAnimationSpeed();
	unsigned int  GetAnimationFrame();
	void  SetAnimationFrame(unsigned int nFrame);
	unsigned int  GetAnimationMaxFrame();
	float GetMaxFrameToSecond();
	float GetCurFrameToSecond();


	void InitCurrentAnimation();
	void PauseAnimation();
	void StopAnimation();
	void PlayAnimation();
	void ResetAnimationControl();

	bool IsAnimationLoop();
	bool IsPlayDone();
	bool IsOncePlayDone();

	RPLAYSTATE GetAnimationState();

	void ClearAnimationEvent();
	void AddAnimationEvent(ANIMATION_EVENT& ani_event);
	void ResetAnimationEventByFrame(int nFrame);

	// 페이스 애니메이션
	void CreateFaceAnimation(bool bNPC = false);
	void StartFaceAnimation(string& strAniName, bool bEye);
	void EndFaceAnimation(string& strAniName, float fEndBledTime);
	void StopFaceAnimation();

	// 상체 애니메이션
	bool SetUpperAnimation(string strAniName);

	// 페이크 비튼 테스트
	void InitFakeBeatenAnimation(tstring& strAniName, float fWeight, float fSpeed);
	void StartFakeBeaten(const string& strCurAni, const float fWeightRate, const float fSpeedRate, const char* szAniPrefix=NULL);


	// 애니메이셔 체크
	bool CheckExistNodeController(string& strNodeName);

	// 애니메이션 샘플링
	void GetSamplingActorNodeData(string strNodeName, unsigned int nStartFrame, unsigned int nEndFrame, int nSampleInterval, std::vector< RMatrix >& _rOUt);
	RActorNode *  GetAnimatedActorNodeTransform(unsigned int nFrame, string& strNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace);
	RActorNode *  GetAnimatedActorNodeTransform2(unsigned int nFrame, string& strNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace);

	virtual XOverlayAnimationInfo* GetOverlayAnimaionInfo();
	virtual BLEND_DATA GetAnimationBlendData(string strSource, string strTarget);
	virtual XAnimationHitSoundInfo* GetAnimationHitSoundInfo();
	virtual XReActionAnimationInfo* GetReActionAnimationInfo();

	TAnimationInfoMgr* GetAnimationInfoMgr() { return &m_toolAnimationInfoMgr; }

	void InitMeshNodeNoiseRef();
};

