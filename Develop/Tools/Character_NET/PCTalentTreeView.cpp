#include "StdAfx.h"
#include "PCTalentTreeView.h"
#include "TAnimationHelper.h"
#include "TNaming.h"

System::Void Character_NET::PCTalentTreeView::TreeInfoClear()
{
	PCTalentTreeClear();
	InitPCTalentTree();
}

System::Void Character_NET::PCTalentTreeView::PCTalentTreeClear()
{
	treeView1->Nodes->Clear();

	for (int i = TS_NONE; i < TS_MAX; i++)
	{
		String^ TalentStyle = gcnew String(TStrings::TalentStyleToString(i));
		treeView1->Nodes->Add(TalentStyle);
	}

	for( int i = WEAPON_1H_SLASH; i < WEAPON_MAX; i++)
	{
		String^ WeaponType = gcnew String(TStrings::WeaponType(i));
		treeView1->Nodes->Add(WeaponType);
	}
}

System::Void Character_NET::PCTalentTreeView::InitPCTalentTree()
{
	TTalentInfoMgr::iterator it = g_pMainApp->m_TTalentInfoMgr.begin();
	for (; it != g_pMainApp->m_TTalentInfoMgr.end(); it++)
	{
		CSTalentInfo* pTalentInfo= (CSTalentInfo*)(*it).second;
		if (pTalentInfo->m_nID >= MIN_NPC_TALENT_ID) continue;

		String^ pTalentName = gcnew String(pTalentInfo->m_szName.c_str());
		pTalentName += String::Format(" ({0})",pTalentInfo->m_nID);

		if (pTalentInfo->m_nStyle != TS_NONE)
		{
			String^ pTalentStyle = gcnew String(TStrings::TalentStyleToString(pTalentInfo->m_nStyle));
			TreeNode^ pTalentStyleNode = g_pMainApp->SearchNode(treeView1->Nodes, pTalentStyle);
			TreeNode^ pTalentNode = pTalentStyleNode->Nodes->Add(pTalentName);

			InitTalentMode(pTalentInfo, pTalentNode);

		}
		else
		{
			WEAPON_TYPE WeaponType = g_pMainApp->GetWeaponType(pTalentInfo);
			String^ pWeaponType = gcnew String(TStrings::WeaponType(WeaponType));
			TreeNode^ pWeaponTypeNode = g_pMainApp->SearchNode(treeView1->Nodes, pWeaponType);
			TreeNode^ pTalentNode = pWeaponTypeNode->Nodes->Add(pTalentName);

			InitTalentMode(pTalentInfo, pTalentNode);
		}
	}
}

System::Void Character_NET::PCTalentTreeView::InitTalentMode( CSTalentInfo* pTalentInfo, TreeNode^ pTalentNode )
{
	PCTalentTreeNodeTag^ talentNodeTag = gcnew PCTalentTreeNodeTag(NT_TALENT);
	//talentNodeTag->nWeaponType = (WEAPON_TYPE)nWeaponIndex;
	talentNodeTag->nTalentID = pTalentInfo->m_nID;
	pTalentNode->Tag = talentNodeTag;

	if(pTalentInfo->ExistMode())
	{
		for (int nModeIndex = 0; nModeIndex < MAX_TALENT_MODE; nModeIndex++)
		{
			CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(nModeIndex);
			if (pModeTalentInfo == NULL) continue;

			String^ strTalentName = gcnew String(pModeTalentInfo->m_szName.c_str());
			String^ strWeaponType = gcnew String(TStrings::WeaponType(nModeIndex));
			strTalentName += String::Format(" ({0})", strWeaponType);
			TreeNode^ treeNodeModeTalent = pTalentNode->Nodes->Add(strTalentName);

			PCTalentTreeNodeTag^ modeTalentNodeTag = gcnew PCTalentTreeNodeTag(NT_TALENT_MODE);
			//modeTalentNodeTag->nWeaponType = nWeaponType;
			modeTalentNodeTag->nTalentID = pModeTalentInfo->m_nID;
			modeTalentNodeTag->nTalentMode = pModeTalentInfo->m_nMode;
			treeNodeModeTalent->Tag = modeTalentNodeTag;
		}
	}
}

System::Void Character_NET::PCTalentTreeView::treeView1_NodeMouseClick( System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e )
{
	//최상위노드이면
	if( e->Node->Parent == nullptr ) return;

	if( e->Node != nullptr)
	{
		PCTalentTreeNodeTag^ ptag = (PCTalentTreeNodeTag^)e->Node->Parent->Tag;
		PCTalentTreeNodeTag^ tag = (PCTalentTreeNodeTag^)e->Node->Tag;

		// 이미 같은 탤런트다.
		CSTalentInfo* pLastTalentInfo = g_pMainApp->GetTalentInfo();
		if(pLastTalentInfo && tag->nTalentID == pLastTalentInfo->m_nID && tag->nTalentMode == pLastTalentInfo->m_nMode)
		{
			g_pMainApp->OnPlayAnimation();
			return;
		}

		//------------------------------------------------------------------------
		// 데이터 초기화

		// 탤런트 히트
		g_pMainApp->InitTalentHitInfo();

		// 탤런트 이펙트, 이벤트
		g_pMainApp->SetTalent(NULL);

		//------------------------------------------------------------------------
		// 탤런트 정보 입력


		SetWeapon(tag->nTalentID, tag->nTalentMode);

		g_pMainApp->SelectTalent(tag->nTalentID, tag->nTalentMode);

		InitPCTalentAnimationListTree(tag->nTalentID, tag->nTalentMode);

		// 칼 궤적 샘플링
		RAnimation* pAnimation = g_pMainApp->GetActor()->GetCurAnimation();
		if(!pAnimation) return;

		string strModelName = TNaming::GetMeshAliasName(g_pMainApp->GetActor());
		g_pMainApp->m_SwordTrailSampling.StartSwordTrailSampling(strModelName, pAnimation);
	}
}

void Character_NET::PCTalentTreeView::SetWeapon( int nTalentID, int nTalentMode )
{
	CSTalentInfo* pTalentInfo = TTalentHelper::GetTalentInfo(nTalentID, nTalentMode);
	if (pTalentInfo == NULL)
	{
		mlog("에러 : 탈렌트가 없습니다. \r");
		return;
	}

	WEAPON_TYPE weapontype = g_pMainApp->GetActor()->GetCurrentWeaponType();
	bool bEquipShield = g_pMainApp->GetActor()->IsEquipShield();

	if(pTalentInfo->m_WeaponAllowedInfo.IsAllowed(weapontype, bEquipShield) == true) return;

	for( int i = WEAPON_1H_SLASH;i < WEAPON_MAX; ++i)
	{
		if(pTalentInfo->m_WeaponAllowedInfo.IsAllowed(WEAPON_TYPE(i), bEquipShield) == true)
		{
			this->comboBox2->SelectedIndex = i-WEAPON_1H_SLASH;
			return;
		}
	}

	//여기로 오면 혹시 방패가 필요한걸지도...
}

void Character_NET::PCTalentTreeView::FindTalent( System::String^ Talent )
{
	treeView1->Nodes->Clear();

	vector<string> vecFindList;
	TStrings::ParseFindString(Talent, vecFindList);

	TTalentInfoMgr::iterator it = g_pMainApp->m_TTalentInfoMgr.begin();
	for (; it != g_pMainApp->m_TTalentInfoMgr.end(); it++)
	{
		CSTalentInfo* pTalentInfo= (CSTalentInfo*)(*it).second;
		if (pTalentInfo->m_nID >= MIN_NPC_TALENT_ID) continue;

		String^ pTalentName = String::Format("{0} ({1})", gcnew String(pTalentInfo->m_szName.c_str()), pTalentInfo->m_nID);
		String^ pTalentNameForSearch = String::Format("{0} {1}", gcnew String(pTalentInfo->m_szName.c_str()), pTalentInfo->m_nID); // 검색용으로 스트링 만들기

		for(vector<string>::iterator it = vecFindList.begin(); it != vecFindList.end(); ++it)
		{
			String^ strFind = gcnew String(it->c_str());
			bool bEqual = IsEqualString(pTalentNameForSearch, strFind);

			String^ tnstr = String::Format("{0}",pTalentInfo->m_nID);	
			if( tnstr->StartsWith(strFind) || tnstr->EndsWith(strFind) || bEqual )
			{
				if (pTalentInfo->m_nStyle != TS_NONE)
				{
					String^ pTalentStyle = gcnew String(TStrings::TalentStyleToString(pTalentInfo->m_nStyle));
					TreeNode^ pTalentStyleNode = g_pMainApp->SearchNode(treeView1->Nodes, pTalentStyle);
					if (pTalentStyleNode == nullptr)
					{
						pTalentStyleNode = treeView1->Nodes->Add(pTalentStyle);
					}
					TreeNode^ pTalentNode = pTalentStyleNode->Nodes->Add(pTalentName);

					InitTalentMode(pTalentInfo, pTalentNode);
				}
				else
				{
					WEAPON_TYPE WeaponType = g_pMainApp->GetWeaponType(pTalentInfo);
					String^ pWeaponType = gcnew String(TStrings::WeaponType(WeaponType));
					TreeNode^ pWeaponTypeNode = g_pMainApp->SearchNode(treeView1->Nodes, pWeaponType);
					if (pWeaponTypeNode == nullptr)
					{
						pWeaponTypeNode = treeView1->Nodes->Add(pWeaponType);
					}
					TreeNode^ pTalentNode = pWeaponTypeNode->Nodes->Add(pTalentName);

					InitTalentMode(pTalentInfo, pTalentNode);
				}
			}
		}
	}

	treeView1->ExpandAll();
}

Void Character_NET::PCTalentTreeView::InitPCTalentAnimationListTree(int nTalentID, int nTalentMode)
{
	treeView_TalentAnimationList->Nodes->Clear();

	CSTalentInfo* pTalentInfo = TTalentHelper::GetTalentInfo(nTalentID, nTalentMode);
	if (pTalentInfo == NULL)
	{
		mlog("에러 : 탈렌트가 없습니다. \r");
		return;
	}

	TAnimationHelper talentAnimationHelper;

	TALENT_ANIMATION_INFO talentAniInfo;

	TreeNode^ tn = nullptr;

	// Casting
	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, TAT_CASTING);
	treeView_TalentAnimationList->Nodes->Add("Casting");
	tn = g_pMainApp->SearchNode(treeView_TalentAnimationList->Nodes, "Casting");
	SetPCTalentAnimationList(talentAniInfo, tn);	

	// Casting Loop
	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, TAT_CASTINGLOOP);
	treeView_TalentAnimationList->Nodes->Add("Casting Loop");
	tn = g_pMainApp->SearchNode(treeView_TalentAnimationList->Nodes, "Casting Loop");
	SetPCTalentAnimationList(talentAniInfo, tn);	

	// Use
	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, TAT_USE);
	treeView_TalentAnimationList->Nodes->Add("Use");
	tn = g_pMainApp->SearchNode(treeView_TalentAnimationList->Nodes, "Use");
	SetPCTalentAnimationList(talentAniInfo, tn);	

	// Ext
	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, TAT_EXTRA);
	treeView_TalentAnimationList->Nodes->Add("Extra");
	tn = g_pMainApp->SearchNode(treeView_TalentAnimationList->Nodes, "Extra");
	SetPCTalentAnimationList(talentAniInfo, tn);	

	// Ext2
	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, TAT_EXTRA2);
	treeView_TalentAnimationList->Nodes->Add("Extra2");
	tn = g_pMainApp->SearchNode(treeView_TalentAnimationList->Nodes, "Extra2");
	SetPCTalentAnimationList(talentAniInfo, tn);	

	// Ext3
	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, TAT_EXTRA3);
	treeView_TalentAnimationList->Nodes->Add("Extra3");
	tn = g_pMainApp->SearchNode(treeView_TalentAnimationList->Nodes, "Extra3");
	SetPCTalentAnimationList(talentAniInfo, tn);	
}

System::Void Character_NET::PCTalentTreeView::SetPCTalentAnimationList( TALENT_ANIMATION_INFO& talentAnimationInfo, TreeNode^ tn )
{
	String^ TalentListType;

	if(talentAnimationInfo.vecTalentAniInfo.size() == 0)
		return;

	// 모션?
	if(talentAnimationInfo.eType == TALT_MOTION)
	{
		string strMotionName = "motion (" + talentAnimationInfo.strMotionName + ")";
		TalentListType = gcnew String(strMotionName.c_str());
	}
	else if(talentAnimationInfo.eType == TALT_ANIMATION)
	{
		TalentListType = gcnew String("animation");
	}

	tn->Nodes->Add(TalentListType);
	tn = tn->FirstNode;

	WEAPON_TYPE currentWeapontype = g_pMainApp->GetActor()->GetCurrentWeaponType();

	for(vector<WEAPON_ANIMATION_INFO>::iterator itInfo = talentAnimationInfo.vecTalentAniInfo.begin(); itInfo != talentAnimationInfo.vecTalentAniInfo.end(); itInfo++)
	{
		TreeNode^ tnChilde = nullptr;

		if(currentWeapontype != (*itInfo).eWeaponType)
			continue;

		// 애니
		for(vector<ANIMATION_INFO>::iterator itAniInfo = (*itInfo).vecAniInfo.begin(); itAniInfo != (*itInfo).vecAniInfo.end(); itAniInfo++)
		{
			String^ AnimationName = gcnew String((*itAniInfo).strAnimationName.c_str());
			tn->Nodes->Add(AnimationName);
		}
	}
}

void Character_NET::PCTalentTreeView::ReLoadPCTalentInfo()
{
	TreeInfoClear();
	FindTalentName(FindTalentNametextBox->Text);
}
