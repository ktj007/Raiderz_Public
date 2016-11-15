#include "stdafx.h"
#include "MockModuleMotion.h"

MockModuleMotion::MockModuleMotion( XObject* pOwner/*=NULL*/ ) : XModuleMotion(pOwner)
{
	
}

void MockModuleMotion::OnInitialized()
{

}

void MockModuleMotion::Init( XMotionLogic* pMotionLogic, XUpperMotionLogic* pUpperMotionLogic )
{
	m_pMotionLogic = pMotionLogic;
	m_pUpperMotionLogic = pUpperMotionLogic;
}
