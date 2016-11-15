#include "stdafx.h"
#include "TPartsHitInfoViewer.h"
#include "Xmlseral.h"
#include "RRenderHelper.h"
#include "RFont.h"

#define PARTS_HIT_INFO_GROUP_ID			"Group : {0}"
#define PARTS_HIT_INFO_CAPULE_ID		"Capsule : {0}"

#define PARTS_HIT_CAPSULE_SELECT		0x20ffff00
#define PARTS_HIT_CAPSILE_NONE_SELECT	0x20C0C0C0

//////////////////////////////////////////////////////////////////////////
void TPartsHitInfoViewer::InitPartsHitInfoViewer()
{
	// Viewer 내용 초기화
	GlobalObjects::g_pMainForm->PartsHitCapsulesView->ClearTreeView();
	GetPartsHitInfoViewerPropertGrid()->SelectedObject = nullptr;

}

TreeView^ TPartsHitInfoViewer::GetPartsHitInfoViewerTreeNode()
{
	return GlobalObjects::g_pMainForm->PartsHitCapsulesView;
}

PropertyGrid^ TPartsHitInfoViewer::GetPartsHitInfoViewerPropertGrid()
{
	return GlobalObjects::g_pMainForm->PartsHitCapsuleAtt;
}

void TPartsHitInfoViewer::ChangePartsHitInfoViewer( System::String^ ModelName )
{
	InitPartsHitInfoViewer();

	TMeshInfoMgr& MeshInfoMgr		= TMeshInfoMgr::GetInstance();

	// 모델 Hit 정보를 가져온다.
	m_strModelName = (char*)MStringToCharPointer(ModelName);

	// 모델이름에 확장자가 들어왔는지 체크
	string strModelName = string(m_strModelName);
	m_strModelName = strModelName.substr(0, strModelName.rfind("."));

	CSMeshInfo * pMeshInfo = MeshInfoMgr.GetInfo(m_strModelName);

	// 없으면 만들어 준다.
	if(pMeshInfo == NULL)
	{
		if(MeshInfoMgr.UseModelHitColCapsule(m_strModelName))
		{
			if(MeshInfoMgr.CreateMeshInfo(m_strModelName))
			{
				pMeshInfo = MeshInfoMgr.GetInfo(m_strModelName);
			}
		}
	}

	if(pMeshInfo)
	{
		// 모델 이름 보여주기
		GlobalObjects::g_pMainForm->PartsHitModelName->Text = ModelName;		

		// 아무것도 없거나, 0번 인덱스를 가진 판정 캡슐이 없다면... 기본 판정 캡슐을 만든다.
		if(pMeshInfo->m_mapHitGroupInfo.size() == 0 ||
			MeshInfoMgr.GetMeshHitInfo()->IsHitGroup(m_strModelName, 0) == false)
		{
			MeshInfoMgr.GetMeshHitInfo()->CreateSimpleHitCapsule(pMeshInfo, g_pMainApp->GetActor());

			NotifyChanging();
		}

		// Viewer 내용 채우기
		for(map<int, CSHitGroup>::iterator itParts = pMeshInfo->m_mapHitGroupInfo.begin();
			itParts != pMeshInfo->m_mapHitGroupInfo.end(); itParts++)
		{
			// 그룹 등록
			TreeNode^ nnode = GetPartsHitInfoViewerTreeNode()->Nodes->Add(String::Format(PARTS_HIT_INFO_GROUP_ID, itParts->second.m_nGroupID));
			nnode->Tag		= itParts->first;

			int nCIndex = 0;
			for(vector<CSHitCapsule>::iterator itCapsule = itParts->second.m_vecHitCapsules.begin();
				itCapsule != itParts->second.m_vecHitCapsules.end(); itCapsule++)
			{
				if(nnode != nullptr)
				{
					// 캡슐 등록
					TreeNode^ cnnod = nnode->Nodes->Add(String::Format(PARTS_HIT_INFO_CAPULE_ID, nCIndex));
					cnnod->Tag		= nCIndex;
				}

				nCIndex++;
			}
		}

		GetPartsHitInfoViewerTreeNode()->ExpandAll();
	}
}

void TPartsHitInfoViewer::CreatePartsHitInfoViewer(int nGroupID, int nCapsuleID /* = -1 */)
{
	if(nGroupID == -1)
		return;

	TreeNode^ pGroupNode = g_pMainApp->SearchNode(GetPartsHitInfoViewerTreeNode()->Nodes, String::Format(PARTS_HIT_INFO_GROUP_ID, nGroupID));
	if(pGroupNode == nullptr)
	{
		// 없으면 생성
		pGroupNode = GetPartsHitInfoViewerTreeNode()->Nodes->Add(String::Format(PARTS_HIT_INFO_GROUP_ID, nGroupID));
		pGroupNode->Tag									= nGroupID;
		GetPartsHitInfoViewerTreeNode()->SelectedNode	= pGroupNode;
		GlobalObjects::g_SelInfo.m_nMeshHitGroupID					= nGroupID;
	}

	if(nCapsuleID == -1)
		return;

	// 캡슐 추가
	TreeNode^ pCapsuleNode = pGroupNode->Nodes->Add(String::Format(PARTS_HIT_INFO_CAPULE_ID, nCapsuleID));

	// 추가된 캡슐 선택노드로 변경
	pCapsuleNode->Tag								= nCapsuleID;
	GetPartsHitInfoViewerTreeNode()->SelectedNode	= pCapsuleNode;
	GlobalObjects::g_SelInfo.m_nMeshHitCapsuleID				= nCapsuleID;
}

void TPartsHitInfoViewer::Reload()
{
	GlobalObjects::g_pMainForm->PartsHitCapsulesView->ClearTreeView();
	GetPartsHitInfoViewerPropertGrid()->SelectedObject = nullptr;

	if(m_strModelName.size())
	{
		ChangePartsHitInfoViewer(gcnew String(m_strModelName.c_str()));
	}
}

CSHitCapsule * TPartsHitInfoViewer::GetPartsHitCapsuleInfo( int nGroupIndex, int nCapsuleIndex )
{
	if(nGroupIndex == -1 || nCapsuleIndex == -1)
	{
		return NULL;
	}

	// 모델 Hit 정보를 가져온다.
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(m_strModelName);
	if(pMeshInfo)
	{
		for(map<int, CSHitGroup>::iterator itParts = pMeshInfo->m_mapHitGroupInfo.begin();
			itParts != pMeshInfo->m_mapHitGroupInfo.end(); itParts++)
		{
			if(itParts->first == nGroupIndex)
			{
				int nCIndex = 0;
				for(vector<CSHitCapsule>::iterator itCapsule = itParts->second.m_vecHitCapsules.begin();
					itCapsule != itParts->second.m_vecHitCapsules.end(); itCapsule++)
				{
					if(nCIndex == nCapsuleIndex)
					{
						return &(*itCapsule);
					}

					nCIndex++;
				}
			}
		}
	}

	return NULL;
}

void TPartsHitInfoViewer::SetPartsHitCapsuleData( int nGroupIndex, int nCapsuleIndex )
{
	// 부분 판정 정보를 가져온다.
	CSHitCapsule * pCapsuleInfo = GetPartsHitCapsuleInfo(nGroupIndex, nCapsuleIndex);
	if(pCapsuleInfo != NULL)
	{
		SetPartsHitCapsuleDataToProperty(pCapsuleInfo);
	}
	else
	{
		// 정보가 없다. 그러므로 기존 부분 판정 캡슐 정보 숨기기
		GetPartsHitInfoViewerPropertGrid()->SelectedObject = nullptr;
	}
}

void TPartsHitInfoViewer::ChangePartsHitCapsuleInfo( int nGroupID, int nCapsuleID )
{
	// 부분 판정 정보를 가져온다.
	CSHitCapsule * pCapsuleInfo = GetPartsHitCapsuleInfo(nGroupID, nCapsuleID);
	if(pCapsuleInfo != NULL)
	{
		// 변경된 데이터를 캡슐 정보에 입력
		SetPropertyToPartsHitCapsuleData(pCapsuleInfo);

		// 변경된 데이터를 가지고 높이 다시 계산
		pCapsuleInfo->m_Capsule.CalcHeight();

		// 변경된 높이를 프로퍼티에게 다시 입력
		SetPartsHitCapsuleDataToProperty(pCapsuleInfo);

		NotifyChanging();
	}
	else
	{
		// 정보가 없다. 그러므로 기존 부분 판정 캡슐 정보 숨기기
		GetPartsHitInfoViewerPropertGrid()->SelectedObject = nullptr;
	}
}

void TPartsHitInfoViewer::SetPartsHitCapsuleDataToProperty(CSHitCapsule * pPartsHitCapsule)
{
	XmlSerializable::PARTSHITCAPSULE^ capsule = (XmlSerializable::PARTSHITCAPSULE^)(GetPartsHitInfoViewerPropertGrid()->SelectedObject);

	if( capsule == nullptr)
	{
		capsule = gcnew XmlSerializable::PARTSHITCAPSULE;
	}

	capsule->반지름		= pPartsHitCapsule->m_Capsule.radius;
	capsule->높이		= pPartsHitCapsule->m_Capsule.height;
	capsule->조절점1_X	= pPartsHitCapsule->m_Capsule.top.x;
	capsule->조절점1_Y	= pPartsHitCapsule->m_Capsule.top.y;
	capsule->조절점1_Z	= pPartsHitCapsule->m_Capsule.top.z;
	capsule->조절점2_X	= pPartsHitCapsule->m_Capsule.bottom.x;
	capsule->조절점2_Y	= pPartsHitCapsule->m_Capsule.bottom.y;
	capsule->조절점2_Z	= pPartsHitCapsule->m_Capsule.bottom.z;
	capsule->인덱스		= pPartsHitCapsule->m_nHitEffectCapsuleID;

	capsule->a유효시작시간	= pPartsHitCapsule->GetActiveTimeBegin();
	capsule->b유효끝시간	= pPartsHitCapsule->GetActiveTimeEnd();

	capsule->LinkedBPart	= pPartsHitCapsule->nLinkedBPart;

	GetPartsHitInfoViewerPropertGrid()->SelectedObject = capsule;

}

void TPartsHitInfoViewer::SetPropertyToPartsHitCapsuleData(CSHitCapsule * pPartsHitCapsule)
{
	XmlSerializable::PARTSHITCAPSULE^ capsule = (XmlSerializable::PARTSHITCAPSULE^)(GetPartsHitInfoViewerPropertGrid()->SelectedObject);
	
	if( capsule == nullptr)
	{
		return;
	}

	pPartsHitCapsule->m_Capsule.radius		= capsule->반지름;
	pPartsHitCapsule->m_Capsule.height		= capsule->높이;
	pPartsHitCapsule->m_Capsule.top.x		= capsule->조절점1_X;
	pPartsHitCapsule->m_Capsule.top.y		= capsule->조절점1_Y;
	pPartsHitCapsule->m_Capsule.top.z		= capsule->조절점1_Z;
	pPartsHitCapsule->m_Capsule.bottom.x	= capsule->조절점2_X;
	pPartsHitCapsule->m_Capsule.bottom.y	= capsule->조절점2_Y;
	pPartsHitCapsule->m_Capsule.bottom.z	= capsule->조절점2_Z;
	pPartsHitCapsule->m_nHitEffectCapsuleID	= capsule->인덱스;
	pPartsHitCapsule->nLinkedBPart			= capsule->LinkedBPart;

	float fActiveTimeBegin = capsule->a유효시작시간;
	float fActiveTimeEnd = capsule->b유효끝시간;

	pPartsHitCapsule->SetActiveTime(fActiveTimeBegin, fActiveTimeEnd);

}

void TPartsHitInfoViewer::RenderEditionCapsule(MCapsule & capsuleInfo, DWORD dwColor)
{
	// 부분 판정 정보를 가져온다.
	MCapsule cap = capsuleInfo;

	//------------------------------------------------------------------------
	// 캡슐 렌더링
	RMatrix matActor			= g_pMainApp->GetActor()->GetWorldTransform();
	vec3 vTop, vBottom;
	matActor.TransformVect(cap.top, vTop);
	matActor.TransformVect(cap.bottom, vBottom);
	cap.top = vTop;
	cap.bottom = vBottom;

	RenderCapsule(capsuleInfo, dwColor);

	g_pMainApp->SetCapsuleRenderState();
	//------------------------------------------------------------------------
	// 캡슐 중앙 선
	RBoundingBox box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
	RMatrix matTM;
	matTM.MakeIdentity();

	RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.bottom, cap.top, 0xFFFF0000);

	RVector t1 = cap.top;
	t1.z -= (cap.height + cap.radius * 1.5f);
	RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.top, t1, 0xFFFF00FF);
	matTM.SetTranslation(t1);
	RRenderHelper::RenderBox(matTM, box, 0xffffffff);
	RRenderHelper::RenderAxis(matTM,30,0.5f);
	g_pMainApp->RenderAxisText(matTM, 30);

	RVector t2 = cap.bottom;
	t2.z -= cap.radius * 2.0f;
	RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.bottom, t2, 0xFFFF00FF);
	matTM.SetTranslation(t2);
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

void TPartsHitInfoViewer::RenderCapsule(MCapsule & capsuleInfo, DWORD dwColor /* = 0x200000ff */)
{
	MCapsule cap = capsuleInfo;

	//------------------------------------------------------------------------
	// 캡슐 렌더링
	RMatrix matActor			= g_pMainApp->GetActor()->GetWorldTransform();
	vec3 vTop, vBottom;
	matActor.TransformVect(cap.top, vTop);
	matActor.TransformVect(cap.bottom, vBottom);
	cap.top = vTop;
	cap.bottom = vBottom;

	g_pMainApp->SetCapsuleRenderState();

	RRenderHelper::RenderCapsule(cap, dwColor);

	g_pMainApp->EndCapsuleRenderState();
}

void TPartsHitInfoViewer::RenderGroup( int nGroupIndex, int nSelectCapsuleID /*= -1*/ )
{
	if(nGroupIndex == -1 || m_strModelName.size() <= 0)
	{
		return;
	}

	// 모델 Hit 정보를 가져온다.
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(m_strModelName);
	if(pMeshInfo)
	{
		for(map<int, CSHitGroup>::iterator itParts = pMeshInfo->m_mapHitGroupInfo.begin();
			itParts != pMeshInfo->m_mapHitGroupInfo.end(); itParts++)
		{
			if(itParts->first == nGroupIndex)
			{
				bool bSelect = false;
				MCapsule renderCapsule;
				MCapsule selectCapsule;
				int nCIndex = 0;
				for(vector<CSHitCapsule>::iterator itCapsule = itParts->second.m_vecHitCapsules.begin();
					itCapsule != itParts->second.m_vecHitCapsules.end(); itCapsule++)
				{
					renderCapsule = (*itCapsule).m_Capsule;

					if(nSelectCapsuleID != -1)
					{
						// 캡슐 선택이라면... 선택 캡슐 이외는 회색으로
						if(nCIndex == nSelectCapsuleID)
						{
							bSelect = true;
							selectCapsule = renderCapsule;
						}
						else
						{
							RenderCapsule(renderCapsule, PARTS_HIT_CAPSILE_NONE_SELECT);
						}
					}
					else
					{
						// 그룹선택이라면 전체가 파랑색
						RenderCapsule(renderCapsule);
					}

					nCIndex++;
				}

				if(bSelect)
				{
					RenderEditionCapsule(selectCapsule, PARTS_HIT_CAPSULE_SELECT);
				}
			}
		}
	}
}

MCapsule * TPartsHitInfoViewer::GetCurrentSelectCapsule()
{
	CSHitCapsule * pPartsHitCapsule = GetPartsHitCapsuleInfo(GlobalObjects::g_SelInfo.m_nMeshHitGroupID, GlobalObjects::g_SelInfo.m_nMeshHitCapsuleID);

	if(pPartsHitCapsule)
	{
		return &(pPartsHitCapsule->m_Capsule);
	}

	return NULL;
}

void TPartsHitInfoViewer::NotifyChanging()
{
	GlobalObjects::g_SaveNotifier.SetSaveMeshInfo(true);
}

CSHitGroup * TPartsHitInfoViewer::GetPartsHitGroupInfo( int nGroupIndex )
{
	if(nGroupIndex == -1)
	{
		return NULL;
	}

	// 모델 Hit 정보를 가져온다.
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(m_strModelName);
	if(pMeshInfo)
	{
		for(map<int, CSHitGroup>::iterator itParts = pMeshInfo->m_mapHitGroupInfo.begin();
			itParts != pMeshInfo->m_mapHitGroupInfo.end(); itParts++)
		{
			if(itParts->first == nGroupIndex)
			{
				return &(itParts->second);
			}
		}
	}

	return NULL;
}
