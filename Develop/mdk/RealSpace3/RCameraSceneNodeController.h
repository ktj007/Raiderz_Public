#ifndef _RCAMERA_SCENE_NODE_CONTROLLER_H
#define _RCAMERA_SCENE_NODE_CONTROLLER_H

#include "RTypes.h"

namespace rs3 {

/// 콘트롤러가 업데이트 되는 타이밍에 따라 타입이 나뉜다
enum RCAMERACONTROLLERTYPE {

	RCCT_PREUPDATE = 0,	// 애니메이션 업데이트 전
	RCCT_POSTUPDATE,	// 애니메이션 업데이트 후
	RCCT_PRERENDER,		// 렌더링 직전
	RCCT_POSTRENDER,	// 렌더링 후

	RCCT_MAX,
};

class RCameraSceneNode;
class RInferredRenderingStrategy;
class RS_API RCameraSceneNodeController
{
	friend RCameraSceneNode;

public:
	// 생성
	RCameraSceneNodeController(RCAMERACONTROLLERTYPE eType = RCCT_PREUPDATE, const char* _pSzCameraControllerID = NULL, bool _bManagedByCamera = true)
	: m_strCameraControllerID(_pSzCameraControllerID)
	, m_eType(eType)
	, m_pCameraSceneNode(NULL)
	, m_bRemoveReserved(false)
	, m_bManagedByCamera(_bManagedByCamera)
	{
	}

	RCAMERACONTROLLERTYPE	GetType() { return m_eType; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 아이디의 알파벳순서대로 업데이트가 됩니다.
	// 나중에 모든 컨트롤을 무시하는 컨트롤러를 만들거라면, 아이디를 z_ 로 시작한다거나.. 이런식으로 하셔서
	// 현재까지 설정된 위치등을 무시시키면 됩니다.
	//
	const std::string&					GetCameraControllerID()	{ return m_strCameraControllerID; }


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 컨트롤러에서 뺄때
	//
	void								RemoveFromCamera();
	void								ReserveToRemove()		{ m_bRemoveReserved = true; }


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 카메라 삭제
	//
	bool								IsRemoveReserved()	{ return m_bRemoveReserved; }
	bool								IsManagedByCamera()	{ return m_bManagedByCamera; }


protected:

	// 소멸
	virtual ~RCameraSceneNodeController(void)
	{
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 메인 업데이트
	//
	virtual void						UpdateCameraController(RMatrix& _rMatApplyToCamera) = 0;
	virtual void						OnClear(){};
	virtual void						OnRender(){};
	virtual void						OnPostRender(){};

	// 카메라로부터 들어갈때, 나올때
	virtual bool						IsBlendableBeforeController(){ return false; }	// 들어갈때, 이미 같은 Controller가 있는지
	virtual void						OnInsertToCamera( RCameraSceneNodeController* _pBeforeController ){}
	virtual void						OnRemoveFromCamera(){}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Controller id / Controller owner camera
	//
	RCAMERACONTROLLERTYPE				m_eType;
	std::string							m_strCameraControllerID;
	RCameraSceneNode*					m_pCameraSceneNode;


	// 컨트롤러 컨테이너에서 제거되기 위한 멤버
	bool m_bRemoveReserved;
	bool m_bManagedByCamera;

private:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RCameraSceneNode에서만 불리는것
	//
	void InsertToCameraWhenControlAdded( RCameraSceneNode* _pCameraSceneNode, RCameraSceneNodeController* _pBeforeController )
	{
		m_pCameraSceneNode = _pCameraSceneNode;
		OnInsertToCamera(_pBeforeController);
	}
};

}

#endif
