#include "StdAfx.h"
#include "AnimationEvent.h"
#include "SearchCustomEffectInfoName.h"

System::Void Character_NET::AnimationEvent::AnimationEvent_Load( System::Object^ sender, System::EventArgs^ e )
{
	vector<string> strNormalEventNameList;

	strNormalEventNameList.push_back("sh_effect");
	strNormalEventNameList.push_back("sh_walk");
	strNormalEventNameList.push_back("sh_sound");
	strNormalEventNameList.push_back("sh_camera");
	strNormalEventNameList.push_back("sh_spell_fire");
	strNormalEventNameList.push_back("sh_throw");
	strNormalEventNameList.push_back("sh_swallowed_disappear");
	//strNormalEventNameList.push_back("sh_sword_trail_start");
	//strNormalEventNameList.push_back("sh_sword_trail_end");
	strNormalEventNameList.push_back("sh_battle_start");
	strNormalEventNameList.push_back("sh_battle_end");
	strNormalEventNameList.push_back("sh_bparts");
	strNormalEventNameList.push_back("sh_face_ani_start");
	strNormalEventNameList.push_back("sh_face_ani_end");
	strNormalEventNameList.push_back("sh_defence_start_sound");

	for(vector<string>::iterator it = strNormalEventNameList.begin(); it != strNormalEventNameList.end(); it++)
	{
		EventNameListBox->Items->Add(String::Format("{0}", gcnew String((*it).c_str())));
	}

	Setting_AnimationEventParam();
}

System::Void Character_NET::AnimationEvent::AnimationEventTab_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void Character_NET::AnimationEvent::EventNameListBox_DoubleClick( System::Object^ sender, System::EventArgs^ e )
{
	EventNameTextBox->Text = ((System::Windows::Forms::ListBox^)sender)->Text;

	Setting_AnimationEventParam();
}

System::Void Character_NET::AnimationEvent::CustomEffectNameListBox_DoubleClick( System::Object^ sender, System::EventArgs^ e )
{
	EffectNameTextBox->Text = ((System::Windows::Forms::ListBox^)sender)->Text;
}

System::Void Character_NET::AnimationEvent::searchbutton_Click( System::Object^ sender, System::EventArgs^ e )
{
	OpenFileDialog^ openEffectNameFileDialog = gcnew OpenFileDialog;

	openEffectNameFileDialog->Filter				= "xml files (*.Effect.xml)|*.Effect.xml|All files (*.*)|*.*";
	openEffectNameFileDialog->RestoreDirectory		= true;
	openEffectNameFileDialog->Multiselect			= false;

	if(openEffectNameFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		String^ FileName			= openEffectNameFileDialog->FileName;
		int nstrIndex				= FileName->LastIndexOf("\\");
		FileName					= FileName->Substring(nstrIndex + 1);
		nstrIndex					= FileName->LastIndexOf(".Effect.xml");
		int nstrSize				= FileName->Length;
		nstrSize					-= nstrIndex;
		Param1textBox->Text		= FileName->Substring(0, nstrIndex);
	}

	delete openEffectNameFileDialog;
}

System::Void Character_NET::AnimationEvent::EventNameTextBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	Setting_AnimationEventParam();
}

System::Void Character_NET::AnimationEvent::Setting_AnimationEventParam()
{
	// 선택한 상황에 따라 유저가 보기 편하게 Param Label 수정
	Param1Show(false);
	Param2Show(false);
	Param3Show(false);
	Param1ComboBoxShow(false);
	Param2CheckBoxShow(false);

	unconditionalcheckBox->Checked = false;

	if(EventNameTextBox->Text == "sh_effect")
	{
		Param1Show(true);
		Param2Show(true);

		label3->Text = L"EffectFileName";
		label4->Text = L"Bone Name";
	}
	else if(EventNameTextBox->Text == "sh_walk")
	{
	}
	else if(EventNameTextBox->Text == "sh_sound")
	{
		Param1Show(true);
		Param2Show(true);
		Param3Show(true);

		label3->Text = L"SoundName";
		label4->Text = L"Bone Name";
		labelParameter3->Text = L"RandomCount(1~9)";
	}
	else if(EventNameTextBox->Text == "sh_camera")
	{
		Param1Show(true);
		Param2Show(true);

		label3->Text = L"CameraFileName";
		label4->Text = L"Max Distance";
	}
	else if(EventNameTextBox->Text == "sh_spell_fire")
	{

	}
	else if(EventNameTextBox->Text == "sh_throw")
	{

	}
	else if(EventNameTextBox->Text == "sh_swallowed_disappear")
	{

	}
	//else if(EventNameTextBox->Text == "sh_sword_trail_start")
	//{
	//	Param2Show(true);
	//	Param1NotSerchShow(true);

	//	label4->Text = L"SamplingUnit(1~9)";
	//	label3->Text = L"Parents Bone";
	//}
	//else if(EventNameTextBox->Text == "sh_sword_trail_end")
	//{
	//}
	else if(EventNameTextBox->Text == "sh_battle_start")
	{
		unconditionalcheckBox->Checked = true;
	}
	else if(EventNameTextBox->Text == "sh_battle_end")
	{
		unconditionalcheckBox->Checked = true;
	}
	else if(EventNameTextBox->Text == "sh_bparts")
	{
		Param1Show(true);
		Param2Show(true);
		Param3Show(true);

		label3->Text = L"PartsIndex";
		label4->Text = L"XY Force";
		labelParameter3->Text = L"Z Force";
	}
	else if(EventNameTextBox->Text == "sh_face_ani_start")
	{
		Param1ComboBoxShow(true);
		Param2CheckBoxShow(true);

		label3->Text = L"Ani Name";
		Param2checkBox->Text = L"Eye Animation";

		SetFacialAniamtionList();
	}
	else if(EventNameTextBox->Text == "sh_face_ani_end")
	{
		Param1ComboBoxShow(true);
		Param2Show(true);

		label3->Text = L"End FaceAnimation";
		label4->Text = L"End Blend Time";

		SetEventFacialAniamtionList();
	}
	else if(EventNameTextBox->Text == "sh_defence_start_sound")
	{
		Param1Show(true);

		label3->Text = L"sound name";
	}
	else
	{
		Param1Show(true);
		Param2Show(true);

		label3->Text = L"Parameter1";
		label4->Text = L"Parameter2";
	}
}

System::Void Character_NET::AnimationEvent::Param1Show( bool bShow )
{
	label3->Hide();
	Param1textBox->Hide();
	searchbutton->Hide();

	if(bShow)
	{
		label3->Show();
		Param1textBox->Show();
		searchbutton->Show();
	}
}

System::Void Character_NET::AnimationEvent::Param1NotSerchShow( bool bShow )
{
	label3->Hide();
	Param1textBox->Hide();
	searchbutton->Hide();

	if(bShow)
	{
		label3->Show();
		Param1textBox->Show();
	}
}

System::Void Character_NET::AnimationEvent::Param2Show( bool bShow )
{
	label4->Hide();
	Param2textBox->Hide();

	if(bShow)
	{
		label4->Show();
		Param2textBox->Show();
	}
}


System::Void Character_NET::AnimationEvent::Param3Show( bool bShow )
{
	labelParameter3->Hide();
	Param3textBox->Hide();

	if(bShow)
	{
		labelParameter3->Show();
		Param3textBox->Show();
	}
}

System::Void Character_NET::AnimationEvent::Param1ComboBoxShow( bool bShow )
{
	label3->Hide();
	Param1comboBox->Hide();
	Param1comboBox->Items->Clear();
	searchbutton->Hide();

	if(bShow)
	{
		label3->Show();
		Param1comboBox->Show();
	}
}

System::String^ Character_NET::AnimationEvent::GetParam1Text()
{
	if(EventNameTextBox->Text == "sh_face_ani_start" ||
		EventNameTextBox->Text == "sh_face_ani_end")
		return Param1comboBox->Text;

	return Param1textBox->Text;
}

System::Void Character_NET::AnimationEvent::SetParam1Text( System::String^ strText )
{
	if(EventNameTextBox->Text == "sh_face_ani_start" ||
		EventNameTextBox->Text == "sh_face_ani_end")
	{
		Param1comboBox->Text = strText;

	}
	else
	{
		Param1textBox->Text = strText;
	}
}

System::Void Character_NET::AnimationEvent::SetFacialAniamtionList()
{
	vector<string> vecList;
	g_pMainApp->m_TabAnimation.GetAnimationList(gcnew String("facial"), vecList);

	for(vector<string>::iterator it = vecList.begin(); it != vecList.end(); ++it)
	{
		Param1comboBox->Items->Add(gcnew String(it->c_str()));
	}
}

System::Void Character_NET::AnimationEvent::SetEventFacialAniamtionList()
{
	vector<string> vecList;
	g_pMainApp->m_TabAnimation.GetFacialAnimationEvent(vecList);

	for(vector<string>::iterator it = vecList.begin(); it != vecList.end(); ++it)
	{
		Param1comboBox->Items->Add(gcnew String(it->c_str()));
	}
}

System::Void Character_NET::AnimationEvent::Param2CheckBoxShow( bool bShow )
{
	Param2checkBox->Hide();

	if(bShow)
	{
		Param2checkBox->Show();
	}
}

System::String^ Character_NET::AnimationEvent::GetParam2Text()
{
	if(EventNameTextBox->Text == "sh_face_ani_start")
	{
		if(Param2checkBox->Checked == true)
			return gcnew String("true");
		else
			return gcnew String("false");
	}

	return Param2textBox->Text;
}

System::Void Character_NET::AnimationEvent::SetParam2Text( System::String^ strText )
{
	if(EventNameTextBox->Text == "sh_face_ani_start")
	{
		if(strText->StartsWith(gcnew String("true")))
			Param2checkBox->Checked = true;
		else
			Param2checkBox->Checked = false;
	}
	else
	{
		Param2textBox->Text = strText;
	}
}

void Character_NET::AnimationEvent::SearchEffectList()
{
	vector<string>vecEffectList;
	string strSource = MStringToCharPointer(search_textBox->Text);

	SearchCustomEffectInfoName	customEffectInfoName;
	customEffectInfoName.Init();
	customEffectInfoName.SearchEffectList(strSource, vecEffectList);
	
	CustomEffectNameListBox->Items->Clear();

	for(vector<string>::iterator it = vecEffectList.begin(); it != vecEffectList.end(); ++it)
	{
		CustomEffectNameListBox->Items->Add(gcnew String(it->c_str()));
	}
}