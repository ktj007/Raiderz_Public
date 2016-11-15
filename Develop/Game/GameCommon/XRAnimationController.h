#pragma once

#include "RSceneNode.h"
#include "RAnimationController.h"
#include "MTstring.h"

using namespace rs3;

class XRAnimationController : public RAnimationController
{
public:
	XRAnimationController(RSNCTRL_TYPE eType = RSNCTRL_POSTANIMATION, RSNCTRL_TYPE eFrameControllerType = RSNCTRL_UPDATE);
	virtual ~XRAnimationController();

	void			OnPreUpdate();
	void			OnUpdate();

	void			DisableAniamtionEventPreFrame(unsigned int nFrame);

	bool			PushActiveNode(tstring& strNodeName);

	bool			IsActiveNode(tstring& strNodeName);
	bool			IsAnimationLoop();

	// 함부로 쓰지 마세요(조심하란말..)
	// 외부에서 직접 XRAnimationController를 쓰지 마시고... XTransformController 클래스를 상속받는 클래스들만 쓰도록 합시다
	// 예로 XBaseAnimationTC::GetAnimatedActorNodeTransform() 함수를 참고 바랍니다.
	void			GetLocalTransformAt(DWORD nFrame, RMatrix& outMatrix, RActorNode* pActorNode);	///< 특정 프레임의 local tm 을 얻는다
	void			GetWorldTransformAt(DWORD nFrame, RMatrix& outMatrix, RActorNode* pActorNode);	///< 특정 프레임의 world tm 을 얻는다
	void			GetResultTransformAt(DWORD nFrame, RMatrix& outMatrix, RActorNode* pActorNode);	///< 특정 프레임의 world tm 을 얻는다


	void			ResetAniamtionSpeed();
	float			GetOrgAniamtionSpeed();

	RAnimationControllerNode* GetControllerNode(tstring& strNodeName);
};