#include "StdAfx.h"
#include "AnimationTabPage.h"
#include "AnimationBlendTable.h"
#include "OverlayAnimationTable.h"
#include "ReActionAnimationTable.h"

void Character_NET::AnimationTabPage::Create_AnimationEventTabPage()
{
	this->m_AnimationEventTabPage = gcnew AnimationEventTabPage();
	this->m_AnimationEventTabPage->TopLevel = false;
	this->m_AnimationEventTabPage->Show();
	this->m_AnimationEventTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
	this->AnimationEvent_tabPage->Controls->Add(this->m_AnimationEventTabPage);
}

void Character_NET::AnimationTabPage::Create_AnimationLinkTestTabPage()
{
	this->m_AnimationLinkTestTabPage = gcnew AnimationLinkTestTabPage();
	this->m_AnimationLinkTestTabPage->TopLevel = false;
	this->m_AnimationLinkTestTabPage->Show();
	this->m_AnimationLinkTestTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
	this->AnimationLinkTest_tabPage->Controls->Add(this->m_AnimationLinkTestTabPage);
}

void Character_NET::AnimationTabPage::Create_OverlayAnimationTestTabPage()
{
	this->m_OverlayAnimationTestTabPage = gcnew OverlayAnimationTestTabPage();
	this->m_OverlayAnimationTestTabPage->TopLevel = false;
	this->m_OverlayAnimationTestTabPage->Show();
	this->m_OverlayAnimationTestTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
	this->OverlayAnimation_tabPage->Controls->Add(this->m_OverlayAnimationTestTabPage);
}

void Character_NET::AnimationTabPage::Create_AnimationHitSoundTabPage()
{
	this->m_AnimationHitSoundTabPage = gcnew AnimationHitSoundTabPage();
	this->m_AnimationHitSoundTabPage->TopLevel = false;
	this->m_AnimationHitSoundTabPage->Show();
	this->m_AnimationHitSoundTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
	this->HitSound_tabPage->Controls->Add(this->m_AnimationHitSoundTabPage);
}

System::Void Character_NET::AnimationTabPage::AnitreeView_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	m_selectAnimationNode = nullptr;

	TreeNode^ selectedNode = this->AnitreeView->GetNodeAt(e->X, e->Y);
	if(selectedNode != nullptr)
	{
		if(selectedNode->Parent != nullptr)
		{
			AnitreeView->SelectedNode = m_selectAnimationNode = selectedNode;
		}
	}
}

System::Void Character_NET::AnimationTabPage::AnitreeView_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	m_selectAnimationNode = nullptr;
}

System::Void Character_NET::AnimationTabPage::AnitreeView_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	if(m_selectAnimationNode != nullptr)
	{
		AnitreeView->DoDragDrop(m_selectAnimationNode->Text, DragDropEffects::Copy);
		AnitreeView->SelectedNode = m_selectAnimationNode;
		m_selectAnimationNode = nullptr;
	}
}

void Character_NET::AnimationTabPage::SetCurrentAnimationLinkListIndex( int nIndex )
{
	if(this->m_AnimationLinkTestTabPage->AnimationLinkTest_listBox->Items->Count > 0 &&
		this->m_AnimationLinkTestTabPage->AnimationLinkTest_listBox->Items->Count > nIndex)
		this->m_AnimationLinkTestTabPage->AnimationLinkTest_listBox->SelectedIndex = nIndex;
}

void Character_NET::AnimationTabPage::SetPauseButtonDown_AnimationLinkTest()
{
	this->m_AnimationLinkTestTabPage->PauseButtonDown();
}

void Character_NET::AnimationTabPage::SetPlayButtonDown_AnimationLinkTest()
{
	this->m_AnimationLinkTestTabPage->PlayButtonDown();
}

System::Void Character_NET::AnimationTabPage::AnimationBlendTableToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	AnimationBlendTable animationBlend_dialog;
	animationBlend_dialog.InitSetAnimationInfo(AnitreeView->SelectedNode->Text);
	animationBlend_dialog.ShowDialog();
	System::Windows::Forms::DialogResult res = animationBlend_dialog.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{

	}
}

System::Void Character_NET::AnimationTabPage::OverlayAnimation_ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	OverlayAnimationTable overlayAnimation_dialog;
	overlayAnimation_dialog.InitSetAnimationInfo(AnitreeView->SelectedNode->Text);
	overlayAnimation_dialog.ShowDialog();
	System::Windows::Forms::DialogResult res = overlayAnimation_dialog.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{

	}
}

System::Void Character_NET::AnimationTabPage::Animation_tabControl_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(Animation_tabControl->SelectedIndex == 3)
	{
		m_AnimationHitSoundTabPage->InitEffectList();
	}
}

System::Void Character_NET::AnimationTabPage::LoopcheckBox_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	RAnimation* pAnimation = g_pMainApp->GetActor()->GetCurAnimation();
	if( pAnimation )
	{
		AnimationLoopType AniLoopType = (LoopcheckBox->Checked==true)?RAniLoopType_Loop:RAniLoopType_HoldLastFrame;
		pAnimation->SetAnimationLoopType(AniLoopType);

		GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);

		g_pMainApp->EventMessage(EVENT_MSG_PLAY_ANIMATION);
	}
}

System::Void Character_NET::AnimationTabPage::ReActionAnimation_ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	GlobalObjects::g_pMainForm->ShowReactionAnimationTable();
}

void Character_NET::AnimationTabPage::AnimationInfoBtnEnable(bool bEnable)
{
	button3->Enabled = bEnable;
	button2->Enabled = bEnable;
	button1->Enabled = bEnable;
}
