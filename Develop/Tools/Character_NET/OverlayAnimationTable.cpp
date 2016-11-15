#include "StdAfx.h"
#include "OverlayAnimationTable.h"
#include "OverlayAnimationInputForm.h"

System::Void Character_NET::OverlayAnimationTable::OverlayAnimationTable_Load( System::Object^ sender, System::EventArgs^ e )
{
	Load_AnimationList();

	AddOverlay_button->Enabled					= false;
	OverlayAnimation_listView->Enabled			= false;

	// 이미 선택된 애니메이션이 있다면...
	if(m_strSelectAniName != nullptr)
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
}

void Character_NET::OverlayAnimationTable::Load_AnimationList()
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

			// 오버레이 데이터가 몇개 존재 하는가?
			OVERLAY_ANIMATION_LIST * pAniList = GetOverlayAniList(strAniName);
			if(pAniList)
			{
				char szBuf[4] = {0,};
				sprintf_s(szBuf, "%d", pAniList->vecOverlayAnimationList.size());

				AniItem->SubItems->Add(gcnew String(szBuf));
			}

			Animation_listView->Items->Add(AniItem);
		}
	}
	Animation_listView->EndUpdate();
}

void Character_NET::OverlayAnimationTable::FindAnimation( System::String^ strAnimation )
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
			OVERLAY_ANIMATION_LIST * pAniList = GetOverlayAniList(strAniName);
			if(pAniList)
			{
				char szBuf[4] = {0,};
				sprintf_s(szBuf, "%d", pAniList->vecOverlayAnimationList.size());

				AniItem->SubItems->Add(gcnew String(szBuf));
			}

			Animation_listView->Items->Add(AniItem);
		}
	}
	Animation_listView->EndUpdate();
}

System::Void Character_NET::OverlayAnimationTable::Load_OverlayAniList()
{
	OverlayAnimation_listView->Enabled = false;
	OverlayAnimation_listView->Items->Clear();
	if(m_strSelectAniName != nullptr)
		m_strSelectAniName->Remove(0);

	int nCount = Animation_listView->SelectedItems->Count;
	if(nCount)
		AddOverlay_button->Enabled = true;
	else
		AddOverlay_button->Enabled = false;

	if(nCount == 1)
	{
		OverlayAnimation_listView->Enabled = true;

		ListView::SelectedListViewItemCollection^ selectOne = Animation_listView->SelectedItems;
		System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
		myEnum->MoveNext();
		ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);

		m_strSelectAniName = item->Text;
		OVERLAY_ANIMATION_LIST * pAniList = GetOverlayAniList(MStringToCharPointer(m_strSelectAniName));
		if(pAniList)
		{
			for(vector<OVERLAY_ANIMATION_INFO>::iterator it = pAniList->vecOverlayAnimationList.begin(); it != pAniList->vecOverlayAnimationList.end(); ++it)
			{
				ListViewItem^ OverlayAniItem = gcnew ListViewItem(gcnew String(it->strOverlayAnimation.c_str()), 0);
				OverlayAniItem->SubItems->Add(gcnew String(it->strBoneName.c_str()));
				OverlayAnimation_listView->Items->Add(OverlayAniItem);
			}
		}
	}
}

OVERLAY_ANIMATION_LIST * Character_NET::OverlayAnimationTable::GetOverlayAniList( string strAni )
{
	XOverlayAnimationInfo* pOverlayAniInfo = g_pMainApp->GetActor()->GetOverlayAnimaionInfo();
	OVERLAY_ANIMATION_LIST * pAniList = pOverlayAniInfo->Get(strAni);
	if(pAniList)
	{
		return pAniList;
	}

	return NULL;
}

System::Void Character_NET::OverlayAnimationTable::OverlayAnimation_listView_DoubleClick( System::Object^ sender, System::EventArgs^ e )
{
	// 선택된 아이템 가져오기
	ListView::SelectedListViewItemCollection^ selectOne = OverlayAnimation_listView->SelectedItems;
	System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
	myEnum->MoveNext();
	ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);

	// 오버레이 입력 창
	string strOverlayAnimation = MStringToCharPointer(item->Text);
	string strBoneName = MStringToCharPointer(item->SubItems[1]->Text);

	if(Show_OverlayAnimationInputForm(strOverlayAnimation, strBoneName))
	{
		// 선택된 아이템 데이터 가져오기
		OVERLAY_ANIMATION_INFO * pOverlayAniInfo = GetOverlayAniInfo(MStringToCharPointer(m_strSelectAniName), MStringToCharPointer(item->Text), MStringToCharPointer(item->SubItems[1]->Text));

		if(pOverlayAniInfo)
		{
			// 중복 체크
			OVERLAY_ANIMATION_INFO * pDuplicationOverlayAniInfo = GetOverlayAniInfo(MStringToCharPointer(m_strSelectAniName), strOverlayAnimation, strBoneName);
			if(pDuplicationOverlayAniInfo != NULL &&
				pDuplicationOverlayAniInfo != pOverlayAniInfo)
			{
				// 중복이다.
				System::Windows::Forms::MessageBox::Show(L"이미 같은 오버레이 애니메이션이 존재 합니다.!", "에러");
			}
			else
			{
				if(pOverlayAniInfo->strOverlayAnimation != strOverlayAnimation ||
					pOverlayAniInfo->strBoneName != strBoneName)
					m_bSave = true;

				pOverlayAniInfo->strOverlayAnimation = strOverlayAnimation;
				pOverlayAniInfo->strBoneName = strBoneName;

				// 갱신
				Load_OverlayAniList();
			}
		}
	}
}

OVERLAY_ANIMATION_INFO * Character_NET::OverlayAnimationTable::GetOverlayAniInfo( string strAni, string strOverlayAni, string strOverlayAniBone )
{
	OVERLAY_ANIMATION_LIST* pList = GetOverlayAniList(strAni);
	if(pList)
	{
		for(vector<OVERLAY_ANIMATION_INFO>::iterator it = pList->vecOverlayAnimationList.begin(); it != pList->vecOverlayAnimationList.end(); ++it)
		{
			if(it->strOverlayAnimation == strOverlayAni &&
				it->strBoneName == strOverlayAniBone)
			{
				return &(*it);
			}
		}
	}

	return NULL;
}

bool Character_NET::OverlayAnimationTable::Show_OverlayAnimationInputForm(string& strOutOverlayAni, string& strOutOverlayAniBone)
{
	// 오버레이 입력 창
	OverlayAnimationInputForm overlayForm;
	overlayForm.OverlayAnimationInputForm_DataLoad();
	overlayForm.SetOverlayAnimationData(gcnew String(strOutOverlayAni.c_str()), gcnew String(strOutOverlayAniBone.c_str()));
	overlayForm.ShowDialog();
	System::Windows::Forms::DialogResult res = overlayForm.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{
		if(overlayForm.OverlayAnimation_comboBox->Text == "")
		{
			// 에러
			System::Windows::Forms::MessageBox::Show(L"오버레이 애니메이션을 입력하시오", "입력 실패!");
		}
		else if(overlayForm.BoneSelect_comboBox->Text == "")
		{
			// 에러
			System::Windows::Forms::MessageBox::Show(L"오버레이 애니메이션 본을 입력하시오", "입력 실패!");
		}
		else
		{
			strOutOverlayAni = MStringToCharPointer(overlayForm.OverlayAnimation_comboBox->Text);
			strOutOverlayAniBone = MStringToCharPointer(overlayForm.BoneSelect_comboBox->Text);

			return true;
		}
	}

	return false;
}

System::Void Character_NET::OverlayAnimationTable::AddOverlay_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	string strOverlayAniName;
	string strOverlayAniBone;
	if(Show_OverlayAnimationInputForm(strOverlayAniName, strOverlayAniBone) == false)
		return ;

	ListView::SelectedListViewItemCollection^ selectOne = Animation_listView->SelectedItems;
	System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
	while(myEnum->MoveNext())
	{
		ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);
		if(AddOverlayAnimationInfo(MStringToCharPointer(item->Text), strOverlayAniName, strOverlayAniBone))
			m_bSave = true;
	}

	// 갱신
	Load_OverlayAniList();
	Load_AnimationList();
}

bool Character_NET::OverlayAnimationTable::AddOverlayAnimationInfo( string strAniName, string strOverlayAni, string strOverlayAniBone )
{
	// 중복 체크
	OVERLAY_ANIMATION_INFO * pOverlayAniInfo = GetOverlayAniInfo(strAniName, strOverlayAni, strOverlayAniBone);
	if(pOverlayAniInfo)
	{
		System::Windows::Forms::MessageBox::Show(String::Format("{0} 애니메이션에 이미 같은 오버레이 애니메이션이 존재 합니다.!", gcnew String(strAniName.c_str())), "에러");
	}
	else
	{
		OVERLAY_ANIMATION_LIST* pList = GetOverlayAniList(strAniName);
		if(pList == NULL)
		{
			TOverlayAnimationInfo* pOverlayAniInfo = static_cast<TOverlayAnimationInfo*>(g_pMainApp->GetActor()->GetOverlayAnimaionInfo());
			pOverlayAniInfo->InsertData(strAniName);

			pList = GetOverlayAniList(strAniName);
		}

		OVERLAY_ANIMATION_INFO aniInfo;
		aniInfo.strOverlayAnimation = strOverlayAni;
		aniInfo.strBoneName = strOverlayAniBone;
		pList->vecOverlayAnimationList.push_back(aniInfo);

		return true;
	}

	return false;
}

System::Void Character_NET::OverlayAnimationTable::OverlayAnimation_listView_KeyUp( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	if ( e->KeyCode == Keys::Delete )
	{
		ListView::SelectedListViewItemCollection^ selectOne = OverlayAnimation_listView->SelectedItems;
		System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
		while(myEnum->MoveNext())
		{
			ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);
			if(DelOverlayAnimationInfo(MStringToCharPointer(m_strSelectAniName), MStringToCharPointer(item->Text), MStringToCharPointer(item->SubItems[1]->Text)))
				m_bSave = true;
		}

		// 갱신
		Load_OverlayAniList();
		Load_AnimationList();
	}
}

bool Character_NET::OverlayAnimationTable::DelOverlayAnimationInfo( string strAniName, string strOverlayAni, string strOverlayAniBone )
{
	// 삭제
	OVERLAY_ANIMATION_LIST* pList = GetOverlayAniList(strAniName);
	if(pList == NULL)
	{
		System::Windows::Forms::MessageBox::Show(String::Format("{0} 애니메이션에 오버레이 애니메이션 정보가 없습니다.!", gcnew String(strAniName.c_str())), "에러");
		return false;
	}

	for(vector<OVERLAY_ANIMATION_INFO>::iterator it = pList->vecOverlayAnimationList.begin(); it != pList->vecOverlayAnimationList.end(); ++it)
	{
		if(it->strOverlayAnimation == strOverlayAni &&
			it->strBoneName == strOverlayAniBone)
		{
			pList->vecOverlayAnimationList.erase(it);
			return true;
		}
	}

	return false;

}

System::Void Character_NET::OverlayAnimationTable::Finish_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(m_bSave)
	{
		//TOverlayAnimationInfo* pOverlayAniInfo = static_cast<TOverlayAnimationInfo*>(g_pMainApp->GetActor()->GetOverlayAnimaionInfo());
		//pOverlayAniInfo->SaveInfo();
		g_pMainApp->GetActor()->GetAnimationInfoMgr()->SaveXml();
	}

	this->DialogResult = System::Windows::Forms::DialogResult::OK;
	this->Close();
}

void Character_NET::OverlayAnimationTable::InitSetAnimationInfo( String^ strAni )
{
	m_strSelectAniName = strAni;
}

int Character_NET::OverlayAnimationTable::FindAnimationItemIndexAnimation( String^ strAnimation )
{
	ListViewItem^ itemFind =Animation_listView->FindItemWithText(strAnimation);
	if(itemFind == nullptr)
		return -1;

	return itemFind->Index;
}