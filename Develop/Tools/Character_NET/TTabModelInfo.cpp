#include "StdAfx.h"
#include "TTabModelInfo.h"
#include "RRenderHelper.h"
#include "toolPick.h"

#define BOXSIZE_DEFAULT 0.5f
#define BOXSIZE_TEMP_SELECT_DEFAULT 0.1f
#define BOXSIZE_SELECT_DEFAULT 0.05f
#define MODEL_DEFAULT_SIZE 200.0f

using namespace System::Windows::Forms::VisualStyles;

TTabModelInfo::TTabModelInfo(void)
{
	InitSelectNode();
}

TTabModelInfo::~TTabModelInfo(void)
{

}

bool TTabModelInfo::SetBipTreeView()
{
	m_vecBoneNodeList.clear();

	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) return false;

	CalSelectNodeDrawBoxSize(pActor);

	GlobalObjects::g_pMainForm->m_ModelInfoTabPage->BiptreeView->Nodes->Clear();
	GlobalObjects::g_pMainForm->m_ModelInfoTabPage->MeshNode_comboBox->Items->Clear();
	GlobalObjects::g_pMainForm->m_ModelInfoTabPage->MeshNode_comboBox->Text = "";
	GlobalObjects::g_pMainForm->m_ModelInfoTabPage->MeshNode_comboBox->Items->Add(gcnew String("all"));
	GlobalObjects::g_pMainForm->m_ModelInfoTabPage->MeshNode_comboBox->SelectedIndex = 0;

	for(int i = 0; i<pActor->GetActorNodeCount();i++)
	{
		RActorNode *pActorNode = pActor->GetActorNodes()[i];
		String^ nodename = gcnew String(pActorNode->m_pMeshNode->m_Name.c_str());

		// 붙을 부모 collection 을 선택
		TreeNodeCollection^ tnParentCollection; 
		if( pActorNode->GetParentActorNode() == NULL)
			tnParentCollection = GlobalObjects::g_pMainForm->m_ModelInfoTabPage->BiptreeView->Nodes;
		else if( pActorNode->GetParentActorNode() != NULL)
		{
			RActorNode *parentActorNode = pActorNode->GetParentActorNode();
			int nID = parentActorNode->m_pMeshNode->m_nID;

			TreeNode^ treeNode = g_pMainApp->SearchNode(GlobalObjects::g_pMainForm->m_ModelInfoTabPage->BiptreeView->Nodes, nID);
			if(treeNode==nullptr)
			{
				tnParentCollection = GlobalObjects::g_pMainForm->m_ModelInfoTabPage->BiptreeView->Nodes;
				mlog("에러 : %s tree node not found", parentActorNode->GetNodeName().c_str());
			}
			else
				tnParentCollection = treeNode->Nodes;
		}else
		{
			_ASSERT(FALSE); // ?
		}

		// 새 트리 node 를 등록한다
		TreeNode^ node = gcnew TreeNode( nodename );
		node->Tag = pActorNode->m_pMeshNode->m_nID;

		// 노드 존재 체크(애니메이션에서....)
		if(CheckExistNodeByAnimation(pActorNode->GetNodeName()) == false)
			node->ForeColor = System::Drawing::Color::Crimson;

		tnParentCollection->Add( node);

		SetModelBipNode(pActorNode);

		if(pActorNode->m_pMeshNode->m_nMtrlCnt > 0)
			GlobalObjects::g_pMainForm->m_ModelInfoTabPage->MeshNode_comboBox->Items->Add(gcnew String(pActorNode->m_pMeshNode->GetName()));
	}

	GlobalObjects::g_pMainForm->m_ModelInfoTabPage->InitMeshNodeInfo();

	return true;
}

void TTabModelInfo::SetSelectedNodeInfo(String^ nodeName)
{
	m_NodeList.clear();
	m_PartsColorTestList.clear();

	const char* bonename = MStringToCharPointer(nodeName);

	RActorNode*  pActorNode = g_pMainApp->GetActorNode(bonename);
	if (pActorNode == NULL) return;

	MFreeCharPointer(bonename);

	stBoneNodeData* pData = GetBoneNodeData(pActorNode);
	if(pData)
	{
		m_pSelectNode = pData;
	}
	else
	{
		m_SelectNodeOtherList.m_pMeNode = pActorNode;
		m_pSelectNode = &m_SelectNodeOtherList;
	}

	m_NodeList.push_back(pActorNode);
	g_pMainApp->m_pSelectedActorNode = pActorNode;
}

void TTabModelInfo::NodeNoneSelectedButtonClick()
{
	m_NodeList.clear();
	m_PartsColorTestList.clear();
}

void TTabModelInfo::SkinNodesSelectButtonClick()
{
	m_NodeList.clear();
	m_PartsColorTestList.clear();

	const char* SkinNodeStr[] =
	{
		"face",
		"chest",
		"hands",
		"legs",
		"feet"
	};

	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) return;

	for (int i = 0; i < 5 ; i++)
	{
		RActorNode*  pActorNode = pActor->GetActorNode(SkinNodeStr[i]);
		if (pActorNode == NULL) continue;

		m_NodeList.push_back(pActorNode);
	}
}

void TTabModelInfo::ItemNodesSelectButtonClick()
{
	m_NodeList.clear();
	m_PartsColorTestList.clear();

	const char* ItemNodeStr[] =
	{
		"hat_item",
		"chest_item",
		"hands_item",
		"legs_item",
		"feet_item",
		"hat_cloth",
		"chest_cloth"
	};

	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) return;

	for (int i = 0; i < 7 ; i++)
	{
		RActorNode*  pActorNode = pActor->GetActorNode(ItemNodeStr[i]);
		if (pActorNode == NULL) continue;

		m_NodeList.push_back(pActorNode);
	}
}

void TTabModelInfo::SetModelBipNode(RActorNode* pActorNode)
{
	if(pActorNode == NULL)
		return;

	if(!pActorNode->m_pMeshNode->isRenderAble())
	{
		if(CheckDelDummyBoneNode(pActorNode, pActorNode->GetParentActorNode()))
			return;

		stBoneNodeData stData;
		stData.m_pParentsNode = pActorNode->GetParentActorNode();
		stData.m_pMeNode = pActorNode;

		int nSameCount =0 ;
		if(CheckSameBoneNode(pActorNode, nSameCount))
		{
			stData.m_fScale += nSameCount * 1.0f;
		}

		m_vecBoneNodeList.push_back(stData);
	}
}

void TTabModelInfo::OnBoneRander()
{

	RRenderHelper::SetRenderState();

	for(vector<stBoneNodeData>::iterator itBone = m_vecBoneNodeList.begin(); itBone != m_vecBoneNodeList.end(); itBone++)
	{
		// 본 그리기...
		DrawBoneBox((*itBone).m_pMeNode, m_fSelectNodeDrawBoxSize, (*itBone).m_fScale);

		// 부모가 있으면 부와 연결하는 선 그리기...
		DrawBoneLinkLine(*itBone);
	}

	// 선택된 본 있다면...
	if(m_pTempSelectNode &&
		m_pSelectNode != m_pTempSelectNode)
	{
		float fTempSize = (m_fSelectNodeDrawBoxSize * BOXSIZE_TEMP_SELECT_DEFAULT) / BOXSIZE_DEFAULT;
		DrawBoneBox(m_pTempSelectNode->m_pMeNode, m_fSelectNodeDrawBoxSize + fTempSize, m_pTempSelectNode->m_fScale, 0xffff00ff);
		DrawBoneName(m_pTempSelectNode->m_pMeNode, 0xff62FF51);
	}
	
	if(m_pSelectNode)
	{
		float fSelectSize = (m_fSelectNodeDrawBoxSize * BOXSIZE_SELECT_DEFAULT) / BOXSIZE_DEFAULT;
		DrawBoneBox(m_pSelectNode->m_pMeNode, m_fSelectNodeDrawBoxSize + fSelectSize, m_pSelectNode->m_fScale, 0xffffff00);
		DrawBoneName(m_pSelectNode->m_pMeNode, 0xffffff00);
	}

	RRenderHelper::EndRenderState();
}

void TTabModelInfo::DrawBoneBox( RActorNode* actorNode, float fSize, float fScale, DWORD dwColor /*= 0xffffffff*/ )
{
	if(actorNode == NULL)
		return;

	if(dwColor == 0xffffffff)
	{
		if(actorNode->GetVisibility() < 1.0f)
			dwColor = 0xff0000;
	}

	RBoundingBox box = RBoundingBox(RVector(-fSize,-fSize,-fSize),RVector(fSize,fSize,fSize));
	RMatrix matTM = actorNode->GetWorldTransform();
	RMatrix matScale;
	matScale.SetScaleMatrix(fScale);
	RRenderHelper::RenderBox(matScale * matTM,box,dwColor);
}

void TTabModelInfo::DrawBoneLinkLine( stBoneNodeData& boneData )
{
	if(boneData.m_pParentsNode == NULL)
		return;
	
	RMatrix matTM;
	matTM.MakeIdentity();

	vec3 v1 = boneData.m_pMeNode->GetWorldPosition();
	vec3 v2 = boneData.m_pParentsNode->GetWorldPosition();

	RRenderHelper::RenderLine(matTM, v1, v2, 0xffffffff);
}

void TTabModelInfo::CheckMousePick( RVector2& pos )
{
	float fMinDist			= FLT_MAX;
	bool bCol				= false;
	m_pTempSelectNode		= NULL;

	for(vector<stBoneNodeData>::iterator itBone = m_vecBoneNodeList.begin(); itBone != m_vecBoneNodeList.end(); itBone++)
	{
		vec3 vMin, vMax;
		RMatrix matTM = (*itBone).m_pMeNode->GetWorldTransform();
		RMatrix matScale;
		matScale.SetScaleMatrix((*itBone).m_fScale);
		RBoundingBox box = RBoundingBox(RVector(-m_fSelectNodeDrawBoxSize,-m_fSelectNodeDrawBoxSize,-m_fSelectNodeDrawBoxSize), RVector(m_fSelectNodeDrawBoxSize,m_fSelectNodeDrawBoxSize,m_fSelectNodeDrawBoxSize));

		MRect rect;
		rect.left = 0;
		rect.right = REngine::GetConfig().m_nWidth;
		rect.top = 0;
		rect.bottom = REngine::GetConfig().m_nHeight;

		float fDist = 0.0f;
		if(CToolPick::AABBPicking(box, MPoint(pos.x, pos.y), rect,matScale *  matTM, g_pMainApp->GetDevice()->GetTransform(RST_VIEW), g_pMainApp->GetDevice()->GetTransform(RST_PROJECTION), fDist))
		{
			stBoneNodeData* pData = &(*itBone);
			if(m_pTempSelectNode)
			{
				if(m_pTempSelectNode->m_pMeNode->GetWorldPosition() == (*itBone).m_pMeNode->GetWorldPosition() &&
					m_pTempSelectNode->m_fScale > (*itBone).m_fScale)
				{
					m_pTempSelectNode = &(*itBone);
				}
			}
			else if(fDist < fMinDist)
			{
				m_pTempSelectNode = &(*itBone);
			}
		}
	}
}

void TTabModelInfo::CalSelectNodeDrawBoxSize( TCharacter* pActor )
{
	// 모델 사이즈에 따라 박스 사이즈를 변화를 주자.
	InitSelectNode();

	if(pActor)
	{
		RBoundingBox actorBox = g_pMainApp->GetActor()->GetAABB();
		float fLen = (actorBox.vmin - actorBox.vmax).Length();

		m_fSelectNodeDrawBoxSize = (fLen * BOXSIZE_DEFAULT) / MODEL_DEFAULT_SIZE;
	}
}

RActorNode* TTabModelInfo::BoneSelect()
{
	m_pSelectNode = NULL;

	if(m_pTempSelectNode)
	{
		m_pSelectNode = m_pTempSelectNode;

		return m_pSelectNode->m_pMeNode;
	}

	return NULL;
}

void TTabModelInfo::InitSelectNode()
{
	m_pSelectNode = NULL;
	m_pTempSelectNode = NULL;
	m_fSelectNodeDrawBoxSize = BOXSIZE_DEFAULT;
}

void TTabModelInfo::DrawBoneName( RActorNode* actorNode, DWORD dwColor )
{
	if(actorNode == NULL)
		return;

	REngine::GetDevice().SetTransform( RST_WORLD, RMatrix::IDENTITY);
	RRenderHelper::RenderText(actorNode->GetWorldPosition(), "굴림", 13, dwColor, actorNode->GetNodeName().c_str(), 11, -5);
}

bool TTabModelInfo::CheckSameBoneNode( RActorNode* pNode, int& nOutCount )
{
	int nSameCount = 0;
	for(vector<stBoneNodeData>::iterator it = m_vecBoneNodeList.begin(); it != m_vecBoneNodeList.end(); it++)
	{
		if(it->m_pMeNode->GetWorldPosition() == pNode->GetWorldPosition())
		{
			// 같은 위치에 있네?
			nSameCount++;
		}
	}

	nOutCount = nSameCount;

	return (nSameCount > 0);
}

bool TTabModelInfo::CheckDelDummyBoneNode( RActorNode* pNode, RActorNode* pParentNode )
{
	if(pParentNode == NULL)
	{
		string strNodeName = pNode->GetNodeName();
		if(strNodeName.find_first_of("dummy_") != string::npos ||
			strNodeName.find_first_of("Dummy_") != string::npos)
		{
			return true;
		}
	}

	return false;
}

string TTabModelInfo::GetSelectNodeName()
{
	string strSelectNodeName;

	if(m_pSelectNode &&
		m_pSelectNode->m_pMeNode)
	{
		return m_pSelectNode->m_pMeNode->GetNodeName();
	}

	return strSelectNodeName;
}

RActorNode* TTabModelInfo::GetSelectNode()
{
	if(m_pSelectNode == NULL)
		return NULL;

	return m_pSelectNode->m_pMeNode;
}

TTabModelInfo::stBoneNodeData* TTabModelInfo::GetBoneNodeData( RActorNode* pActorNode )
{
	for(vector<stBoneNodeData>::iterator it = m_vecBoneNodeList.begin(); it != m_vecBoneNodeList.end(); it++)
	{
		if(it->m_pMeNode == pActorNode)
		{
			return &(*it);
		}
	}

	return NULL;
}

bool TTabModelInfo::CheckExistNodeByAnimation( string strNodeName )
{
	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) return false;

	if(pActor->CheckExistNodeController(strNodeName))
		return true;

	return false;
}

bool TTabModelInfo::ReadySkinColorTest()
{
	m_PartsColorTestList.clear();

	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) 
		return false;

	vector<string> vecNodeStr;
	vecNodeStr.push_back("face");
	vecNodeStr.push_back("chest");
	vecNodeStr.push_back("hands");
	vecNodeStr.push_back("legs");
	vecNodeStr.push_back("feet");

	SetPartsColorTestActorNode(pActor, vecNodeStr);

	return !(m_PartsColorTestList.empty());
}

bool TTabModelInfo::ReadyHairColorTest()
{
	m_PartsColorTestList.clear();

	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) 
		return false;

	vector<string> vecNodeStr;
	vecNodeStr.push_back("hair");

	SetPartsColorTestActorNode(pActor, vecNodeStr);

	return !(m_PartsColorTestList.empty());
}

bool TTabModelInfo::ReadyItemColorTest()
{
	m_PartsColorTestList.clear();

	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) 
		return false;

	vector<string> vecNodeStr;
	vecNodeStr.push_back("hat_item");
	vecNodeStr.push_back("chest_item");
	vecNodeStr.push_back("hands_item");
	vecNodeStr.push_back("legs_item");
	vecNodeStr.push_back("feet_item");
	vecNodeStr.push_back("hat_cloth");
	vecNodeStr.push_back("chest_cloth");

	SetPartsColorTestActorNode(pActor, vecNodeStr);

	return !(m_PartsColorTestList.empty());
}

void TTabModelInfo::SetPartsColorTestActorNode( TCharacter* pActor, vector<string>& vecActorNodeList )
{
	for(vector<string>::iterator it = vecActorNodeList.begin(); it != vecActorNodeList.end(); ++it)
	{
		RActorNode*  pActorNode = pActor->GetActorNode(it->c_str());
		if (pActorNode == NULL) 
			continue;

		m_PartsColorTestList.push_back(pActorNode);
	}
}
