#include "stdafx.h"
#include "XMyActionNPCInteraction.h"
#include "XEventID.h"
#include "XController.h"
#include "XModuleMyControl.h"
#include "XCameraManager.h"
#include "XPost_Interaction.h"
#include "XInteractionInfo.h"


XMyActionNPCInteraction::~XMyActionNPCInteraction()
{
	// 이벤트 없이강제로 끝날때를 생각해서 한번 더 검사해 본다.(https://martini.neowiz.com/issues/2618)
	XGetInteractionInfo().m_bNPCInteractionState = false;
	
	ForceInteractionEnd();
}

bool XMyActionNPCInteraction::CheckEnterable( void* pParam )
{
	m_pOwner->StopRun();

	return true;
}

XEventResult XMyActionNPCInteraction::Event( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_VIRTUAL_KEY_PRESSEDONCE:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			switch (nVirtualKey)
			{
			case VKEY_FORWARD:
			case VKEY_BACKWARD:
			case VKEY_LEFT:
			case VKEY_RIGHT:

				if(m_bEndPost == false)
					XPostNPCInteraction_End();

				return MR_TRUE;
			}
		}
		break;
	case XEVTL_MYACTION_NPC_INTERACTION_END:
		{
			InteractionEnd();
			m_bEndPost = true;
		}
	}

	return MR_FALSE;
}

void XMyActionNPCInteraction::Enter( void* pParam/*=NULL*/ )
{
	// NPC Interaction에만 해당합니다.

	if ( global.script)	global.script->GetGlueGameEvent().OnGameEvent( "ACTION", "INTERACTION", 1);

	// 타겟 가져오기
	MUID uidTarget = *(MUID*)pParam;

	// 인터렉션 카메라
	if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
	{
		XCameraBackViewController* pBackViewCamera = global.camera->GetCameraController<XCameraBackViewController>();
		if (pBackViewCamera)
		{
			pBackViewCamera->StartInteraction(uidTarget);
		}
	}

	XGetInteractionInfo().m_bNPCInteractionState = true;
	m_bEndStart = false;
	m_bEndPost = false;

//	MCursorSystem::Set("defCursor");
}

void XMyActionNPCInteraction::Exit()
{
	if ( global.script)	global.script->GetGlueGameEvent().OnGameEvent( "ACTION", "INTERACTION", 0);

	XGetInteractionInfo().m_bNPCInteractionState = false;

	// 이벤트 없이강제로 끝날때를 생각해서 한번 더 검사해 본다.
	InteractionEnd();
}

void XMyActionNPCInteraction::InteractionEnd()
{
	if(m_bEndStart)
		return;

	// 인터렉션 카메라
	if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
	{
		XCameraBackViewController* pBackViewCamera = global.camera->GetCameraController<XCameraBackViewController>();
		if (pBackViewCamera)
		{
			pBackViewCamera->EndInteration();
			m_bEndStart = true;
			m_pOwner->DoActionIdle();
		}
	}
}

bool XMyActionNPCInteraction::OnSpecialAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand )
{
	// 종료 중이라면... 이동가능하다.
	return IsMovable();
}

bool XMyActionNPCInteraction::IsMovable()
{
	return m_bEndStart;

}

void XMyActionNPCInteraction::ForceInteractionEnd()
{
	if(m_bEndStart)
		return;

	// 인터렉션 카메라
	if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
	{
		XCameraBackViewController* pBackViewCamera = global.camera->GetCameraController<XCameraBackViewController>();
		if (pBackViewCamera)
		{
			pBackViewCamera->ForceEndInteration();
			m_bEndStart = true;
		}
	}
}
