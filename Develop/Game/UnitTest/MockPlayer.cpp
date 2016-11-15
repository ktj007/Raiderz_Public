#include "stdafx.h"
#include "MockPlayer.h"

void MockPlayer::SetMockModuleMotion( MockModuleMotion* pModuleMotion, bool bReg /*= false*/ )
{
	// 기존 모듈 삭제
	bool bRegist = false;
	XModule* pModule = m_Modules.GetModule(XMID_MOTION);
	if(pModule)
	{
		m_Modules.RemoveModule(m_pModuleMotion);
		m_pModuleMotion->Finalize();
		bRegist = true;
	}

	// 주소 복사
	SAFE_DELETE(m_pModuleMotion);
	m_pModuleMotion = m_pMockModuleMotion = pModuleMotion; 

	// 등록되어 있었던 것만 재등록한다.
	if(m_pModuleMotion && (bRegist || bReg))
	{
		m_pModuleMotion->Initialize();
		m_Modules.AddModule(m_pModuleMotion, true);
	}
}

void MockPlayer::SetMockModuleAction( MockModuleAction* pModuleAction, bool bReg /*= false*/ )
{
	// 기존 모듈 삭제
	bool bRegist = false;
	XModule* pModule = m_Modules.GetModule(XMID_ACTION);
	if(pModule)
	{
		m_Modules.RemoveModule(m_pModuleAction);
		m_pModuleAction->Finalize();
		bRegist = true;
	}
		
	// 주소 복사
	SAFE_DELETE(m_pModuleAction);
	m_pModuleAction = m_pMockModuleAction = pModuleAction;

	// 등록되어 있었던 것만 재등록한다.
	if(m_pModuleAction && (bRegist || bReg))
	{
		m_pModuleAction->Initialize();
		m_Modules.AddModule(m_pModuleAction, true);
	}
}

void MockPlayer::SetMockModuleCollision( MockModuleCollision* pModuleCollision, bool bReg /*= false*/ )
{
	// 기존 모듈 삭제
	bool bRegist = false;
	XModule* pModule = m_Modules.GetModule(XMID_COLLISION);
	if(pModule)
	{
		m_Modules.RemoveModule(m_pModuleCollision);
		m_pModuleCollision->Finalize();
		bRegist = true;
	}

	// 주소 복사
	SAFE_DELETE(m_pModuleCollision);
	m_pModuleCollision = m_pMockModuleCollision = pModuleCollision;

	// 등록되어 있었던 것만 재등록한다.
	if(m_pModuleCollision && (bRegist || bReg))
	{
		m_pModuleCollision->Initialize();
		m_Modules.AddModule(m_pModuleCollision, true);
	}
}
