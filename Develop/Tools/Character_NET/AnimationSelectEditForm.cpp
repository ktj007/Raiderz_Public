#include "StdAfx.h"
#include "AnimationSelectEditForm.h"


void Character_NET::AnimationSelectEditForm::FindAnimation( System::String^ strAnimation )
{
	Animation_listBox->Items->Clear();

	if( g_pMainApp->GetActor() == NULL || g_pMainApp->GetActor()->m_pMesh == NULL)
		return;

	RMeshAnimationMgr* pAMgr = &g_pMainApp->GetActor()->m_pMesh->m_AniMgr;

	vector<string> strErrorMessage;

	Animation_listBox->BeginUpdate();
	for(RAnimationHashList_Iter it = pAMgr->m_NodeHashList.begin(); it != pAMgr->m_NodeHashList.end();it++)
	{
		RAnimation* pAnimation = *it;
		if(pAnimation)
		{
			string strAniName = pAnimation->GetAliasName();

			if(String::IsNullOrEmpty(strAnimation) == false &&
				IsEqualString(gcnew String(strAniName.c_str()), strAnimation) == false)
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

			Animation_listBox->Items->Add(Name);
		}
	}

	Animation_listBox->EndUpdate();
}

void Character_NET::AnimationSelectEditForm::SetEditAnimationList( vector<string>& vecEditAnimation )
{
	Edit_listBox->Items->Clear();

	for(vector<string>::iterator it = vecEditAnimation.begin(); it != vecEditAnimation.end(); ++it)
	{
		Edit_listBox->Items->Add(gcnew String(it->c_str()));
	}
}

System::Void Character_NET::AnimationSelectEditForm::Animation_listBox_MouseDoubleClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	String^ strSelectAnimation = Animation_listBox->SelectedItem->ToString();

	AddEditAnimation(strSelectAnimation);
}

bool Character_NET::AnimationSelectEditForm::FindEditAnimation( System::String^ strAnimation )
{
	System::Collections::IEnumerator^ ie;
	ie = Edit_listBox->Items->GetEnumerator();
	while(ie->MoveNext())
	{
		String^ strEditAnimation = ie->Current->ToString();
		if(strEditAnimation == strAnimation)
		{
			// 같다.
			return true;
		}
	}

	return false;
}

bool Character_NET::AnimationSelectEditForm::AddEditAnimation( System::String^ strAnimation )
{
	bool bFind = FindEditAnimation(strAnimation);
	if(bFind)
	{
		// 이미 등록되어 있다.
		System::Windows::Forms::MessageBox::Show(String::Format("{0} 이미 등록되어 있습니다!", strAnimation), "등록 실패!");		
	}
	else
	{
		Edit_listBox->Items->Add(strAnimation);
	}

	return !bFind;
}

void Character_NET::AnimationSelectEditForm::GetEditAnimationList( vector<string>& vecOutEditAnimation )
{
	vecOutEditAnimation.clear();

	System::Collections::IEnumerator^ ie;
	ie = Edit_listBox->Items->GetEnumerator();
	while(ie->MoveNext())
	{
		String^ strEditAnimation = ie->Current->ToString();
		vecOutEditAnimation.push_back(MStringToCharPointer(strEditAnimation));
	}
}

void Character_NET::AnimationSelectEditForm::DelEditAnimation()
{
	Edit_listBox->Items->Remove(Edit_listBox->SelectedItem);
}
