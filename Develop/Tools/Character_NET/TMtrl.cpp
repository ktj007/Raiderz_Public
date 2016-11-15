#include "StdAfx.h"
#include "TMtrl.h"
#include "RSceneManager.h"
#include "RMaterialResource.h"
#include "RMaterialResourceMgr.h"

TMtrl::TMtrl(void)
{
	Init();
}

TMtrl::~TMtrl(void)
{
	m_MaterialTable.clear();
}

void TMtrl::Init()
{
	m_nItemID = 0;
}

bool TMtrl::Export_NPC()
{
	TCharacter* pActor = g_pMainApp->GetActor();

	MXml xml;
	if (xml.LoadFile(FILENAME_NPC_CHANGEMTRL_TOOL) == false)
	{
		MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "euc-kr", "");
		xml.Doc()->LinkEndChild(pDec);

		MXmlElement *pRootElement = new MXmlElement(NPC_XML_TAG_MAIET);
		xml.Doc()->LinkEndChild(pRootElement);
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pRootElement = docHandle.FirstChild(NPC_XML_TAG_MAIET).Element();

	MXmlElement* pElement = pRootElement->FirstChildElement();
	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		int nNPCID;
		_Attribute(&nNPCID,	pElement, NPC_XML_ATTR_ID);

		if( g_pMainApp->m_NPC.IsValidNPCID(nNPCID) )
		{
			TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(nNPCID);
			if (pNPCInfo == NULL)	return false;

			MXmlElement *pMeshElement = pElement->FirstChildElement();

			for( int i = 0; i < pActor->GetActorNodeCount(); ++i )
			{
				RActorNode *pActorNode = pActor->GetActorNodes()[i];

				// Dummy나 Bone은 출력하지 않는다.
				if( pActorNode->m_pMeshNode->isBone() || pActorNode->m_pMeshNode->isDummyMesh() )
					continue;

				MXmlElement *pMaterialElement = pMeshElement->FirstChildElement();
				for( int j = 0; j < pActorNode->m_pMeshNode->m_nMtrlCnt; ++j )
				{
					_SetAttribute(pMaterialElement,"SubID", j);
					_SetAttribute(pMaterialElement,"MtrlName", pActorNode->m_vecMaterialTable.at(j)->m_pMaterial->GetName().c_str());

					pMaterialElement = pMaterialElement->NextSiblingElement();
				}

				pMeshElement = pMeshElement->NextSiblingElement();
			}

			xml.SaveFile(FILENAME_NPC_CHANGEMTRL_TOOL);

			System::Windows::Forms::MessageBox::Show(gcnew String("NPC Export !!"), "완료");
			TNPCInfoMgr::GetInstance().Load();

			return true;
		}
	}

	MXmlElement *pNPCElement = new MXmlElement(NPC_XML_TAG_NPC);
	pRootElement->LinkEndChild(pNPCElement);
	AddSubMtrl_NPC(pNPCElement, g_pMainApp->m_NPC.GetNPCID());

	xml.SaveFile(FILENAME_NPC_CHANGEMTRL_TOOL);

	System::Windows::Forms::MessageBox::Show(gcnew String("NPC Export !!"), "완료");
	TNPCInfoMgr::GetInstance().Load();

	return true;
}

bool TMtrl::AddSubMtrl_NPC(MXmlElement* pNPCElement, int nNPCID)
{
	TCharacter* pActor = g_pMainApp->GetActor();

	//	if(pNPCInfo->bIsSubMtrl == false)	// 재질 변경 내역이 없으면 패스
	//		continue;

	TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(nNPCID);
	if (pNPCInfo == NULL)	return false;

	_SetAttribute(pNPCElement,NPC_XML_ATTR_ID, pNPCInfo->nID);
	_SetAttribute(pNPCElement,NPC_XML_ATTR_NAME, pNPCInfo->strName.c_str());
	//	_SetAttribute(pNPCElement,NPC_XML_ATTR_MODEL, pNPCInfo->strModelFolder.c_str());
	_SetAttribute(pNPCElement,NPC_XML_ATTR_MESH_NAME, pNPCInfo->strMeshName.c_str());	// .elu file name

	//	MXmlElement *pMeshListElement = new MXmlElement("MESHLIST");
	//	pNPCElement->LinkEndChild(pMeshListElement);
	//	_SetAttribute(pMeshListElement,"Count", m_pActor->GetActorNodeCount());
	for( int i = 0; i < pActor->GetActorNodeCount(); ++i )
	{
		RActorNode *pActorNode = pActor->GetActorNodes()[i];
		// Dummy나 Bone은 출력하지 않는다.
		if( pActorNode->m_pMeshNode->isBone() || pActorNode->m_pMeshNode->isDummyMesh() )
			continue;

		MXmlElement *pMeshElement = new MXmlElement("MESH");
		pNPCElement->LinkEndChild(pMeshElement);
		_SetAttribute(pMeshElement, "index", i);

		// <Sub>
		_SetAttribute(pMeshElement, "SubCount", pActorNode->m_pMeshNode->m_nMtrlCnt);
		for( int j = 0; j < pActorNode->m_pMeshNode->m_nMtrlCnt; ++j )
		{
			MXmlElement *pMaterialElement = new MXmlElement("MATERIAL");
			pMeshElement->LinkEndChild(pMaterialElement);	
			_SetAttribute(pMaterialElement,"SubID", j);
			_SetAttribute(pMaterialElement,"MtrlName", pActorNode->m_vecMaterialTable[j]->m_pMaterial->GetName().c_str());
		}
	}

	return true;
}

bool TMtrl::Export_Item()
{
	MXml xml;
	if (xml.LoadFile(FILENAME_ITEM_CHANGEMTRL_TOOL) == false)
	{
		MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "euc-kr", "");
		xml.Doc()->LinkEndChild(pDec);

		MXmlElement *pRootElement = new MXmlElement(NPC_XML_TAG_MAIET);
		xml.Doc()->LinkEndChild(pRootElement);
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pRootElement = docHandle.FirstChild(NPC_XML_TAG_MAIET).Element();

	MXmlElement* pElement = pRootElement->FirstChildElement();
	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		int nItemID;
		_Attribute(&nItemID, pElement, NPC_XML_ATTR_ID);

		if( nItemID == m_nItemID )
		{
			CSItemData* pItem = g_pMainApp->GetItemMgr()->GetItemData(nItemID);
			if (pItem == NULL)	
				return false;

			// <ITEM id="1010" name="소울 프라이머 모자" mesh_name="soul_primer" mesh_name_by_sex="True" node_name="hat" type="equipment" sex="All" slot="head" buyprice="1000" sellprice="500" weight="9" max_durability="50" AP="5" />
			MXmlElement *pMeshElement = pElement->FirstChildElement();
			MXmlElement *pMaterialElement = pMeshElement->FirstChildElement();
			for( size_t j = 0; j < g_pMainApp->m_pSelectedActorNode->m_vecMaterialTable.size(); ++j )
			{
				_SetAttribute(pMaterialElement,"SubID", (int)j);
				_SetAttribute(pMaterialElement,"MtrlName", g_pMainApp->m_pSelectedActorNode->m_vecMaterialTable.at(j)->m_pMaterial->GetName());

				pMaterialElement = pMaterialElement->NextSiblingElement();
			}

			pMeshElement = pMeshElement->NextSiblingElement();

			xml.SaveFile(FILENAME_ITEM_CHANGEMTRL_TOOL);

			System::Windows::Forms::MessageBox::Show(gcnew String("Item Export !!"), "완료");
			TItemManager* pItemMgr = g_pMainApp->GetItemMgr();
			if ( !pItemMgr->LoadFromFile_SubMtrl(FILENAME_ITEM_CHANGEMTRL_TOOL) )
				mlog("에러 : pItemMgr->LoadFromFile() - 아이템 재질 테이블 불러오기 실패");

			return true;
		}
	}

	MXmlElement *pItemElement = new MXmlElement(NPC_XML_TAG_ITEM);
	if(AddSubMtrl_Item(pItemElement, m_nItemID) == false)
	{
		SAFE_DELETE(pItemElement);
		return false;
	}
	else
	{
		pRootElement->LinkEndChild(pItemElement);
	}

	xml.SaveFile(FILENAME_ITEM_CHANGEMTRL_TOOL);

	System::Windows::Forms::MessageBox::Show(gcnew String("Item Export !!"), "완료");
	TItemManager* pItemMgr = g_pMainApp->GetItemMgr();
	if ( !pItemMgr->LoadFromFile_SubMtrl(FILENAME_ITEM_CHANGEMTRL_TOOL) )
		mlog("에러 : pItemMgr->LoadFromFile() - 아이템 재질 테이블 불러오기 실패");

	return true;
}

bool TMtrl::AddSubMtrl_Item(MXmlElement* pItemElement, int nItemID)
{
	CSItemData* pItem = g_pMainApp->GetItemMgr()->GetItemData(nItemID);
	if (pItem == NULL)	
		return false;

	if( g_pMainApp->m_pSelectedActorNode == NULL )
	{
		System::Windows::Forms::MessageBox::Show(gcnew String("선택된 노드가 없습니다. 다시 선택해주세요"), "실패");
		return false;
	}

	_SetAttribute(pItemElement,NPC_XML_ATTR_ID, pItem->m_nID);
	_SetAttribute(pItemElement,NPC_XML_ATTR_NAME, pItem->m_strName.c_str());
	//	_SetAttribute(pItemElement,NPC_XML_ATTR_MESH_NAME, pItem->m_strMeshName.c_str());

	if (!pItem->m_strNodeNames.empty())
	{
		_SetAttribute(pItemElement,NPC_XML_ATTR_MESH_NODE_NAME, pItem->m_strNodeNames[1].c_str());
	}

	int nCurrentID = -1;	// ActorNode ID
	RActorNode *pActorNode = 0;
	for( int i = 0; i < g_pMainApp->GetActor()->GetActorNodeCount(); ++i )
	{
		pActorNode = g_pMainApp->GetActor()->GetActorNodes()[i];

		// Dummy나 Bone은 출력하지 않는다.
		if( pActorNode->m_pMeshNode->isBone() || pActorNode->m_pMeshNode->isDummyMesh() )
			continue;

		if( stricmp( pItem->m_strNodeNames[1].c_str(), pActorNode->m_strName.c_str() ) == 0 )
			nCurrentID = i;
	}

	if(nCurrentID < 0)
		return false;

	pActorNode = g_pMainApp->GetActor()->GetActorNodes()[nCurrentID];

	MXmlElement *pMeshElement = new MXmlElement("MESH");
	pItemElement->LinkEndChild(pMeshElement);
	_SetAttribute(pMeshElement, "index", nCurrentID);

	// <Sub>
	_SetAttribute(pMeshElement, "SubCount", (int)g_pMainApp->m_pSelectedActorNode->m_vecMaterialTable.size());
	for( size_t j = 0; j < g_pMainApp->m_pSelectedActorNode->m_vecMaterialTable.size(); ++j )
	{
		MXmlElement *pMaterialElement = new MXmlElement("MATERIAL");
		pMeshElement->LinkEndChild(pMaterialElement);	
		_SetAttribute(pMaterialElement,"SubID", (int)j);
		_SetAttribute(pMaterialElement,"MtrlName", g_pMainApp->m_pSelectedActorNode->m_vecMaterialTable.at(j)->m_pMaterial->GetName());
	}

	return true;
}

void TMtrl::SetItemDataSubMtrl( int nItemID )
{
	m_nItemID = nItemID;

	CSItemData* pItem = g_pMainApp->GetItemMgr()->GetItemData(nItemID);
	if (pItem == NULL)	
		return ;

	for ( int i = 0; i < (int)pItem->m_vecMtrls.size(); ++i )
	{
		for ( int j = 0; j < pItem->m_vecMtrls[i]->nSubCount; ++j )
		{
			g_pMainApp->GetActor()->ChangeMatetial(pItem->m_strNodeNames[1].c_str(), j, pItem->m_vecMtrls[i]->vecSubMtrls[j]);
		}
	}
}

void TMtrl::SetNPCInfoSubMtrl( int NPCID )
{
	TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(NPCID);
	if (pNPCInfo == NULL)	return;

	//if( pNPCInfo->vecMtrls.size() == 0 )
	//{
	//	for( int i = 0; i < (int)g_pMainApp->GetActor()->GetActorNodeCount(); ++i )
	//	{
	//		RActorNode *pActorNode = g_pMainApp->GetActor()->GetActorNodes()[i];

	//		// Dummy나 Bone은 출력하지 않는다.
	//		if( pActorNode->m_pMeshNode->isBone() || pActorNode->m_pMeshNode->isDummyMesh() )
	//			continue;

	//		char szSubName[10];
	//		for( int j = 0; j < pActorNode->m_pMeshNode->m_nMtrlCnt; ++j )
	//		{
	//			sprintf(szSubName, ".%d", j);
	//			string szMeshName = pActorNode->m_pMeshNode->m_pBaseMesh->GetName() + szSubName;
	//			RMaterialResource* pMaterialResource = REngine::GetSceneManager().CreateResource< RMaterialResource >( szMeshName.c_str() );

	//			int nMaterialID = pActorNode->m_pMeshNode->m_pMtrlTableInfo[j].mtrlid;
	//			pActorNode->m_vecMaterialTable[j] = pMaterialResource;
	//			pActorNode->m_vecMaterialTable[j]->m_pMaterial->nUserIndex = nMaterialID;

	//			REngine::GetSceneManager().ReleaseResource( pMaterialResource );
	//		}
	//		pActorNode->OnCreateMaterials();
	//	}
	//}

	for ( int i = 0; i < (int)pNPCInfo->vecMtrls.size(); ++i )
	{
		for ( int j = 0; j < pNPCInfo->vecMtrls[i]->nSubCount; ++j )
		{
			// RActor 를 통해서 호출되어야 한다
			g_pMainApp->GetActor()->ChangeMatetial(pNPCInfo->vecMtrls[i]->nID, j, pNPCInfo->vecMtrls[i]->vecSubMtrls[j]);
		}
	}
}

//************************************
// Method:    ExportMtrlSet
// FullName:  CMainApplication::ExportMtrlSet
// Access:    public 
// Returns:   void
// Qualifier: 교체 가능한 Material 정보를 XML 데이터로 출력 한다.
//************************************
bool TMtrl::ExportMtrlSet()
{
	//if( g_pMainApp->m_SelectedTab == TAB_MTRLSTATE_ITEM )
	//{
	//	if(Export_Item())
	//		GlobalObjects::g_SaveNotifier.SetSaveChangeMtrlItem(false);
	//}
	//else 
	if( g_pMainApp->m_SelectedTab == TAB_MTRLSTATE_NPC )
	{
		if(Export_NPC())
			GlobalObjects::g_SaveNotifier.SetSaveChangeMtrlNPC(false);
	}
	else
	{
		if(Export_Item())
			GlobalObjects::g_SaveNotifier.SetSaveChangeMtrlItem(false);
	}

	return true;
}

bool TMtrl::SetMeshTreeView()
{
	GlobalObjects::g_pMainForm->treeViewMeshList->Nodes->Clear();

	//if( g_pMainApp->m_SelectedTab == TAB_MTRLSTATE_ITEM )
	if( g_pMainApp->m_SelectedTab2 == TAB_TABSTATE_ITEM )	// ITEM Page인가
	{
		return SetMeshTreeView_Item();
	}
	else if( g_pMainApp->m_SelectedTab == TAB_MTRLSTATE_NPC )
	{
		return SetMeshTreeView_NPC();
	}

	return true;
}

bool TMtrl::SetMeshTreeView_Item()
{
	TCharacter* pActor = g_pMainApp->GetActor();

	if(m_nItemID <= 0)
	{
		mlog("에러 : 선택된 아이템이 없습니다!!");
		return false;
	}

	CSItemData* pItem = g_pMainApp->GetItemMgr()->GetItemData(m_nItemID);
	if (pItem == NULL)	
	{
		mlog("에러 : %s 아이템이 없습니다!!", m_nItemID);
		return false;
	}

	string strNodeName;
	// [0] : hat / [1] : hat_item
	// 위와 같이 두개가 존재하는데 실제 메쉬의 이름은 hat_item 을 사용한다.
	// 그런데 장착 메쉬 같은 경우에는 장착후에 메쉬가 붙기 때문에 [0]hp_lweapon 과 같은 0번 메쉬만 존재한다.
	if( pItem->m_strNodeNames.size() > 1 )	// 하나 이상일때 즉, [1]이 존재하는가
		strNodeName = pItem->m_strNodeNames[1];
	else
		strNodeName = pItem->m_strNodeNames[0];

	RActorNode*		pActorNode	= pActor->GetActorNode( strNodeName.c_str() );
	if(pActorNode == NULL)
	{
		mlog("에러 : %s 액터노드를 찾을수 없습니다!! 성별이 틀릴수 있음!!", strNodeName.c_str());
		return false;
	}

	// 일반적인 Mesh 라면
	String^ nodename = gcnew String(pActorNode->m_pMeshNode->m_Name.c_str());

	// 붙을 부모 collection 을 선택
	TreeNodeCollection^ tnParentCollection; 
	tnParentCollection = GlobalObjects::g_pMainForm->treeViewMeshList->Nodes;

	// 새 트리 node 를 등록한다
	TreeNode^ node = gcnew TreeNode( nodename );
	node->Tag = pActorNode->m_pMeshNode->m_nID;
	tnParentCollection->Add( node );

	TreeNode^ treeNode = SearchNode(GlobalObjects::g_pMainForm->treeViewMeshList->Nodes, nodename);
	if(treeNode == nullptr)
	{
		mlog("에러 : tree node not found");
	}
	else
		tnParentCollection = treeNode->Nodes;

	for( int j = 0; j < pActorNode->m_pMeshNode->m_nMtrlCnt; ++j )
	{
		String^ strMtrlID = String::Format("MtrlID : {0}", j+1);
		TreeNode^ nodeMtrlID = gcnew TreeNode( strMtrlID );
		tnParentCollection->Add( nodeMtrlID );

		treeNode = SearchNode(GlobalObjects::g_pMainForm->treeViewMeshList->Nodes, strMtrlID);
		if (treeNode == nullptr)
		{
			continue;
		}

		TreeNode^ nodeTexname = gcnew TreeNode( gcnew String(pActorNode->m_vecMaterialTable.at(j)->m_pMaterial->GetName().c_str()) );
		nodeTexname->Tag = j;	// pMtrlTable의 Mtrl Index로 사용한다.
		treeNode->Nodes->Add( nodeTexname );
	}

	GlobalObjects::g_pMainForm->treeViewMeshList->ExpandAll();

	return true;
}

bool TMtrl::SetMeshTreeView_NPC()
{
	TCharacter* pActor = g_pMainApp->GetActor();

	if(pActor == NULL)
		return false;

	for( int i = 0; i < pActor->GetActorNodeCount(); ++i )
	{
		RActorNode *pActorNode = pActor->GetActorNodes()[i];

		// Dummy나 Bone은 출력하지 않는다.
		if( pActorNode->m_pMeshNode->isBone() || pActorNode->m_pMeshNode->isDummyMesh() )
			continue;

		// 일반적인 Mesh 라면
		String^ nodename = gcnew String(pActorNode->m_pMeshNode->m_Name.c_str());

		// 붙을 부모 collection 을 선택
		TreeNodeCollection^ tnParentCollection; 
		tnParentCollection = GlobalObjects::g_pMainForm->treeViewMeshList->Nodes;

		// 새 트리 node 를 등록한다
		TreeNode^ node = gcnew TreeNode( nodename );
		node->Tag = pActorNode->m_pMeshNode->m_nID;

		TreeNode^ nodeExist = SearchNode(GlobalObjects::g_pMainForm->treeViewMeshList->Nodes, nodename);
		if (nodeExist != nullptr)
			continue;

		tnParentCollection->Add( node );

		TreeNode^ treeNode = SearchNode(GlobalObjects::g_pMainForm->treeViewMeshList->Nodes, nodename);
		if(treeNode == nullptr)
		{
			mlog("에러 : tree node not found");
		}
		else
			tnParentCollection = treeNode->Nodes;

		for( int j = 0; j < pActorNode->m_pMeshNode->m_nMtrlCnt; ++j )
		{
			String^ strMtrlID = String::Format("MaterialID : {0}", j+1);	// 표기는 1부터
			TreeNode^ nodeMtrl = gcnew TreeNode( strMtrlID );
			tnParentCollection->Add( nodeMtrl );

			//treeNode = SearchNode(GlobalObjects::g_pMainForm->treeViewMeshList->Nodes, strMtrlID);
			//if (treeNode == nullptr)
			//	continue;

			TreeNode^ nodeTexname = gcnew TreeNode( gcnew String(pActorNode->m_vecMaterialTable.at(j)->m_pMaterial->GetName().c_str()) );
			nodeTexname->Tag = j;	// pMtrlTable의 Mtrl Index로 사용한다.
			nodeMtrl->Nodes->Add( nodeTexname );
		}
	}

	GlobalObjects::g_pMainForm->treeViewMeshList->ExpandAll();	// 모두 펼침

	return true;
}

bool TMtrl::ChangeMaterialSet()
{
	//if( g_pMainApp->m_SelectedTab == TAB_MTRLSTATE_ITEM )
	//if( g_pMainApp->m_SelectedTab2 == TAB_TABSTATE_ITEM )	// ITEM Page인가
	//{
	//	ChangeMaterialSet_Item();
	//	GlobalObjects::g_SaveNotifier.SetSaveChangeMtrlItem(true);
	//}
	//else 
	if( g_pMainApp->m_SelectedTab == TAB_MTRLSTATE_NPC )
	{
		ChangeMaterialSet_NPC();
		GlobalObjects::g_SaveNotifier.SetSaveChangeMtrlNPC(true);
	}
	else
	{
		ChangeMaterialSet_Item();
		GlobalObjects::g_SaveNotifier.SetSaveChangeMtrlItem(true);
	}

	return true;
}

bool TMtrl::ChangeMaterialSet_Item()
{
	if(!g_pMainApp->m_pSelectedActorNode)
		return false;

	TCharacter* pActor = g_pMainApp->GetActor();

	CSItemData* pItem = g_pMainApp->GetItemMgr()->GetItemData(m_nItemID);
	if (pItem == NULL)	return false;

	RActorNode* pActorNode = pActor->GetActorNode(pItem->m_strNodeNames[1].c_str());
	if(pActorNode == NULL) return false;

	// 현재 선택된 treeviewMeshList 의 항목을 얻는다.
	TreeNode^ treeNode = GlobalObjects::g_pMainForm->treeViewMeshList->SelectedNode;
	if( treeNode == nullptr || treeNode->GetNodeCount(false) ) // 마지막 노드인가
		return false;

	// 변경 시킬 Material은 SubMtrlListBox 에서 얻는다. 이것은 재질의 이름 정보이다.
	Object^ SelectedItem	= GlobalObjects::g_pMainForm->listBoxMtrlList->SelectedItem;
	String^ strSelectedItem = SelectedItem->ToString();

	// 이름 정보로 실제 RMaterial 정보를 얻는다.
	int nIndex = GlobalObjects::g_pMainForm->listBoxMtrlList->Items->IndexOf(strSelectedItem);
	g_pMainApp->GetActor()->ChangeMatetial( pActorNode->m_strName, (int)treeNode->Tag, nIndex );


	return true;
}

bool TMtrl::ChangeMaterialSet_NPC()
{
	if(!g_pMainApp->m_pSelectedActorNode)
		return false;

	// 현재 선택된 treeviewMeshList 의 항목을 얻는다.
	TreeNode^ treeNode = GlobalObjects::g_pMainForm->treeViewMeshList->SelectedNode;
	if( treeNode == nullptr && treeNode->GetNodeCount(false) ) // 마지막 노드 이어야 한다.
		return false;

	// 변경 시킬 Material은 SubMtrlListBox 에서 얻는다. 이것은 재질의 이름 정보이다.
	Object^ SelectedItem	= GlobalObjects::g_pMainForm->listBoxMtrlList->SelectedItem;
	String^ strSelectedItem = SelectedItem->ToString();

	// 이름 정보로 실제 RMaterial 정보를 얻는다.
	int nIndex = GlobalObjects::g_pMainForm->listBoxMtrlList->Items->IndexOf(strSelectedItem);
	g_pMainApp->GetActor()->ChangeMatetial( g_pMainApp->m_pSelectedActorNode->m_strName, (int)treeNode->Tag, nIndex, 3.0f );

	return true;
}

bool TMtrl::SetMtrlList()
{
	GlobalObjects::g_pMainForm->listBoxMtrlList->Items->Clear();
	if(!g_pMainApp->m_pSelectedActorNode)
		return false;

	for( size_t i = 0; i < m_MaterialTable.size(); ++i )
	{
		// 08.04.21
		RMaterial* pMaterial = m_MaterialTable.at(i)->m_pMaterial;
		//RMaterial* pMaterial = m_pActor->m_pMesh->m_pMtrlList->at(i);
		mlog("정보 : %d", pMaterial->IsUseUVAnimation());
		String^ nodename = gcnew String(pMaterial->GetName().c_str());
		//String^ nodename = gcnew String(pMaterial->GetTexLayerByType(RTT_DIFFUSE)->TextureName.c_str());

		GlobalObjects::g_pMainForm->listBoxMtrlList->Items->Add(nodename);
	}

	return true;
}

//************************************
// Method:    SetCurrentMaterialList
// FullName:  CMainApplication::SetCurrentMaterialList
// Access:    public 
// Returns:   void
// Qualifier: 현재 선택된 아이템의 BaseMesh의 m_pMaterialList의 XML 데이터를 Material Resource로 생성한다.
//			  생성한 Resource는 m_CurrentMaterialList에서 관리한다.
//************************************
void TMtrl::SetCurrentMaterialList( int nItemID )
{
	// Material Data가 있으면 삭제
	// Material Resource 삭제
	m_MaterialTable.clear();
	//	for(int i = 0; i < (int)m_MaterialTable.size(); ++i)
	//	{
	//		m_pSceneManager->ReleaseResource( m_MaterialTable[i] );
	//	}	

	//if( g_pMainApp->m_SelectedTab == TAB_MTRLSTATE_ITEM )
	if( g_pMainApp->m_SelectedTab2 == TAB_TABSTATE_ITEM )	// ITEM Page인가
	{
		SetCurrentMaterialList_Item(nItemID);
	}
	else if( g_pMainApp->m_SelectedTab == TAB_MTRLSTATE_NPC )
	{
		SetCurrentMaterialList_NPC(nItemID);
	}
}

void TMtrl::SetCurrentMaterialList_Item( int nItemID )
{
	CSItemData* pItem = g_pMainApp->GetItemMgr()->GetItemData(nItemID);
	if(!pItem) return;

	TCharacter* pActor = g_pMainApp->GetActor();

	string strNodeName;
	// [0] : hat / [1] : hat_item
	// 위와 같이 두개가 존재하는데 실제 메쉬의 이름은 hat_item 을 사용한다.
	// 그런데 장착 메쉬 같은 경우에는 장착후에 메쉬가 붙기 때문에 [0]hp_lweapon 과 같은 0번 메쉬만 존재한다.
	if( pItem->m_strNodeNames.size() > 1 )	// 하나 이상일때 즉, [1]이 존재하는가
		strNodeName = pItem->m_strNodeNames[1];
	else
		strNodeName = pItem->m_strNodeNames[0];

	RActorNode*		pActorNode	= pActor->GetActorNode( strNodeName.c_str() );
	if(pActorNode == NULL)
		return;
	mlog(pActorNode->m_strName.c_str());
	RMeshNode*		pMeshNode	= pActorNode->m_pMeshNode;

	char szSubName[10];
	for( int i = 0; i < (int)pMeshNode->m_pBaseMesh->GetMaterialResourceCount(); ++i )
	{
		sprintf(szSubName, ".%d", i);
		string szMeshName = pMeshNode->m_pBaseMesh->GetName() + szSubName;
		RMaterialResource* pMaterialResource = g_pMainApp->GetSceneMgr()->CreateResource< RMaterialResource >( szMeshName.c_str() );
		//pMaterialResource->BuildAll();
		// 		if( pMaterialResource->GetState() == RResource::RRS_EMPTY )
		// 		{
		// 			pMaterialResource->Fill();
		// 			pMaterialResource->Load();
		// 		}
		m_MaterialTable.push_back(pMaterialResource);
		g_pMainApp->GetSceneMgr()->ReleaseResource(pMaterialResource);
	}
}

void TMtrl::SetCurrentMaterialList_NPC( int nNPCID )
{
	TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(nNPCID);
	if (pNPCInfo == NULL)	return;

	TCharacter* pActor = g_pMainApp->GetActor();

	RActorNode*		pActorNode	= pActor->GetActorNodes()[0];	// 같은 메쉬노드를 참조하기 때문에 아무 액터노드나 얻어온다.
	if(pActorNode == NULL)
		return;
	RMeshNode*		pMeshNode	= pActorNode->m_pMeshNode;

	char szSubName[10];
	for( int i = 0; i < (int)pMeshNode->m_pBaseMesh->GetMaterialResourceCount(); ++i )
	{
		sprintf(szSubName, ".%d", i);
		string szMeshName = pMeshNode->m_pBaseMesh->GetName() + szSubName;
		RMaterialResource* pMaterialResource = g_pMainApp->GetSceneMgr()->CreateResource< RMaterialResource >( szMeshName.c_str() );
		//pMaterialResource->BuildAll();
		// 		if( pMaterialResource->GetState() == RResource::RRS_EMPTY )
		// 		{
		// 			pMaterialResource->Fill();
		// 			pMaterialResource->Load();
		// 		}
		m_MaterialTable.push_back(pMaterialResource);
		g_pMainApp->GetSceneMgr()->ReleaseResource(pMaterialResource);
	}
}