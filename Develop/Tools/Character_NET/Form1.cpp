#include "StdAfx.h"
#include "Form1.h"

#include "HelpForm.h"
#include "RConfiguration.h"
#include "RUtil.h"
//#include "TSkillHitArea.h"
#include "ToolConfig.h"
//#include "TextOneBox.h"
#include "TalentEffectBase.h"
#include "TTalentEffectMgr.h"
#include "RAnimationResource.h"

namespace Character_NET 
{
	System::Void Form1::MainForm_Load(System::Object^  sender, System::EventArgs^  e)
	{
		assert(g_pMainApp==NULL);

		HWND hWnd;

		try
		{
			hWnd = (HWND)(MainView->Handle.ToPointer());
			assert(hWnd!=NULL);
		}
		catch (System::NullReferenceException^ e)
		{
			System::Windows::Forms::MessageBox::Show(e->ToString());
			Close();
		}

		m_InitLoadingWindow.Show();

		g_pMainApp = new CMainApplication();
		if ( !g_pMainApp->Init(hWnd))
		{
			Close();
			return;
		}

		g_pMainApp->InitState(MainView->Width,MainView->Height);
		GlobalObjects::g_pMainForm->InitLoadingInfoText("완료");

		// [4/9/2007 madduck] - g_pMainapp->Init() 로 이동.
		//XSkillInfoMgr::GetInstance().Load("../../Data/System/skill.xml", "../../Data/System/skill_hit_info.xml", NULL);
		//g_pMainApp->UpdateSkillComboBox();
		//TSkillHitInfoMgr::GetInstance().Load("../../Data/System/skill_hit_info.xml");

		this->Bounds = System::Drawing::Rectangle(System::Drawing::Point(CToolConfig::GetInstance().start_X, CToolConfig::GetInstance().start_Y),
			System::Drawing::Size(CToolConfig::GetInstance().WinForm_width, CToolConfig::GetInstance().WinForm_height));
		//this->ClientSize = System::Drawing::Size(CToolConfig::GetInstance().WinForm_width, CToolConfig::GetInstance().WinForm_height);
		//this->ClientSize.Width = CToolConfig::GetInstance().WinForm_width;
		//this->ClientSize.Height = CToolConfig::GetInstance().WinForm_height;
		this->splitContainer1->SplitterDistance = CToolConfig::GetInstance().split1;
		this->splitContainer2->SplitterDistance = CToolConfig::GetInstance().split2;
		if( CToolConfig::GetInstance().maximize)
		{
			//최대화.
			this->WindowState = FormWindowState::Maximized;
		}

		m_InitLoadingWindow.Close();
	}
	System::Void Form1::MainLoop()
	{
		if( g_pMainApp == NULL )
			return;

		g_pMainApp->OnUpdate();
		if ( !g_pMainApp->OnDraw())
		{
			g_pMainApp->OnLostDevice();
			g_pMainApp->OnResetDevice();
		}

		if(g_pMainApp->GetActor() == NULL) return;
		if(g_pMainApp->m_bTalentAnimation)
			m_FrameSliderController->Update_TalentFrame(g_pMainApp->GetActor(), &g_pMainApp->m_TalentAnimation);
		else
			m_FrameSliderController->Update_AniFrame(g_pMainApp->GetActor());
	}

	System::Void Form1::MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) 
	{
		CToolConfig::GetInstance().start_X = this->Bounds.X;
		CToolConfig::GetInstance().start_Y = this->Bounds.Y;
		if( this->WindowState == FormWindowState::Maximized )
		{
			CToolConfig::GetInstance().maximize = true;
		}
		else
			CToolConfig::GetInstance().maximize = false;

		CToolConfig::GetInstance().WinForm_width = this->Bounds.Width;//this->ClientSize.Width;
		CToolConfig::GetInstance().WinForm_height= this->Bounds.Height; //this->ClientSize.Height;
		CToolConfig::GetInstance().split1 = this->splitContainer1->SplitterDistance;
		CToolConfig::GetInstance().split2 = this->splitContainer2->SplitterDistance;
		CToolConfig::GetInstance().SaveConfig();

		g_pMainApp->Destroy();
		SAFE_DELETE(g_pMainApp);
	}
	System::Void Form1::OpenMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

		//openFileDialog1->InitialDirectory = "c:\\";
		openFileDialog1->Filter = "xml files (*.xml)|*.xml|elu files (*.elu)|*.elu|ani files (*.ani)|*.ani|All files (*.*)|*.*";
		openFileDialog1->FilterIndex = 2;
		openFileDialog1->RestoreDirectory = true;
		openFileDialog1->Multiselect		= true;

		if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			//String^ FileName = openFileDialog1->FileName;	//path까지 다 들어온다.
			//g_pMainApp->MeshFileOpen(FileName);

			// TODO: 폴더이름으로 (플레이어 NPC 몬스터) parent이름을 설정하면 좋은데...

			array<String^>^ FileNames = openFileDialog1->FileNames;
			int size = FileNames->Length;

			for each ( String^ FileName in FileNames )
			{
				g_pMainApp->MeshFileOpen(FileName);
			}
		}

		delete openFileDialog1;
	}
	System::Void Form1::SaveMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->SaveModelAnimation();
	}
	System::Void Form1::showBoneToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->m_TMenu.SetFillMode(RFILL_SOLID);
		g_pMainApp->m_TMenu.OnShowBones();

		int nState = g_pMainApp->m_TMenu.GetShowBoneType();
		if(nState > 0)
			g_pMainApp->m_TMenu.SetFillMode(RFILL_WIRE);
	}
	System::Void Form1::ShowNormalToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->m_TMenu.OnShowNormal();
	}
	System::Void Form1::showWireToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->m_TMenu.SetFillMode();
	}
	System::Void Form1::cameraResetSpaceBarToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->CameraReset();
	}
	System::Void Form1::lightToCameraposToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->EventMessage(EVENT_MSG_LIGHT_TO_CAMERA);
	}
	System::Void Form1::MainView_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
	{
		Point mouseDownLocation = Point(e->X,e->Y);
		int istat = GetMouseButtonState(e->Button);

		g_pMainApp->OnMouseMove(istat, RVector2((float)e->X, (float)e->Y), e->Delta);
	}
	System::Void Form1::MainForm_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) 
	{
		bool dragObjectIsMyClass = (e->Data->GetDataPresent(DataFormats::FileDrop) == true);
		if (dragObjectIsMyClass)
		{
			e->Effect = DragDropEffects::Copy;
		}
	}
	System::Void Form1::MainForm_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) 
	{
		System::Array^ a = (System::Array^)e->Data->GetData(DataFormats::FileDrop);

		int count = 0;
		while(a->GetValue(count))
		{
			String^ strFileName = a->GetValue(count)->ToString();
			g_pMainApp->SetModelFullPathName(strFileName);
			g_pMainApp->MeshFileOpen(strFileName);
			count++;
		}
	}
	System::Void Form1::AnitrackBar_Scroll(System::Object^  sender, System::EventArgs^  e) 
	{
		m_FrameSliderController->FrameSliderScroll(g_pMainApp->GetActor(), &g_pMainApp->m_TalentAnimation, AnitrackBar->Maximum, AnitrackBar->Value);
		m_FrameSliderController->pressPauseButton();
		GlobalObjects::g_pMainForm->m_AnimationTabPage->SetPauseButtonDown_AnimationLinkTest();
	}
	System::Void Form1::Control1_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->m_SelectedTab2 = (TAB_TABSTATE)tabControl2->SelectedIndex;
	}
	System::Void Form1::stopbutton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		m_FrameSliderController->pressStopButton();
		g_pMainApp->OnStopAnimation();
	}
	System::Void Form1::playbutton_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if(g_pMainApp->OnPlayAnimation())
			m_FrameSliderController->pressPlayButton();
	}
	System::Void Form1::pausebutton_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		m_FrameSliderController->pressPauseButton();
		g_pMainApp->OnPauseAnimation();
	}
	System::Void Form1::MainForm_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		g_pMainApp->OnMouseMove(0, RVector2(0,0), e->Delta);
	}
	System::Void Form1::AniSpeedtrackBar_Scroll(System::Object^  sender, System::EventArgs^  e) 
	{
		if( g_pMainApp->GetActor() == NULL) return;

		float s = AniSpeedtrackBar->Value / ANISPEED;
		g_pMainApp->GetActor()->SetAnimationSpeed(s);

		AniSpeednumericUpDown->Value = Convert::ToDecimal(s);
	}
	System::Void Form1::backgroundColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		ColorDialog^ colorDialog = gcnew System::Windows::Forms::ColorDialog();
		if(colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			g_pMainApp->SetClearClolor(colorDialog->Color.ToArgb());
		}
	}
	System::Void Form1::PartsColorPreviewMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		m_AmbientColorPickerDialog->Close();
			m_ColorPickerDialog->Close();
			m_ColorPickerDialog = gcnew ColorPickerDlg();

			ColorPickerMove();

			m_ColorPickerDialog->Show();
	}
	System::Void Form1::lightToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->OnOffLight();
	}
	System::Void Form1::feedbackToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		System::Diagnostics::Process::Start("IExplore.exe", "http://dev:8181/projects/MyPrj");
	}
	System::Void Form1::FeedBack2ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		System::Diagnostics::Process::Start("IExplore.exe", "http://iworks2.maiet.net/wiki/charactertool_net");
	}
	System::Void Form1::OpentoolStripButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		OpenMenuItem_Click(sender, e);
	}
	System::Void Form1::DrawBoxtoolStripButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->m_TMenu.OnShowAABB();
	}
	System::Void Form1::LighrtoolStripButton_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->OnOffLight();
	}
	System::Void Form1::MainView_Resize(System::Object^  sender, System::EventArgs^  e) 
	{
		if( MainView == nullptr) return;

		// 값이 0이면 다음과 같은 값으로 줘서 Reset이 안돼는 것을 막자
		if(MainView->Width <= 0)
			MainView->Width = 800;
		if(MainView->Height <= 0)
			MainView->Height = 600;

		REngine::GetConfig().m_nWidth = MainView->Width;
		REngine::GetConfig().m_nHeight = MainView->Height;
		g_pMainApp->InitState(MainView->Width,MainView->Height, false);
		g_pMainApp->OnResetDevice();
	}
	System::Void Form1::AmbienttoolStripButton1_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		m_ColorPickerDialog->Close();
		m_AmbientColorPickerDialog->Close();

		m_AmbientColorPickerDialog = gcnew ColorPickerDlg();
		m_AmbientColorPickerDialog->Show();
	}
	System::Void Form1::LightToCameratoolStripButton1_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->EventMessage(EVENT_MSG_LIGHT_TO_CAMERA);
	}
	System::Void Form1::collisionObjectToolStripMenuItem_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->SetActorCollision();
	}
	System::Void Form1::MainForm_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		if ( e->KeyChar == (char)Keys::Space)
		{
			if(g_pMainApp->GetActor() == NULL)
				return;

			if(m_FrameSliderController->GetAnimationState() == APS_STOP ||
				m_FrameSliderController->GetAnimationState() == APS_PAUSE)
				playbutton_Click(sender, e);
			else
				pausebutton_Click(sender, e);
		}
		else if(e->KeyChar == '/')
		{
			g_pMainApp->OnPlayAnimationByShourtCut();
		}
	}
	//이름은 이래도 mainview에 붙어 있음.
	System::Void Form1::MainForm_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
	{
		if (e->KeyCode == Keys::D1)
		{
			int i = 0;
		}
		//else if( e->KeyCode == Keys::D4)
		//{
		// g_pMainApp->ShowItemList(11);
		// g_pMainApp->EquipItem(0);
		//}
		// TODO: 넘 하드 코드인데.
		else if( e->KeyCode == Keys::D5) //sword
		{
			g_pMainApp->SelectItemInItemList(11);
			bool bEquipItem = g_pMainApp->m_EquipItems.EquipItemSlotIndex(1);
		}
		else if( e->KeyCode == Keys::D6)
		{
			g_pMainApp->SelectItemInItemList(11);
			bool bEquipItem = g_pMainApp->m_EquipItems.EquipItemSlotIndex(2);
		}
		else if( e->KeyCode == Keys::D7)
		{
			g_pMainApp->SelectItemInItemList(11);
			bool bEquipItem = g_pMainApp->m_EquipItems.EquipItemSlotIndex(3);
		}
		// Breakable Parts 테스트용
		//else if(e->KeyCode == Keys::D0)
		//{
		//	if(g_pMainApp->GetActor())
		//	{
		//		g_pMainApp->m_pSelectedActorNode = NULL;

		//		g_pMainApp->GetBreakablePartsManager()->SetBreakableParts(1, g_pMainApp->GetActor());
		//		mlog("%s Breakable Parts 테스트를 합니다.", "b_parts_01");
		//	}
		//}
	}
	System::Void Form1::SaveNPCHitInfo_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		TMeshInfoMgr::GetInstance().Save();
	}
	//스피드 적용 
	System::Void Form1::SpeedOkbutton_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		RAnimation* pAnimation = g_pMainApp->GetActor()->GetCurAnimation();
		if( pAnimation == NULL) return;

		float s = AniSpeedtrackBar->Value / ANISPEED;
		pAnimation->SetSpeed(s);
		g_pMainApp->EventMessage(EVENT_MSG_PLAY_ANIMATION);

		GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);
	}
	System::Void Form1::SaveAsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->SaveAs();
	}
	System::Void Form1::InfomationToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		HelpForm^ helpform = gcnew HelpForm;
		helpform->ShowDialog();
	}
	System::Void Form1::ReloadToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->ReloadCharacter();
	}
	System::Void Form1::eXitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		Close();
	}
	System::Void Form1::selectModelTabToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if( tabControl1->SelectedIndex == 0) return;
		tabControl1->SelectedIndex = 0;
		//g_pMainApp->ShowTabs(0);
	}
	System::Void Form1::selectNPCListTablToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if(tabControl1->SelectedIndex == 1) return;
		tabControl1->SelectedIndex = 1;
		//g_pMainApp->ShowTabs(1);
	}
	System::Void Form1::tabControl1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->ShowTabs(tabControl1->SelectedIndex);
		g_pMainApp->UpdateModelScale();
	}
	System::Void Form1::FullscreenToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->OnFullScreen();
	}
	System::Void Form1::LoopPlayButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->OnLoopAnimation();
	}
	System::Void Form1::animationPlayToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->OnPlayAnimation();
	}
	System::Void Form1::animationStopToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->OnStopAnimation();
	}
	System::Void Form1::animationLoopPlayToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->OnLoopAnimation();
	}
	System::Void Form1::transitionBlendingToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->OnTransitionBlending();
	}
	System::Void Form1::LoopPlaycheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
	}
	System::Void Form1::LoopPlaycheckBox_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->OnLoopAnimation();
	}
	System::Void Form1::ShowWireShade_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		g_pMainApp->m_TMenu.OnShowWire();
	}
	System::Void Form1::AniSpeedResetbutton_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		// TODO: 리셋값을 1로 하는지 애니메이션에 지정된 값으로 하는지.
		AniSpeedtrackBar->Value = (int)ANISPEED;
		g_pMainApp->GetActor()->SetAnimationSpeed(1.0f);
		AniSpeednumericUpDown->Value = Decimal(ANISPEED / 10.f);
	}
	System::Void Form1::AniSpeednumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		float v = (float)Convert::ToDouble(AniSpeednumericUpDown->Value);
		float fValue = v * ANISPEED;
		AniSpeedtrackBar->Value = Convert::ToInt32(fValue);
		float s = AniSpeedtrackBar->Value / ANISPEED;
		if(g_pMainApp->GetActor())
		{
			g_pMainApp->GetActor()->SetAnimationSpeed(s);
		}
	}
	System::Void Form1::RedoToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		m_TalentTabPage->m_TalentHitTabController->RedoCommand();
	}
	System::Void Form1::UndoToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		m_TalentTabPage->m_TalentHitTabController->UndoCommand();
	}
	System::Void Form1::MainView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		m_TalentTabPage->m_TalentHitTabController->AddDo();

		int istat = GetMouseButtonState(e->Button);
		g_pMainApp->OnMouseDown(istat, RVector2((float)e->X, (float)e->Y), e->Delta);
	}
	System::Void Form1::MainView_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		m_TalentTabPage->m_TalentHitTabController->AddReDo();
		m_MeshHitTabController->RefreshMeshHitColInfo();
	}

	System::Void Form1::ItemOpenbutton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		// TODO: 파일을 로드해서 파일명과 노드정보들을 뷰에 보여준다음 그정보로
		//		CSItemData 를 만들어서 아이템을 착용시킨다.
		OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

		openFileDialog1->Filter = "xml files (*.xml)|*.xml|elu files (*.elu)|*.elu|ani files (*.ani)|*.ani|All files (*.*)|*.*";
		openFileDialog1->FilterIndex = 2;
		openFileDialog1->RestoreDirectory = true;
		openFileDialog1->Multiselect		= true;

		if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			array<String^>^ FileNames = openFileDialog1->FileNames;
			int size = FileNames->Length;

			for each ( String^ FileName in FileNames )
			{
				//g_pMainApp->SetItemReview(FileName);
			}
		}

		delete openFileDialog1;
	}
	System::Void Form1::MainView_Click(System::Object^  sender, System::EventArgs^  e)
	{
		MainView->Focus();
	}
	System::Void Form1::EffectInfoNameComboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
	}

	System::Void Form1::TalentEffectAttGrid_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e)
	{
		m_TalentEffectTabController->OnEffectAttGrid_PropertyValueChanged();
	}


	System::Void Form1::Delattbutton_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		m_TalentEffectTabController->OnDelEffectInfoBtn_Click();
	}

	System::Void Form1::RefSelectBtn_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if(CurrentID->Text == "")
		{
			System::Windows::Forms::MessageBox::Show(L"탤런트를 선택하세요", "에러");
			return;
		}

		CharacterToolForms::TalentEffectRegisterEdit^ regEdit = gcnew CharacterToolForms::TalentEffectRegisterEdit();

		regEdit->TalentModecomboBox->Items->Clear();
		for (int i = 0; i < WEAPON_MAX; ++i)
		{
			string strWeapon = TStrings::WeaponType(i);
			String^ str = String::Format("{0}", gcnew String(strWeapon.c_str()));
			regEdit->TalentModecomboBox->Items->Add(str);
		}

		int nTalentID = atoi(MStringToCharPointer(RefTextBox->Text));
		regEdit->SetText(nTalentID, 0);
		regEdit->ShowDialog();
		
		System::Windows::Forms::DialogResult res = regEdit->DialogResult;
		if (res == System::Windows::Forms::DialogResult::OK)
		{
			int nSelectTalentID = 0;
			int nSelectTalentMode = 0;

			TTalentInfo* refTalent = TTalentHelper::GetTalentInfo(regEdit->m_nTalentID, regEdit->m_nTalentMode);
			if(refTalent)
			{
				nSelectTalentID = regEdit->m_nTalentID;
				nSelectTalentMode = regEdit->m_nTalentMode;

				// 레퍼런스 입력
				string strTalentID = TStrings::TalentIDString_WithMode(nSelectTalentID, nSelectTalentMode);
				RefTextBox->Text = gcnew String(String::Format("{0}", gcnew String(strTalentID.c_str())));
			}
			else
			{
				RefTextBox->Text = "";

				if (regEdit->m_nTalentID != 0
					|| regEdit->m_nTalentMode != 0)
				{
					if(System::Windows::Forms::MessageBox::Show(regEdit, L"탤런트가 존재하지 않습니다.", "에러")
						== System::Windows::Forms::DialogResult::OK)
					{
						RefSelectBtn_Click(sender, e);
					}
					return;
				}
			}

			m_TalentEffectTabController->SetTalentEffectReference(nSelectTalentID, nSelectTalentMode);
		}
	}

	System::Void Form1::RefTextBox_DoubleClick(System::Object^  sender, System::EventArgs^  e) 
	{
		RefSelectBtn_Click(sender, e);
	}

	System::Void Form1::TalentEvent_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e)
	 {
		 //// 탤런트가 선택이 되었는가?
		 //if(m_TalentEventController->IsExistenceTalentEvent() == false)
		 //{
			// // 아무것도 없다.
			// System::Windows::Forms::MessageBox::Show(L"탤런트를 선택하세요!", "에러");
			// return;
		 //}

		 ////g_pMainApp->SetPropertyTalentEvent(false);
		 //int nEventType = m_TalentEventController->GetCurrentTalentEventType();
		 //if(nEventType >= 0)
		 //{
			// m_TalentEventController->SetCurrentPropertyTalentEvent(true);
		 //}
		 //else
		 //{
			// TalentEventAtt->SelectedObject = nullptr;
		 //}
	 }
	System::Void Form1::TalentEventAttGrid_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e)
	 {
		 //// 탤런트가 선택이 되었는가?
		 //if(m_TalentEventController->IsExistenceTalentEvent() == false)
		 //{
			// // 아무것도 없다.
			// System::Windows::Forms::MessageBox::Show(L"탤런트를 선택하세요!", "에러");
			// return;
		 //}

		 //m_TalentEventController->SetCurrentPropertyTalentEvent(false);
	 }
	System::Void Form1::TalentInfoSave(System::Object^  sender, System::EventArgs^  e)
	 {
		 // 탤런트 이펙트, 이벤트, 충돌 정보 저장
		 g_pMainApp->SaveTalentExt(true, true, true);
	 }
	System::Void Form1::CurrentID_DoubleClick(System::Object^  sender, System::EventArgs^  e) 
	{
		CharacterToolForms::TextOneBox AddEventIDTextBox;

		AddEventIDTextBox.TitleLabel->Text = gcnew String("탤런트 ID");
		AddEventIDTextBox.ShowDialog();

		System::Windows::Forms::DialogResult res = AddEventIDTextBox.DialogResult;
		if (res == System::Windows::Forms::DialogResult::OK)
		{
			int nTalentID = atoi(MStringToCharPointer(AddEventIDTextBox.TextValue->Text));
			CSTalentInfo* pTalentInfo = TTalentHelper::GetTalentInfo(nTalentID, 0);

			// 이펙트 정보 보여주기
			g_pMainApp->SetTalentEffect(pTalentInfo);
		}
	}
	System::Void Form1::TalentEffectBase_Click(System::Object^  sender, System::EventArgs^  e)
	{
		TalentEffectBase TalentEffectBaseBox;

		// 버튼에서는 기본으로 한다.
		TalentEffectBaseBox.SetTalentEffectBaseInfo(g_pMainApp->m_pTalentEffectMgr);
		TalentEffectBaseBox.ShowDialog();

		System::Windows::Forms::DialogResult res = TalentEffectBaseBox.DialogResult;
		if (res == System::Windows::Forms::DialogResult::OK)
		{
			if(TalentEffectBaseBox.GetTalentEffectBaseInfo(g_pMainApp->m_pTalentEffectMgr) == true)
			{
				GlobalObjects::g_SaveNotifier.SetSaveTalentEffect(true);
			}
		}
	}

	System::Void Form1::ViewReferenceMeshToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if(g_pMainApp->m_ReferenceMesh.m_pReferenceMesh == NULL )
		{
			ViewreferenceMeshToolStripMenuItem->Checked = false;
			g_pMainApp->m_ReferenceMesh.m_bUseRefMesh = false;
			return;
		}

		if (ViewreferenceMeshToolStripMenuItem->Checked)
		{
			ViewreferenceMeshToolStripMenuItem->Checked = false;
			g_pMainApp->m_ReferenceMesh.m_bUseRefMesh = false;
		}
		else
		{
			ViewreferenceMeshToolStripMenuItem->Checked = true;
			g_pMainApp->m_ReferenceMesh.m_bUseRefMesh = true;
		}
	}

	System::Void Form1::SetReferenceMeshToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

		//openFileDialog1->InitialDirectory = "c:\\";
		openFileDialog1->Filter = "elu files (*.elu)|*.elu";
		openFileDialog1->FilterIndex = 2;
		openFileDialog1->RestoreDirectory	= true;
		openFileDialog1->Multiselect		= false;

		std::string strFileName;
		if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			//String^ FileName = openFileDialog1->FileName;	//path까지 다 들어온다.
			//g_pMainApp->MeshFileOpen(FileName);

			// TODO: 폴더이름으로 (플레이어 NPC 몬스터) parent이름을 설정하면 좋은데...

			array<String^>^ FileNames = openFileDialog1->FileNames;
			int size = FileNames->Length;

			for each ( String^ FileName in FileNames )
			{
				const char* chFileName = MStringToCharPointer(FileName);
				strFileName = chFileName;
				MFreeCharPointer(chFileName);
			}

			if (g_pMainApp->m_ReferenceMesh.m_strRefMeshName != strFileName)
			{
				g_pMainApp->m_ReferenceMesh.m_strRefMeshName = strFileName;
				if (g_pMainApp->m_ReferenceMesh.m_pReferenceMesh)
				{
					g_pMainApp->GetSceneMgr()->RemoveSceneNode(g_pMainApp->m_ReferenceMesh.m_pReferenceMesh,false);
					SAFE_DELETE(g_pMainApp->m_ReferenceMesh.m_pReferenceMesh);
				}

				g_pMainApp->m_ReferenceMesh.m_pReferenceMesh = new TCharacter;
				g_pMainApp->m_ReferenceMesh.m_pReferenceMesh->RActor::Create(strFileName.c_str());
				g_pMainApp->GetSceneMgr()->AddSceneNode(g_pMainApp->m_ReferenceMesh.m_pReferenceMesh,false);
//				g_pMainApp->m_ReferenceMesh.m_pReferenceMesh->SetUsingPassRenderer(false);
			}
		}
		delete openFileDialog1;
	}

	System::Void Form1::Add_PartsCapsule_Click(System::Object^  sender, System::EventArgs^  e)
	{
		// 부분 판정 캡슐 추가
		if(PartsHitCapsulesView->SelectedNode != nullptr)
		{
			// 선택된 노드가 있다면...
			// 단 그룹 아이디가 0번은 캡슐 하나만 만들 수 있다.
			if(GlobalObjects::g_SelInfo.m_nMeshHitGroupID == 0)
			{
				int nNextCapsuleID = g_pMainApp->GetNextCapsuleIndex(GlobalObjects::g_SelInfo.m_nMeshHitGroupID);
				if(nNextCapsuleID > 0)
				{
					System::Windows::Forms::MessageBox::Show(gcnew String("선택된 그룹(0)은 캡슐 하나만 만들 수 있습니다."), "판정 캡슐 추가 에러");
					return;
				}
			}

			g_pMainApp->CreateMeshHitCapsuleInfo(GlobalObjects::g_SelInfo.m_nMeshHitGroupID);
		}
		else
		{
			// 선택된 노드가 없다면...
			if (System::Windows::Forms::MessageBox::Show("판정 그룹이 없거나, 선택을 하지 않았습니다. 그룹을 새로 만드시겠습니까?", "판정 그룹"
				,MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes)
			{
				// 선택시에는 그룹을 만든다.
				int nGroupIndex = g_pMainApp->CreateMeshHitGroupInfo();
				if(nGroupIndex > 0)
				{
					// 완료
					int nCapsuleID	= -1;
					g_pMainApp->CreateMeshHitCapsuleInfo(nGroupIndex);
				}
			}
		}
	}

	System::Void Form1::Add_PartsGroup_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		// 그룹 추가
		g_pMainApp->CreateMeshHitGroupInfo();
	}

	System::Void Form1::Del_PartsCapsule_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		m_MeshHitTabController->DelPartsCapsule_Click(sender, e);
	}

	System::Void Form1::PartsHittreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) 
	{
		GlobalObjects::g_SelInfo.m_nMeshHitGroupID			= -1;
		GlobalObjects::g_SelInfo.m_nMeshHitCapsuleID		= -1;
		GlobalObjects::g_pMainForm->partsHitCapsuleToolStripMenuItem->Checked = false;

		//최상위노드이면
		if( e->Node->Parent == nullptr )
		{
			int n = e->Node->Index;
			if (n >= 0)
			{
				Object^ tag								= e->Node->Tag;
				if(tag != nullptr)
				{
					GlobalObjects::g_SelInfo.m_nMeshHitGroupID			= (int)tag;
					GlobalObjects::g_SelInfo.m_nMeshHitCapsuleID		= -1;
					GlobalObjects::g_pMainForm->partsHitCapsuleToolStripMenuItem->Checked = true;
				}
			}
		}
		else
		{
			int pn = e->Node->Parent->Index;
			int n = e->Node->Index;
			if( n >= 0)
			{
				Object^ tagParent						= e->Node->Parent->Tag;
				Object^ tag								= e->Node->Tag;
				if(tag != nullptr && tagParent != nullptr)
				{
					GlobalObjects::g_SelInfo.m_nMeshHitGroupID			= (int)tagParent;
					GlobalObjects::g_SelInfo.m_nMeshHitCapsuleID		= (int)tag;
					GlobalObjects::g_pMainForm->partsHitCapsuleToolStripMenuItem->Checked = true;
				}
			}
		}

		g_pMainApp->SetPartsHitCapsuleInfo(GlobalObjects::g_SelInfo.m_nMeshHitGroupID, GlobalObjects::g_SelInfo.m_nMeshHitCapsuleID);
	}

	System::Void Form1::PartsHitCapsule_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e) 
	{
		g_pMainApp->ChangePartsHitCapsuleInfo(GlobalObjects::g_SelInfo.m_nMeshHitGroupID, GlobalObjects::g_SelInfo.m_nMeshHitCapsuleID);
	}

	System::Void Form1::PartsHitCapsuleStripMenuItem_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->CheckShowPartsHitCapsule();
	}

	System::Void Form1::AllTalentTime_Click( System::Object^ sender, System::EventArgs^ e )
	{
		DWORD dwStartTime = GetTickCount();

		g_pMainApp->AllTalentTimeRefresh();
		g_pMainApp->SaveTalentExt(true, true, true);

		mlog("정보 : 저장완료 : %d ms\r", GetTickCount() - dwStartTime);		
	}
	
	// 재질 조합
	System::Void Form1::buttonApply_Click( System::Object^ sender, System::EventArgs^ e )
	{
		g_pMainApp->m_Mtrl.ChangeMaterialSet();
	}

	System::Void Form1::listBoxMtrlList_MouseDoubleClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		g_pMainApp->m_Mtrl.ChangeMaterialSet();
	}

	System::Void Form1::buttonExport_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		g_pMainApp->m_Mtrl.ExportMtrlSet();
	}

	System::Void Form1::MeshColtreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e)
	{
		GlobalObjects::g_SelInfo.m_nMeshColID			= -1;

		int n = e->Node->Index;
		if( n >= 0)
		{
			Object^ tag								= e->Node->Tag;
			if(tag != nullptr)
			{
				GlobalObjects::g_SelInfo.m_nMeshColID		= (int)tag;
				GlobalObjects::g_pMainForm->toolStripMenuItem2->Checked = true;
			}
		}
		else
		{
			GlobalObjects::g_SelInfo.m_nMeshColID			= -1;
			GlobalObjects::g_pMainForm->toolStripMenuItem2->Checked = false;
		}

		m_MeshHitTabController->Serialize();
	}

	System::Void Form1::MeshColCapsuleStripMenuItem_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		g_pMainApp->CheckShowMeshColCapsule();
	}

	System::Void Form1::MeshColCapsule_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e)
	{
		g_pMainApp->ChangeMeshColInfo(GlobalObjects::g_SelInfo.m_nMeshColID);
	}

	System::Void Form1::Add_CollisionCapsule_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		m_MeshHitTabController->AddCollisionCapsule_Click(sender, e);
	}

	System::Void Form1::Del_CollisionCapsule_Click(System::Object^  sender, System::EventArgs^  e)
	{
		m_MeshHitTabController->DelCollision_Click(sender, e);
	}

	System::Void Form1::SaveAlltoolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		g_pMainApp->SaveAll();
	}

	System::Void Form1::MoveDummycheckBox_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
	{
		g_pMainApp->m_bMoveDummy = MoveDummycheckBox->Checked;
	}
	System::Void Form1::miTalentExport_Click( System::Object^ sender, System::EventArgs^ e )
	{
		//System::Diagnostics::Process^ ps = gcnew System::Diagnostics::Process;
		//ps->StartInfo->FileName = "TalentExporter.exe";
		//ps->StartInfo->WorkingDirectory = "./";
		//ps->StartInfo->WindowStyle = System::Diagnostics::ProcessWindowStyle::Normal;

		//ps->Start();
		g_pMainApp->StartTalentPosInfoExporter();
	}

	System::Void Form1::Unselected_toolStripButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		Bottom_toolStripButton->Checked = false;
		Top_toolStripButton->Checked	= false;
		TopBottom_toolStripButton->Checked = false;
		X_toolStripButton->Checked	= false;
		Y_toolStripButton->Checked = false;
		Z_toolStripButton->Checked = false;
		XY_toolStripButton->Checked = false;
	}

	System::Void Form1::check_XYZ()
	{
		if( X_toolStripButton->Checked	== false &&
			Y_toolStripButton->Checked == false &&
			Z_toolStripButton->Checked == false &&
			XY_toolStripButton->Checked == false )
		{
			XY_toolStripButton->Checked = true;
		}
	}

	System::Void Form1::Bottom_toolStripButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( Bottom_toolStripButton->Checked)
		{
			Top_toolStripButton->Checked = false;
			TopBottom_toolStripButton->Checked = false;

			check_XYZ();
		}
	}

	System::Void Form1::Top_toolStripButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( Top_toolStripButton->Checked)
		{
			Bottom_toolStripButton->Checked = false;
			TopBottom_toolStripButton->Checked = false;

			check_XYZ();
		}
	}

	System::Void Form1::TopBottom_toolStripButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( TopBottom_toolStripButton->Checked)
		{
			Top_toolStripButton->Checked = false;
			Bottom_toolStripButton->Checked = false;

			check_XYZ();
		}
	}

	System::Void Form1::X_toolStripButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( X_toolStripButton->Checked)
		{
			Y_toolStripButton->Checked = false;
			Z_toolStripButton->Checked = false;
			XY_toolStripButton->Checked = false;
		}
	}

	System::Void Form1::Y_toolStripButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( Y_toolStripButton->Checked)
		{
			X_toolStripButton->Checked = false;
			Z_toolStripButton->Checked = false;
			XY_toolStripButton->Checked = false;
		}
	}

	System::Void Form1::Z_toolStripButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( Z_toolStripButton->Checked)
		{
			X_toolStripButton->Checked = false;
			Y_toolStripButton->Checked = false;
			XY_toolStripButton->Checked = false;
		}
	}

	System::Void Form1::XY_toolStripButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( XY_toolStripButton->Checked)
		{
			X_toolStripButton->Checked = false;
			Y_toolStripButton->Checked = false;
			Z_toolStripButton->Checked = false;
		}
	}

	System::Void Form1::Unselect_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		Unselected_toolStripButton_Click(nullptr, nullptr);
	}

	System::Void Form1::Bottom_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		Bottom_toolStripButton->Checked = !Bottom_toolStripButton->Checked;
		Bottom_toolStripButton_Click(nullptr, nullptr);
	}

	System::Void Form1::Top_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		Top_toolStripButton->Checked = !Top_toolStripButton->Checked;
		Top_toolStripButton_Click(nullptr, nullptr);
	}

	System::Void Form1::TopBottom_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		TopBottom_toolStripButton->Checked = !TopBottom_toolStripButton->Checked;
		TopBottom_toolStripButton_Click(nullptr, nullptr);
	}

	System::Void Form1::X_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		X_toolStripButton->Checked = !X_toolStripButton->Checked;
		X_toolStripButton_Click(nullptr, nullptr);
	}

	System::Void Form1::Y_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		Y_toolStripButton->Checked = !Y_toolStripButton->Checked;
		Y_toolStripButton_Click(nullptr, nullptr);
	}

	System::Void Form1::Z_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		Z_toolStripButton->Checked = !Z_toolStripButton->Checked;
		Z_toolStripButton_Click(nullptr, nullptr);
	}

	System::Void Form1::XY_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		XY_toolStripButton->Checked = !XY_toolStripButton->Checked;
		XY_toolStripButton_Click(nullptr, nullptr);
	}

	System::Void Form1::Opa_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CapsuleOpa_toolStripButton->Checked = !CapsuleOpa_toolStripButton->Checked;
	}
	System::Void Form1::ItemPreviewtreeView_NodeMouseClick( System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e )
	{
		g_pMainApp->m_TabModelInfo.NodeNoneSelectedButtonClick();

		// TODO: parent이면 무기일수 있으니 수정해야한다. ** 아래 소스도 다...**
		if( e->Node->Parent == nullptr)
		{
			int childcount = e->Node->GetNodeCount(false);

			if (childcount == 1)
			{
				g_pMainApp->m_EquipItems.EquipReviewItems(e->Node->Text, e->Node->FirstNode->Text, ITEMTYPE_ARMOR);
			}
			else
			{
				if(e->Button == System::Windows::Forms::MouseButtons::Right)
				{
					TreeNode^ tn = nullptr;
					System::Collections::IEnumerator^ myNodes = (safe_cast<System::Collections::IEnumerable^>(e->Node->Nodes))->GetEnumerator();
					while (myNodes->MoveNext())
					{
						tn = safe_cast<TreeNode^>(myNodes->Current);
						g_pMainApp->m_EquipItems.EquipReviewItems(e->Node->Text, tn->Text, ITEMTYPE_ARMOR);
					}				
				}
			}
		}
		else
		{
			g_pMainApp->m_EquipItems.EquipReviewItems(e->Node->Parent->Text, e->Node->Text, ITEMTYPE_ARMOR);

			// if(e->Button == System::Windows::Forms::MouseButtons::Left)
			// {
			//	 String^ str = e->Node->Text;
			//	 string sstr = MStringToCharPointer(str);
			//	 g_pMainApp->m_ItemSlotnum = g_pMainApp->GetNodetoItemslot(sstr);
			//	 g_pMainApp->EquipReviewItems(e->Node->Parent->Text, e->Node->Text, ITEMTYPE_EQUIPMENT);
			// }
			// else
			//	 ItemPreviewtreeView->SelectedNode = e->Node;
			//}
			//else
			//{
			// if(e->Button == System::Windows::Forms::MouseButtons::Left)
			// {
			//	 //g_pMainApp->EquipReviewWeapon(e->Node->Text, "longsword_02", "dummy_1hs_r", "RWeapon", "1hs");
			//	 //g_pMainApp->EquipReviewItems(e->Node->Text, e->Node->Text);
			// }
			// else
			//	 ItemPreviewtreeView->SelectedNode = e->Node;
		}

		g_pMainApp->m_TabModelInfo.SetBipTreeView();
	}

	System::Void Form1::Refresh_button_Click( System::Object^ sender, System::EventArgs^ e )
	{
		g_pMainApp->RefreshModels();
		g_pMainApp->SetReviewItems();
	}

	System::Void Form1::SlotHead_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		String^ str = (sender->ToString())->ToLower();
		string sstr = MStringToCharPointer(str);
		g_pMainApp->m_EquipItems.SetItemSlotNum(g_pMainApp->GetNodetoItemslot(sstr));
		if( ItemPreviewtreeView->SelectedNode->Parent != nullptr)
			g_pMainApp->m_EquipItems.EquipReviewItems(ItemPreviewtreeView->SelectedNode->Parent->Text,
			ItemPreviewtreeView->SelectedNode->Text, ITEMTYPE_ARMOR);
	}

	System::Void Form1::LW_loc_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		String^ str = (sender->ToString())->ToLower();
		g_pMainApp->m_EquipItems.SetItemSlotNum(ITEMSLOT_LWEAPON);
		g_pMainApp->m_EquipItems.SetItemParentName(MStringToCharPointer(str));
		if( ItemPreviewtreeView->SelectedNode->Parent == nullptr)
			g_pMainApp->m_EquipItems.EquipReviewItems(ItemPreviewtreeView->SelectedNode->Text, ItemPreviewtreeView->SelectedNode->Text);
		else
			g_pMainApp->m_EquipItems.EquipReviewItems(ItemPreviewtreeView->SelectedNode->Parent->Text, ItemPreviewtreeView->SelectedNode->Text);
	}

	System::Void Form1::RW_loc_toolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		String^ str = (sender->ToString())->ToLower();
		g_pMainApp->m_EquipItems.SetItemSlotNum(ITEMSLOT_RWEAPON);
		g_pMainApp->m_EquipItems.SetItemParentName(MStringToCharPointer(str));
		if( ItemPreviewtreeView->SelectedNode->Parent == nullptr)
			g_pMainApp->m_EquipItems.EquipReviewItems(ItemPreviewtreeView->SelectedNode->Text, ItemPreviewtreeView->SelectedNode->Text);
		else
			g_pMainApp->m_EquipItems.EquipReviewItems(ItemPreviewtreeView->SelectedNode->Parent->Text, ItemPreviewtreeView->SelectedNode->Text);
	}

	System::Void Form1::basicMesh_button_Click( System::Object^ sender, System::EventArgs^ e )
	{
		g_pMainApp->BasicMesh();
	}

	System::Void Form1::hDRToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		static bool bHDREnable = false;
		bHDREnable = !bHDREnable;
		g_pMainApp->SetHDREnable(bHDREnable);

		hDRToolStripMenuItem->Checked = bHDREnable;
	}

	System::Void Form1::drawGridToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		static bool bShowGrid = true;
		bShowGrid = !bShowGrid;
		g_pMainApp->ShowGrid(bShowGrid);
	}

	void Form1::Create_PCTalentTab()
	{
		this->m_PCTalenttreeView = gcnew PCTalentTreeView();
		//this->m_PCTalenttreeView->TopLevel = false;
		this->m_PCTalenttreeView->Show();
		this->m_PCTalenttreeView->Dock = System::Windows::Forms::DockStyle::Fill;
		//this->m_PCTalenttreeView->Location = System::Drawing::Point(0, 0);
		//this->m_PCTalenttreeView->Name = L"PCTalenttreeView";
		//this->m_PCTalenttreeView->Size = System::Drawing::Size(198, 573);
		//this->m_PCTalenttreeView->TabIndex = 1;
		this->tabPage2->Controls->Add(this->m_PCTalenttreeView);
	}

	void Form1::Create_ModelTabPage()
	{
		this->m_ModelTabPage = gcnew ModelTabPage();
		this->m_ModelTabPage->TopLevel = false;
		this->m_ModelTabPage->Show();
		this->m_ModelTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
		this->tabPage4->Controls->Add(this->m_ModelTabPage);
	}

	void Form1::Create_NPCListTabPage()
	{
		this->m_NPCListTabPage = gcnew NPCListTabPage();
		//this->m_NPCListTabPage->TopLevel = false;
		this->m_NPCListTabPage->Show();
		this->m_NPCListTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
		this->tabPage1->Controls->Add(this->m_NPCListTabPage);
	}

	void Form1::Create_ItemTabPage()
	{
		this->m_ItemTabPage = gcnew ItemTabPage();
		this->m_ItemTabPage->TopLevel = false;
		this->m_ItemTabPage->Show();
		this->m_ItemTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
		this->Itemtab->Controls->Add(this->m_ItemTabPage);

	}

	void Form1::Create_TalentTabPage()
	{
		this->m_TalentTabPage = gcnew TalentTabPage();
		this->m_TalentTabPage->TopLevel = false;
		this->m_TalentTabPage->Show();
		this->m_TalentTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
		this->Talenttab->Controls->Add(this->m_TalentTabPage);
	}

	void Form1::Create_AnimationTabPage()
	{
		this->m_AnimationTabPage = gcnew AnimationTabPage();
		this->m_AnimationTabPage->TopLevel = false;
		this->m_AnimationTabPage->Show();
		this->m_AnimationTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
		this->Animationtab->Controls->Add(this->m_AnimationTabPage);
	}

	void Form1::Create_ModelInfoTabPage()
	{
		this->m_ModelInfoTabPage = gcnew ModelInfoTabPage();
		this->m_ModelInfoTabPage->TopLevel = false;
		this->m_ModelInfoTabPage->Show();
		this->m_ModelInfoTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
		this->ModelInfotab->Controls->Add(this->m_ModelInfoTabPage);
	}

	void Form1::Create_EventTabPage()
	{
		this->m_EventTabPage = gcnew EventTabPage();
		this->m_EventTabPage->TopLevel = false;
		this->m_EventTabPage->Show();
		this->m_EventTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
		this->TalentEvent->Controls->Add(this->m_EventTabPage);
	}

	int Form1::GetMouseButtonState( System::Windows::Forms::MouseButtons btn )
	{
		switch ( btn )
		{
		case System::Windows::Forms::MouseButtons::Left:
			return 1;
			break;
		case System::Windows::Forms::MouseButtons::Right:
			return 2;
			break;
		case System::Windows::Forms::MouseButtons::Middle:
			return 3;
			break;
		case System::Windows::Forms::MouseButtons::XButton1:
			return 4;
			//eventString = "X1";
			break;
		case System::Windows::Forms::MouseButtons::XButton2:
			return 5;
			//eventString = "X2";
			break;
		case System::Windows::Forms::MouseButtons::None:
		default:
			break;
		}

		return 0;
	}

	void Form1::InitLoadingInfoText(string strLoadingInfo)
	{
		if( m_InitLoadingWindow.Visible )
			m_InitLoadingWindow.InitLoadingInfoText( strLoadingInfo );
	}

	System::Void Form1::HitEffect_propertyGrid_PropertyValueChanged( System::Object^ s, System::Windows::Forms::PropertyValueChangedEventArgs^ e )
	{
		g_pMainApp->ChangeMeshHitEffectCapsuleInfo(GlobalObjects::g_SelInfo.m_nMeshHitEffectCapsuleID);
	}

	System::Void Form1::HitEffect_treeView_AfterSelect( System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e )
	{
		GlobalObjects::g_SelInfo.m_nMeshHitEffectCapsuleID			= -1;

		int n = e->Node->Index;
		if( n >= 0)
		{
			Object^ tag								= e->Node->Tag;
			if(tag != nullptr)
			{
				GlobalObjects::g_SelInfo.m_nMeshHitEffectCapsuleID		= (int)tag;
				GlobalObjects::g_pMainForm->toolStripMenuItem4->Checked = true;
			}
		}
		else
		{
			GlobalObjects::g_SelInfo.m_nMeshHitEffectCapsuleID			= -1;
			GlobalObjects::g_pMainForm->toolStripMenuItem4->Checked = false;
		}

		m_MeshHitTabController->Serialize();
	}

	void Form1::Create_MeshInfoTabPage()
	{
		this->m_MeshInfoTabPage = gcnew MeshInfoTabPage();
		this->m_MeshInfoTabPage->TopLevel = false;
		this->m_MeshInfoTabPage->Show();
		this->m_MeshInfoTabPage->Dock = System::Windows::Forms::DockStyle::Fill;
		this->MeshInfotab->Controls->Add(this->m_MeshInfoTabPage);
	}

	System::Void Form1::showLogWindowToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if(showLogWindowToolStripMenuItem->Checked == true)
		{
			int nLogTextBoxHeight = LogtextBox->Size.Height;
	
			// 프레임 그룹 박스 위치 수정
			System::Drawing::Point tempPoint = groupBox3->Location;
			groupBox3->Location = System::Drawing::Point(tempPoint.X, tempPoint.Y - nLogTextBoxHeight);

			// 스케일 그룹 박스 위치 수정
			tempPoint = groupBox4->Location;
			groupBox4->Location = System::Drawing::Point(tempPoint.X, tempPoint.Y - nLogTextBoxHeight);

			// 메인창 크기 조절
			System::Drawing::Size centerFormSize = splitContainer1->Size;
			splitContainer1->Size = System::Drawing::Size(centerFormSize.Width, centerFormSize.Height - nLogTextBoxHeight);

			//this->Size = System::Drawing::Size(mainFormSize.Width, mainFormSize.Height - nLogTextBoxHeight);

			LogtextBox->Show();
		}
		else
		{
			LogtextBox->Hide();

			int nLogTextBoxHeight = LogtextBox->Size.Height;

			// 프레임 그룹 박스 위치 수정
			System::Drawing::Point tempPoint = groupBox3->Location;
			groupBox3->Location = System::Drawing::Point(tempPoint.X, tempPoint.Y + nLogTextBoxHeight);

			// 스케일 그룹 박스 위치 수정
			tempPoint = groupBox4->Location;
			groupBox4->Location = System::Drawing::Point(tempPoint.X, tempPoint.Y + nLogTextBoxHeight);

			// 메인창 크기 조절
			System::Drawing::Size centerFormSize = splitContainer1->Size;
			splitContainer1->Size = System::Drawing::Size(centerFormSize.Width, centerFormSize.Height + nLogTextBoxHeight);


			//this->Size = System::Drawing::Size(mainFormSize.Width, mainFormSize.Height - nLogTextBoxHeight);
		}
	}

	void Form1::ShowReactionAnimationTable()
	{
		m_ReActionAnimationTable = gcnew ReActionAnimationTable();
		m_ReActionAnimationTable->Owner = this;
		m_ReActionAnimationTable->Show();
	}

}//namespace Character_NET
