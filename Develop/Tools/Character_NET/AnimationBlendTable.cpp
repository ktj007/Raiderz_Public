#include "StdAfx.h"
#include "AnimationBlendTable.h"
#include "TAnimationBlendTable.h"
#include "AnimationBlendInputForm.h"

System::Void Character_NET::AnimationBlendTable::AnimationBlendTable_Load( System::Object^ sender, System::EventArgs^ e )
{
	Load_AnimationList();

	AddAnimationBlend_button->Enabled				= false;
	BlendAnimation_listView->Enabled				= false;

	// 이미 선택된 애니메이션이 있다면...
	if(m_strSelectAniName != nullptr)
	{
		ReSetCurrentSelectListView();
	}
}

System::Void Character_NET::AnimationBlendTable::Finish_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(m_bSave)
	{
		g_pMainApp->m_AnimationBledTable.SaveBlendInfo();
	}

	this->DialogResult = System::Windows::Forms::DialogResult::OK;
	this->Close();
}

System::Void Character_NET::AnimationBlendTable::AddAnimationBlend_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(m_strSelectAniName == nullptr)
	{
		// 에러
		System::Windows::Forms::MessageBox::Show(L"적용 할 애니메이션을 입력하시오", "선택 실패!");
		return;
	}

	vector<string> strAniSourceList;
	GetSelectSourceAniList(strAniSourceList);

	vector<string> strAniBlendList;
	float fBlendTime = 0.0f;

	if(Show_AnimationBlendInputForm(strAniSourceList, strAniBlendList, fBlendTime, false) == false)
		return ;

	for(vector<string>::iterator itSource = strAniSourceList.begin(); itSource != strAniSourceList.end(); ++itSource)
	{
		for(vector<string>::iterator it = strAniBlendList.begin(); it != strAniBlendList.end(); ++it)
		{
			if(AddAnimationBlendInfo(*itSource, *it, fBlendTime))
				m_bSave = true;
		}
	}

	// 갱신
	Load_AnimationBlendList();
	ReLoad_AnimationList();
}

void Character_NET::AnimationBlendTable::FindAnimation( System::String^ strAnimation )
{
	Animation_listView->Items->Clear();

	if( g_pMainApp->GetActor() == NULL || g_pMainApp->GetActor()->m_pMesh == NULL)
		return;

	RMeshAnimationMgr* pAMgr = &g_pMainApp->GetActor()->m_pMesh->m_AniMgr;

	vector<string> strErrorMessage;

	Animation_listView->BeginUpdate();
	for(RAnimationHashList_Iter it = pAMgr->m_NodeHashList.begin(); it != pAMgr->m_NodeHashList.end();it++)
	{
		RAnimation* pAnimation = *it;
		if(pAnimation)
		{
			string strAniName = pAnimation->GetAliasName();

			if(IsEqualString(gcnew String(strAniName.c_str()), strAnimation) == false)
				continue;

			//파일이 존재하는냐?
			RAnimation* pAnimation = g_pMainApp->m_TabAnimation.GetAnimation(strAniName.c_str());
			if( pAnimation == NULL
				|| pAnimation->GetID() < 0
				|| pAnimation->GetResource() == NULL
				|| !ExistFile(pAnimation->GetResource()->GetName().c_str()))
			{
				continue;
			}

			String^ Name = gcnew String(strAniName.c_str() );

			ListViewItem^ AniItem = gcnew ListViewItem(Name, 0);

			// 오버레이 데이터가 몇개 존재 하는가?
			ANIMATION_BLEND_LIST * pAniBlendList = GetAniBlendList(strAniName);
			if(pAniBlendList && 
				pAniBlendList->vecTargetList.size() > 0)
			{
				char szBuf[4] = {0,};
				sprintf_s(szBuf, "%d", pAniBlendList->vecTargetList.size());

				AniItem->SubItems->Add(gcnew String(szBuf));
			}

			Animation_listView->Items->Add(AniItem);
		}
	}
	Animation_listView->EndUpdate();
}

ANIMATION_BLEND_LIST * Character_NET::AnimationBlendTable::GetAniBlendList( string strAni )
{
	ANIMATION_BLEND_LIST* pAniBlendList = g_pMainApp->m_AnimationBledTable.GetAnimationBlendList(strAni);
	if(pAniBlendList)
	{
		return pAniBlendList;
	}

	return NULL;
}

int Character_NET::AnimationBlendTable::FindAnimationItemIndexAnimation(String^ strAnimation)
{
	ListViewItem^ itemFind =Animation_listView->FindItemWithText(strAnimation);
	if(itemFind == nullptr)
		return -1;

	return itemFind->Index;
}

void Character_NET::AnimationBlendTable::Load_AnimationList()
{
	Animation_listView->Items->Clear();

	if( g_pMainApp->GetActor() == NULL || g_pMainApp->GetActor()->m_pMesh == NULL)
		return;

	RMeshAnimationMgr* pAMgr = &g_pMainApp->GetActor()->m_pMesh->m_AniMgr;

	vector<string> strErrorMessage;

	Animation_listView->BeginUpdate();
	for(RAnimationHashList_Iter it = pAMgr->m_NodeHashList.begin(); it != pAMgr->m_NodeHashList.end();it++)
	{
		RAnimation* pAnimation = *it;
		if(pAnimation)
		{
			string strAniName = pAnimation->GetAliasName();

			//파일이 존재하는냐?
			RAnimation* pAnimation = g_pMainApp->m_TabAnimation.GetAnimation(strAniName.c_str());
			if( pAnimation == NULL
				|| pAnimation->GetID() < 0
				|| pAnimation->GetResource() == NULL
				|| !ExistFile(pAnimation->GetResource()->GetName().c_str()))
			{
				continue;
			}

			String^ Name = gcnew String(strAniName.c_str() );

			ListViewItem^ AniItem = gcnew ListViewItem(Name, 0);

			// 블렌딩 데이터가 몇개 존재 하는가?
			ANIMATION_BLEND_LIST * pAniBlendList = GetAniBlendList(strAniName);
			if(pAniBlendList &&
				pAniBlendList->vecTargetList.size() > 0)
			{
				char szBuf[4] = {0,};
				sprintf_s(szBuf, "%d", pAniBlendList->vecTargetList.size());

				AniItem->SubItems->Add(gcnew String(szBuf));
			}

			Animation_listView->Items->Add(AniItem);
		}
	}
	Animation_listView->EndUpdate();
}

void Character_NET::AnimationBlendTable::Load_AnimationBlendList()
{
	animationBlendData_Modify_button->Enabled = false;
	animationBlendData_Reverse_button->Enabled = false;
	BlendAnimation_listView->Enabled = false;
	BlendAnimation_listView->Items->Clear();
	if(m_strSelectAniName != nullptr)
		m_strSelectAniName->Remove(0);

	int nCount = Animation_listView->SelectedItems->Count;
	if(nCount)
		AddAnimationBlend_button->Enabled = true;
	else
		AddAnimationBlend_button->Enabled = false;

	if(nCount == 1)
	{
		BlendAnimation_listView->Enabled = true;

		ListView::SelectedListViewItemCollection^ selectOne = Animation_listView->SelectedItems;
		System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
		myEnum->MoveNext();
		ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);

		m_strSelectAniName = item->Text;
		ANIMATION_BLEND_LIST * pAniBlendList = GetAniBlendList(MStringToCharPointer(m_strSelectAniName));
		if(pAniBlendList)
		{
			for(vector<ANIMATION_BLEND_DATA>::iterator it = pAniBlendList->vecTargetList.begin(); it != pAniBlendList->vecTargetList.end(); ++it)
			{
				ListViewItem^ AniBlendItem = gcnew ListViewItem(gcnew String(it->strTarget.c_str()), 0);

				char szBuf[16] = {0,};
				sprintf_s(szBuf, "%.4f", it->fDurationTime);
				AniBlendItem->SubItems->Add(gcnew String(szBuf));
				BlendAnimation_listView->Items->Add(AniBlendItem);
			}
		}
	}
}

bool Character_NET::AnimationBlendTable::Show_AnimationBlendInputForm( vector<string>& vecSorceAni, vector<string>& vecOutBlendAni, float& fBlendTime, bool bDisableAniText )
{
	// 블렌딩 입력 창
	AnimationBlendInputForm blendForm;
	blendForm.SetAnimationBlendData(vecSorceAni, vecOutBlendAni, fBlendTime);
	blendForm.SetDiableForm(bDisableAniText);
	blendForm.ShowDialog();
	System::Windows::Forms::DialogResult res = blendForm.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{
		if(blendForm.BlendAnimation_textBox->Text == "")
		{
			// 에러
			System::Windows::Forms::MessageBox::Show(L"블랜딩 할 애니메이션을 입력하시오", "입력 실패!");
		}
		else if(blendForm.BlendValue_textBox->Text == "")
		{
			// 에러
			System::Windows::Forms::MessageBox::Show(L"블랜딩 시간을 입력하시오", "입력 실패!");
		}
		else
		{
			blendForm.GetTargetAnimation(vecOutBlendAni);
			fBlendTime = atof(MStringToCharPointer(blendForm.BlendValue_textBox->Text));

			return true;
		}
	}

	return false;
}

bool Character_NET::AnimationBlendTable::AddAnimationBlendInfo( string strAniName, string strBlendAni, float fBlendTime )
{
	// 중복 체크
	ANIMATION_BLEND_DATA * pAniBlendInfo = GetAniBlendInfo(strAniName, strBlendAni);
	if(pAniBlendInfo)
	{
		System::Windows::Forms::MessageBox::Show(String::Format("{0} 애니메이션에 이미 같은 블랜딩 애니메이션이 존재 합니다.!", gcnew String(strAniName.c_str())), "에러");
	}

	g_pMainApp->m_AnimationBledTable.InsertBlendAnimationData(strAniName, strBlendAni, fBlendTime);
	return true;
}

bool Character_NET::AnimationBlendTable::DelAnimationBlendInfo( string strAniName, string strBlendAni )
{
	// 삭제
	ANIMATION_BLEND_LIST* pList = GetAniBlendList(strAniName);
	if(pList == NULL)
	{
		System::Windows::Forms::MessageBox::Show(String::Format("{0} 애니메이션에 블랜딩 애니메이션 정보가 없습니다.!", gcnew String(strAniName.c_str())), "에러");
		return false;
	}

	g_pMainApp->m_AnimationBledTable.DeleteBlendAnimationData(strAniName, strBlendAni);

	return true;
}

ANIMATION_BLEND_DATA * Character_NET::AnimationBlendTable::GetAniBlendInfo( string strAni, string strBlendAni )
{
	ANIMATION_BLEND_LIST* pList = GetAniBlendList(strAni);
	if(pList)
	{
		for(vector<ANIMATION_BLEND_DATA>::iterator it = pList->vecTargetList.begin(); it != pList->vecTargetList.end(); ++it)
		{
			if(it->strTarget == strBlendAni)
			{
				return &(*it);
			}
		}
	}

	return NULL;
}

System::Void Character_NET::AnimationBlendTable::BlendAnimation_listView_KeyUp( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	if ( e->KeyCode == Keys::Delete )
	{
		ListView::SelectedListViewItemCollection^ selectOne = BlendAnimation_listView->SelectedItems;
		System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
		while(myEnum->MoveNext())
		{
			ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);
			if(DelAnimationBlendInfo(MStringToCharPointer(m_strSelectAniName), MStringToCharPointer(item->Text)))
				m_bSave = true;
		}

		// 갱신
		Load_AnimationBlendList();
		ReLoad_AnimationList();
	}
}

System::Void Character_NET::AnimationBlendTable::BlendAnimation_listView_DoubleClick( System::Object^ sender, System::EventArgs^ e )
{
	// 선택된 소스 가져오기 - 오직 하나뿐이다. 사용상 그렇게 된다.
	vector<string> vecSourceAni;
	vecSourceAni.push_back(MStringToCharPointer(m_strSelectAniName));

	// 선택된 아이템 가져오기
	ListView::SelectedListViewItemCollection^ selectOne = BlendAnimation_listView->SelectedItems;
	System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
	myEnum->MoveNext();
	ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);

	// 블렌딩 입력 창
	vector<string> vecOutBlendList;
	vecOutBlendList.push_back(MStringToCharPointer(item->Text));

	float fBlendTime = atof(MStringToCharPointer(item->SubItems[1]->Text));

	if(Show_AnimationBlendInputForm(vecSourceAni, vecOutBlendList, fBlendTime, false) == false)
	{
		return;
	}

	// 선택된 아이템 데이터 가져오기
	ANIMATION_BLEND_DATA * pBlendAniInfo = GetAniBlendInfo(MStringToCharPointer(m_strSelectAniName), MStringToCharPointer(item->Text));

	if(pBlendAniInfo)
	{
		for(vector<string>::iterator it = vecOutBlendList.begin(); it != vecOutBlendList.end(); ++it)
		{
			if(pBlendAniInfo->strTarget != *it ||
				pBlendAniInfo->fDurationTime != fBlendTime)
				m_bSave = true;

			g_pMainApp->m_AnimationBledTable.ModifyBlendAnimationData(MStringToCharPointer(m_strSelectAniName), MStringToCharPointer(item->Text), *it, fBlendTime);
		}

		// 갱신
		Load_AnimationBlendList();
		ReSetCurrentSelectListView();
	}
}

System::Void Character_NET::AnimationBlendTable::Animation_listView_DoubleClick( System::Object^ sender, System::EventArgs^ e )
{
	ListView::SelectedListViewItemCollection^ selectOne = Animation_listView->SelectedItems;
	System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
	myEnum->MoveNext();
	ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);

	m_strSelectAniName = item->Text;
	string strAniName = MStringToCharPointer(m_strSelectAniName);

	vector<string> vecSourceAni;
	vecSourceAni.push_back(strAniName);

	vector<string> vecAniBlendList;
	float fBlendTime = 0.0f;
	if(Show_AnimationBlendInputForm(vecSourceAni, vecAniBlendList, fBlendTime, false))
	{			
		bool bAddOk = false;
		for(vector<string>::iterator it = vecAniBlendList.begin(); it != vecAniBlendList.end(); ++it)
		{
			if(AddAnimationBlendInfo(strAniName, *it, fBlendTime))
			{
				bAddOk = true;
			}
		}

		if(bAddOk)
		{
			m_bSave = true;

			// 갱신
			Load_AnimationBlendList();
			ReLoad_AnimationList();
		}
	}
}

void Character_NET::AnimationBlendTable::ReLoad_AnimationList()
{
	FindAnimation(SearchAnimation_textBox->Text);
	ReSetCurrentSelectListView();
}

void Character_NET::AnimationBlendTable::InitSetAnimationInfo( String^ strAni )
{
	m_strSelectAniName = strAni;
}

void Character_NET::AnimationBlendTable::ReSetCurrentSelectListView()
{
	int nIndex = FindAnimationItemIndexAnimation(m_strSelectAniName);
	if(nIndex != -1)
	{
		ListView::SelectedIndexCollection^ indexList = Animation_listView->SelectedIndices;
		indexList->Clear();
		indexList->Add(nIndex);

		int nScrollIndex = nIndex + 12;
		if(Animation_listView->Items->Count <= nScrollIndex)
			nScrollIndex = Animation_listView->Items->Count - 1;

		Animation_listView->EnsureVisible(nScrollIndex);
	}
}

System::Void Character_NET::AnimationBlendTable::animationBlendData_Modify_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	vector<string> vecTargetAnimation;
	GetSelectBlendAniList(vecTargetAnimation);
	if(vecTargetAnimation.empty())
	{
		// 에러
		System::Windows::Forms::MessageBox::Show(L"적용 할 블렌딩 애니메이션을 입력하시오", "선택 실패!");
		return;
	}

	// 소스는 하나다
	vector<string> vecSourceAnimation;
	vecSourceAnimation.push_back(MStringToCharPointer(m_strSelectAniName));

	// 수정
	float fBlendTime = 0.0f;
	if(Show_AnimationBlendInputForm(vecSourceAnimation, vecTargetAnimation, fBlendTime, true) == false)
	{
		return;
	}

	bool bModify = false;
	for(vector<string>::iterator itTarget = vecTargetAnimation.begin(); itTarget != vecTargetAnimation.end(); ++itTarget)
	{
		// 선택된 아이템 데이터 가져오기
		ANIMATION_BLEND_DATA * pBlendAniInfo = GetAniBlendInfo(MStringToCharPointer(m_strSelectAniName), *itTarget);

		if(pBlendAniInfo)
		{
			if(pBlendAniInfo->fDurationTime != fBlendTime)
				bModify = true;

			g_pMainApp->m_AnimationBledTable.ModifyBlendAnimationData(MStringToCharPointer(m_strSelectAniName), *itTarget, *itTarget, fBlendTime);
		}
	}

	if(bModify)
	{
		// 갱신
		Load_AnimationBlendList();
		ReSetCurrentSelectListView();

		m_bSave = true;
	}
}

System::Void Character_NET::AnimationBlendTable::animationBlendData_Reverse_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	vector<string> vecTargetAnimation;
	GetSelectBlendAniList(vecTargetAnimation);
	if(vecTargetAnimation.empty())
	{
		// 에러
		System::Windows::Forms::MessageBox::Show(L"적용 할 블렌딩 애니메이션을 입력하시오", "선택 실패!");
		return;
	}

	// 소스는 하나다
	vector<string> vecSourceAnimation;
	vecSourceAnimation.push_back(MStringToCharPointer(m_strSelectAniName));

	// 역적용
	bool bModify = false;
	for(vector<string>::iterator itTarget = vecTargetAnimation.begin(); itTarget != vecTargetAnimation.end(); ++itTarget)
	{
		// 선택된 아이템 데이터 가져오기
		ANIMATION_BLEND_DATA * pBlendAniInfo = GetAniBlendInfo(MStringToCharPointer(m_strSelectAniName), *itTarget);

		if(pBlendAniInfo)
		{
			bModify = true;
			g_pMainApp->m_AnimationBledTable.ModifyBlendAnimationData(*itTarget, MStringToCharPointer(m_strSelectAniName), MStringToCharPointer(m_strSelectAniName), pBlendAniInfo->fDurationTime);
		}
	}

	if(bModify)
	{
		// 갱신
		Load_AnimationList();
		ReSetCurrentSelectListView();

		m_bSave = true;
	}
}

void Character_NET::AnimationBlendTable::GetSelectSourceAniList( vector<string>& vecSelectAni )
{
	ListView::SelectedListViewItemCollection^ selectOne = Animation_listView->SelectedItems;
	System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
	while(myEnum->MoveNext())
	{
		ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);

		vecSelectAni.push_back(MStringToCharPointer(item->Text));
	}
}

void Character_NET::AnimationBlendTable::GetSelectBlendAniList( vector<string>& vecBlendAni )
{
	ListView::SelectedListViewItemCollection^ selectOne = BlendAnimation_listView->SelectedItems;
	System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
	while(myEnum->MoveNext())
	{
		ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);

		vecBlendAni.push_back(MStringToCharPointer(item->Text));
	}
}

System::Void Character_NET::AnimationBlendTable::BlendAnimation_listView_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	int nCount = BlendAnimation_listView->SelectedItems->Count;
	if(nCount)
	{
		animationBlendData_Modify_button->Enabled = true;
		animationBlendData_Reverse_button->Enabled = true;
	}
}
