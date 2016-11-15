#include "stdafx.h"
#include "TTalentEffectTabController.h"
#include "TTalentEffectViewer.h"
#include "TTalentEffectMgr.h"
#include "TTalentHelper.h"
#include "TClipboard.h"
#include "DereferenceTalentIDViewer.h"

TTalentEffectTabController::TTalentEffectTabController() : m_pTalentEffectViewer(NULL)
{

}

TTalentEffectTabController::~TTalentEffectTabController()
{

}

void TTalentEffectTabController::Init(TTalentEffectMgr* pTalentEffectMgr, TTalentEffectViewer* pTalentEffectViewer)
{
	m_pTalentEffectMgr = pTalentEffectMgr;
	m_pTalentEffectViewer = pTalentEffectViewer;
}

void TTalentEffectTabController::Serialize()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	string strTalentID = TStrings::TalentIDString_WithMode(pTalentInfo->m_nID, pTalentInfo->m_nMode);
	GlobalObjects::g_pMainForm->CurrentID->Text = String::Format("{0}", gcnew String(strTalentID.c_str()));

	UpdateReferenceControls();
	UpdateTreeView();
	UpdatePropertyGrid();
	UpdateDereferenceControls();
}

bool TTalentEffectTabController::CreateTalentEffectInfo( System::String^ NewEffectInfoName )
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return false;

	const char* chrName				= MStringToCharPointer(NewEffectInfoName);


	if(m_pTalentEffectMgr->CreateTalentEffectInfo(pTalentInfo->GetEffectDataMgr(), chrName) == NULL)
	{
		return false;
	}

	GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
	return true;
}

bool TTalentEffectTabController::DeleteTalentEffectInfo(System::String ^ DelEffectInfoName)
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return false;

	const char* chrEffectName		= MStringToCharPointer(DelEffectInfoName);
	int nRemainCount				= 0;

	bool ret = m_pTalentEffectMgr->DeleteTalentEffectInfo(pTalentInfo->GetEffectDataMgr(), chrEffectName, nRemainCount);

	if(ret)
	{
		GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
	}

	if(nRemainCount <= 0)
	{
	}

	return ret;
}


XTalentEffectInfo * TTalentEffectTabController::GetTalentEffectInfo(System::String^ EffectInfoName)
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return false;

	const char* chrEffectName		= MStringToCharPointer(EffectInfoName);

	XTalentEffectInfo * pEffectInfo = NULL;

	pEffectInfo						= m_pTalentEffectMgr->GetTalentEffectInfo(pTalentInfo->GetEffectDataMgr(), chrEffectName);

	if(pEffectInfo == NULL)
	{
		GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject = nullptr;
		return NULL;
	}

	return pEffectInfo;
}


bool TTalentEffectTabController::SetTalentEffectReference( int nRefID, int nRefMode )
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return false;

	XTalentEffectDataMgr* pTalentEffectData = pTalentInfo->GetEffectDataMgr();
	if (pTalentEffectData == NULL) return false;

	if(	(pTalentEffectData->m_nRefID != nRefID)  ||
		(pTalentEffectData->m_nRefMode != nRefMode))
	{
		GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
	}

	pTalentEffectData->m_nRefID = nRefID;
	pTalentEffectData->m_nRefMode = nRefMode;

	Serialize();
	return true;
}


void TTalentEffectTabController::UpdateTalentEffectTreeView( XTalentEffectDataMgr* pTalentEffectData )
{
	GlobalObjects::g_pMainForm->TalentEffectTreeView->ClearTreeView();

	for (XTalentEffectDataMgr::iterator itor = pTalentEffectData->begin(); itor != pTalentEffectData->end(); ++itor)
	{
		XTalentEffectInfo* pEffectInfo = (*itor);

		// 히트 방향 정보는 빼자
		if(pEffectInfo->m_nEffectType == HIT_EFFECT_POSDIR)
			continue;

		String^ strNodeName = gcnew String(pEffectInfo->m_strTalentEffectInfoName.c_str());
		TreeNode^ node = GlobalObjects::g_pMainForm->TalentEffectTreeView->Nodes->Add( strNodeName );

		if (m_strSelEffectName == pEffectInfo->m_strTalentEffectInfoName)
		{
			GlobalObjects::g_pMainForm->TalentEffectTreeView->SelectedNode = node;
		}
//		node->ExpandAll();
	}

}

void TTalentEffectTabController::OnEffectTreeView_AfterSelect( System::Windows::Forms::TreeViewEventArgs^ e )
{
	m_strSelEffectName = "";
	m_strSelHitEffectPosDir = "";

	if (e->Node == nullptr) return;
	if (e->Node->Index < 0) return;

	m_strSelEffectName = MStringToCharPointer(e->Node->Text);

	UpdatePropertyGrid();
}

void TTalentEffectTabController::UpdateTalentEffectPropertyGrid( XTalentEffectDataMgr* pTalentEffectData )
{
	XTalentEffectInfo* pEffectInfo = m_pTalentEffectMgr->GetTalentEffectInfo(pTalentEffectData, m_strSelEffectName.c_str());
	if (pEffectInfo == NULL) 
	{
		GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = false;
		return;
	}

	GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = true;

	m_pTalentEffectViewer->SetPropertyGridFromEffectInfo(pEffectInfo);
}

void TTalentEffectTabController::OnEffectAttGrid_PropertyValueChanged()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	XTalentEffectDataMgr* pTalentEffectData = pTalentInfo->GetEffectDataMgr();

	if(GlobalObjects::g_pMainForm->TalentEffectList_tabControl->SelectedIndex == 0)
		OnTalentEffectAttGrid_PropertyValueChanged(pTalentEffectData);
	else if(GlobalObjects::g_pMainForm->TalentEffectList_tabControl->SelectedIndex == 1) 
		OnHitEffectPosDirAttGrid_PropertyValueChanged(pTalentEffectData);
}

void TTalentEffectTabController::OnAddEffectInfoBtn_Click()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	string strNewName = GetNewEffectInfoName(pTalentInfo, "이펙트");

	if (CreateTalentEffectInfo(gcnew String(strNewName.c_str())) == true)
	{
		GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);

		Serialize();
	}
}

void TTalentEffectTabController::OnDelEffectInfoBtn_Click()
{
	if (m_strSelEffectName.empty()) return;

	String^ strEffectName = gcnew String(m_strSelEffectName.c_str());
	if(DeleteTalentEffectInfo(strEffectName))
	{
		Serialize();
	}
	else
	{
		// 삭제 실패
		System::Windows::Forms::MessageBox::Show(L"삭제 할 이펙트 이름이 없거나 ID 에러입니다.", "에러");
	}

}

void TTalentEffectTabController::UpdateReferenceControls()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	bool bControlEnabled = false;
	int nRefID = pTalentInfo->GetEffectDataMgr()->m_nRefID;
	if (nRefID > 0)
	{
		int nRefMode = pTalentInfo->GetEffectDataMgr()->m_nRefMode;
		string strTalentID = TStrings::TalentIDString_WithMode(nRefID, nRefMode);

		GlobalObjects::g_pMainForm->RefTextBox->Text = gcnew String(String::Format("{0}", gcnew String(strTalentID.c_str())));

		bControlEnabled = false;
	}
	else
	{
		GlobalObjects::g_pMainForm->RefTextBox->Text = gcnew String("");

		bControlEnabled = true;
	}

	//GlobalObjects::g_pMainForm->TalentEffectTreeView->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->TalentEffectAttGrid->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->AddEffectInfoBtn->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->Delattbutton->Enabled = bControlEnabled;
	
	GlobalObjects::g_pMainForm->HitEffectPosDirtreeView->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->Enable_button->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->Disable_button->Enabled = bControlEnabled;
}

void TTalentEffectTabController::CopyEffectInfo()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	ArrayList^ arrSelectedEffectInfo = GlobalObjects::g_pMainForm->TalentEffectTreeView->SelectedNodes;

	Character_NET::TClipboardEffectInfo^ clipboardEffectInfo = gcnew Character_NET::TClipboardEffectInfo();

	System::Collections::IEnumerator^ myEnumerator = arrSelectedEffectInfo->GetEnumerator();
	while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
	{
		TreeNode^ node = safe_cast<TreeNode^>(myEnumerator->Current);

		string effectText = MStringToCharPointer(node->Text);

		XTalentEffectInfo* pEffectInfo = m_pTalentEffectMgr->GetTalentEffectInfo(pTalentInfo->GetEffectDataMgr(), effectText.c_str());
		if (pEffectInfo == NULL) continue;

		// 클립보드로 복사
		clipboardEffectInfo->CopyFrom(pEffectInfo);
	}


	TClipboardHelper::CopyEffectInfo(clipboardEffectInfo);
}

void TTalentEffectTabController::PasteEffectInfo()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	// 붙여넣기가 추가로 들어간다.
	TClipboardEffectInfo^ clipboardEffectInfo = TClipboardHelper::PasteEffectInfo();
	if (clipboardEffectInfo == nullptr) return;

	int nCount = clipboardEffectInfo->m_arrEffectInfo->Count;
	for(int i = 0; i < nCount; ++i)
	{
		XTalentEffectInfo tempEffectInfo;
		clipboardEffectInfo->CopyTo(i, &tempEffectInfo);

		tempEffectInfo.m_strTalentEffectInfoName = GetNewEffectInfoName(pTalentInfo, tempEffectInfo.m_strTalentEffectInfoName);

		String^ strEffectInfoName = gcnew String(tempEffectInfo.m_strTalentEffectInfoName.c_str());
		if(CreateTalentEffectInfo(strEffectInfoName))
		{
			XTalentEffectInfo * pTalentEffectInfo = GetTalentEffectInfo(strEffectInfoName);
			if(pTalentEffectInfo != NULL)
			{
				pTalentEffectInfo->copyData(tempEffectInfo);
			}
		}
	}

	Serialize();


	GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
}

void TTalentEffectTabController::GetTalentEffectImpactList( ArrayList^ impactList )
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) 
	{
		return;
	}

	XTalentEffectDataMgr* pTalentEffectData = pTalentInfo->GetEffectDataMgr();
	XTalentEffectInfo* pEffectInfo = m_pTalentEffectMgr->GetTalentEffectInfo(pTalentEffectData, m_strSelEffectName.c_str());
	if (pEffectInfo == NULL) 
	{
		return;
	}

	impactList->Add(IMPACTPOSITION_STRING_BONE_POS);

	if(pEffectInfo->m_nEffectType == USE_EFFECT)
	{
		impactList->Add(IMPACTPOSITION_STRING_TALENT_HIT_CAPSUL_POS);
	}
	else
	{
		impactList->Add(IMPACTPOSITION_STRING_MODEL_POS);
		impactList->Add(IMPACTPOSITION_STRING_HIT_POS);
	}
}

void TTalentEffectTabController::GetTalentEffectTypeList( ArrayList^ effectTypeList )
{
	effectTypeList->Add(EFFECTYPE_STRING_CAST_EFFECT);
	effectTypeList->Add(EFFECTYPE_STRING_USE_EFFECT);
	effectTypeList->Add(EFFECTYPE_STRING_ATTACK_EFFECT);
	effectTypeList->Add(EFFECTYPE_STRING_MISS_EFFECT);
	effectTypeList->Add(EFFECTYPE_STRING_SHOCK_EFFECT);
	effectTypeList->Add(EFFECTYPE_STRING_DURATION_EFFECT);
	effectTypeList->Add(EFFECTYPE_STRING_DEFENSE_EFFECT);
	effectTypeList->Add(EFFECTYPE_STRING_CAST_END_EFFECT);
}

void TTalentEffectTabController::GetTalentEffectDirectionList( ArrayList^ directionList )
{
	directionList->Add(EFFECT_DIRECTION_NONE);
	directionList->Add(EFFECT_DIRECTION_FRONT_USER);
	directionList->Add(EFFECT_DIRECTION_SWORD_DIR);
}

void TTalentEffectTabController::OnHitEffectPosDirTreeView_AfterSelect( System::Windows::Forms::TreeViewEventArgs^ e )
{
	m_strSelEffectName = "";
	m_strSelHitEffectPosDir = "";

	if (e->Node == nullptr) return;
	if (e->Node->Index < 0) return;

	m_strSelHitEffectPosDir = MStringToCharPointer(e->Node->Text);

	UpdatePropertyGrid();
}

void TTalentEffectTabController::UpdateHitEffectPosDirPropertyGrid( XTalentEffectDataMgr* pTalentEffectData )
{
	vector<XTalentEffectInfo *>	vecTalentEffectInfo;
	m_pTalentEffectMgr->GetTalentEffectInfo(pTalentEffectData, HIT_EFFECT_POSDIR, vecTalentEffectInfo);
	if(vecTalentEffectInfo.empty())
	{
		GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = false;
		return;
	}

	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	for(vector<XTalentEffectInfo *>::iterator it = vecTalentEffectInfo.begin(); it != vecTalentEffectInfo.end(); ++it)
	{
		for(vector<XTalentHitEffectPosDir>::iterator itPosDir = (*it)->m_vecTalentHitEffectPosDir.begin(); itPosDir != (*it)->m_vecTalentHitEffectPosDir.end(); ++itPosDir)
		{
			if(nn != itPosDir->strMeshName)
				continue;

			char chBuff[16] = {0,};
			sprintf_s(chBuff, "%f", itPosDir->fHitTime);
			
			if(m_strSelHitEffectPosDir == chBuff)
			{
				GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = true;

				m_pTalentEffectViewer->SetPropertyGridFromHitEffectPosDirInfo(&(*itPosDir));
				return;
			}
		}		
	}

	GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = false;
}

void TTalentEffectTabController::UpdateHitEffectPosDirTreeView( XTalentEffectDataMgr* pTalentEffectData )
{
	GlobalObjects::g_pMainForm->HitEffectPosDirtreeView->Nodes->Clear();
	GlobalObjects::g_pMainForm->Enable_button->Enabled = true;
	GlobalObjects::g_pMainForm->Disable_button->Enabled = false;

	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	for (XTalentEffectDataMgr::iterator itor = pTalentEffectData->begin(); itor != pTalentEffectData->end(); ++itor)
	{
		XTalentEffectInfo* pEffectInfo = (*itor);

		// 히트 방향 정보가 아닌건 빼자
		if(pEffectInfo->m_nEffectType != HIT_EFFECT_POSDIR)
			continue;

		if(pEffectInfo->m_vecTalentHitEffectPosDir.empty() == false)
		{
			GlobalObjects::g_pMainForm->Enable_button->Enabled = false;
			GlobalObjects::g_pMainForm->Disable_button->Enabled = true;
		}

		for(vector<XTalentHitEffectPosDir>::iterator itPosDir = pEffectInfo->m_vecTalentHitEffectPosDir.begin(); itPosDir != pEffectInfo->m_vecTalentHitEffectPosDir.end(); ++itPosDir)
		{
			if(nn != itPosDir->strMeshName)
				continue;

			char chBuff[16] = {0,};
			sprintf_s(chBuff, "%f", itPosDir->fHitTime);

			String^ strNodeName = gcnew String(chBuff);
			TreeNode^ node = GlobalObjects::g_pMainForm->HitEffectPosDirtreeView->Nodes->Add( strNodeName );

			if (m_strSelHitEffectPosDir == chBuff)
			{
				GlobalObjects::g_pMainForm->HitEffectPosDirtreeView->SelectedNode = node;
			}
		}
	}
}

void TTalentEffectTabController::UpdateTreeView()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	XTalentEffectDataMgr* pTalentEffectData = pTalentInfo->GetEffectDataMgr();

	if (pTalentEffectData->m_nRefID > 0)
	{
		pTalentInfo = TTalentHelper::GetTalentInfo(pTalentEffectData->m_nRefID, pTalentEffectData->m_nRefMode);
		if (pTalentInfo == NULL) return;

		pTalentEffectData = pTalentInfo->GetEffectDataMgr();
	}

	UpdateTalentEffectTreeView(pTalentEffectData);
	UpdateHitEffectPosDirTreeView(pTalentEffectData);
}

void TTalentEffectTabController::UpdatePropertyGrid()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) 
	{
		GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = false;
		return;
	}

	XTalentEffectDataMgr* pTalentEffectData = pTalentInfo->GetEffectDataMgr();

	if(pTalentEffectData->m_nRefID > 0)
	{
		XTalentInfo * pTalentInfoTemp = TTalentHelper::GetTalentInfo(pTalentEffectData->m_nRefID, pTalentEffectData->m_nRefMode);
		if (pTalentInfoTemp != NULL) 
		{
			if(pTalentInfoTemp->GetEffectDataMgr() != NULL)
				pTalentEffectData = pTalentInfoTemp->GetEffectDataMgr();
		}
	}

	if(GlobalObjects::g_pMainForm->TalentEffectList_tabControl->SelectedIndex == 0)
		UpdateTalentEffectPropertyGrid(pTalentEffectData);
	else if(GlobalObjects::g_pMainForm->TalentEffectList_tabControl->SelectedIndex == 1) 
		UpdateHitEffectPosDirPropertyGrid(pTalentEffectData);
}

void TTalentEffectTabController::OnTalentEffectAttGrid_PropertyValueChanged( XTalentEffectDataMgr* pTalentEffectData )
{
	XTalentEffectInfo* pEffectInfo = m_pTalentEffectMgr->GetTalentEffectInfo(pTalentEffectData, m_strSelEffectName.c_str());
	if (pEffectInfo == NULL) return;

	m_pTalentEffectViewer->SetEffectInfoFromPropertyGrid(pEffectInfo);
	GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);

	// 이펙트 정보 이름이 바뀔수 있으므로 변경된 이펙트로 다시 입력.
	m_strSelEffectName = pEffectInfo->m_strTalentEffectInfoName;
	Serialize();
}

void TTalentEffectTabController::OnHitEffectPosDirAttGrid_PropertyValueChanged( XTalentEffectDataMgr* pTalentEffectData )
{
	vector<XTalentEffectInfo *>	vecTalentEffectInfo;
	m_pTalentEffectMgr->GetTalentEffectInfo(pTalentEffectData, HIT_EFFECT_POSDIR, vecTalentEffectInfo);
	if(vecTalentEffectInfo.empty())
	{
		return;
	}

	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	for(vector<XTalentEffectInfo *>::iterator it = vecTalentEffectInfo.begin(); it != vecTalentEffectInfo.end(); ++it)
	{
		for(vector<XTalentHitEffectPosDir>::iterator itPosDir = (*it)->m_vecTalentHitEffectPosDir.begin(); itPosDir != (*it)->m_vecTalentHitEffectPosDir.end(); ++itPosDir)
		{
			if(nn != itPosDir->strMeshName)
				continue;

			char chBuff[16] = {0,};
			sprintf_s(chBuff, "%f", itPosDir->fHitTime);

			if (m_strSelHitEffectPosDir == chBuff)
			{
				GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
				m_pTalentEffectViewer->SetHitEffectPosDirInfoFromPropertyGrid(&(*itPosDir));

				Serialize();
				return;
			}
		}
	}
}

void TTalentEffectTabController::OnEnableHitEffectPosDirBtn_Click()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) 
	{
		// 탤러트 히트정보
		if(pTalentInfo->m_HitInfo.m_nReferenceID > 0)
		{
			pTalentInfo = TTalentHelper::GetTalentInfo(pTalentInfo->m_HitInfo.m_nReferenceID, pTalentInfo->m_HitInfo.m_nReferenceMode);

			if(pTalentInfo == NULL)
			{
				GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = false;
				return;
			}
		}
		else
		{
			GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = false;
			return;
		}
	}

	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	// 플레이어만 적용
	if(nn != "hf" && nn != "hm")
		return;

	XTalentEffectDataMgr* pTalentEffectData = pTalentInfo->GetEffectDataMgr();

	vector<XTalentEffectInfo *>	vecTalentEffectInfo;
	m_pTalentEffectMgr->GetTalentEffectInfo(pTalentEffectData, HIT_EFFECT_POSDIR, vecTalentEffectInfo);
	if(vecTalentEffectInfo.empty())
	{
		XTalentEffectInfo * pEffectInfo = new XTalentEffectInfo();
		pEffectInfo->m_nEffectType = HIT_EFFECT_POSDIR;
		pTalentEffectData->push_back(pEffectInfo);

		m_pTalentEffectMgr->GetTalentEffectInfo(pTalentEffectData, HIT_EFFECT_POSDIR, vecTalentEffectInfo);
	}

	// 하나뿐이어야 한다.
	bool bEnable = false;
	for(vector<XTalentEffectInfo *>::iterator it = vecTalentEffectInfo.begin(); it != vecTalentEffectInfo.end(); ++it)
	{
		// 시그먼트에 있는 판정 타임으로 이펙트 구하기
		for(vector<CSHitSegment>::iterator itSeg = pTalentInfo->m_HitInfo.m_vSegments.begin(); itSeg != pTalentInfo->m_HitInfo.m_vSegments.end(); ++itSeg)
		{
			XTalentHitEffectPosDir dirData;
			dirData.strMeshName = nn;
			dirData.fHitTime = itSeg->m_fCheckTime;
			dirData.vHitEffectPos = vec3(0, 0, 100);

			(*it)->m_vecTalentHitEffectPosDir.push_back(dirData);

			bEnable = true;
		}
	}

	if(bEnable)
	{
		GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
		Serialize();
	}
}

void TTalentEffectTabController::OnDisableHitEffectPosDirBtn_Click()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) 
	{
		GlobalObjects::g_pMainForm->TalentEffectAttGrid->Visible = false;
		return;
	}

	XTalentEffectDataMgr* pTalentEffectData = pTalentInfo->GetEffectDataMgr();

	vector<XTalentEffectInfo *>	vecTalentEffectInfo;
	m_pTalentEffectMgr->GetTalentEffectInfo(pTalentEffectData, HIT_EFFECT_POSDIR, vecTalentEffectInfo);
	if(vecTalentEffectInfo.empty())
	{
		return;
	}

	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	for(vector<XTalentEffectInfo *>::iterator it = vecTalentEffectInfo.begin(); it != vecTalentEffectInfo.end(); ++it)
	{
		for(vector<XTalentHitEffectPosDir>::iterator itPosDir = (*it)->m_vecTalentHitEffectPosDir.begin(); itPosDir != (*it)->m_vecTalentHitEffectPosDir.end(); ++itPosDir)
		{
			if(nn != itPosDir->strMeshName)
				continue;

			char chBuff[16] = {0,};
			sprintf_s(chBuff, "%f", itPosDir->fHitTime);

			if (m_strSelHitEffectPosDir == chBuff)
			{
				// 사용하지 않을 수 있나?
				if(itPosDir->vSwordTrailHitUp != vec3::AXISZ)
				{
					// 삭제 불가능
					System::Windows::Forms::MessageBox::Show(L"피격 이펙트 방향값이 있으므로 삭제할 수 없습니다.", "에러");
					return;
				}
			}
		}
	}

	if(m_pTalentEffectMgr->DeleteTalentEffectInfo(pTalentEffectData, HIT_EFFECT_POSDIR))
	{
		GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);

		Serialize();
	}

	return;
}

void TTalentEffectTabController::RenderHitEffectPosDir()
{

}

std::string TTalentEffectTabController::GetNewEffectInfoName( TTalentInfo* pTalentInfo, string strEffectName, int nCount /*= 0*/ )
{		
	char chNameTemp[1024] = {0,};
	if(nCount > 0)
		sprintf_s(chNameTemp, "%s(%d)", strEffectName.c_str(), nCount + 1);
	else
		sprintf_s(chNameTemp, "%s", strEffectName.c_str());

	// 중복 체크
	if(m_pTalentEffectMgr->CheckDuplicationTalentEffectInfo(pTalentInfo->GetEffectDataMgr(), chNameTemp) == true)
	{
		++nCount;
		return GetNewEffectInfoName(pTalentInfo, strEffectName, nCount);
	}

	return chNameTemp;
}

void TTalentEffectTabController::UpdateDereferenceControls()
{
	// 역참조 조사
	GlobalObjects::g_pMainForm->Deref_button->Enabled = false;

	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	int nTalentID = pTalentInfo->m_nID;

	for(map<int , CSTalentInfo* >::iterator it = g_pMainApp->m_TTalentInfoMgr.begin(); it != g_pMainApp->m_TTalentInfoMgr.end(); ++it)
	{
		TTalentInfo* pCheckTalentInfo = (TTalentInfo*)(it->second);
		
		if(pCheckTalentInfo->GetEffectDataMgr()->m_nRefID == nTalentID)
		{
			GlobalObjects::g_pMainForm->Deref_button->Enabled = true;
			return;
		}
	}
}

void TTalentEffectTabController::OnDeDereferenceTalentEffectIDViewer()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	DereferenceTalentIDViewer dereferenceDlg;
	dereferenceDlg.SetTalentID(pTalentInfo->m_nID);
	dereferenceDlg.SetDerefType(DT_EFFECT);
	dereferenceDlg.ShowDialog();
}
