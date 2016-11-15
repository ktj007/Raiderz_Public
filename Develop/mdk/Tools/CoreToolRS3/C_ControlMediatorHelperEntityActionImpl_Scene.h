#pragma once

#include "C_ControlMediatorHelperAction.h"

class CControlMediatorHelperEntityActionImpl_Scene : public CControlMediatorHelperEntityAction
{
public:
	CControlMediatorHelperEntityActionImpl_Scene(CControlMediator* _pControlMediator) { m_pControlMediator_ = _pControlMediator; }
	virtual ~CControlMediatorHelperEntityActionImpl_Scene(){}

	virtual bool OnCheckAddSelection(CEntityObject* _pEntityObject, bool _bUpdateUI) { return false; }

	virtual void OnRemoveSelection(CEntityObject* _pEntityObject, bool _bUpdateUI) {}
	virtual void OnRemoveSelectionAll(bool _bUpdateUI) {}

	virtual void OnDeleteSelectedObject(ENTITY_OBJECTLIST& _list, ENTITY_OBJECTLIST& _copyList) {}
	virtual void OnPasteCopiedObject(ENTITY_OBJECTLIST& _list, ENTITY_OBJECTLIST& _copyList) {}

	virtual void OnDeleteSelectedObject(LOGIC_OBJECTLIST& _list, LOGIC_OBJECTLIST& _copyList);

	virtual void OnAddToLogicObjectManager(CEntityObject* _pEntityObject, int& _nCnt, int& _nIdx) {}
	virtual void OnRemoveFromLogicObjectManager(CEntityObject* _pEntityObject, int& _nCnt, int& _nIdx) {}

	virtual void OnGetNavigationDirection(CEntityObject* _pEntityObject, const rs3::RVector& _vPos, rs3::RVector& _outDir) {}
	virtual void OnChagnedEnvWeather(const char* _pSzName) {}
	virtual void OnToolBarNewEntityClicked() {}
	virtual void OnToolBarEntityOnTerrainClicked() {}
	virtual void OnToolBarSelectionLocked(bool _bLock) {}

private:
	CControlMediator* m_pControlMediator_;
};
