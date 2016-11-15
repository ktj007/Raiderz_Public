#include "stdafx.h"
#include "XBPartNonPlayer.h"
#include "XModuleBParts.h"
#include "XModuleEntity.h"
#include "XModuleMovable.h"
#include "XModuleNonControl.h"
#include "XModuleMotion.h"
#include "XModuleCollision.h"
#include "XModuleUI.h"
#include "CSNPCInfoHelper.h"
#include "XGameState.h"

#include "MPhysX4R3/MPxDynamicActorNodeTC.h"

MImplementRTTI(XBPartNonPlayer, XNonPlayer);

#include "RSceneManager.h"
#include "RDebugPassRenderer.h"

XBPartNonPlayer::XBPartNonPlayer( MUID uid, UIID nUIID )
:XNonPlayer(uid, nUIID)
, m_fScale(1.0f)
, m_bOnlyOne(false)
, m_vServerPos(vec3::ZERO)
, m_matRender(MMatrix::IDENTITY)
, m_bBPartLootable(false)
{

}

XBPartNonPlayer::~XBPartNonPlayer()
{

}

void XBPartNonPlayer::InBPartNPC( XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_PBART* pBPartNPCCacheInfo, bool bAppearEffect /*= true*/, bool bLoadingAsync /*= true*/, MMatrix* pMatRender /*= NULL*/ )
{
	m_bLoadingAsync = bLoadingAsync;

	XNPCInfo* pNPCTempInfo = info.npc->Get(XCONST::BPART_DEFAULT_NPC_ID);
	if (pNPCTempInfo == NULL)
		return;

	if (GetEntityType() == ETID_NPC)
	{
		TD_UPDATE_CACHE_NPC tempInfo;
		tempInfo.nStatusFlag = pBPartNPCCacheInfo->nStatusFlag;

		InitNPC(pNPCTempInfo, &tempInfo);
	}

	m_fScale = pNPCInfo->fScale;

	vec3 vStartDir;
	if (m_pModuleEntity && 
		m_pModuleBParts)
	{
		m_pModuleEntity->SerializeNPC(pNPCTempInfo, m_bLoadingAsync);

		wstring strBPartsActorNodeName = m_pModuleBParts->GetBreakablePartsActorNodeName(pBPartNPCCacheInfo->nBPartID);
		string strActorNodeName = MPxDynamicActorNodeTC::GetFragActorNodeName(MLocale::ConvUTF16ToAnsi(strBPartsActorNodeName).c_str(), pBPartNPCCacheInfo->nBPartFlag);
		if(pBPartNPCCacheInfo->nBPartFlag == -1)
		{
			strActorNodeName = MLocale::ConvUTF16ToAnsi(strBPartsActorNodeName).c_str();
		}

		m_pModuleEntity->SerializeBPartNPC(pNPCInfo, MLocale::ConvAnsiToUTF16(strActorNodeName.c_str()), pNPCTempInfo->strMeshName, m_bLoadingAsync);


		vec3 pos = pBPartNPCCacheInfo->vPos;

		if (CSNPCInfoHelper::IsUsingGravityNPC(pNPCInfo->nNpcType))
		{
			if ( gg.currentgamestate  &&  gg.currentgamestate->GetWorld())
				pos.z = gg.currentgamestate->GetWorld()->GetGroundZ(pos, NPC_PICKING_MAP_HEIGHT);	// 일단 서버에서 충돌처리가 없으므로..
		}
		else
		{
			if (m_pModuleMovable)
			{
				bool bUsingGravity = false;
				m_pModuleMovable->SetInitialFactors(bUsingGravity);
			}
		}

		// by pok, InGame
		if(m_pModuleNonControl)
		{
			vStartDir = vec3::AXISY;

			m_pModuleNonControl->OnEnterGame(pos, vStartDir, this);

			if(pMatRender)
			{
				m_matRender = *pMatRender;

				//MMatrix matTest(MMatrix::IDENTITY);
				//matTest.SetLocalMatrix(pos, vStartDir, vec3::AXISZ);
				//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( matActorWorld, 6, 60.f);
				//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( *pMatRender, 5, 60.f, 0xffffff00);
				//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( matTest, 7, 60.f, 0xffff0000);
			}
			else
			{
				m_pModuleEntity->SetScale(m_fScale);
				m_strMeshName = MLocale::ConvUTF16ToAnsi(pNPCTempInfo->strMeshName);
			}
		}
	}

	if (m_pModuleCollision)
	{
		m_pModuleCollision->InitCol(pNPCInfo->pMeshInfo);
	}

	SetNPCIcon();
}

void XBPartNonPlayer::OnEvent( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_MOTION_PLAYONCE_NPC_DIE:
		{
			if(m_matRender == MMatrix::IDENTITY && m_strMeshName.empty() == false)
			{
				RActorNode* pActorNode = GetActor()->GetActorNode(m_strMeshName.c_str());
				if(pActorNode)
				{
					GetActor()->SetLocalAABB(NULL);
					GetActor()->RefreshInteractionAABB();
					GetActor()->UpdateTransform();
					GetActor()->UpdateBoundingVolume();

					vec3 vTe = GetActor()->GetAABB().GetCenter() - GetPosition();
					float fOffsetHeight = GetActor()->GetAABB().GetCenter().z - GetActor()->GetAABB().minz;

					//vTe.z -= fOffsetHeight;

					vec3 vStart	= GetPosition() + RVector(0,0,100);
					vec3 vEnd	= GetPosition() + RVector(0,0,-1000);
					vec3 vOutPos;
					vec3 vOutNor;
					vec3 vResultNor = vec3::AXISZ;
					vec3 vResultDir = -vec3::AXISY;

					if(m_pModuleCollision->PickRayToTerrain(vStart, vEnd, vOutPos, vOutNor))
					{
						vOutNor.Normalize();
					//	vResultNor		= vOutNor.Normalize();
					//	vec3 vCross		= vResultDir.CrossProduct(vOutNor);
					//	vResultDir		= vOutNor.CrossProduct(vCross);
					}

					vec3 vLastMove = vOutNor * (fOffsetHeight - 10.0f);

					m_matRender.SetLocalMatrix(GetPosition()-vTe+vLastMove, vResultDir, vResultNor);
					//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis(GetActor()->GetLocalTransform(), 10, 60.f, 0xffff0000);
				}
			}


			//if(m_matRender != MMatrix::IDENTITY)
			//else 
			{
				MMatrix matTest;
				matTest.SetLocalMatrix(GetPosition(), GetDirection(), GetUpvector());
				//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( matTest, 7, 60.f, 0xff0000ff);
				//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( GetActor()->GetLocalTransform(), 6, 60.f);

				vec3 vRenderDir(vec3(m_matRender._21, m_matRender._22, m_matRender._23));
				vec3 vRenderUp(vec3(m_matRender._31, m_matRender._32, m_matRender._33));

				MMatrix matWorldInverse;
				MMatrix matActorWorld(MMatrix::IDENTITY);
				matActorWorld.SetLocalMatrix(GetPosition(), vRenderDir, vRenderUp);

				matActorWorld.GetInverse(&matWorldInverse);
				MMatrix matResult = m_matRender * matWorldInverse;

				m_pModuleEntity->SetScale(m_fScale);

				m_vRenderPos = matResult.GetTranslation();
				m_vRenderDir = -vRenderDir;
				m_vRenderUp = vRenderUp;

				m_pModuleEntity->SetRenderPosition(m_vRenderPos);
				m_pModuleEntity->SetRenderUpDir(m_vRenderUp, m_vRenderDir);
			}

			GetActor()->SetLocalAABB(NULL);

			GetActor()->RefreshInteractionAABB();
			GetActor()->UpdateTransform();
			GetActor()->UpdateBoundingVolume();


			//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( GetActor()->GetWorldTransform(), 7, 60.f, 0xffff00ff);

			//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( GetActor()->GetLocalTransform(), 7, 60.f, 0xff0000ff);
			//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( GetActor()->GetWorldTransform(), 8, 60.f, 0xff00ff00);

			float fLen = (m_vServerPos - GetPosition()).Length();

			// 루팅 범위를 넘어셨다면... 루팅이 되지 않게 한다.
			if(GAME_INTERACTION_BPART_LOOTING_DISTANCE < fLen)
			{
				m_bLootable = false;
			}
		}
		break;
	}

	XObject::OnEvent(msg);
}

void XBPartNonPlayer::Despawn()
{
	GetActor()->SetVisible(false);
	gg.omgr->ReserveDelete(GetUID(), 0.0f);
}
