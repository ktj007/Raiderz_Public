#include "StdAfx.h"
#include "XCutScenePlayer.h"
#include "XModuleCutSceneControl.h"
#include "XModuleMovable.h"
#include "XMovableFactor.h"
#include "XTalentHelper.h"
#include "XCutScenePlayerActionNA.h"
#include "XCutScenePlayerActionTalent.h"


#ifdef _DEBUG_SHOW_INTERPOLATION_TARGET_INFO_NPC
#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"
#endif

XCutScenePlayer* XModuleCutSceneControl::GetOwner()
{ 
	return AsCutScenePlayer(m_pOwner);
}

XModuleCutSceneControl::XModuleCutSceneControl(XObject* pOwner) 
: XModuleActorControl(pOwner)
{
	m_pActionFSM = new XCutScenePlayerActionFSM(this);
}

XModuleCutSceneControl::~XModuleCutSceneControl(void)
{
	m_pActionFSM->DeleteAll();
	SAFE_DELETE(m_pActionFSM);
}

void XModuleCutSceneControl::OnInitialized()
{
	XModuleActorControl::OnInitialized();
	InitStates();
}

void XModuleCutSceneControl::InitStates()
{
	m_pActionFSM->AddState(new XCutScenePlayerActionNA(this));
	m_pActionFSM->AddState(new XCutScenePlayerActionTalent(this));

	m_pActionFSM->ChangeState(CUTSCENEPLAYER_ACTION_STATE_NA);
}

void XModuleCutSceneControl::DoAction( CUTSCENEPLAYER_ACTION_STATE nState, void* pParam/*=NULL*/ )
{
	if (m_pActionFSM->GetCurrentStateID() != nState)
	{
		m_pActionFSM->ChangeState(nState, pParam);
	}
}

bool XModuleCutSceneControl::UsableTalent(int nTalentID)
{
	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo( nTalentID, m_pOwner);
	if(pTalentInfo == NULL)
	{
		return false;
	}

	if(m_pOwner->IsPlayer())
	{
		TALENT_STYLE nMyPlayerStyle = TS_NONE;
		WEAPON_TYPE nWeaponType = gvar.MyInfo.EquipmentSlot.GetCurrWeaponType();
		
		if(nWeaponType == WEAPON_1H_SLASH || nWeaponType == WEAPON_1H_BLUNT || nWeaponType == WEAPON_1H_PIERCE )
			nMyPlayerStyle = TS_DEFENDER;
		else if(nWeaponType == WEAPON_TWO_HANDED || nWeaponType == WEAPON_2H_BLUNT )
			nMyPlayerStyle = TS_BERSERKER;
		else if(nWeaponType == WEAPON_STAFF)
			nMyPlayerStyle = TS_CLERIC;

		if(pTalentInfo->m_nStyle == TS_COMMON || pTalentInfo->m_nStyle == nMyPlayerStyle)
			return true;
		else
			return false;
	}
	else if(m_pOwner->IsNPC())
	{
		XCutScenePlayer* pNPC = GetOwner();
		XNPCInfo* pNPCInfo = pNPC->GetInfo(true);

		if (pTalentInfo->m_setNPCID.find(pNPCInfo->nID) == pTalentInfo->m_setNPCID.end())
		{
			return false;
		}
	}

	return true;
}

void XModuleCutSceneControl::OnUseTalent(int nTalentID, vec3& vPos, vec3& vDir)
{
	if(UsableTalent(nTalentID) == false)
	{
		return;
	}

	DoAction(CUTSCENEPLAYER_ACTION_STATE_TALENT);

	// 거리차가 너무 많이 난 경우엔 워프시킨다.
	vec3 pos_diff = vPos.DistanceTo(GetOwner()->GetPosition());

	if ( pos_diff > PROCESS_MOVE_EXCEPTION_DISTANCE)
	{
		GetOwner()->SetPosition( vPos);
	}

	// 이동/회전 속도를 결정
	XMovableFactor::SInterpolation param = m_pModuleMovable->GetMovableFactor().INTERPOLATION;
	float fMovSpeed	= param.m_fMoveSpeed;
	float fRotSpeed = 0.0f;
	unsigned int nBoost	= BOOST_MOVE | BOOST_ROTATE; 
	
	//CutScene의 경우 MyPlayer, NPC 에 상관없이 Talent 이동, 회전을 적용한다.
	//float fRotSpeed		= GetOwner()->GetInfo(true)->fRotateSpeed;	 // param.m_fRotateSpeed;
	//unsigned int nBoost	= BOOST_MOVE;

	//XCutScenePlayer* pNPC = GetOwner();
	//XNPCInfo* pNPCInfo = pNPC->GetInfo(true);
	//if(pNPCInfo && pNPCInfo->IsRotationTalent(nTalentID))
	//{
	//	// 회전 탤런트일 경우 회전 속도를 다시 계산함
	//	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID);

	//	float angle = m_pOwner->GetDirection().AngleToXY( vDir);
	//	fRotSpeed = abs( RadianToDegree( angle) / pTalentInfo->m_ActAnimationTime.GetTime());
	//}
	//else
	//	nBoost |= BOOST_ROTATE;

	fMovSpeed = max( fMovSpeed, 50.0f);			// 최소 이동 속도값 설정
	fRotSpeed = max( fRotSpeed, 20.0f);			// 최소 회전 속도값 설정


	XMovableGoParam* pGoParam = new XMovableGoParam(XMovableGoParam::E_GO_INTERPOLATION);
	pGoParam->Set( vPos, vDir, fMovSpeed, fRotSpeed, nBoost, MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_USING_TALENT, true);
	pGoParam->SetGoInterpolationForce( true);
	m_pModuleMovable->TriggerGo( pGoParam);
	delete pGoParam;

	XModuleActorControl::OnUseTalent(nTalentID, vPos, vDir);
}