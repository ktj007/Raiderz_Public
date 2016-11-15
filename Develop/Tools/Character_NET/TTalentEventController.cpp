#include "stdafx.h"
#include "TTalentEventController.h"
#include "Form1.h"
#include "TTalentEventMgr.h"
#include "TTalentEventViewer.h"
#include "CSTalentInfo.h"
#include "Xmlseral.h"
#include "TClipboard.h"
#include "DereferenceTalentIDViewer.h"

namespace Character_NET {

TreeView^ TTalentEventController::GetEventViewerTreeNode()
{
	return m_pMainForm->TalentEventTreeView;
}

void TTalentEventController::Clear()
{
	m_pMainForm->CurrentTalentID2->Text = nullptr;

	m_pMainForm->TalentEventTreeView->ClearTreeView();
	GetEventViewerTreeNode()->Nodes->Add(gcnew String(TALENT_EVENT_NORMAL));
	GetEventViewerTreeNode()->Nodes->Add(gcnew String(TALENT_EVENT_ACT));		

	m_pMainForm->TalentEventAtt->SelectedObject = nullptr;

	m_pEventViewControl->SelTalentInfo(NULL);
}

void TTalentEventController::SetTalentEvent( CSTalentInfo* pTalentInfo )
{
	m_pEventViewControl->SelTalentInfo(pTalentInfo);
	if (pTalentInfo == NULL) return;

	//------------------------------------------------------------------------
	// 탤런트 이벤트

	string strTalentID = TStrings::TalentIDString_WithMode(pTalentInfo->m_nID, pTalentInfo->m_nMode);

	m_pMainForm->CurrentTalentID2->Text = String::Format("{0}", gcnew String(strTalentID.c_str()));

	// 탤런트 이벤트에서 트리뷰어에 기본적인 헤드를 입력한다.
	m_pMainForm->TalentEventTreeView->ClearTreeView();
	GetEventViewerTreeNode()->Nodes->Add(gcnew String(TALENT_EVENT_NORMAL));
	GetEventViewerTreeNode()->Nodes->Add(gcnew String(TALENT_EVENT_ACT));	

	m_pTalentEventMgr->m_nMaxNormalIndex	= 0;
	m_pTalentEventMgr->m_nMaxActIndex		= 0;

	UpdateReferenceControls();
	UpdateDereferenceControls();

	// 탤런트 이벤트 데이터 입력
	SetTalentEventView(pTalentInfo);

	m_pMainForm->TalentEventTreeView->ExpandAll();
	m_pMainForm->TalentEventAtt->SelectedObject = nullptr;
}

bool TTalentEventController::SetTalentEventView( CSTalentInfo* pTalentInfo )
{
	int nFullCount = 0;
	CSTalentInfo* pEventTalentInfo = pTalentInfo;

	for(int j = 0; j < TES_MAX; j++)
	{
		if (pEventTalentInfo->m_Events.m_nRefID > 0)
		{
			pEventTalentInfo = TTalentHelper::GetTalentInfo(pEventTalentInfo->m_Events.m_nRefID, pEventTalentInfo->m_Events.m_nRefMode);
			if (pEventTalentInfo == NULL) continue;
		}

		int nCount = m_pTalentEventMgr->GetCount(pEventTalentInfo, (TALENT_EVENT_STARTTYPE)j);

		nFullCount += nCount;
		for(int i = 0; i < nCount; i++)
		{
			CSTalentEventInfo * pEventInfo = m_pTalentEventMgr->GetTalentEventInfo(pEventTalentInfo, i, (TALENT_EVENT_STARTTYPE)j);
			if(pEventInfo)
			{
				TreeNode^ tn = m_pEventViewControl->AddTalentEventView(j);
				m_pEventViewControl->ChangeEventTreeNodeName(m_pEventViewControl->GetTextEventType(pEventInfo->m_nEvent), tn);
			}
		}
	}

	if(nFullCount == 0)
		return false;

	return true;
}

TreeNode^ TTalentEventController::AddTalentEventView( int nStart )
{
	// 성공
	char chIndex[64]		= {0, };
	String^ strEventStart;
	TreeNode^ tn			= nullptr;
	if(nStart == 0)
	{
		strEventStart = gcnew String(TALENT_EVENT_NORMAL);
		tn = g_pMainApp->SearchNode(GetEventViewerTreeNode()->Nodes, strEventStart);

		if(tn == nullptr)
		{
			// 내용이 없다.
			return nullptr;
		}

		sprintf(chIndex, "Event_%d", ++(m_pTalentEventMgr->m_nMaxNormalIndex));

		tn->Nodes->Add(gcnew String(chIndex));
	}
	else if(nStart == 1)
	{
		strEventStart = gcnew String(TALENT_EVENT_ACT);
		tn = g_pMainApp->SearchNode(GetEventViewerTreeNode()->Nodes, strEventStart);

		if(tn == nullptr)
		{
			// 내용이 없다.
			return nullptr;
		}

		sprintf(chIndex, "ActEvent_%d", ++(m_pTalentEventMgr->m_nMaxActIndex));

		tn->Nodes->Add(gcnew String(chIndex));
	}

	return g_pMainApp->SearchNode(tn->Nodes, gcnew String(chIndex));
}

void TTalentEventController::Init( TTalentEventMgr* pTalentEventMgr )
{
	m_pTalentEventMgr = pTalentEventMgr;
	m_pEventViewControl->SetEventManager(m_pTalentEventMgr);
}

void TTalentEventController::CreateTalentEvent( int nStart )
{
	if (m_pEventViewControl->GetSelTalentInfo() == NULL) return;

	bool bFailed = false;
	float fStartTime = 0.0f;

	// 현재 애니메이션 시간을 가져온다.
	RAnimation* pAnimation = g_pMainApp->GetActor()->GetCurAnimation();
	if(pAnimation)
	{
		fStartTime = g_pMainApp->GetActor()->GetCurFrameToSec();
	}

	TreeNode^ tn = nullptr;
	if(m_pTalentEventMgr->CreateTalentEventInfo(m_pEventViewControl->GetSelTalentInfo(), fStartTime, (TALENT_EVENT_STARTTYPE)nStart))
	{
		tn = m_pEventViewControl->AddTalentEventView(nStart);
		if(tn == nullptr)
			bFailed = true;
	}
	else
	{
		bFailed = true;
	}

	if (bFailed)
	{
		// 실패 및 중복
		System::Windows::Forms::MessageBox::Show(L"생성 실패!", "에러");
		return;
	}

	// 성공
	// 새로 만든 이벤트를 선택 노드로 한다.
	GetEventViewerTreeNode()->SelectedNode = tn;

	// 정보를 보여준다.
	SetCurrentPropertyTalentEvent(true);
	
	m_pEventViewControl->NotifyChanging();

	// 자동으로 펼친다.
	GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventTreeView->ExpandAll();
}

void TTalentEventController::SetCurrentPropertyTalentEvent( bool bNewData )
{
	m_pEventViewControl->SetCurrentPropertyTalentEvent(bNewData);

	CSTalentEventInfo * pTalentEventInfo = m_pEventViewControl->GetCurrentTalentEventInfo();
	if(pTalentEventInfo == NULL)
		return;

	MMatrix matCalWorld;
	CalProjectileStartPos(pTalentEventInfo, matCalWorld);			// 발사체 발사 위치 계산
	CalProjectilHitCapsuleCheckTime(pTalentEventInfo, matCalWorld);	// 히트캡슐 발사 시간 계산
}

void TTalentEventController::CalProjectileStartPos(CSTalentEventInfo * pTalentEventInfo, MMatrix& matCalWorld)
{
	TCharacter* pActor = g_pMainApp->GetActor();
	if(pActor == NULL) 
		return;

	if(pTalentEventInfo == NULL ||
		pTalentEventInfo->m_nEvent != TE_LAUNCH_PROJECTILE)
		return;

	int nFrame = pActor->GetSecToFrame(pTalentEventInfo->m_fTime1);

	// 발사체 위치 좌표 계산하기
	RMatrix matResult;
	RActorNode* pActorNode = NULL;

	if(g_pMainApp->GetMotion()->GetMotion() != NULL)
	{
		int nActIndex = g_pMainApp->m_TalentAnimation.GetUseActAniIndex();
		int nUseAniIndex  = g_pMainApp->GetMotion()->GetCurAniMaxSequence();
		nFrame += g_pMainApp->GetMotion()->GetCurTalentFrame(nActIndex);

		for (int j = nActIndex; j < nUseAniIndex; ++j)
		{
			if(g_pMainApp->GetMotion()->GetCurTalentFrame(j + 1) >= nFrame)
			{
				g_pMainApp->GetMotion()->SetSequence(j);
				int nCalFrame = nFrame - g_pMainApp->GetMotion()->GetCurTalentFrame(j);

				pActorNode = pActor->GetAnimatedActorNodeTransform(nCalFrame, pTalentEventInfo->m_ProjectileInfo.m_strBoneName, matResult, E_TS_LOCAL_RESULT);
				pActor->GetAnimatedActorNodeTransform(nCalFrame, pTalentEventInfo->m_ProjectileInfo.m_strBoneName, matCalWorld, E_TS_WORLD);
				break;
			}
		}
	}
	else
	{
		pActorNode = pActor->GetAnimatedActorNodeTransform(nFrame, pTalentEventInfo->m_ProjectileInfo.m_strBoneName, matResult, E_TS_LOCAL_RESULT);
		pActor->GetAnimatedActorNodeTransform(nFrame, pTalentEventInfo->m_ProjectileInfo.m_strBoneName, matCalWorld, E_TS_WORLD);
	}

	// 발사체 위치 정보
	vec3 vStartPos = matResult.GetTranslation();
	if(pActorNode)
	{
		// 속성값에 변화가 있는가?
		vec3 vOrgPos;

		string strSexName = pActor->CurrentSexType();
		// 여자가 기준
		if(strSexName != HUMAN_MALE)
		{
			vOrgPos = pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos;
			pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos = vStartPos;
		}

		// 속성값 변경
		m_pEventViewControl->SetCurrentPropertyTalentEvent(true);

		if(vOrgPos.x + 0.001f < vStartPos.x || vOrgPos.x - 0.001f > vStartPos.x ||
			vOrgPos.y + 0.001f < vStartPos.y ||	vOrgPos.y - 0.001f > vStartPos.y ||
			vOrgPos.z + 0.001f < vStartPos.z ||	vOrgPos.z - 0.001f > vStartPos.z)
			m_pEventViewControl->NotifyChanging();
	}
	//else
	//{
	//	char msg[1024] = {0,};
	//	sprintf(msg, "%s - 본이 존재하지 않습니다.", pTalentEventInfo->m_ProjectileInfo.m_strBoneName.c_str());
	//	System::Windows::Forms::MessageBox::Show(gcnew String(msg), "에러");
	//}
}

void TTalentEventController::DeleteTalentEvent()
{
	bool bDel = m_pEventViewControl->DeleteTalentEvent();
	if(bDel)
	{
		m_pEventViewControl->NotifyChanging();
	}
	else
	{
		// 선택이 되어 있지 않다.
		System::Windows::Forms::MessageBox::Show(L"이벤트를 선택하세요!", "에러");
	}
}

int TTalentEventController::GetCurrentTalentEventType()
{
	CSTalentEventInfo * pEventInfo = m_pEventViewControl->GetCurrentTalentEventInfo();
	if(pEventInfo)
	{
		return (int)(pEventInfo->m_nEvent);
	}

	return -1;
}

bool TTalentEventController::IsExistenceTalentEvent()
{
	if (m_pEventViewControl->GetSelTalentInfo() != NULL) return true;

	return false;
}

TTalentEventController::TTalentEventController( EventTabPage^ pForm ) : m_pMainForm(pForm)
{
	m_pEventViewControl = new TTalentEventViewer();
}

TTalentEventController::~TTalentEventController()
{
	SAFE_DELETE(m_pEventViewControl);
}

void TTalentEventController::GetTalentHitInfoSegmentIndex( ArrayList^ indexList )
{
	m_pEventViewControl->GetTalentHitInfoSegmentIndex(indexList);
}

void TTalentEventController::GetTalentHitInfoCapsuleIndex( ArrayList^ indexList, int nIndex )
{
	m_pEventViewControl->GetTalentHitInfoCapsuleIndex(indexList, (TALENT_EVENT)nIndex);
}

void TTalentEventController::GetProjectileVisualType( ArrayList^ typeList )
{
	m_pEventViewControl->GetProjectileVisualType(typeList);
}

void TTalentEventController::GetProjectileType( ArrayList^ typeList )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if(pTalentInfo)
	{
		if(pTalentInfo->m_EffectInfo.m_nPoint == CSEffectInfo::POINT_HITCAPSULE)
		{
			typeList->Add(TALENT_XML_VALUE_PROJECTILETYPE_HITCAPSULE_GUIDED);
			return;
		}
		else if(pTalentInfo->m_setNPCID.size() != 0) // npc이면
		{
			typeList->Add(TALENT_XML_VALUE_PROJECTILETYPE_MISSILE);			
		}

		typeList->Add(TALENT_XML_VALUE_PROJECTILETYPE_GUIDED);
		typeList->Add(TALENT_XML_VALUE_PROJECTILETYPE_HITCAPSULE_GUIDED);
	}
}

void TTalentEventController::GetTalentEventTypeList( ArrayList^ eventList )
{
	int nTypeMaxCount = (int)TE_MAX;
	for(int i = 0; i < nTypeMaxCount; i++)
	{
		eventList->Add(gcnew String(m_pEventViewControl->GetTextEventType((TALENT_EVENT)i).c_str()));
	}
}

void TTalentEventController::OnTalentEventCopy()
{
	CSTalentInfo * pTalentInfo = m_pEventViewControl->GetCurrentTalentInfo();
	if(pTalentInfo == NULL)
	{
		// 선택한 탤런트가 없다.
		System::Windows::Forms::MessageBox::Show(L"우선 탤런트를 선택하세요!", "에러");
		return;
	}

	//---------------------------------------------------------------------------//
	ArrayList^ arrSelectedEventInfo = m_pMainForm->TalentEventTreeView->SelectedNodes;

	Character_NET::TClipboardEventInfo^ clipboardEventInfo = gcnew Character_NET::TClipboardEventInfo();

	System::Collections::IEnumerator^ myEnumerator = arrSelectedEventInfo->GetEnumerator();
	while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
	{
		TreeNode^ node = safe_cast<TreeNode^>(myEnumerator->Current);

		string eventText = MStringToCharPointer(node->Text);
		if(eventText == TALENT_EVENT_ACT ||	eventText == TALENT_EVENT_NORMAL)
			return;

		TALENT_EVENT_STARTTYPE eStartType = TES_NORMAL;

		string eventParentText = MStringToCharPointer(node->Parent->Text);
		if(eventParentText == TALENT_EVENT_ACT)
			eStartType = TES_ACT;

		CSTalentEventInfo* pEventInfo = m_pTalentEventMgr->GetTalentEventInfo(pTalentInfo, node->Index, eStartType);
		if (pEventInfo == NULL) 
			continue;

		// 클립보드로 복사
		clipboardEventInfo->CopyFrom(pEventInfo);
	}


	TClipboardHelper::CopyEventInfo(clipboardEventInfo);
}

void TTalentEventController::OnTalentEventPasete()
{
	CSTalentInfo * pTalentInfo = m_pEventViewControl->GetCurrentTalentInfo();
	if(pTalentInfo == NULL)
	{
		// 선택한 탤런트가 없다.
		System::Windows::Forms::MessageBox::Show(L"우선 탤런트를 선택하세요!", "에러");
		return;
	}

	TreeNode^ tn = GetEventViewerTreeNode()->SelectedNode;
	// 노드가 root라면... 삭제 할 수 없다.
	if(tn == nullptr)
	{
		// 선택한 이벤트가 없다.
		System::Windows::Forms::MessageBox::Show(L"붙일 곳(Normal, Act)을 선택하세요!", "에러");
		return;
	}

	// 붙여넣기가 추가로 들어간다.
	TClipboardEventInfo^ clipboardEvectInfo = TClipboardHelper::PasteEventInfo();
	if (clipboardEvectInfo == nullptr) return;

	int nCount = clipboardEvectInfo->m_arrEventInfo->Count;
	for(int i = 0; i < nCount; ++i)
	{
		CSTalentEventInfo tempEventInfo;
		clipboardEvectInfo->CopyTo(i, &tempEventInfo);

		TALENT_EVENT_STARTTYPE eTalentEventType = TES_NORMAL;
		if(tn->Text == TALENT_EVENT_ACT ||
			(tn->Parent != nullptr && tn->Parent->Text == TALENT_EVENT_ACT))
		{
			eTalentEventType = TES_ACT;
		}

		m_pTalentEventMgr->AddTalentEventInfo(pTalentInfo, tempEventInfo, eTalentEventType);
	}


	SetTalentEvent(pTalentInfo);
}

void TTalentEventController::CalProjectilHitCapsuleCheckTime( CSTalentEventInfo * pTalentEventInfo, MMatrix matWorld )
{
	CSTalentInfo* pTalentInfo = m_pEventViewControl->GetCurrentTalentInfo();
	if(pTalentEventInfo == NULL ||
		pTalentInfo == NULL ||
		pTalentEventInfo->m_ProjectileInfo.m_nType != TPT_HITCAPSULE_GUIDED)
		return;

	int nSegment = pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;
	int nCapsule = pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;

	if(nSegment < 0 ||
		nCapsule < 0)
		return;

	if((int)pTalentInfo->m_HitInfo.m_vSegments.size() <= nSegment)
		return;

	if((int)pTalentInfo->m_HitInfo.m_vSegments[nSegment].m_vCapsules.size() <= nCapsule)
		return;

	vec3 vWorldStartPos;
	matWorld.TransformVect(pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos, vWorldStartPos);
	vec3 vHitCapsuleCenter = pTalentInfo->m_HitInfo.m_vSegments[nSegment].m_vCapsules[nCapsule].GetCenter();
	float fLen = (vHitCapsuleCenter - vWorldStartPos).Length();
	float fMoveTime = fLen / pTalentEventInfo->m_ProjectileInfo.m_fSpeed;

	pTalentInfo->m_HitInfo.m_vSegments[nSegment].m_fCheckTime = pTalentEventInfo->m_fTime1 + fMoveTime;

	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
}

void TTalentEventController::OnTalentEventRefSelect(System::Object^  sender, System::EventArgs^  e)
{
	CSTalentInfo * pTalentInfo = m_pEventViewControl->GetCurrentTalentInfo();
	if(pTalentInfo == NULL)
	{
		// 선택한 탤런트가 없다.
		System::Windows::Forms::MessageBox::Show(L"우선 탤런트를 선택하세요!", "에러");
		return;
	}

	CharacterToolForms::TalentEffectRegisterEdit^ regEdit = gcnew CharacterToolForms::TalentEffectRegisterEdit();

	regEdit->Text = String::Format("레퍼런스 탤런트 이벤트 선택");
	regEdit->TalentModecomboBox->Items->Clear();
	for (int i = 0; i < WEAPON_MAX; ++i)
	{
		string strWeapon = TStrings::WeaponType(i);
		String^ str = String::Format("{0}", gcnew String(strWeapon.c_str()));
		regEdit->TalentModecomboBox->Items->Add(str);
	}

	int nTalentID = atoi(MStringToCharPointer(GlobalObjects::g_pMainForm->m_EventTabPage->EventRefTextBox->Text));
	regEdit->SetText(nTalentID, 0);
	regEdit->ShowDialog();

	System::Windows::Forms::DialogResult res = regEdit->DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{
		int nSelectTalentID = 0;
		int nSelectTalentMode = 0;

		TTalentInfo* refTalent = TTalentHelper::GetTalentInfo(regEdit->m_nTalentID, regEdit->m_nTalentMode);
		if(refTalent)
		{
			nSelectTalentID = regEdit->m_nTalentID;
			nSelectTalentMode = regEdit->m_nTalentMode;

			// 레퍼런스 입력
			string strTalentID = TStrings::TalentIDString_WithMode(nSelectTalentID, nSelectTalentMode);
			GlobalObjects::g_pMainForm->m_EventTabPage->EventRefTextBox->Text = gcnew String(String::Format("{0}", gcnew String(strTalentID.c_str())));
		}
		else
		{
			GlobalObjects::g_pMainForm->m_EventTabPage->EventRefTextBox->Text = "";

			if (regEdit->m_nTalentID != 0
				|| regEdit->m_nTalentMode != 0)
			{
				if(System::Windows::Forms::MessageBox::Show(regEdit, L"탤런트가 존재하지 않습니다.", "에러")
					== System::Windows::Forms::DialogResult::OK)
				{
					GlobalObjects::g_pMainForm->m_EventTabPage->EventRefSelectBtn_Click(sender, e);
				}
				return;
			}
		}

		SetTalentEventReference(nSelectTalentID, nSelectTalentMode);
	}
}

void TTalentEventController::SetTalentEventReference( int nRefID, int nRefMode )
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if(	(pTalentInfo->m_Events.m_nRefID != nRefID)  ||
		(pTalentInfo->m_Events.m_nRefMode != nRefMode))
	{
		GlobalObjects::g_SaveNotifier.SetSaveTalentEvent(true);
	}

	pTalentInfo->m_Events.m_nRefID = nRefID;
	pTalentInfo->m_Events.m_nRefMode = nRefMode;

	SetTalentEvent(pTalentInfo);
}

void TTalentEventController::UpdateReferenceControls()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	bool bControlEnabled = false;
	int nRefID = pTalentInfo->m_Events.m_nRefID;
	if (nRefID > 0)
	{
		int nRefMode = pTalentInfo->m_Events.m_nRefMode;
		string strTalentID = TStrings::TalentIDString_WithMode(nRefID, nRefMode);

		GlobalObjects::g_pMainForm->m_EventTabPage->EventRefTextBox->Text = gcnew String(String::Format("{0}", gcnew String(strTalentID.c_str())));

		bControlEnabled = false;
	}
	else
	{
		GlobalObjects::g_pMainForm->m_EventTabPage->EventRefTextBox->Text = gcnew String("");

		bControlEnabled = true;
	}

	//GetEventViewerTreeNode()->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventInsertBtn->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventDeleteBtn->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventCopy->Enabled = bControlEnabled;
	GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventPaste->Enabled = bControlEnabled;
}

void TTalentEventController::CheckSegmentIndexCountAndCapsuleIndexCount()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if(pTalentInfo == NULL)
		return;

	int nSegIndex = -1;
	int nCapsuleIndex = -1;

	m_pEventViewControl->GetTalentHitInfoSegmentIndexAndCapsuleIndex(nSegIndex, nCapsuleIndex);

	if(nSegIndex == -1)
		return;

	int nSegCount = (int)pTalentInfo->m_HitInfo.m_vSegments.size();

	if(nSegCount == 0)
	{
		// 세그와 캡슐정보가 없으면... 초기화 시킨다.
		CSTalentEventInfo * pTalentEventInfo = m_pEventViewControl->GetCurrentTalentEventInfo();
		if(pTalentEventInfo == NULL)
			return;

		pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType = -1;
		pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType = -1;
		m_pEventViewControl->SetCurrentPropertyTalentEvent(true);
		return;
	}

	if(nSegIndex >= nSegCount)
	{
		System::Windows::Forms::MessageBox::Show(L"세그먼트 인덱스 범위를 넘어셨습니다.", "에러");

		return;
	}

	int nCapsuleCount = (int)pTalentInfo->m_HitInfo.m_vSegments[nSegIndex].m_vCapsules.size();
	if(nCapsuleIndex >= nCapsuleCount)
	{
		System::Windows::Forms::MessageBox::Show(L"캡슐 인덱스 범위를 넘어셨습니다.", "에러");
	}
}

void TTalentEventController::OnDeDereferenceTalentEffectIDViewer()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	DereferenceTalentIDViewer dereferenceDlg;
	dereferenceDlg.SetTalentID(pTalentInfo->m_nID);
	dereferenceDlg.SetDerefType(DT_EVENT);
	dereferenceDlg.ShowDialog();
}

void TTalentEventController::UpdateDereferenceControls()
{
	// 역참조 조사
	GlobalObjects::g_pMainForm->m_EventTabPage->Deref_button->Enabled = false;

	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	int nTalentID = pTalentInfo->m_nID;

	for(map<int , CSTalentInfo* >::iterator it = g_pMainApp->m_TTalentInfoMgr.begin(); it != g_pMainApp->m_TTalentInfoMgr.end(); ++it)
	{
		TTalentInfo* pCheckTalentInfo = (TTalentInfo*)(it->second);

		if(pCheckTalentInfo->m_Events.m_nRefID == nTalentID)
		{
			GlobalObjects::g_pMainForm->m_EventTabPage->Deref_button->Enabled = true;
			return;
		}
	}
}

}
