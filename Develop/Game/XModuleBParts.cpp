#include "stdafx.h"
#include "XModuleBParts.h"
#include "XEventID.h"
#include "XModuleEntity.h"
#include "XCharacter.h"
#include "XNonPlayer.h"
#include "XEffectManager.h"
#include "XGame.h"
#include "RMesh.h"

#include "MTime.h"

#include "XModuleCollision.h"

//#include "RSceneManager.h"
//#include "RDebugPassRenderer.h"

// 물리 시뮬레이션이 적용되는 브레이커블 파츠
#define _USE_PHX_BPARTS
#include "MPhysX4R3/MPxDynamicActorNodeTC.h"

//////////////////////////////////////////////////////////////////////////
#define BREAKABLEPARTS_BASIC_NAME			L"b_parts_"
#define BREAKABLEPARTS_BASIC_EFFECT			L"ef_bparts"
//////////////////////////////////////////////////////////////////////////
XModuleBParts::XModuleBParts( XObject* pOwner/*=NULL*/ )
: XModule(pOwner)
{
	m_BPartActorManager.Init(pOwner);

	m_vForceDir[UBD_FRONT]		= vec3(0, -1, 0);
	m_vForceDir[UBD_BACK]		= vec3(0, 1, 0);
	m_vForceDir[UBD_LEFT]		= vec3(1, 0, 0);
	m_vForceDir[UBD_RIGHT]		= vec3(-1, 0, 0);
}

XModuleBParts::~XModuleBParts(void)
{

}
void XModuleBParts::OnInitialized()
{

}

void XModuleBParts::OnSubscribeEvent()
{

}

XEventResult XModuleBParts::OnEvent( XEvent& msg )
{
	return MR_FALSE;
}

void XModuleBParts::OnUpdate( float fDelta )
{
	PFC_THISFUNC;
	// 브레이커블 파츠 대기가 있는가...
	XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	if(m_WaitBrokenParts.bWait)
	{
		if( pModuleEntity->GetActor()->GetUsable() )
		{
			for (set<int>::iterator itr = m_WaitBrokenParts.setParts.begin(); itr != m_WaitBrokenParts.setParts.end(); ++itr)
			{
				int nIndex = *itr;
				SetBrokenParts(nIndex);
			}
			m_WaitBrokenParts.Init();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// BPart NPC 생성
	m_BPartActorManager.Update(fDelta);
}

void XModuleBParts::SetBreakablePartsEffect(RActorNode* pBreakablePartsNode, XObject* pObject, wstring strEffectName)
{
	if(pBreakablePartsNode != NULL)
	{
		std::wstring strEffectNodeName( MLocale::ConvAnsiToUTF16(pBreakablePartsNode->GetNodeName().c_str()) + wstring(L"_effect"));
		MMatrix matWorld;
		RActorNode* pEffectPositionNode = pObject->GetActor()->GetActorNodeTransform(strEffectNodeName.c_str(), matWorld, E_TS_WORLD, true);
		if(pEffectPositionNode == NULL)
			pEffectPositionNode = pObject->GetActor()->GetActorNodeTransform(MLocale::ConvAnsiToUTF16(pBreakablePartsNode->GetNodeName().c_str()).c_str(), matWorld, E_TS_WORLD, true);

		//// 이펙트 생성
		//// [2007/11/16] isnara
		//// ActorNode를 지우게 되므로 AttachActorNodeEffect()에서 CreatePos()로 변경한다.
		//RMatrix matLocal;
		//RVector3 vPos;
		//pActor->GetActorNodePos(pEffectPositionNode->GetNodeName().c_str(), vPos, E_TS_WORLD);

		//matLocal.SetLocalMatrix(vPos, vec3(1,0,0), vec3(0,0,1));
		//MUID uid = global.emgr->AddXEffect(pEffectName, NULL, NULL, matLocal);

		EFFECT_CREATE_DATA effectCreateData;
		effectCreateData.strEffectName	= strEffectName;
		effectCreateData.uidActor		= pObject->GetUID();
		effectCreateData.matPostMatrix.SetTranslation(matWorld.GetTranslation());

		global.emgr->AddXEffect(effectCreateData);
	}
}

void XModuleBParts::SetBreakableParts(int nIndex, float fXYForce, float fZForce)
{
	_SetBreakableParts(nIndex, false, MUID::ZERO, fXYForce, fZForce);
}

void XModuleBParts::SetBreakableParts(MUID uidAttacker, int nIndex)
{
	_SetBreakableParts(nIndex, true, uidAttacker);
}

void XModuleBParts::_SetBreakableParts( int nIndex, bool bEffect, MUID uidAttacker/*=MUID::ZERO*/, float fXYForce/*=-1.0f*/, float fZForce/*=-1.0f*/, TD_USABLE_BPART* pUsableBpart /*= NULL*/, int nUsableBpartCount /*= 0*/ )
{
	XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	XCharacter* pActor = pModuleEntity->GetActor();
	if(pActor == NULL)
		return;

	// 파괴시 메쉬 로딩이 다 되어 있어야 한다.
	if(pActor->GetUsable())
	{
		// 파일명
		wstring strNodeName = GetBreakablePartsActorNodeName(nIndex);
		RActorNode* pBreakablePartsNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(strNodeName).c_str());

		// 브레이커블 파츠가 액터에 존재하지 않는 경우
		if (pBreakablePartsNode == NULL)
		{
			// by pok. 파괴된 bparts에 대해서도 파괴요청을 다시할 수 있게 되어서 로그를 삭제합니다.
			//mlog0("이미 파괴되어 존재하지 않는 브레커블 파츠에 대해 파괴를 명령했습니다.\n");
			return;
		}

		// 브레이커블 파츠가 시뮬레이션중일 경우
		MPxDynamicActorNodeTC* pDynamicActorNodeTC = pModuleEntity->GetTransformController<MPxDynamicActorNodeTC>();
		MPxTcDynamicActorNodesGroup* pDynamicActorNodesGroup = static_cast<MPxTcDynamicActorNodesGroup*>(pDynamicActorNodeTC->GetDynamicActorNodesGroup(nIndex));
		if (pDynamicActorNodesGroup)
		{
			// by pok. 파괴된 bparts에 대해서도 파괴요청을 다시할 수 있게 되어서 로그를 삭제합니다.
			//mlog0("파괴해서 시뮬레이션 중인 브레커블 파츠에 대해 다시 파괴를 요청했습니다\n");
			return;
		}

		// 성인 등급이 아니면 파츠를 삭제하지 않는다
		if(CheckMature(nIndex) == false)
			return;

		if (bEffect)
		{
			// 브레커블 파츠 이팩트
			//MTimeChecker tc;
			//tc.Start();
			SetBreakablePartsEffect(pBreakablePartsNode, pModuleEntity->GetOwner(), BREAKABLEPARTS_BASIC_EFFECT);
			//unsigned int nElapsed = tc.Check();
			//if (nElapsed > 5)
			//{
			//	mlog("[b_parts] %u ms : making BreakablePart Effect - overtime than 5ms\n", nElapsed);
			//}
			//mlog("1. %u ms : 브레이커블 파츠 이팩트\n", nElapsed);
		}

		// 시뮬레이션 준비
		//tc.Start();
		std::vector<std::string> vList;
		if (pDynamicActorNodeTC->MakeAndPrepareSimulationActorNodes(pBreakablePartsNode, vList) == false)
		{
			// 시뮬레이션 가능하지 않는 브레커블 파츠
			SetBrokenParts(nIndex);
			return;
		}

		// BPart 준비
		m_BPartActorManager.AddIndex(nIndex);

		std::vector<std::string> vBPartList;
		m_BPartActorManager.GetBPartActorNodeList(pBreakablePartsNode, pUsableBpart, nUsableBpartCount, vBPartList, vList);

		// 시뮬레이션을 할 브레커블 파츠 그룹 만들기
		pDynamicActorNodesGroup = static_cast<MPxTcDynamicActorNodesGroup*>(pDynamicActorNodeTC->CreateDynamicActorNodesGroup(nIndex));
		_ASSERT(pDynamicActorNodesGroup);

		// 브레이커블 파츠 위치 변경
		ChangeTransformForBPartActorNode(vList);

		// 시뮬레이션 force
		vec3 vForce, vForceDir = vec3::AXISZ;
		vec3 vPartsPos(pBreakablePartsNode->GetWorldPosition());

		if (uidAttacker != MUID::ZERO)
		{
			XActor* pObjAttacker = gg.omgr->FindActor(uidAttacker);
			if (pObjAttacker)
			{
				vForceDir = vPartsPos - pObjAttacker->GetPosition();
				vForceDir.z = 0.f;
				vForceDir.Normalize();
			}
		}
		else
		{
			vForceDir = vPartsPos - m_pOwner->GetPosition();
			vForceDir.z = 0.f;
			vForceDir.Normalize();
		}

		vForce = MakeUpwardForce(fXYForce, fZForce, pModuleEntity, vForceDir);

		bool bSetForce = true;
		if(pUsableBpart)
			bSetForce = false;

		//mlog("2. %u ms : 물리노드 시뮬레이션 준비 및 그룹만들기\n", tc.Check());

		//////////////////////////////////////////////////////////////////////////
		//GSTAR용 하드코딩
		if (pActor->GetMeshName().find("rengot_temple_entrance.elu") != std::string::npos)
		{
			vPartsPos = pActor->GetAABB().GetCenter() + pActor->GetPosition();
			vForce = pActor->GetDirection();
			vForce.z = 1.f;
			vForce.Normalize();
			vForce *= 700.f;
		}
		//////////////////////////////////////////////////////////////////////////

		// start simulation
		pDynamicActorNodesGroup->StartPhysicSimulation(vList, vForce, vPartsPos, MLocale::ConvUTF16ToAnsi(strNodeName).c_str(), bSetForce);

		//////////////////////////////////////////////////////////////////////////
		// set BPart simulation info
		if(pUsableBpart)
		{
			int nBpartGroupID = global.phx->GetBPartGroupID();

			pDynamicActorNodesGroup->SetMaterial(global.phx->GetMaterialIdx_BPart());
			pDynamicActorNodesGroup->SetGroupId(vBPartList, nBpartGroupID);

			for(int iBPart = 0; iBPart < nUsableBpartCount; ++iBPart)
			{
				vec3 vBPartForce;
				TD_USABLE_BPART* pBPartInfo = &pUsableBpart[iBPart];
				vec3 vBPartForceDir = (pBPartInfo->vPos - vPartsPos).Normalize();
				//vBPartForceDir.z = 0.f;
				//vBPartDir.Normalize();
				vBPartForce = (pBPartInfo->vPos - vPartsPos) * 1.2f;
				vBPartForce.z = (pBPartInfo->vPos - vPartsPos).Length() * 0.8f;

				//vBPartForce = MakeUpwardForce(fXYForce, fZForce, pModuleEntity, vBPartForceDir);

				pDynamicActorNodesGroup->SetForce(vBPartList[iBPart], vBPartForce, vPartsPos);
			}

			for(std::vector<std::string>::iterator itList = vList.begin(); itList != vList.end(); ++itList)
			{
				bool bExist = false;
				for(std::vector<std::string>::iterator itBPart =  vBPartList.begin(); itBPart != vBPartList.end(); ++itBPart)
				{
					if(*itList == *itBPart)
					{
						bExist = true;
						break;
					}
				}

				if(bExist == false)
				{
					pDynamicActorNodesGroup->SetForce(*itList, vForce, vPartsPos);
				}
			}
		}
		
		//////////////////////////////////////////////////////////////////////////
		// start sound
		XModuleCollision* pModuleCollision = m_pOwner->GetModuleCollision();
		if (pModuleCollision && pModuleCollision->GetMeshInfo() && nIndex >= 1 && nIndex <= 4)
		{
			const tstring& start_sound = pModuleCollision->GetMeshInfo()->m_strBPartsStartSound[nIndex-1];
			if (false == start_sound.empty())
			{
				if ( global.sound)
					global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(start_sound.c_str()).c_str(), NULL, 0, m_pOwner->GetUID());
			}
		}
	}
	else
	{
		SetBrokenParts(nIndex);
	}
}


void XModuleBParts::SetBrokenParts(int nIndex)
{
	// 성인 등급이 아니면 파츠를 삭제하지 않는다.
	if(CheckMature(nIndex) == false)
	{
		return;
	}

	XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	XCharacter* pActor = pModuleEntity->GetActor();
	if(pActor == NULL)
		return;

	// 파괴시 메쉬 로딩이 다 되어 있어야 한다.
	if( pActor->GetUsable() )
	{
		// 파일명
		wstring strNodeName = GetBreakablePartsActorNodeName(nIndex);

		// 파괸된 것은 그냥 삭제
		// pActor->DeleteActorNode(chNodeName);
		// 삭제하지 않고 가려주기
		RActorNode* pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(strNodeName).c_str());
		if (pActorNode)
			pActorNode->SetVisible(false);
	}
	else
	{
		// 메시 로딩이 끝났때까지 기다린다.
		m_WaitBrokenParts.bWait = true;
		m_WaitBrokenParts.setParts.insert(nIndex);
	}
}

bool XModuleBParts::CheckMature(int nIndex)
{
	if (nIndex > XNPCInfo::BREKABLE_PARTS_MATURE) nIndex = 1;

	// NPC인 경우에만 성인 등급을 체크한다. (NPC만 파츠가 부서지기 때문이다)
	if(m_pOwner != NULL &&
		m_pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNPC = AsNPC(m_pOwner);
		if(pNPC &&
			pNPC->GetInfo())
		{
			if(pNPC->GetInfo()->bPartsMature[nIndex - 1] == false ||			// bPartsMature이 false면 연령과 관계없이 무조건 보여준다.
				(pNPC->GetInfo()->bPartsMature[nIndex - 1] == true &&			// bPartsMature이 true면 연령이 18세 이상만 보여준다.
					gg.game->Check18Over()))
			{
				return true;
			}
		}
	}

	return false;
}

void XModuleBParts::BreakPartsRecovery()
{
	XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	XCharacter* pActor = pModuleEntity->GetActor();
	if(pActor == NULL)
		return;

	for (int nIndex = 0; nIndex < 4;nIndex++)
	{
		// 파일명
		wstring strNodeName = GetBreakablePartsActorNodeName(nIndex);

		// 삭제하지 않고 가려주기
		RActorNode* pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(strNodeName).c_str());
		if (pActorNode)
			pActorNode->SetVisible(true);




		

		int nCount = 0;

		//while (true)
		//{
		//	std::string strActorNodeName = MPxDynamicActorNodeTC::GetFragActorNodeName(MLocale::ConvUTF16ToAnsi(strNodeName).c_str(), nCount);
		//	RMeshNode* pMeshNodeFrag = pActor->m_pMesh->GetNode(strActorNodeName.c_str());
		//	if (pMeshNodeFrag == NULL)
		//	{
		//		std::string strImActorNodeName(strActorNodeName);
		//		strImActorNodeName += "_im";
		//		pMeshNodeFrag = pActor->m_pMesh->GetNode(strImActorNodeName.c_str());
		//	}

		//	if (pMeshNodeFrag)
		//	{
		//		pActor->AddMeshNode(pMeshNodeFrag, NULL, strActorNodeName.c_str());
		//		++nCount;
		//	}
		//	else
		//	{
		//		break;
		//	}
		//}
	}
}

void XModuleBParts::ClearForReload()
{
	XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL) return;

	MPxDynamicActorNodeTC* pDynamicActorNodeTC = pModuleEntity->GetTransformController<MPxDynamicActorNodeTC>();
	if (pDynamicActorNodeTC)
	{
		pDynamicActorNodeTC->ClearForReload();
	}
}

std::wstring XModuleBParts::GetBreakablePartsActorNodeName( int nIndex )
{
	wchar_t chNodeName[257] = {0,};
	swprintf(chNodeName, L"%s%02d", BREAKABLEPARTS_BASIC_NAME, nIndex);

	return chNodeName;
}

void XModuleBParts::SetBreakablePartEx( MUID uidAttacker, int nIndex, TD_USABLE_BPART* pUsableBpart, int nUsableBpartCount )
{
	_SetBreakableParts(nIndex, true, uidAttacker, -1, -1, pUsableBpart, nUsableBpartCount);
}

void XModuleBParts::ChangeTransformForBPartActorNode( std::vector<std::string>& _rActorNodeName )
{
	// 파츠 ActorNode가 땅속에 파묻혀서 홈런이 되는걸 막기 위한 코드
	// ActorNode 위치를 땅 위로 올리는 것이다.
	for(std::vector<std::string>::iterator it = _rActorNodeName.begin(); it != _rActorNodeName.end(); ++it)
	{
		RActorNode* pActorNode = m_pOwner->GetActor()->GetActorNode(it->c_str());
		if(pActorNode && pActorNode->m_pMeshNode)
		{
			MMatrix matWorldActorNode = pActorNode->GetWorldTransform();

			vec3* pBox = new vec3[8];
			pActorNode->m_pMeshNode->m_boundingBox.GetPoints(pBox);

			float fColGap = 0;
			for(int i = 0; i < 8; ++i)
			{
				vec3 vBoxPos;
				matWorldActorNode.TransformVect(pBox[i], vBoxPos);
				float fGroundZ = gg.currentgamestate->GetWorld()->GetGroundZ(vBoxPos);
				
				if(vBoxPos.z < fGroundZ &&
					fColGap < (fGroundZ - vBoxPos.z))
				{
					fColGap = fGroundZ - vBoxPos.z;
				}
			}

			SAFE_DELETE_ARRAY(pBox);

			if(fColGap > 0)
			{
				matWorldActorNode._43 += fColGap;

				MMatrix matParentNode(MMatrix::IDENTITY);
				MMatrix matParentNodeinverse(MMatrix::IDENTITY);

				RActorNode* pParentNode = pActorNode->GetParentActorNode();
				if(pParentNode)
				{
					matParentNode = pParentNode->GetWorldTransform();
				}
				else
				{
					matParentNode = m_pOwner->GetActor()->GetWorldTransform();
				}

				matParentNode.GetInverse(&matParentNodeinverse);

				pActorNode->SetTransform(matWorldActorNode * matParentNodeinverse);
			}
		}
	}
}

void XModuleBParts::SetLootableBPartNPC( MUID uid, bool bLootable )
{
	m_BPartActorManager.SetLootableBPartNPC(uid, bLootable);
}

vec3 XModuleBParts::MakeUpwardForce( float fXYForce, float fZForce, XModuleEntity * pModuleEntity, vec3 vForceDir )
{
	vec3 vForce = vec3::ZERO;

	MPxDynamicActorNodeTC* pDynamicActorNodeTC = pModuleEntity->GetTransformController<MPxDynamicActorNodeTC>();
	if(pDynamicActorNodeTC == NULL)
		return vForce;

	if (fXYForce < 0.0f && fZForce < 0.0f)
	{
		pDynamicActorNodeTC->MakeUpwardRandomForce(vForce, &vForceDir);
	}
	else
	{
		pDynamicActorNodeTC->MakeUpwardForce(vForce, &vForceDir, fXYForce, fZForce);
	}

	return vForce;
}
