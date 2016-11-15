#include "stdafx.h"
#include "XCutScenePlayer.h"
#include "XCutSceneObject.h"

#include "XModuleEntity.h"
#include "XModuleMovable.h"
#include "XModuleMotion.h"
#include "XModuleTalent.h"
#include "XModuleCutSceneControl.h"
#include "XModuleEffect.h"
#include "XGameTransformControllerManager.h"

#include "XNPCInfo.h"
#include "XTalentHelper.h"

MImplementRTTI(XCutScenePlayer, XObject);

XCutScenePlayer::XCutScenePlayer(MUID uid, const std::wstring& _rStrActorType)
:XActor(uid, 0), m_pNPCInfo(NULL), m_nMode(0) 
{
	m_nTypeID = ETID_CUTSCENE;
	m_nNpcId = XCutSceneObjectActor::GetActorIdFromEntityInfo(_rStrActorType);
	m_pModuleCutSceneControl		= new XModuleCutSceneControl(this);
}

XCutScenePlayer::~XCutScenePlayer()
{
	m_pNPCInfo = NULL;
}

XNPCInfo* XCutScenePlayer::GetInfo(bool bCurrentMode)
{
	return m_pNPCInfo;
}

void XCutScenePlayer::RegisterModules()
{
	XActor::RegisterModules();

	m_pModuleCutSceneControl->Initialize();
	m_Modules.AddModule(m_pModuleCutSceneControl, true);
}

void XCutScenePlayer::UnregisterModules()
{
	m_Modules.RemoveModule(m_pModuleCutSceneControl);
	m_pModuleCutSceneControl->Finalize();

	XActor::UnregisterModules();
}

bool XCutScenePlayer::OnCreate()
{
	// 사용 할 이펙트 종류 등록
	if(m_pModuleEffect)
	{
		m_pModuleEffect->SetRegisterUseEffectID((1 << XEFT_ID_HEAD) | (1 << XEFT_ID_BUFF) | ( 1<< XEFT_ID_NPCICON) | (1 << XEFT_ID_TALENT) | (1 << XEFT_ID_SWORDTRAIL));
	}

	return true;
}

void XCutScenePlayer::SerializeCutScenePlayer()
{
	if (IsPlayer() == true)
	{
		// 일단 my player 만 존재
		m_pModuleEntity->SerializeMyPlayer(gvar.MyInfo.ChrInfo, &gvar.MyInfo.EquipmentSlot);
	}
	else
	{
		m_pNPCInfo = info.npc->Get(m_nNpcId);
		if (m_pNPCInfo == NULL)
			return;

		m_pModuleEntity->SetScale(m_pNPCInfo->fScale);
		m_pModuleEntity->SerializeNPC(m_pNPCInfo, false);
	}
}

void XCutScenePlayer::SetTransform( const vec3& vPos, const vec3& vDir, bool bUsingCollision )
{
	SetPosition(vPos);
	SetDirection(vDir);

	if (false == bUsingCollision)
	{
		_ASSERT(m_pModuleMovable);
		m_pModuleMovable->ForceStop(true);
	}
}

void XCutScenePlayer::SetAnimation( const std::wstring& _rAnimationName )
{
	if (_rAnimationName.find(L"motion_") == 0)
	{
		//motion_ : 7
		std::wstring strMotionName = _rAnimationName.substr(7);
		_ASSERT(m_pModuleMotion);

		m_pModuleMotion->ChangeMotion(strMotionName.c_str());
	}
	else
	{
		std::wstring strToSet(_rAnimationName);

		if(IsPlayer() == true && strToSet == L"idle")
		{//내 캐릭터의 기본 애니메이션 상태는 무기에 따라 자동 설정된다.
			strToSet = XTalentHelper::MakeFullPlayerTalentAniName(strToSet.c_str(), GetStance(), GetCurrWeaponType());
		}

		m_pModuleEntity->GetAnimationController()->SetAnimation(strToSet, true);
	}
}

void XCutScenePlayer::OnUseTalent( const int nTalent )
{
	m_pModuleCutSceneControl->OnUseTalent(nTalent, GetPosition(), GetDirection());
}

