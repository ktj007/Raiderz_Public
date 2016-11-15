#include "StdAfx.h"
#include "C_ControlDoAction.h"

#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperResource.h"
#include "WS_WorkSpaceManager.h"
#include "VR_EtcDecal.h"
#include "VR_EtcCollision.h"
#include "VR_EtcDecal.h"
#include "M_ToolDecal.h"

using namespace rs3;
void CControlDoAction::EtcPathSelect(CControlMediator* _pController)
{
	if (_pController->GetToolSceneManager()->GetCurrentTerrain())
		_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );

	CDoActionEtcPathSelect* pEtcPathSelectAction = _pController->GetWorkSpaceMgr()->GetDoActionEtcPathSelect();
	if (pEtcPathSelectAction && pEtcPathSelectAction->DoPreAction(_pController) == false)
		pEtcPathSelectAction->Act(_pController);
}

void CControlDoAction::EtcCamera(CControlMediator* _pController)
{
	if (_pController->GetToolSceneManager()->GetCurrentTerrain())
		_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );
}

void CControlDoAction::EtcMapView(CControlMediator* _pController)
{
	if (!_pController->GetHelperAction()->GetSelectedSceneObject()->empty())
		_pController->GetHelperAction()->RemoveAllSelection(true);

	if (_pController->GetToolSceneManager()->GetCurrentTerrain())
		_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );
}

// for development
void CControlDoAction::EtcDev(CControlMediator* _pController)
{
	_pController->GetWorkSpaceMgr()->RunningWBDevAction();
}

void CControlDoAction::EtcDecal(CControlMediator* _pController)
{
	const RVector* pMousePosVec = _pController->GetHelperAction()->GetCurrentPickingPosition();
	if( pMousePosVec == NULL )
		return;

	CRollupEtcDecal* pRollupDecal = _pController->GetViewPtr<CRollupEtcDecal>();
	if ( _pController->m_mouseInfo.mouseState.bLButton && pRollupDecal->m_bCreate )
	{
		rs3::RToolSceneManager* pToolSceneManager = _pController->GetToolSceneManager();
		if(pToolSceneManager == NULL)
			return;

		CRollupEtcCollision* pCollision = _pController->GetViewPtr<CRollupEtcCollision>();
		CControlMediatorHelperResource* pMediatorHelpr = _pController->GetHelperResource();
		if( pMediatorHelpr->GetToolDecal() )
			pMediatorHelpr->GetToolDecal()->Create(pToolSceneManager, pRollupDecal, *pMousePosVec, pCollision);

		Sleep(100);
	}
	else if( pRollupDecal->m_bCreate == false )	// »èÁ¦
	{
		CControlMediatorHelperResource* pMediatorHelpr = _pController->GetHelperResource();
		if( pMediatorHelpr->GetToolDecal() )
			pMediatorHelpr->GetToolDecal()->Destroy();
	}
}