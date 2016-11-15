#pragma once

#include "WS_CutSceneLogicObjectBase.h"

class CCutSceneLogicObjectMove : public CCutSceneLogicObjectBase
{
private:
	XCutSceneObjectControllerMove* m_pXCutSceneController;
	int m_nControllerIdx;

protected:
	virtual void OnRender();
	virtual void OnSelected_(bool bSelected);

public:
	CCutSceneLogicObjectMove()
		: CCutSceneLogicObjectBase()
		, m_pXCutSceneController(NULL)
		, m_nControllerIdx(-1) {}

	virtual ~CCutSceneLogicObjectMove() {}

	void SetXCutSceneController(XCutSceneObjectControllerMove* pXCutSceneController, int nCtrlIdx);
	int GetCtrlIdx(){ return m_nControllerIdx; }
};