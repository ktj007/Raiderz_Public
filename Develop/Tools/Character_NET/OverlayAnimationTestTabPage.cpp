#include "StdAfx.h"
#include "OverlayAnimationTestTabPage.h"


System::Void Character_NET::OverlayAnimationTestTabPage::UpperPartAnimationTest_textBox_DragDrop( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e )
{
	Object^ obj = e->Data->GetData(DataFormats::Text);
	UpperPartAnimation_textBox->Text = obj->ToString();
}

System::Void Character_NET::OverlayAnimationTestTabPage::UpperPartAnimationTest_textBox_DragEnter( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e )
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

System::Void Character_NET::OverlayAnimationTestTabPage::LowerPartAnimationTest_textBox_DragDrop( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e )
{
	Object^ obj = e->Data->GetData(DataFormats::Text);
	LowerPartAnimation_textBox->Text = obj->ToString();
}

System::Void Character_NET::OverlayAnimationTestTabPage::LowerPartAnimationTest_textBox_DragEnter( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e )
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

System::Void Character_NET::OverlayAnimationTestTabPage::SetLowerPartAnimation( string strAniName )
{
	UpperPartAnimation_textBox->Text = "";
	LowerPartAnimation_textBox->Text = gcnew String(strAniName.c_str());
}

System::Void Character_NET::OverlayAnimationTestTabPage::Play_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(g_pMainApp->GetActor() == NULL)
		return;

	g_pMainApp->OnStopAnimation();
	GlobalObjects::g_pMainForm->m_FrameSliderController->pressStopButton();
	g_pMainApp->m_NormalAnimation.AnimationLinkListTestStop();
	g_pMainApp->GetMotion()->CurMotionClear();
	GlobalObjects::g_pMainForm->m_AnimationTabPage->SetPauseButtonDown_AnimationLinkTest();


	GlobalObjects::g_pMainForm->m_FrameSliderController->pressPlayButton();
	g_pMainApp->m_NormalAnimation.UpperAnimationTestPlay(MStringToCharPointer(UpperPartAnimation_textBox->Text), MStringToCharPointer(LowerPartAnimation_textBox->Text), g_pMainApp->GetActor());
}

System::Void Character_NET::OverlayAnimationTestTabPage::Stop_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(g_pMainApp->GetActor() == NULL)
		return;

	g_pMainApp->m_NormalAnimation.UpperAnimationTestStop();
}