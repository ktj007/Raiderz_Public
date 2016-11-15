#include "StdAfx.h"
#include "AnimationHitSoundTabPage.h"


System::Void Character_NET::AnimationHitSoundTabPage::HitSoundAddbutton_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void Character_NET::AnimationHitSoundTabPage::AnimationHitSoundTabPage_Load( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void Character_NET::AnimationHitSoundTabPage::HitSound_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	g_pMainApp->m_TabAnimation.ModifyHitSoundInfo();
}

System::Void Character_NET::AnimationHitSoundTabPage::HitSound_comboBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{

}

void Character_NET::AnimationHitSoundTabPage::InitEffectList()
{
	if(m_bSoundLoad)
		return;

	m_bSoundLoad = true;
	vector<string> vecSoundList;

	g_pMainApp->m_pSoundMgr.GetSoundList(vecSoundList);
	g_pMainApp->m_TabAnimation.GetCustomEffectList(vecSoundList);

	for(vector<string>::iterator itSoundList = vecSoundList.begin(); itSoundList != vecSoundList.end(); itSoundList++)
	{
		HitSound_comboBox->Items->Add(gcnew String((*itSoundList).c_str()));
	}
}

System::Void Character_NET::AnimationHitSoundTabPage::OK_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	g_pMainApp->m_TabAnimation.ModifyHitSoundInfo();
}