#include "stdafx.h"
#include "TMeshHitEffectInfoViewer.h"
#include "Xmlseral.h"
#include "RRenderHelper.h"
#include "RFont.h"

#define MESH_COL_INFO_CAPULE_ID			"Capsule : {0}"

#define MESH_COL_CAPSULE_SELECT			0x2000FFFF
#define MESH_COL_CAPSILE_NONE_SELECT	0x20C0C0C0

//////////////////////////////////////////////////////////////////////////
void TMeshHitEffectInfoViewer::InitMeshHitEffectInfoViewer()
{
	// Viewer 내용 초기화
	GlobalObjects::g_pMainForm->HitEffect_treeView->ClearTreeView();
	GetMeshHitEffectInfoViewerPropertGrid()->SelectedObject = nullptr;

	GlobalObjects::g_SelInfo.m_nMeshHitEffectCapsuleID				= -1;
}

TreeView^ TMeshHitEffectInfoViewer::GetMeshHitEffectInfoViewerTreeNode()
{
	return GlobalObjects::g_pMainForm->HitEffect_treeView;
}

PropertyGrid^ TMeshHitEffectInfoViewer::GetMeshHitEffectInfoViewerPropertGrid()
{
	return GlobalObjects::g_pMainForm->HitEffect_propertyGrid;
}

void TMeshHitEffectInfoViewer::ChangeMeshHitEffectInfoViewer( System::String^ ModelName )
{
	InitMeshHitEffectInfoViewer();
	
	TMeshInfoMgr& MeshInfoMgr		= TMeshInfoMgr::GetInstance();
	TMeshHitEffectInfoMgr* MeshHitEffectInfoMgr = MeshInfoMgr.GetMeshHitEffectInfo();

	// 모델 Hit 정보를 가져온다.
	m_strModelName = (char*)MStringToCharPointer(ModelName);

	// 모델이름에 확장자가 들어왔는지 체크
	string strModelName = string(m_strModelName);
	m_strModelName = strModelName.substr(0, strModelName.rfind("."));

	CSMeshInfo * pMeshInfo = MeshInfoMgr.GetMeshInfo(m_strModelName);

	// 없으면 그냥 나가자.
	if(pMeshInfo == NULL)
	{
		return;
	}

	if(pMeshInfo)
	{
		// 모델 이름 보여주기
		GlobalObjects::g_pMainForm->HitEffectModelName->Text = ModelName;		

		// 아무것도 없으면... 그냥 나간다.
		if(MeshHitEffectInfoMgr->GetNextHitEffectCapsuleIndex(m_strModelName) == 0)
		{
			return;
		}

		// Viewer 내용 채우기
		int nCIndex = 0;
		for(vector<CSColCapsule>::iterator itCapsule = pMeshInfo->m_vecHitEffectInfo.begin();
			itCapsule != pMeshInfo->m_vecHitEffectInfo.end(); itCapsule++)
		{
			// 캡슐 등록
			TreeNode^ cnnod = GetMeshHitEffectInfoViewerTreeNode()->Nodes->Add(String::Format(MESH_COL_INFO_CAPULE_ID, nCIndex));
			cnnod->Tag		= nCIndex;

			nCIndex++;
		}

		GetMeshHitEffectInfoViewerTreeNode()->ExpandAll();
	}
}

void TMeshHitEffectInfoViewer::CreateMeshHitEffectInfoViewer(int nCapsuleID /* = -1 */)
{
	if(nCapsuleID == -1)
		return;

	// 캡슐 추가
	TreeNode^ pCapsuleNode = GetMeshHitEffectInfoViewerTreeNode()->Nodes->Add(String::Format(MESH_COL_INFO_CAPULE_ID, nCapsuleID));

	// 추가된 캡슐 선택노드로 변경
	pCapsuleNode->Tag								= nCapsuleID;
	GetMeshHitEffectInfoViewerTreeNode()->SelectedNode	= pCapsuleNode;
	GlobalObjects::g_SelInfo.m_nMeshHitEffectCapsuleID	= nCapsuleID;
}

void TMeshHitEffectInfoViewer::Reload()
{
	GlobalObjects::g_pMainForm->HitEffect_treeView->ClearTreeView();
	GetMeshHitEffectInfoViewerPropertGrid()->SelectedObject = nullptr;

	if(m_strModelName.size())
	{
		ChangeMeshHitEffectInfoViewer(gcnew String(m_strModelName.c_str()));
	}
}

CSColCapsule * TMeshHitEffectInfoViewer::GetMeshHitEffectCapsuleInfo( int nCapsuleIndex )
{
	if(nCapsuleIndex == -1)
	{
		return NULL;
	}

	// 모델 Hit 정보를 가져온다.
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(m_strModelName);

	if(pMeshInfo)
	{
		int nCIndex = 0;
		for(vector<CSColCapsule>::iterator itCapsule = pMeshInfo->m_vecHitEffectInfo.begin();
			itCapsule != pMeshInfo->m_vecHitEffectInfo.end(); itCapsule++)
		{
			if(nCIndex == nCapsuleIndex)
			{
				return &(*itCapsule);
			}

			nCIndex++;
		}
	}

	return NULL;
}

void TMeshHitEffectInfoViewer::SetMeshHitEffectCapsuleData( int nCapsuleIndex )
{
	// 충돌 캡슐 정보를 가져온다.
	CSColCapsule * pCapsuleInfo = GetMeshHitEffectCapsuleInfo( nCapsuleIndex);
	if(pCapsuleInfo != NULL)
	{
		SetMeshHitEffectCapsuleDataToProperty(pCapsuleInfo);
	}
	else
	{
		// 정보가 없다. 그러므로 기존 충돌 캡슐 정보 숨기기
		GetMeshHitEffectInfoViewerPropertGrid()->SelectedObject = nullptr;
	}
}

void TMeshHitEffectInfoViewer::ChangeMeshHitEffectCapsuleInfo( int nCapsuleID )
{
	// 충돌 캡슐 정보를 가져온다.
	CSColCapsule * pCapsuleInfo = GetMeshHitEffectCapsuleInfo(nCapsuleID);
	if(pCapsuleInfo != NULL)
	{
		// 변경된 데이터를 캡슐 정보에 입력
		SetPropertyToMeshHitEffectCapsuleData(pCapsuleInfo);

		// 변경된 데이터를 가지고 높이 다시 계산
		pCapsuleInfo->m_Capsule.CalcHeight();

		// 변경된 높이를 프로퍼티에게 다시 입력
		SetMeshHitEffectCapsuleDataToProperty(pCapsuleInfo);

		NotifyChanging();
	}
	else
	{
		// 정보가 없다. 그러므로 기존 충돌 캡슐 정보 숨기기
		GetMeshHitEffectInfoViewerPropertGrid()->SelectedObject = nullptr;
	}
}

void TMeshHitEffectInfoViewer::SetMeshHitEffectCapsuleDataToProperty(CSColCapsule * pMeshColCapsule)
{
	XmlSerializable::MESHCOLCAPSULE^ capsule = (XmlSerializable::MESHCOLCAPSULE^)(GetMeshHitEffectInfoViewerPropertGrid()->SelectedObject);

	if( capsule == nullptr)
	{
		capsule = gcnew XmlSerializable::MESHCOLCAPSULE;
	}

	capsule->반지름		= pMeshColCapsule->m_Capsule.radius;
	capsule->높이		= pMeshColCapsule->m_Capsule.height;
	capsule->조절점1_X	= pMeshColCapsule->m_Capsule.top.x;
	capsule->조절점1_Y	= pMeshColCapsule->m_Capsule.top.y;
	capsule->조절점1_Z	= pMeshColCapsule->m_Capsule.top.z;
	capsule->조절점2_X	= pMeshColCapsule->m_Capsule.bottom.x;
	capsule->조절점2_Y	= pMeshColCapsule->m_Capsule.bottom.y;
	capsule->조절점2_Z	= pMeshColCapsule->m_Capsule.bottom.z;
	capsule->본1		= gcnew String(pMeshColCapsule->m_strBoneTop.c_str());
	capsule->본2		= gcnew String(pMeshColCapsule->m_strBoneBottom.c_str());

	GetMeshHitEffectInfoViewerPropertGrid()->SelectedObject = capsule;
}

void TMeshHitEffectInfoViewer::SetPropertyToMeshHitEffectCapsuleData(CSColCapsule * pMeshColCapsule)
{
	XmlSerializable::MESHCOLCAPSULE^ capsule = (XmlSerializable::MESHCOLCAPSULE^)(GetMeshHitEffectInfoViewerPropertGrid()->SelectedObject);
	
	if( capsule == nullptr)
	{
		return;
	}

	pMeshColCapsule->m_Capsule.radius		= capsule->반지름;
	pMeshColCapsule->m_Capsule.height		= capsule->높이;
	pMeshColCapsule->m_Capsule.top.x		= capsule->조절점1_X;
	pMeshColCapsule->m_Capsule.top.y		= capsule->조절점1_Y;
	pMeshColCapsule->m_Capsule.top.z		= capsule->조절점1_Z;
	pMeshColCapsule->m_Capsule.bottom.x		= capsule->조절점2_X;
	pMeshColCapsule->m_Capsule.bottom.y		= capsule->조절점2_Y;
	pMeshColCapsule->m_Capsule.bottom.z		= capsule->조절점2_Z;
	pMeshColCapsule->m_strBoneTop			= string(MStringToCharPointer(capsule->본1));
	pMeshColCapsule->m_strBoneBottom		= string(MStringToCharPointer(capsule->본2));
}

void TMeshHitEffectInfoViewer::RenderEditionCapsule(MCapsule & capsuleInfo, DWORD dwColor)
{
	// 부분 판정 정보를 가져온다.
	MCapsule cap = capsuleInfo;

	//------------------------------------------------------------------------
	// 캡슐 렌더링
	RenderCapsule(cap, dwColor);

	g_pMainApp->SetCapsuleRenderState();
	//------------------------------------------------------------------------
	// 캡슐 중앙 선
	RBoundingBox box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
	RMatrix matTM;
	matTM.MakeIdentity();

	RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.bottom, cap.top, 0xFFFFFF00);

	float fMinZ = (cap.top.z < cap.bottom.z) ? cap.top.z : cap.bottom.z;
	RVector t1 = cap.top;
	t1.z = fMinZ - cap.radius * 1.5f;
	RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.top, t1, 0xFFFF00FF);
	matTM.SetTranslation(t1);
	RRenderHelper::RenderBox(matTM, box, 0xffffffff);
	RRenderHelper::RenderAxis(matTM,30,0.5f);
	g_pMainApp->RenderAxisText(matTM, 30);

	RVector t2 = cap.bottom;
	t2.z = fMinZ - cap.radius * 2.0f;
	RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.bottom, t2, 0xFFFF00FF);
	matTM.SetTranslation(t1);
	RRenderHelper::RenderBox(matTM, box, 0xffffffff);
	RRenderHelper::RenderAxis(matTM,30,0.5f);
	g_pMainApp->RenderAxisText(matTM, 30);

	//------------------------------------------------------------------------
	// 번호 찍기
	// billboard
	rs3::RFont::BeginFont(/*D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE*/);

	g_pMainApp->GetDevice()->SetTransform(RST_VIEW, g_pMainApp->GetCamera()->GetViewMatrix());

	rs3::RMatrix matInverse;
	g_pMainApp->GetDevice()->GetTransform( rs3::RST_VIEW ).GetInverse( &matInverse );

	g_pMainApp->GetCamera()->GetViewMatrix().GetInverse( &matInverse );

	rs3::RMatrix matTextReflectAndScale
		(
			2.0f,    0.f,    0.f,   0.f,
			0.f,     -2.0f,  0.f,   0.f,
			0.f,      0.f,    2.0f, 0.f,
			0.f,      0.f,    0.f,   1.f
		);

	matInverse._41 = t1.x;
	matInverse._42 = t1.y;
	matInverse._43 = t1.z;

	RMatrix matT = matTextReflectAndScale * matInverse;

	if( GlobalObjects::g_pMainForm->Bottom_toolStripButton->Checked ||
		GlobalObjects::g_pMainForm->TopBottom_toolStripButton->Checked)
		g_pMainApp->GetFont()->DrawTextIn3DSpace( matT, L"1", -1, 0xFFFF0000);
	else
		g_pMainApp->GetFont()->DrawTextIn3DSpace( matT, L"1" );

	matInverse._41 = t2.x;
	matInverse._42 = t2.y;
	matInverse._43 = t2.z;

	matT = matTextReflectAndScale * matInverse;

	if( GlobalObjects::g_pMainForm->Top_toolStripButton->Checked ||
		GlobalObjects::g_pMainForm->TopBottom_toolStripButton->Checked)
		g_pMainApp->GetFont()->DrawTextIn3DSpace( matT, L"2", -1, 0xFFFF0000);
	else
		g_pMainApp->GetFont()->DrawTextIn3DSpace( matT, L"2" );

	rs3::RFont::EndFont();

	g_pMainApp->EndCapsuleRenderState();
}

void TMeshHitEffectInfoViewer::RenderCapsule(MCapsule & capsuleInfo, DWORD dwColor /* = 0x200000ff */)
{
	g_pMainApp->SetCapsuleRenderState();

	RRenderHelper::RenderCapsule(capsuleInfo, dwColor);

	g_pMainApp->EndCapsuleRenderState();
}

void TMeshHitEffectInfoViewer::RenderGroup( int nSelectCapsuleID /*= -1*/, bool bSolo /*= false*/ )
{
	if(m_strModelName.size() <= 0)
		return;

	// 모델 Hit 정보를 가져온다.
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(m_strModelName);
	if(pMeshInfo)
	{
		bool bSelect = false;
		MCapsule renderCapsule;
		MCapsule selectCapsule;
		int nCIndex = 0;
		for(vector<CSColCapsule>::iterator itCapsule = pMeshInfo->m_vecHitEffectInfo.begin();
			itCapsule != pMeshInfo->m_vecHitEffectInfo.end(); itCapsule++)
		{
			renderCapsule = (*itCapsule).m_Capsule;
			
			// 애니메이션이 가능하게 본이 설정되어 있는가?
			CalAnimationRenderCapsule(&(*itCapsule), renderCapsule);

			if(nSelectCapsuleID != -1)
			{
				// 캡슐 선택이라면... 선택 캡슐 이외는 회색으로
				if(nCIndex == nSelectCapsuleID)
				{
					bSelect = true;
					selectCapsule = renderCapsule;
				}
				else if(bSolo == false)
				{
					RenderCapsule(renderCapsule, MESH_COL_CAPSILE_NONE_SELECT);
				}
			}
			else if(bSolo == false)
			{
				// 그룹선택이라면 전체가 분홍색
				RenderCapsule(renderCapsule);
			}

			nCIndex++;
		}
		
		if(bSelect)
		{
			RenderEditionCapsule(selectCapsule, MESH_COL_CAPSULE_SELECT);
		}
	}
}

MCapsule * TMeshHitEffectInfoViewer::GetCurrentSelectCapsule()
{
	CSColCapsule * pMeshColCapsule = GetMeshHitEffectCapsuleInfo(GlobalObjects::g_SelInfo.m_nMeshHitEffectCapsuleID);

	if(pMeshColCapsule)
	{
		return &(pMeshColCapsule->m_Capsule);
	}

	return NULL;
}

void TMeshHitEffectInfoViewer::CalAnimationRenderCapsule( CSColCapsule * pColCapsule, MCapsule & outCapsule )
{
	RActorNode* pTopNode		= g_pMainApp->GetActorNode(pColCapsule->m_strBoneTop);
	RActorNode* pBottomNode		= g_pMainApp->GetActorNode(pColCapsule->m_strBoneBottom);
	RMatrix matActor			= g_pMainApp->GetActor()->GetWorldTransform();

	if(pTopNode)
	{
		vec3 vLocalTop	= pTopNode->GetWorldPosition() - g_pMainApp->GetActor()->GetWorldPosition();
		vec3 vTopPos;
		matActor.TransformVect(pColCapsule->m_Capsule.top, vTopPos);

		outCapsule.top = vTopPos + vLocalTop;
	}

	if(pBottomNode)
	{
		vec3 vLocalBottom	= pBottomNode->GetWorldPosition() - g_pMainApp->GetActor()->GetWorldPosition();
		vec3 vBottomPos;
		matActor.TransformVect(pColCapsule->m_Capsule.bottom, vBottomPos);

		outCapsule.bottom = vBottomPos + vLocalBottom;
	}

	outCapsule.CalcHeight();
}

void TMeshHitEffectInfoViewer::NotifyChanging()
{
	GlobalObjects::g_SaveNotifier.SetSaveMeshInfo(true);
}