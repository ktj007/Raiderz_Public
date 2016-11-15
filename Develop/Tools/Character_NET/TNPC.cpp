#include "StdAfx.h"
#include "TNPC.h"
#include "Xmlseral.h"

TNPC::TNPC(void)
: m_NPCID(0)
{
}

void TNPC::CreateColtFile(String^ filename)
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.LinkEndChild(pDec);

	MXmlElement *pMaiet = new MXmlElement(TALENT_XML_TAG_MAIET);
	MXmlElement *pColt = new MXmlElement("COLT");	

	pMaiet->LinkEndChild(pColt);
	xml.LinkEndChild(pMaiet);

	const char* strfilename= MStringToCharPointer(filename);

	xml.SaveFile(strfilename);	

	MFreeCharPointer(strfilename);
}

void TNPC::COLTEdit()
{
	if (m_NPCID == 0) return;
	String^ str = String::Format("../../Dev/GameServer/npc/colt/{0:D7}.colt.xml",m_NPCID);
	if(System::IO::File::Exists (str) == false)
	{
		mlog("정보 : Colt파일이 존재하지 않아 생성합니다.");
		CreateColtFile(str);
	}

	System::Diagnostics::Process::Start(/*"notepad.exe", */str);
}

void TNPC::ScriptEdit()
{
	if (m_NPCID == 0) return;
	String^ str = String::Format("../../Dev/GameServer/npc/NPC_{0:D7}.lua",m_NPCID);
	if(System::IO::File::Exists (str) == false)
	{
		mlog("정보 : 스크립트파일이 존재하지 않아 생성합니다.");
		System::IO::File::CreateText(str);
	}
	
	System::Diagnostics::Process::Start(/*"notepad.exe", */str);
}

//bool TNPC::IsEqualString( System::String^ NPCName, System::String^ FindName )
//{
//	array<String^>^split = NPCName->Split( );
//	IEnumerator^ myEnum = split->GetEnumerator();
//	while ( myEnum->MoveNext() )
//	{
//		String^ s = safe_cast<String^>(myEnum->Current);
//		if(s->StartsWith(FindName) || s->EndsWith(FindName))
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

//void TNPC::FindNPCGroup(System::String^ NPCID)
//{
//	GlobalObjects::g_pMainForm->NPCListtreeView->Nodes->Clear();
//
//	for( TNPCInfoMgr::iterator itor = TNPCInfoMgr::GetInstance().begin();
//		itor != TNPCInfoMgr::GetInstance().end(); ++itor)
//	{
//		TNPCInfo* pNPCInfo = (TNPCInfo*)(*itor).second;
//		String^ strName = gcnew String(pNPCInfo->strName.c_str());
//
//		String^ strNameWithNPCID = gcnew String(strName);
//		strNameWithNPCID = strNameWithNPCID + " (" + pNPCInfo->nID + ")";
//
//		String^ tnstrName = gcnew String(pNPCInfo->strName.c_str());
//		bool bEqual = bEqual = IsEqualString(tnstrName, NPCID);
//
//		String^ tnstr = String::Format("{0}",pNPCInfo->nID);	
//		if( tnstr->StartsWith(NPCID) || tnstr->EndsWith(NPCID) || bEqual )
//		{
//			TreeNode^ nnode = GlobalObjects::g_pMainForm->NPCListtreeView->Nodes->Add(strNameWithNPCID);
//			nnode->Tag = pNPCInfo->nID;
//
//			TreeNode^ skillnode;
//
//			bool bExistTalent = false;
//			for (TTalentInfoMgr::iterator itorTalentInfo = g_pMainApp->m_TTalentInfoMgr.begin();
//				itorTalentInfo != g_pMainApp->m_TTalentInfoMgr.end(); ++itorTalentInfo)
//			{
//				CSTalentInfo* pTalentInfo = static_cast<CSTalentInfo*>((*itorTalentInfo).second);
//				if (pTalentInfo->m_setNPCID.find(pNPCInfo->nID) != pTalentInfo->m_setNPCID.end())
//				{
//					int nSkillID = pTalentInfo->m_nID;
//					if (!bExistTalent)
//					{
//						skillnode = nnode->Nodes->Add("탈렌트");
//						bExistTalent = true;
//					}
//
//					String^ strskillName = gcnew String(pTalentInfo->m_szName.c_str());
//					strskillName = strskillName + "(" + nSkillID + ")";
//					TreeNode^ tnode = skillnode->Nodes->Add(strskillName);
//					tnode->Tag = nSkillID;
//				}
//			}
//		}
//	}
//}

bool TNPC::SetNPC( int NPCID )
{
	TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(NPCID);
	if (pNPCInfo == NULL) return false;

	g_pMainApp->SetTalent(NULL);
	g_pMainApp->GetMotion()->SetWeaponType(WEAPON_NONE);

	char szBuf[_MAX_DIR] = {0,};
	sprintf_s(szBuf, "%s%s%s.elu", PATH_MODEL_TOOL
		, pNPCInfo->strMeshPath.c_str(), pNPCInfo->strMeshName.c_str()
		, pNPCInfo->strMeshName.c_str());

	g_pMainApp->m_TabModel.SetModelType("NPC");
	g_pMainApp->SetModelFullPathName(szBuf);

	bool bLoad = g_pMainApp->MeshFileOpen(szBuf);

	if( !bLoad)
		return false;

	SetNPCScale();

	SetNpcEquipItem(pNPCInfo);

	SetNpcColor(pNPCInfo);

	g_pMainApp->m_TabModelInfo.SetBipTreeView();

	g_pMainApp->MeshInfoRefresh(pNPCInfo->strMeshName);

	g_pMainApp->SetMeshColInfo(gcnew String(pNPCInfo->strMeshName.c_str()));
	g_pMainApp->SetMeshHitInfo(gcnew String(pNPCInfo->strMeshName.c_str()));
	g_pMainApp->SetMeshHitEffectInfo(gcnew String(pNPCInfo->strMeshName.c_str()));

	// 모델리스트 선택
	TreeNode^ tn = g_pMainApp->SearchNode(GlobalObjects::g_pMainForm->m_ModelTabPage->ModeltreeView->Nodes, gcnew String(pNPCInfo->strMeshName.c_str()));
	GlobalObjects::g_pMainForm->m_ModelTabPage->ModeltreeView->SelectedNode = tn;

	return true;
}

void TNPC::SetNPCScale()
{
	if (g_pMainApp->GetActor() == NULL) return;

	TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(m_NPCID);
	if (pNPCInfo == NULL) return;

	g_pMainApp->GetActor()->SetScale(pNPCInfo->fScale);
}

void TNPC::SetNpcEquipItem( TNPCInfo* pNPCInfo )
{
	for (int i = 0; i < TNPCInfo::MESH_PARTS_COUNT; i++)
	{
		if(pNPCInfo->nPartsItemID[i] != 0)
			g_pMainApp->m_EquipItems.EquipItemID(pNPCInfo->nPartsItemID[i], NPCPartsSlot2Slot(i));
	}
	//bool bEquipItem = g_pMainApp->m_EquipItems.EquipItemID(itemid, e->Node->Parent->Index);

}

SH_ITEM_SLOT TNPC::NPCPartsSlot2Slot( int nSlot )
{
	switch(nSlot)
	{
	case NPC_PARTS_SLOT_HAIR:		return ITEMSLOT_CHARM; // (임시 헤러가 없어서)
	case NPC_PARTS_SLOT_FACE:		return ITEMSLOT_FACE;
	case NPC_PARTS_SLOT_HEAD:		return ITEMSLOT_HEAD;
	case NPC_PARTS_SLOT_CHEST:		return ITEMSLOT_BODY;
	case NPC_PARTS_SLOT_LEG:		return ITEMSLOT_LEG;
	case NPC_PARTS_SLOT_HANDS:		return ITEMSLOT_HANDS;
	case NPC_PARTS_SLOT_FEET:		return ITEMSLOT_FEET;
	case NPC_PARTS_SLOT_LWEAPON:	return ITEMSLOT_LWEAPON;
	case NPC_PARTS_SLOT_RWEAPON:	return ITEMSLOT_RWEAPON;
	}

	return ITEMSLOT_MAX;
}

void TNPC::SetNpcColor( TNPCInfo* pNPCInfo )
{
	// 텍스쳐 색깔 
	if (pNPCInfo->IsUseTexColor())
	{
		g_pMainApp->GetActor()->SetColor(pNPCInfo->nTexColor);
	}

	// 인간형 피부색
	if (pNPCInfo->nSkinColor != 0xFFFFFFFF)
	{
		SetSkin(pNPCInfo->nSkinColor);
	}

	// 머리색
	if(pNPCInfo->nHairColor != 0xFFFFFFFF)
	{
		SetHair(pNPCInfo->nHairColor);
	}
}

void TNPC::SetSkin( unsigned int nColor )
{
	RActorNode* pActorNode = g_pMainApp->GetActor()->GetActorNode("hat");
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = g_pMainApp->GetActor()->GetActorNode("face");
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = g_pMainApp->GetActor()->GetActorNode("chest");
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = g_pMainApp->GetActor()->GetActorNode("hands");
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = g_pMainApp->GetActor()->GetActorNode("legs");
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = g_pMainApp->GetActor()->GetActorNode("feet");
	if (pActorNode) pActorNode->SetPartsColor(nColor);
}

void TNPC::SetHair( unsigned int nColor )
{
	RActorNode* pActorNode = g_pMainApp->GetActor()->GetActorNode("hair");
	if (pActorNode) pActorNode->SetPartsColor(nColor);
}