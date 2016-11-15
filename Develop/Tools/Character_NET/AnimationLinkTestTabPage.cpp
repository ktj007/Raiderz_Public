#include "StdAfx.h"
#include "AnimationLinkTestTabPage.h"
#include "EditText_One.h"

System::Void Character_NET::AnimationLinkTestTabPage::AnimationLinkTest_listBox_DragDrop( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e )
{
	Object^ obj = e->Data->GetData(DataFormats::Text);

	String^ animationString = obj->ToString();
	AnimationLinkTest_listBox->Items->Add(obj->ToString());

	AnimationLinkTest_Stop();
	CheckAnimationBlend();
}

System::Void Character_NET::AnimationLinkTestTabPage::AnimationLinkTest_listBox_DragEnter( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e )
{
	if(e->Data->GetDataPresent(DataFormats::Text))
	{
		e->Effect = DragDropEffects::Copy;
	}
	else
	{
		e->Effect = DragDropEffects::None;
	}
}

System::Void Character_NET::AnimationLinkTestTabPage::AnimationLinkTest_listBox_KeyUp( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	// delete키로 삭제하기
	if ( e->KeyCode == Keys::Delete )
	{
		AnimationLinkTest_Stop();
		AnimationLinkTest_listBox->Items->Remove(AnimationLinkTest_listBox->SelectedItem);
		CheckAnimationBlend();
	}
}

System::Void Character_NET::AnimationLinkTestTabPage::Clear_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	AnimationLinkTest_Stop();

	AnimationLinkTest_listBox->Items->Clear();
	AnimationLinkTestSpeed_UpDown->Value = Convert::ToDecimal(1.0f);
}

System::Void Character_NET::AnimationLinkTestTabPage::Play_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(AnimationLinkTest_listBox->Items->Count <= 0)
		return;

	g_pMainApp->OnStopAnimation();
	g_pMainApp->m_NormalAnimation.AnimationLinkListTestPlay(AnimationLinkTest_listBox);
	g_pMainApp->m_bPlayPauseButtonStateByShourtCut = true;

	PlayButtonDown();
}

System::Void Character_NET::AnimationLinkTestTabPage::Down_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(AnimationLinkTest_listBox->SelectedItem == nullptr)
		return;

	int nSelectedIndex = AnimationLinkTest_listBox->SelectedIndex;
	int nUpItemIndex = nSelectedIndex + 1;
	
	if(nUpItemIndex >= AnimationLinkTest_listBox->Items->Count)
	{
		return;
	}
	
	System::String^ itemString = AnimationLinkTest_listBox->SelectedItem->ToString();
	AnimationLinkTest_listBox->Items->RemoveAt(nSelectedIndex);
	AnimationLinkTest_listBox->Items->Insert(nUpItemIndex, itemString);

	CheckAnimationBlend();

	AnimationLinkTest_listBox->SelectedIndex = nUpItemIndex;
}

System::Void Character_NET::AnimationLinkTestTabPage::Up_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(AnimationLinkTest_listBox->SelectedItem == nullptr)
		return;

	int nSelectedIndex = AnimationLinkTest_listBox->SelectedIndex;
	int nDownItemIndex = nSelectedIndex - 1;

	if(nDownItemIndex < 0)
	{
		return;
	}

	System::String^ itemString = AnimationLinkTest_listBox->SelectedItem->ToString();
	AnimationLinkTest_listBox->Items->RemoveAt(nSelectedIndex);
	AnimationLinkTest_listBox->Items->Insert(nDownItemIndex, itemString);

	CheckAnimationBlend();

	AnimationLinkTest_listBox->SelectedIndex = nDownItemIndex;
}

System::Void Character_NET::AnimationLinkTestTabPage::Stop_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	AnimationLinkTest_Stop();
}

System::Void Character_NET::AnimationLinkTestTabPage::AnimationLinkTest_Stop()
{
	g_pMainApp->m_NormalAnimation.AnimationLinkListTestStop();

	PauseButtonDown();

	m_eAnimationLinkTestState = LTS_STOP;
}

System::Void Character_NET::AnimationLinkTestTabPage::Pause_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	g_pMainApp->m_NormalAnimation.AnimationPause();
	PauseButtonDown();
}

System::Void Character_NET::AnimationLinkTestTabPage::AnimationLinkTestSpeed_UpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
{
	float v = (float)Convert::ToDouble(AnimationLinkTestSpeed_UpDown->Value);
	g_pMainApp->m_NormalAnimation.AnimationLinkListTestSpeed(v);
}

void Character_NET::AnimationLinkTestTabPage::PauseButtonDown()
{
	Play_button->Show();
	Pause_button->Hide();

	m_eAnimationLinkTestState = LTS_PAUSE;
}

void Character_NET::AnimationLinkTestTabPage::PlayButtonDown()
{
	Play_button->Hide();
	Pause_button->Show();

	m_eAnimationLinkTestState = LTS_PLAY;
}

void Character_NET::AnimationLinkTestTabPage::CheckAnimationBlend()
{
	int nCount = AnimationLinkTest_listBox->Items->Count;

	for(int n = 0; n < nCount; ++n)
	{
		int nNextIndex = n + 1;
		if(nNextIndex >= nCount)
			continue;

		String^ itemSource = AnimationLinkTest_listBox->Items[n]->ToString();
		String^ itemTarget = AnimationLinkTest_listBox->Items[nNextIndex]->ToString();

		int fndpos = itemSource->IndexOf("(");
		if (fndpos != -1)
			itemSource = itemSource->Substring(0, fndpos)->Trim();
		
		fndpos = itemTarget->IndexOf("(");
		if (fndpos != -1)
			itemTarget = itemTarget->Substring(0, fndpos)->Trim();


		string strSource = MStringToCharPointer(itemSource);
		string strTarget = MStringToCharPointer(itemTarget);

		ANIMATION_BLEND_DATA * pBlendData = g_pMainApp->m_AnimationBledTable.GetAnimationBlendData(strSource, strTarget);
		if(pBlendData == NULL)
		{
			AnimationLinkTest_listBox->Items->RemoveAt(n);
			AnimationLinkTest_listBox->Items->Insert(n, gcnew String(strSource.c_str()));
			continue;
		}

		char szBuf[32] = {0,};
		sprintf_s(szBuf, "%s (%f)", strSource.c_str(), pBlendData->fDurationTime);
		
		AnimationLinkTest_listBox->Items->RemoveAt(n);
		AnimationLinkTest_listBox->Items->Insert(n, gcnew String(szBuf));
	}
}

System::Void Character_NET::AnimationLinkTestTabPage::AnimationCutTime_ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(AnimationLinkTest_listBox->SelectedItem == nullptr)
		return;

	EditText_One dialogAnimationCutTime;
	//dialogAnimationCutTime.Edit_textBox->Text = 
	dialogAnimationCutTime.ShowDialog();
	System::Windows::Forms::DialogResult res = dialogAnimationCutTime.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{
		int nSelectedIndex = AnimationLinkTest_listBox->SelectedIndex;
		float fEndTime = atof(MStringToCharPointer(dialogAnimationCutTime.Edit_textBox->Text));

		g_pMainApp->m_NormalAnimation.AnimationLinkListTestEndTime(nSelectedIndex, fEndTime);
	}
}