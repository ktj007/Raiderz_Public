#include "StdAfx.h"
#include "OverlayAnimationInputForm.h"

void Character_NET::OverlayAnimationInputForm::OverlayAnimationInputForm_DataLoad()
{
	if( g_pMainApp->GetActor() == NULL || g_pMainApp->GetActor()->m_pMesh == NULL)
		return;

	m_nFullCount = 0;
	m_nCurrCount = 0;

	RMeshAnimationMgr* pAMgr = &g_pMainApp->GetActor()->m_pMesh->m_AniMgr;
	m_nFullCount = pAMgr->m_NodeHashList.size() + g_pMainApp->GetActor()->GetActorNodeCount();

	m_loadProgressBar.Show();

	SetAnimation();
	SetBoneName();
}

void Character_NET::OverlayAnimationInputForm::SetAnimation()
{
	RMeshAnimationMgr* pAMgr = &g_pMainApp->GetActor()->m_pMesh->m_AniMgr;

	vector<string> strErrorMessage;

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

			OverlayAnimation_comboBox->Items->Add(gcnew String(strAniName.c_str()));
		}

		SetProgressBar(++m_nCurrCount);
	}
}

void Character_NET::OverlayAnimationInputForm::SetBoneName()
{
	for(int i = 0; i<g_pMainApp->GetActor()->GetActorNodeCount();i++)
	{
		RActorNode *pActorNode = g_pMainApp->GetActor()->GetActorNodes()[i];
		BoneSelect_comboBox->Items->Add(gcnew String(pActorNode->m_pMeshNode->m_Name.c_str()));

		SetProgressBar(++m_nCurrCount);
	}
}

void Character_NET::OverlayAnimationInputForm::SetOverlayAnimationData( String^ strOverlayAniName, String^ strBoneName )
{
	OverlayAnimation_comboBox->Text = strOverlayAniName;
	BoneSelect_comboBox->Text = strBoneName;
}

void Character_NET::OverlayAnimationInputForm::SetProgressBar( int nCount )
{
	float fValue = ((float)nCount / (float)m_nFullCount) * 100.0f;

	if(fValue > 100.0f)
		fValue = 100.0f;

	m_loadProgressBar.SetProGressBarValue((int)fValue);
}
