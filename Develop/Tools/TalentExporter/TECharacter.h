#pragma once

#include "RActor.h"
#include "RActorController.h"
#include "CSItemData.h"
#include "XCharacter.h"
#include "TETransformControllerManager.h"
#include "TEBaseAnimationTC.h"

using namespace rs3;

/// SoulHunt 의 캐릭터 메쉬, RActor의 래퍼클래스
// item장착및 무기들의 장착등의 캐릭터의 규칙은 여기에 모두 정의된다.
// Character_Net 툴에서 같이 써야 하므로 *** 절대*** 아무거나 include 하지 말것

class TECharacter : public XCharacter
{
	MDeclareRTTI;
private:
	RActorNode*			m_pQuiverNode;
	RActorController*	m_pSpineController;

	TETransformControllerManager* m_pTC;
	TEBaseAnimationTC *	m_pBaseAnimationTC;

protected:
	void AddQuiver();			// 화살통
	void RemoveQuiver();

public:
	TECharacter(void);
	~TECharacter(void);

	virtual bool Create(const TCHAR* szMeshName, bool bBackgroundCreation = false);
	virtual bool Create(RMesh* pMesh, bool bBackgroundCreation);
	virtual void Destroy();

	//////////////////////////////////////////////////////////////////////////
	// 애니메이션
	void SetBaseAnimationTC(TEBaseAnimationTC* pTC) { m_pBaseAnimationTC = pTC; }
	TETransformControllerManager* GetAnimationTC() { return m_pTC; }
	void UpdateAnimationTC(float fDela);
	void ForceUpdateBaseAnimationTC();

	bool SetAnimation(tstring& strAniName, bool bForce=false, bool bSameFrame = false, bool bTestPlay = false);
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

	bool IsAnimationLoop();
	bool IsPlayDone();
	bool IsOncePlayDone();

	RPLAYSTATE GetAnimationState();

	void ClearAnimationEvent();
	void AddAnimationEvent(ANIMATION_EVENT& ani_event);
	void ResetAnimationEventByFrame(int nFrame);

	// 애니메이셔 체크
	bool CheckExistNodeController(string& strNodeName);

	// 애니메이션 샘플링
	void GetSamplingActorNodeData(tstring strNodeName, unsigned int nStartFrame, unsigned int nEndFrame, int nSampleInterval, std::vector< RMatrix >& _rOUt);
	RActorNode *  GetAnimatedActorNodeTransform(unsigned int nFrame, tstring& strNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace);
};

