#include "stdafx.h"
#include "XCutSceneObjectController.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

//////////////////////////////////////////////////////////////////////////

XCutSceneObjectController::XCutSceneObjectController( XCutSceneObject* pOwner, TYPE _eType )
: m_pOwner(pOwner), m_eType(_eType)
{
	m_fStartTime = 0.f;
	m_fEndTime = pOwner->GetClapper()->GetDuration();
}

void XCutSceneObjectController::GetRange( float& _f1, float& _f2 )
{
	_f1 = m_fStartTime;
	_f2 = m_fEndTime;
}