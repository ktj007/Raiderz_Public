#include "StdAfx.h"
#include "RActorController.h"
#include "RActor.h"

namespace rs3 {

RActorController::RActorController(RSNCTRL_TYPE eType) : RSceneNodeController(eType), m_pActor(NULL)
{

}

RActorController::~RActorController()
{

}

void RActorController::OnAdd()
{
	m_pActor = MDynamicCast(RActor, m_pSceneNode);
	_ASSERT(m_pActor);
}

void RActorController::OnRemove()
{
	m_pActor = NULL;
}

void RActorController::RefreshAABB()
{
	_ASSERT(m_pActor != NULL);
	m_pActor->SetAABB( m_pActor->ForceUpdateAABB() );
}

void RActorController::UpdateActorForRender()
{
	_ASSERT(m_pActor != NULL);
	m_pActor->UpdateForRender();
}

void RActorController::SetAutoUpdate(bool bEnable)
{
	_ASSERT(m_pActor != NULL);

	if( bEnable )
		++m_pActor->m_AutoUpdateContext.m_nAutoUpdateControllers;
	else
		--m_pActor->m_AutoUpdateContext.m_nAutoUpdateControllers;

	m_pActor->SetAutoUpdate(m_pActor->m_AutoUpdateContext.CheckAutoUpdateContext());
}

}
