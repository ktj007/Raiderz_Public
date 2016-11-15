#include "StdAfx.h"
#include "ModelInfoTabPage.h"

class TMeshNodeViewerInfo
{
public:
	struct MeshNodeViewData
	{
		string			strMeshNodeName;
		float			m_fOrgVisibility;

		MeshNodeViewData()
		{
			m_fOrgVisibility = 0.0f;
		}
	};

	vector<MeshNodeViewData>		m_vecMeshNodeData;

public:
	TMeshNodeViewerInfo(){}

	void Add(string strName, float fOrg)
	{
		MeshNodeViewData newData;
		newData.strMeshNodeName = strName;
		newData.m_fOrgVisibility = fOrg;

		m_vecMeshNodeData.push_back(newData);
	}

	void Clear()
	{
		m_vecMeshNodeData.clear();
	}

	float GetMeshNodeOrgVisibility(string strName)
	{
		for(vector<MeshNodeViewData>::iterator it = m_vecMeshNodeData.begin(); it != m_vecMeshNodeData.end(); ++it)
		{
			if(it->strMeshNodeName == strName)
				return it->m_fOrgVisibility;
		}

		return 1.0f;
	}

} meshNodeViewerInfo;

//////////////////////////////////////////////////////////////////////////
void Character_NET::ModelInfoTabPage::InitModelInfoTab( const char* FileName )
{
	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) return;

	FileNametextBox->Text = gcnew String(FileName);
	BipCounttextBox->Text = String::Format("{0}", pActor->GetActorNodeCount());
	PolygoncounttextBox->Text = String::Format("{0}",GetActorNodesPolygonCount());
	TexMemtextBox->Text = String::Format("{0} kb", GetTexMemSize());
}

int Character_NET::ModelInfoTabPage::GetActorNodesPolygonCount()
{
	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) return 0;

	int count = 0;
	for(int i = 0; i<pActor->GetActorNodeCount();i++)
	{
		RActorNode *pActorNode = pActor->GetActorNodes()[i];
		count += pActorNode->m_pMeshNode->m_nFaceCnt;
	}
	return count;
}

/************************************************************************/
/* kb로 계산해서 넘김													*/
/************************************************************************/
unsigned long Character_NET::ModelInfoTabPage::GetTexMemSize()
{
	TCharacter* pActor = g_pMainApp->GetActor();
	if (pActor == NULL) return 0;

	unsigned long memsize = 0;
	for(int i = 0; i < pActor->GetActorNodeCount();i++)
	{
		RActorNode *pActorNode = pActor->GetActorNodes()[i];
		for(int j = 0; j < (int)pActorNode->m_vecMaterialTable.size(); ++j)
		{
			RMaterial* pMaterial = pActorNode->m_vecMaterialTable.at(j)->m_pMaterial;
			// TODO: 
			// [4/23/2007 madduck] - 같은 텍스쳐를 두번 계산해서 한번만 돌림. 다를수도 있는데 그땐....우짜지.
			//for( int j = 0; j < 4; j++)
			for( int k = 0; k < 1; k++)
			{
				RMtrlTexInfo* pMtrlTexInfo = pMaterial->GetTexLayer(k);
				if( pMtrlTexInfo == NULL || pMtrlTexInfo->GetTexture() == 0) continue;
				//mlog("텍스쳐 로딩 : %s \r", pMtrlTexInfo->TextureName.c_str());
				RTextureInfo* pTextureInfo = (RTextureInfo*)GetDevice()->GetTextureInfo(pMtrlTexInfo->GetTexture());
				memsize += pTextureInfo->GetSize();
			}
		}
	}
	return memsize/1024;
}

void Character_NET::ModelInfoTabPage::NoneSelectedNodeInfo()
{
	BipParenttextBox->Text = nullptr;
	BipNametextBox->Text = nullptr;

	Mat11textBox->Text = nullptr;
	Mat12textBox->Text = nullptr;
	Mat13textBox->Text = nullptr;
	Mat14textBox->Text = nullptr;
	Mat21textBox->Text = nullptr;
	Mat22textBox->Text = nullptr;
	Mat23textBox->Text = nullptr;
	Mat24textBox->Text = nullptr;
	Mat31textBox->Text = nullptr;
	Mat32textBox->Text = nullptr;
	Mat33textBox->Text = nullptr;
	Mat34textBox->Text = nullptr;
	Mat41textBox->Text = nullptr;
	Mat42textBox->Text = nullptr;
	Mat43textBox->Text = nullptr;
	Mat44textBox->Text = nullptr;
}

void Character_NET::ModelInfoTabPage::SetSelectedNodeInfo( String^ nodeName )
{
	BipNametextBox->Text = nodeName;

	RActorNode*  pActorNode = g_pMainApp->GetActorNode(MStringToCharPointer(nodeName));
	if (pActorNode == NULL) return;

	if(pActorNode->GetParentActorNode() == NULL)
	{
		BipParenttextBox->Text = nullptr;
		return;
	}
	else
	{
		BipParenttextBox->Text = gcnew String(pActorNode->GetParentActorNode()->GetNodeName().c_str());
	}

	RMatrix* matinverse	= &pActorNode->GetBaseLocalTransform();
	//OffsetTm = 뼈의 초기 월드 행렬(움직임이 없는 고정된 상태에서 계산된 뼈 월드 행렬)의 역행렬
	//현재 그냥 local 행렬

	Mat11textBox->Text = String::Format("{0:F}", matinverse->_11);
	Mat12textBox->Text = String::Format("{0:F}", matinverse->_12);
	Mat13textBox->Text = String::Format("{0:F}", matinverse->_13);
	Mat14textBox->Text = String::Format("{0:F}", matinverse->_14);
	Mat21textBox->Text = String::Format("{0:F}", matinverse->_21);
	Mat22textBox->Text = String::Format("{0:F}", matinverse->_22);
	Mat23textBox->Text = String::Format("{0:F}", matinverse->_23);
	Mat24textBox->Text = String::Format("{0:F}", matinverse->_24);
	Mat31textBox->Text = String::Format("{0:F}", matinverse->_31);
	Mat32textBox->Text = String::Format("{0:F}", matinverse->_32);
	Mat33textBox->Text = String::Format("{0:F}", matinverse->_33);
	Mat34textBox->Text = String::Format("{0:F}", matinverse->_34);
	Mat41textBox->Text = String::Format("{0:F}", matinverse->_41);
	Mat42textBox->Text = String::Format("{0:F}", matinverse->_42);
	Mat43textBox->Text = String::Format("{0:F}", matinverse->_43);
	Mat44textBox->Text = String::Format("{0:F}", matinverse->_44);
}

System::Void Character_NET::ModelInfoTabPage::MeshNode_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	String^ strSelectMeshNodeName = MeshNode_comboBox->Text;

	int nCount = MeshNode_comboBox->Items->Count;
	for(int i = 0; i < nCount; i++)
	{
		String^ itemString = MeshNode_comboBox->Items[i]->ToString();

		RActorNode* pActorNode = g_pMainApp->GetActor()->GetActorNode( MStringToCharPointer(itemString));

		if(pActorNode)
		{
			if(strSelectMeshNodeName == itemString ||
				strSelectMeshNodeName == gcnew String("all"))
			{
				pActorNode->SetVisible(true);

				if(pActorNode->GetVisibility() == 0.0f)
					pActorNode->SetVisibility(1.0f);

				if(strSelectMeshNodeName == gcnew String("all"))
				{
					float fOrgVisibility = meshNodeViewerInfo.GetMeshNodeOrgVisibility(MStringToCharPointer(itemString));
					if(fOrgVisibility == 0.0f)
						pActorNode->SetVisibility(0.0f);
				}
			}
			else
			{
				pActorNode->SetVisible(false);

				float fOrgVisibility = meshNodeViewerInfo.GetMeshNodeOrgVisibility(MStringToCharPointer(itemString));
				pActorNode->SetVisibility( fOrgVisibility);
			}

		}
	}
}

void Character_NET::ModelInfoTabPage::InitMeshNodeInfo()
{
	meshNodeViewerInfo.Clear();
	int nCount = MeshNode_comboBox->Items->Count;
	for(int i = 0; i < nCount; i++)
	{
		String^ itemString = MeshNode_comboBox->Items[i]->ToString();

		RActorNode* pActorNode = g_pMainApp->GetActor()->GetActorNode( MStringToCharPointer(itemString));
		if(pActorNode)
		{
			meshNodeViewerInfo.Add(MStringToCharPointer(itemString), pActorNode->GetVisibility());
		}
	}
}
