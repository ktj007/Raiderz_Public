#pragma once

#include "WS_CutSceneLogicObjectBase.h"

class CCutSceneLogicObjectEffect : public CCutSceneLogicObjectBase
{
private:
	XCutSceneObjectControllerEffect* m_pXCutSceneController;

protected:
	virtual void OnRender();

public:
	CCutSceneLogicObjectEffect() : CCutSceneLogicObjectBase(), m_pXCutSceneController(NULL) {}
	virtual ~CCutSceneLogicObjectEffect() {}

	void SetXCutSceneController(XCutSceneObjectControllerEffect* pXCutSceneController);
};