#include "stdafx.h"
#include "TTalentHitTabController.h"
#include "Form1.h"
#include "Xmlseral_Talent.h"
#include "TPartsHitInfoViewer.h"
#include "TClipboard.h"
#include "THitAreaRenderer.h"
#include "TCalcTalentHitEffectDir.h"
#include "DereferenceTalentIDViewer.h"

TTalentHitTabController::TTalentHitTabController(  )
: m_bFromSetSegmentParam(false)
, m_nSegID(-1)
, m_nCapsuleID(-1)
,m_bHitCapsuleAniSync(false)
,m_fPrePlayTime(0.0f)
, m_bHitCapsuleAllView(false)
{

}

TTalentHitTabController::~TTalentHitTabController()
{

}

void TTalentHitTabController::Serialize()
{
	UpdateReferenceControls();
	UpdateTreeView();
	UpdateControlsEnabled();
	UpdateDereferenceControls();
}

void TTalentHitTabController::AddSegment()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	CSHitSegment t;
	MCapsule capsule;

	capsule.bottom.Set(0, -50, 0);
	capsule.top.Set(0, -50, 100);
	capsule.radius = 50;
	capsule.height = 100;

	t.m_vCapsules.push_back(capsule);
	t.m_vDoHitCheck.push_back(true);

	pTalentInfo->m_HitInfo.m_vSegments.push_back(t);
}

void TTalentHitTabController::DeleteSegment( int segID )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	int p = 0;

	for (vector<CSHitSegment>::iterator i=pTalentInfo->m_HitInfo.m_vSegments.begin(); i!=pTalentInfo->m_HitInfo.m_vSegments.end(); ++i)
	{
		if (p == segID)
		{
			pTalentInfo->m_HitInfo.m_vSegments.erase(i);
			return;
		}
		p++;
	}
}

void TTalentHitTabController::ApplySegmentParams( int segID )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if (m_bFromSetSegmentParam) return;

	if (pTalentInfo->m_HitInfo.m_vSegments.empty()) return;
	if (pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.empty()) return;

	if( m_nCapsuleID < 0) return;

	float rad, hei;
	RVector d = pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[m_nCapsuleID].GetDir();
	RVector b;

	XmlSerializable::TALENTCAPSULE^ capsule = 
		(XmlSerializable::TALENTCAPSULE^)(GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->SelectedObject);

	rad = capsule->반지름;
	hei = capsule->높이;
	b.x = capsule->조절점1_X;
	b.y = capsule->조절점1_Y;
	b.z = capsule->조절점1_Z;
	pTalentInfo->m_HitInfo.m_vSegments[segID].m_fCheckTime = capsule->판정딜레이;

	if (d.Length() < FLT_EPSILON)
		d = RVector::AXISZ;

	pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[m_nCapsuleID] = RCapsule(b ,d, rad, hei);
	pTalentInfo->m_HitInfo.m_vSegments[segID].m_vDoHitCheck[m_nCapsuleID] = capsule->DoHitCheck;

	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
}

void TTalentHitTabController::OnChangeSegmentTop( int segID )
{
	if (segID < 0) return;

	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if (m_bFromSetSegmentParam) return;

	if (pTalentInfo->m_HitInfo.m_vSegments.empty()) return;
	if (pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.empty()) return;

	if( m_nCapsuleID < 0) return;

	float rad;
	RVector b, t;

	XmlSerializable::TALENTCAPSULE^ capsule =
		(XmlSerializable::TALENTCAPSULE^)(GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->SelectedObject);
	rad = capsule->반지름;
	b.x = capsule->조절점1_X;
	b.y = capsule->조절점1_Y;
	b.z = capsule->조절점1_Z;
	t.x = capsule->조절점2_X;
	t.y = capsule->조절점2_Y;
	t.z = capsule->조절점2_Z;
	pTalentInfo->m_HitInfo.m_vSegments[segID].m_fCheckTime = capsule->판정딜레이;
	pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[m_nCapsuleID] = RCapsule(b, t, rad);
	pTalentInfo->m_HitInfo.m_vSegments[segID].m_vDoHitCheck[m_nCapsuleID] = capsule->DoHitCheck;

	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);

	//char buf[128];
	//sprintf(buf, "%d", (int)m_pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[m_nCapsuleID].height);
	//GlobalObjects::g_pMainForm->CapsuleHeight->Text = gcnew String(buf);
	capsule->높이 = pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[m_nCapsuleID].height;
	GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->Refresh();

	UpdateTalentHitEffectDir(pTalentInfo, segID);
}

void TTalentHitTabController::SetCapsuleProperty( int segID, int capsuleID )
{
	if (segID < 0) return;
	if (capsuleID < 0) return;

	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if (pTalentInfo->m_HitInfo.m_nReferenceID > 0)
	{
		pTalentInfo = TTalentHelper::GetTalentInfo(pTalentInfo->m_HitInfo.m_nReferenceID, pTalentInfo->m_HitInfo.m_nReferenceMode);
	}


	m_bFromSetSegmentParam = true;

	if (pTalentInfo->m_HitInfo.m_vSegments.empty()) return;
	if (pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.empty()) return;

	int size = pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.size();
	if ( size <= capsuleID ) return;

	m_nSegID = segID;
	m_nCapsuleID = capsuleID;

	XmlSerializable::TALENTCAPSULE^ capsule =
		(XmlSerializable::TALENTCAPSULE^)(GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->SelectedObject);

	bool bg = false;
	if( capsule == nullptr)
	{
		bg = true;
		capsule = gcnew XmlSerializable::TALENTCAPSULE;
		//mlog("gcnew XmlSerializable::TALENTCAPSULE. \r");
	}

	capsule->반지름		= pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[capsuleID].radius;
	capsule->높이		= pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[capsuleID].height;
	capsule->조절점1_X	= pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[capsuleID].bottom.x;
	capsule->조절점1_Y	= pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[capsuleID].bottom.y;
	capsule->조절점1_Z	= pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[capsuleID].bottom.z;
	capsule->조절점2_X	= pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[capsuleID].top.x;
	capsule->조절점2_Y	= pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[capsuleID].top.y;
	capsule->조절점2_Z	= pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules[capsuleID].top.z;
	capsule->DoHitCheck = pTalentInfo->m_HitInfo.m_vSegments[segID].m_vDoHitCheck[capsuleID]; 
	capsule->판정딜레이	= pTalentInfo->m_HitInfo.m_vSegments[segID].m_fCheckTime;

	if( bg )
	{
		GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->SelectedObject = capsule;
		//mlog("바인딩. \r");
	}
	else
		GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->Refresh();

	m_bFromSetSegmentParam = false;
}

void TTalentHitTabController::AddCapsule()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if (pTalentInfo == NULL || m_nSegID == -1) return;

	if( pTalentInfo->m_HitInfo.m_vSegments.size() == 0) return;

	int ncount = pTalentInfo->m_HitInfo.m_vSegments.size();
	if( ncount < (m_nSegID +1)) return;

	MCapsule capsule;
	capsule.bottom.Set(0, -50, 0);
	capsule.top.Set(0, -50, 100);
	capsule.radius = 50;
	capsule.height = 100;

	pTalentInfo->m_HitInfo.m_vSegments[m_nSegID].m_vCapsules.push_back(capsule);
	pTalentInfo->m_HitInfo.m_vSegments[m_nSegID].m_vDoHitCheck.push_back(true);
}

void TTalentHitTabController::DeleteCapsule( int segID, int capsuleID )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	// TODO: 현재 선택된 세그먼트의
	//CSHitSegment t;

	int nSegCount = pTalentInfo->m_HitInfo.m_vSegments.size();
	if( nSegCount == 0) return;
	if( nSegCount < (segID+1)) return;
	if( pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.size() == 0) return;

	if( capsuleID == 0 && pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.size() == 1)
	{
		GlobalObjects::g_pMainForm->m_TalentTabPage->m_TalentHitTabController->DeleteSegment(segID);
		return;
	}

	int p = 0;

	for( vector<MCapsule>::iterator i = pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.begin();
		i != pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.end(); ++i)
	{
		if( p == capsuleID)
		{
			pTalentInfo->m_HitInfo.m_vSegments[segID].m_vCapsules.erase(i);
			return;
		}
		p++;
	}
}

void TTalentHitTabController::MoveSegmentBottom( int dx, int dy, int dz )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;


	if( m_nSegID == -1 || m_nCapsuleID == -1)
	{
		mlog("에러 : 세그먼트나 캡슐의 선택이 잘못되었습니다. 캡슐을 선택하세요.\r");
		return;
	}

	int n = m_nSegID;// = GlobalObjects::g_pMainForm->HitSegmentList->SelectedIndex;
	if (n < 0) return;

	if (pTalentInfo->m_HitInfo.m_vSegments.empty()) return;
	if (pTalentInfo->m_HitInfo.m_vSegments[n].m_vCapsules.empty()) return;

	RVector v = RVector(0.f, 1.f, 0.f);// -m_pCamera->GetDirection();//m_pActor->GetDirection();

	v.z = 0;
	v.Normalize();

	RVector side;

	side = v.CrossProduct(RVector::AXISZ);

	RVector mov;

	mov = dy * 3.0f * v;
	mov += dx * 3.0f * -side;
	mov += dz * 3.0f * RVector::AXISZ;

	pTalentInfo->m_HitInfo.m_vSegments[n].m_vCapsules[m_nCapsuleID].bottom += mov;
	pTalentInfo->m_HitInfo.m_vSegments[n].m_vCapsules[m_nCapsuleID].CalcHeight();

	//SetCapsuleProperty(n, m_nCapsuleID);
	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
}

void TTalentHitTabController::MoveSegmentTop( int dx, int dy, int dz )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;


	if( m_nSegID == -1 || m_nCapsuleID == -1)
	{
		mlog("에러 : 세그먼트나 캡슐의 선택이 잘못되었습니다. 캡슐을 선택하세요.\r");
		return;
	}

	int n = m_nSegID;// = GlobalObjects::g_pMainForm->HitSegmentList->SelectedIndex;
	if (n < 0) return;

	if (pTalentInfo->m_HitInfo.m_vSegments.empty()) return;
	if (pTalentInfo->m_HitInfo.m_vSegments[n].m_vCapsules.empty()) return;

	RVector v = RVector(0.f, 1.f, 0.f);//-m_pCamera->GetDirection();//m_pActor->GetDirection();

	v.z = 0;
	v.Normalize();

	RVector side;

	side = v.CrossProduct(RVector::AXISZ);

	RVector mov;

	mov = dy * 3.0f * v;
	mov += dx * 3.0f * -side;
	mov += dz * 3.0f * RVector::AXISZ;

	pTalentInfo->m_HitInfo.m_vSegments[n].m_vCapsules[m_nCapsuleID].top += mov;

	pTalentInfo->m_HitInfo.m_vSegments[n].m_vCapsules[m_nCapsuleID].CalcHeight();

	//SetCapsuleProperty(n, m_nCapsuleID);
	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
}

void TTalentHitTabController::AddDo()
{
	if(GlobalObjects::g_pMainForm->tabControl1->SelectedIndex == 0) return;

	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if( m_nSegID == -1 || m_nCapsuleID == -1) return;
	if (pTalentInfo->m_HitInfo.m_vSegments.empty()) return;

	if (GlobalObjects::g_pMainForm->Bottom_toolStripButton->Checked || GlobalObjects::g_pMainForm->Top_toolStripButton->Checked)
	{
		CapsuleInfo* info = new CapsuleInfo;
		info->skillid = pTalentInfo->m_nID;
		info->segid = m_nSegID;
		info->capsuleid = m_nCapsuleID;
		info->capsule = pTalentInfo->m_HitInfo.m_vSegments[m_nSegID].m_vCapsules[m_nCapsuleID];
		info->fCheckTime = pTalentInfo->m_HitInfo.m_vSegments[m_nSegID].m_fCheckTime;

		m_queueCapsuleInfo.push_front(info);
	}
}

void TTalentHitTabController::AddReDo()
{
	if(GlobalObjects::g_pMainForm->tabControl1->SelectedIndex == 0) return;

	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if( m_nSegID == -1 || m_nCapsuleID == -1) return;
	if (pTalentInfo->m_HitInfo.m_vSegments.empty()) return;

	SetCapsuleProperty(m_nSegID, m_nCapsuleID);

	if (GlobalObjects::g_pMainForm->Bottom_toolStripButton->Checked || GlobalObjects::g_pMainForm->Top_toolStripButton->Checked)
	{
		CapsuleInfo* info = new CapsuleInfo;
		info->skillid = pTalentInfo->m_nID;
		info->segid = m_nSegID;
		info->capsuleid = m_nCapsuleID;
		info->capsule = pTalentInfo->m_HitInfo.m_vSegments[m_nSegID].m_vCapsules[m_nCapsuleID];
		info->fCheckTime = pTalentInfo->m_HitInfo.m_vSegments[m_nSegID].m_fCheckTime;

		m_queueCapsuleInfo.push_back(info);
	}
}

void TTalentHitTabController::CopyCapsule()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	ArrayList^ arrSelectedTalentHitInfo = GlobalObjects::g_pMainForm->m_TalentTabPage->HitSegmenttreeView->SelectedNodes;

	Character_NET::TClipboardGroupCapsuleBoxData^ clipboardCapsuleList = gcnew Character_NET::TClipboardGroupCapsuleBoxData();

	System::Collections::IEnumerator^ myEnumerator = arrSelectedTalentHitInfo->GetEnumerator();
	while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
	{
		TreeNode^ node = safe_cast<TreeNode^>(myEnumerator->Current);
		Object^ tag	= node->Tag;
		if(tag == nullptr)
			continue;

		if(node->Level == 0)
		{
			// 세그먼트
			if((int)pTalentInfo->m_HitInfo.m_vSegments.size() <= (int)tag)
				continue;

			Character_NET::TClipboardCollisionData^ clipboardSegmentCapsuleData = gcnew Character_NET::TClipboardCollisionData();
			for(int i = 0; i < (int)pTalentInfo->m_HitInfo.m_vSegments[(int)tag].m_vCapsules.size(); ++i)
			{
				// 클립보드로 복사
				MCapsule copyedCapsule = pTalentInfo->m_HitInfo.m_vSegments[(int)tag].m_vCapsules[i];
				float fCopyedCheckTime = pTalentInfo->m_HitInfo.m_vSegments[(int)tag].m_fCheckTime;

				Character_NET::TClipboardCapsuleData^ clipboardCapsuleData = gcnew Character_NET::TClipboardCapsuleData();
				clipboardCapsuleData->Capsule = copyedCapsule;
				clipboardCapsuleData->fCheckTime = fCopyedCheckTime;
				
				clipboardSegmentCapsuleData->m_Capsule.CopyFrom(clipboardCapsuleData);
			}

			clipboardCapsuleList->AddGroupData(clipboardSegmentCapsuleData);
		}
		else
		{
			Object^ parentTag = node->Parent->Tag;

			if (((int)parentTag >= 0) && ((int)tag >= 0))
			{
				if( (int)pTalentInfo->m_HitInfo.m_vSegments.size() <= (int)parentTag ||
					(int)pTalentInfo->m_HitInfo.m_vSegments[(int)parentTag].m_vCapsules.size() <= (int)tag)
					continue;

				MCapsule copyedCapsule = pTalentInfo->m_HitInfo.m_vSegments[(int)parentTag].m_vCapsules[(int)tag];
				float fCopyedCheckTime = pTalentInfo->m_HitInfo.m_vSegments[(int)parentTag].m_fCheckTime;

				// 클립보드로 복사
				Character_NET::TClipboardCapsuleData^ clipboardCapsuleData = gcnew Character_NET::TClipboardCapsuleData();
				clipboardCapsuleData->Capsule = copyedCapsule;
				clipboardCapsuleData->fCheckTime = fCopyedCheckTime;

				clipboardCapsuleList->m_Capsule.CopyFrom(clipboardCapsuleData);
			}
		}
	}

	TClipboardHelper::CopyGroupCapsuleData(clipboardCapsuleList);
}

void TTalentHitTabController::PasteCapsule()
{
	Character_NET::TClipboardGroupCapsuleBoxData^ clipboardCapsuleList = TClipboardHelper::PasteGroupCapsuleData();

	if (clipboardCapsuleList == nullptr) return;

	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	int nSelectSegmentID = -1;
	TreeNode^ node = GlobalObjects::g_pMainForm->m_TalentTabPage->HitSegmenttreeView->SelectedNode;
	if(node != nullptr)
	{
		Object^ tag = node->Tag;
		nSelectSegmentID = (int)tag;
		if(node->Level == 1)
		{
			nSelectSegmentID = (int)node->Parent->Tag;
		}
	}

	// 그룹
	int nSegmentCount = clipboardCapsuleList->m_arrGroup->Count;
	for(int j = 0; j < nSegmentCount; ++j)
	{
		TClipboardCollisionData^ pCopySegmentData = clipboardCapsuleList->GetGroupData(j);
		if(pCopySegmentData == nullptr)
			continue;

		int nNewSegIndex = AddEmptySegment(pTalentInfo);

		for(int nCapsuleIndex = 0; nCapsuleIndex < pCopySegmentData->m_Capsule.m_arrCapsule->Count; ++nCapsuleIndex)
		{
			Character_NET::TClipboardCapsuleData^ clipboardCapsuleData = gcnew Character_NET::TClipboardCapsuleData();
			 pCopySegmentData->m_Capsule.CopyTo(nCapsuleIndex, clipboardCapsuleData);
			 
			MCapsule capsuleNew = clipboardCapsuleData->Capsule;

			pTalentInfo->m_HitInfo.m_vSegments[nNewSegIndex].m_vCapsules.push_back(capsuleNew);
			pTalentInfo->m_HitInfo.m_vSegments[nNewSegIndex].m_vDoHitCheck.push_back(true);
			pTalentInfo->m_HitInfo.m_vSegments[nNewSegIndex].m_fCheckTime = clipboardCapsuleData->fCheckTime;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 캡슐
	int nCapsuleCount = clipboardCapsuleList->m_Capsule.m_arrCapsule->Count;

	if(nCapsuleCount > 0 && nSelectSegmentID == -1)
	{
		//선택된 세그먼트가 없으면 만들어라...
		nSelectSegmentID = AddEmptySegment(pTalentInfo);
	}

	if((int)pTalentInfo->m_HitInfo.m_vSegments.size() <= nSelectSegmentID)
		return;

	for(int i = 0; i < nCapsuleCount; ++i)
	{
		Character_NET::TClipboardCapsuleData^ clipboardCapsuleData = gcnew Character_NET::TClipboardCapsuleData();
		clipboardCapsuleList->m_Capsule.CopyTo(i, clipboardCapsuleData);

		pTalentInfo->m_HitInfo.m_vSegments[nSelectSegmentID].m_vCapsules.push_back(clipboardCapsuleData->Capsule);
		pTalentInfo->m_HitInfo.m_vSegments[nSelectSegmentID].m_vDoHitCheck.push_back(true);
		pTalentInfo->m_HitInfo.m_vSegments[nSelectSegmentID].m_fCheckTime = clipboardCapsuleData->fCheckTime;

		SetCapsuleProperty(nSelectSegmentID, pTalentInfo->m_HitInfo.m_vSegments[nSelectSegmentID].m_vCapsules.size() - 1);
	}

	Serialize();
	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
}

void TTalentHitTabController::UndoCommand()
{
	if (m_queueCapsuleInfo.empty()) return;

	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	CapsuleInfo* info = m_queueCapsuleInfo.front();
	if( info && info->skillid == pTalentInfo->m_nID)
	{
		SetDo(info);
		ApplySegmentParams(info->segid);

		m_queueCapsuleInfo.pop_front();

		m_queueCapsuleInfo.push_back(info);
	}
}

void TTalentHitTabController::RedoCommand()
{
	if (m_queueCapsuleInfo.empty()) return;

	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	CapsuleInfo* info = m_queueCapsuleInfo.back();
	if( info && info->skillid == pTalentInfo->m_nID)
	{
		SetDo(info);
		ApplySegmentParams(info->segid);

		m_queueCapsuleInfo.pop_back();

		m_queueCapsuleInfo.push_front(info);
	}
}

void TTalentHitTabController::ClearQueue()
{
	while(!m_queueCapsuleInfo.empty())
	{
		CapsuleInfo* info = m_queueCapsuleInfo.front();
		m_queueCapsuleInfo.pop_front();
		delete info;
	}
}

void TTalentHitTabController::SetDo( CapsuleInfo* capsuleinfo )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	XmlSerializable::TALENTCAPSULE^ capsule =
		(XmlSerializable::TALENTCAPSULE^)(GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->SelectedObject);

	bool bg = false;
	if( capsule == nullptr)
	{
		bg = true;
		capsule = gcnew XmlSerializable::TALENTCAPSULE;
		mlog("정보 : gcnew XmlSerializable::TALENTCAPSULE \r");
	}

	capsule->반지름		= capsuleinfo->capsule.radius;
	capsule->높이		= capsuleinfo->capsule.height;
	capsule->조절점1_X	= capsuleinfo->capsule.bottom.x;
	capsule->조절점1_Y	= capsuleinfo->capsule.bottom.y;
	capsule->조절점1_Z	= capsuleinfo->capsule.bottom.z;
	capsule->조절점2_X	= capsuleinfo->capsule.top.x;
	capsule->조절점2_Y	= capsuleinfo->capsule.top.y;
	capsule->조절점2_Z	= capsuleinfo->capsule.top.z;
	capsule->판정딜레이	= capsuleinfo->fCheckTime;
	capsule->DoHitCheck = capsuleinfo->bDohitCheck;

	if( bg )
	{
		GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->SelectedObject = capsule;
		//mlog("바인딩. \r");
	}
	else
		GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->Refresh();

	pTalentInfo->m_HitInfo.m_vSegments[capsuleinfo->segid].m_vCapsules[capsuleinfo->capsuleid] = capsuleinfo->capsule;
	pTalentInfo->m_HitInfo.m_vSegments[capsuleinfo->segid].m_vDoHitCheck[capsuleinfo->capsuleid] = capsuleinfo->bDohitCheck;
}

void TTalentHitTabController::Init()
{
}

void TTalentHitTabController::Clear()
{
	GlobalObjects::g_SelInfo.m_nMeshHitGroupID = -1;
	GlobalObjects::g_SelInfo.m_nMeshHitCapsuleID = -1;

}

void TTalentHitTabController::DeleteSelSegment()
{
	DeleteSegment(m_nSegID);
}

void TTalentHitTabController::DeleteSelCapsule()
{
	DeleteCapsule(m_nSegID, m_nCapsuleID);
}

void TTalentHitTabController::ClearSel()
{
	m_nSegID = -1;
	m_nCapsuleID = -1;
}

void TTalentHitTabController::OnDeleteSegmentClick()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	ArrayList^ arrSelectedTalentHitInfo = GlobalObjects::g_pMainForm->m_TalentTabPage->HitSegmenttreeView->SelectedNodes;

	int nCount = arrSelectedTalentHitInfo->Count;
	if(nCount == 0)
	{
		System::Windows::Forms::MessageBox::Show(gcnew String("선택된 세그먼트나 캡슐이 없습니다."), "탤런트 판정 삭제 에러");
		return;
	}

	if (System::Windows::Forms::MessageBox::Show(String::Format("선택된 {0}개의 세그먼트나 캡슐을 삭제 하시겠습니까? 세그먼트 삭제시에는 포함된 모든 캡슐이 삭제 됩니다.", nCount), "탤런트 판정 삭제 에러"
		,MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes)
	{
		vector<delGroupCapsuleData> vecDelIndex;

		System::Collections::IEnumerator^ myEnumerator = arrSelectedTalentHitInfo->GetEnumerator();
		while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
		{
			TreeNode^ node = safe_cast<TreeNode^>(myEnumerator->Current);
			Object^ tag	= node->Tag;
			if(tag == nullptr)
				continue;

			if(node->Level == 0)
			{
				// 세그먼트
				if((int)pTalentInfo->m_HitInfo.m_vSegments.size() <= (int)tag)
					continue;

				delGroupCapsuleData delData;
				delData.nGroupID = (int)tag;
				vecDelIndex.push_back(delData);
			}
			else
			{
				Object^ parentTag = node->Parent->Tag;
				if(parentTag == nullptr)
					continue;

				delGroupCapsuleData delData;
				delData.nGroupID = (int)parentTag;
				delData.nCapsuleID = (int)tag;
				vecDelIndex.push_back(delData);
			}
		}

		// 삭제
		std::sort(vecDelIndex.begin(), vecDelIndex.end(), delGroupCapsuleData::descending);
		for(vector<delGroupCapsuleData>::iterator it = vecDelIndex.begin(); it != vecDelIndex.end(); ++it)
		{
			if(it->nCapsuleID == -1)
				DeleteSegment(it->nGroupID);
			else
				DeleteCapsule(it->nGroupID, it->nCapsuleID);
		}
	}

	Serialize();
	ClearSel();

	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
}

void TTalentHitTabController::OnAddSegmentClick()
{
	AddSegment();
	Serialize();
	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
}

void TTalentHitTabController::OnChangeSelSegmentTop()
{
	OnChangeSegmentTop(m_nSegID);
}

void TTalentHitTabController::SetSelCapsuleProperty()
{
	SetCapsuleProperty(m_nSegID, m_nCapsuleID);
}

void TTalentHitTabController::OnHitTreeViewAfterSelect( System::Windows::Forms::TreeViewEventArgs^ e )
{
	m_vecShowCapsuleID.clear();
	int i = e->Node->GetNodeCount(true);
	//최상위노드이면
	if( e->Node->Parent == nullptr )
	{
		GlobalObjects::g_pMainForm->m_TalentTabPage->Capsule_propertyGrid->SelectedObject = nullptr;
		int n = e->Node->Index;
		if (n >= 0)
		{
			int nCount = e->Node->GetNodeCount(true);

			for(int i =0 ; i < nCount; ++i)
				m_vecShowCapsuleID.push_back(i);

			m_nSegID = n;
		}
	}
	else
	{
		int pn = e->Node->Parent->Index;
		int n = e->Node->Index;
		if( n >= 0)
			SetCapsuleProperty(pn, n);

		m_vecShowCapsuleID.push_back(n);
	}

}

void TTalentHitTabController::OnAddCapsuleClick()
{
	AddCapsule();
	Serialize();
	GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
}

void TTalentHitTabController::UpdateTreeView()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	CSTalentHitInfo* HitInfo = &pTalentInfo->m_HitInfo;

	if (pTalentInfo->m_HitInfo.m_nReferenceID > 0)
	{
		CSTalentInfo* pRefTalentInfo = TTalentHelper::GetTalentInfo(pTalentInfo->m_HitInfo.m_nReferenceID, pTalentInfo->m_HitInfo.m_nReferenceMode);
		if(pRefTalentInfo == NULL) return;

		HitInfo = &pRefTalentInfo->m_HitInfo;
	}

	GlobalObjects::g_pMainForm->m_TalentTabPage->HitSegmenttreeView->ClearTreeView();

	for (size_t i=0; i<HitInfo->m_vSegments.size(); i++)
	{
		TreeNode^ node = GlobalObjects::g_pMainForm->m_TalentTabPage->HitSegmenttreeView->Nodes->Add(String::Format("Segment : {0}",	i));
		node->Tag = (int)i;
		for( size_t j = 0; j < HitInfo->m_vSegments[i].m_vCapsules.size(); j++)
		{
			TreeNode^ childNode = node->Nodes->Add(String::Format("Capsule: {0}",	j));
			childNode->Tag = (int)j;
		}

		node->ExpandAll();
	}
}

void TTalentHitTabController::OnHitReferenceTextChanged(int nRefTalentID, int nRefTalentMode)
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if( (pTalentInfo->m_HitInfo.m_nReferenceID != nRefTalentID) ||
		(pTalentInfo->m_HitInfo.m_nReferenceMode != nRefTalentMode ))
	{
		GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
	}

	TTalentInfo* refTalent = TTalentHelper::GetTalentInfo(nRefTalentID, nRefTalentMode);
	if(refTalent)
	{
		pTalentInfo->m_HitInfo.m_nReferenceID = nRefTalentID;
		pTalentInfo->m_HitInfo.m_nReferenceMode = nRefTalentMode;
		pTalentInfo->m_HitInfo.m_vSegments.clear();
		//g_pMainApp->m_TTalentInfoMgr.CopyHitFromReference(pTalentInfo);  // 카피를 해야 하나? 가져다 쓰면 안되나..
	}
	else
	{
		pTalentInfo->m_HitInfo.m_nReferenceID = 0;
		pTalentInfo->m_HitInfo.m_nReferenceMode = 0;
		pTalentInfo->m_HitInfo.m_vSegments.clear();
	}

	Serialize();
}

void TTalentHitTabController::OnHitReferenceTextChanged(System::Object^  sender)
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	System::Windows::Forms::TextBox^ tb = (System::Windows::Forms::TextBox^)sender;
	String^ strRefTalentID = tb->Text;

	int nRefTalentID = 0;
	
	if (strRefTalentID->Length > 0)
	{
		try {
			nRefTalentID = Convert::ToInt32(strRefTalentID);
		}
		catch (...) {}
	}

	int nLastRefID = pTalentInfo->m_HitInfo.m_nReferenceID;

	pTalentInfo->m_HitInfo.m_nReferenceID = 0;

	if (nRefTalentID > 0)
	{
		CSTalentInfo* pRefTalentInfo = g_pMainApp->m_TTalentInfoMgr.Get(nRefTalentID);
		if (pRefTalentInfo)
		{
			pTalentInfo->m_HitInfo.m_nReferenceID = nRefTalentID;
			pTalentInfo->m_HitInfo.m_nReferenceMode = 0;
			g_pMainApp->m_TTalentInfoMgr.CopyHitFromReference(pTalentInfo);
		}
	}


	Serialize();

	if (nLastRefID != pTalentInfo->m_HitInfo.m_nReferenceID)
	{
		GlobalObjects::g_SaveNotifier.SetSaveTalentHitInfo(true);
	}
}

void TTalentHitTabController::UpdateControlsEnabled()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	Character_NET::Form1^ pMainForm = GlobalObjects::g_pMainForm;

	bool bEnabled = true;

	if (pTalentInfo->m_HitInfo.m_nReferenceID > 0)
	{
		bEnabled = false;
	}

	//pMainForm->m_TalentTabPage->HitSegmenttreeView->Enabled = bEnabled;

	pMainForm->m_TalentTabPage->AddSegment->Enabled = bEnabled;
	pMainForm->m_TalentTabPage->AddCapsule->Enabled = bEnabled;
	pMainForm->m_TalentTabPage->DeleteSegment->Enabled = bEnabled;
	pMainForm->m_TalentTabPage->CapsuleCopy->Enabled = bEnabled;
	pMainForm->m_TalentTabPage->CapsulePaste->Enabled = bEnabled;
	pMainForm->m_TalentTabPage->Capsule_propertyGrid->Enabled = bEnabled;
}

void TTalentHitTabController::UpdateReferenceControls()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	int nSelectTalentID = pTalentInfo->m_HitInfo.m_nReferenceID;
	int nSelectTalentMode = pTalentInfo->m_HitInfo.m_nReferenceMode;

	if(nSelectTalentID > 0)
	{
		// 레퍼런스 입력
		string strTalentID = TStrings::TalentIDString_WithMode(nSelectTalentID, nSelectTalentMode);
		GlobalObjects::g_pMainForm->m_TalentTabPage->tbHitReference->Text = gcnew String(String::Format("{0}", gcnew String(strTalentID.c_str())));
	}
	else
	{
		GlobalObjects::g_pMainForm->m_TalentTabPage->tbHitReference->Text = gcnew String("");
	}
}

void TTalentHitTabController::Render()
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if(pTalentInfo->m_HitInfo.m_nReferenceID > 0 )
	{
		pTalentInfo = TTalentHelper::GetTalentInfo(pTalentInfo->m_HitInfo.m_nReferenceID, pTalentInfo->m_HitInfo.m_nReferenceMode);
		if (pTalentInfo == NULL) return;
	}

	if(m_bHitCapsuleAllView)
	{
		THitAreaRenderer::GetInstance().AllRender(pTalentInfo, m_nSegID, GlobalObjects::g_pMainForm->CapsuleOpa_toolStripButton->Checked);
	}

	for(vector<int>::iterator itCapsuleID = m_vecShowCapsuleID.begin(); itCapsuleID != m_vecShowCapsuleID.end(); ++itCapsuleID)
	{
		THitAreaRenderer::GetInstance().Render(pTalentInfo, m_nSegID, *itCapsuleID,
			true, GlobalObjects::g_pMainForm->CapsuleOpa_toolStripButton->Checked);
	}
}

void TTalentHitTabController::OnAniSync_checkBox_CheckedChanged( bool bCheck )
{
	m_bHitCapsuleAniSync = bCheck;
}

void TTalentHitTabController::Play( float fTime )
{
	if(m_bHitCapsuleAniSync == false ||
		fTime < 0)
		return;

	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	if(pTalentInfo->m_HitInfo.m_nReferenceID > 0 )
	{
		pTalentInfo = TTalentHelper::GetTalentInfo(pTalentInfo->m_HitInfo.m_nReferenceID, pTalentInfo->m_HitInfo.m_nReferenceMode);
		if (pTalentInfo == NULL) return;
	}

	int nSeg = 0;
	for(vector<CSHitSegment>::iterator itHit = pTalentInfo->m_HitInfo.m_vSegments.begin(); itHit != pTalentInfo->m_HitInfo.m_vSegments.end(); ++itHit)
	{
		if(m_fPrePlayTime < (*itHit).m_fCheckTime &&
			fTime >= (*itHit).m_fCheckTime)
		{
			GlobalObjects::g_pMainForm->m_TalentTabPage->HitSegmenttreeView->SelectedNode =
				g_pMainApp->SearchNode(GlobalObjects::g_pMainForm->m_TalentTabPage->HitSegmenttreeView->Nodes, String::Format("Segment : {0}", nSeg));

			break;
		}

		++nSeg;
	}

	m_fPrePlayTime = fTime;
}

void TTalentHitTabController::UpdateTalentHitEffectDir( CSTalentInfo* pTalentInfo, bool bCheck /*= false*/ )
{
	// 히트 방향 새로 고침
	TCalcTalentHitEffectDir hitDir;
	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	// NPC는... 하지 말자
	if(pTalentInfo->m_setNPCID.size() > 0)
		return;

	if(hitDir.CalcTalentHitEffectDir(static_cast<XTalentInfo*>(pTalentInfo), nn, bCheck))
	{
		GlobalObjects::g_pMainForm->m_TalentEffectTabController->Serialize();
		GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
	}
}

void TTalentHitTabController::UpdateTalentHitEffectDir( CSTalentInfo* pTalentInfo, int nSegID /*= -1*/ )
{
	// 히트 방향 새로 고침
	TCalcTalentHitEffectDir hitDir;
	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	// NPC는... 하지 말자
	if(pTalentInfo->m_setNPCID.size() > 0)
		return;

	if(hitDir.CalcTalentHitEffectDir(static_cast<XTalentInfo*>(pTalentInfo), nn, nSegID))
	{
		GlobalObjects::g_pMainForm->m_TalentEffectTabController->Serialize();
		GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
	}
}

void TTalentHitTabController::OnHitTreeViewAllSelect( bool bCheck )
{
	m_bHitCapsuleAllView = bCheck;
}

void TTalentHitTabController::OnDeDereferenceTalentEffectIDViewer()
{
	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	DereferenceTalentIDViewer dereferenceDlg;
	dereferenceDlg.SetTalentID(pTalentInfo->m_nID);
	dereferenceDlg.SetDerefType(DT_HIT);
	dereferenceDlg.ShowDialog();
}

void TTalentHitTabController::UpdateDereferenceControls()
{
	// 역참조 조사
	GlobalObjects::g_pMainForm->m_TalentTabPage->Deref_button->Enabled = false;

	TTalentInfo* pTalentInfo = (TTalentInfo*)g_pMainApp->GetTalentInfo();
	if (pTalentInfo == NULL) return;

	int nTalentID = pTalentInfo->m_nID;

	for(map<int , CSTalentInfo* >::iterator it = g_pMainApp->m_TTalentInfoMgr.begin(); it != g_pMainApp->m_TTalentInfoMgr.end(); ++it)
	{
		TTalentInfo* pCheckTalentInfo = (TTalentInfo*)(it->second);

		if(pCheckTalentInfo->m_HitInfo.m_nReferenceID == nTalentID)
		{
			GlobalObjects::g_pMainForm->m_TalentTabPage->Deref_button->Enabled = true;
			return;
		}
	}
}

int TTalentHitTabController::AddEmptySegment( CSTalentInfo* pTalentInfo )
{
	CSHitSegment t;

	pTalentInfo->m_HitInfo.m_vSegments.push_back(t);

	// 인덱스로 쓸 번호를 넘긴다.
	return pTalentInfo->m_HitInfo.m_vSegments.size() - 1;
}
