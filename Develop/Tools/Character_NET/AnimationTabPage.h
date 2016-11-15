#pragma once

#include "StdAfx.h"
#include "RAnimationResource.h"

#include "AnimationEventTabPage.h"
#include "AnimationLinkTestTabPage.h"
#include "OverlayAnimationTestTabPage.h"
#include "AnimationHitSoundTabPage.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// AnimationTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class AnimationTabPage : public System::Windows::Forms::Form
	{
	public:
		AnimationTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//

			m_selectAnimationNode = nullptr;

			Create_AnimationEventTabPage();
			Create_AnimationLinkTestTabPage();
			Create_OverlayAnimationTestTabPage();
			Create_AnimationHitSoundTabPage();
		}

		void Create_AnimationEventTabPage();
		void Create_AnimationLinkTestTabPage();
		void Create_OverlayAnimationTestTabPage();
		void Create_AnimationHitSoundTabPage();

		void AnimationInfoBtnEnable(bool bEnable);

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~AnimationTabPage()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox11;
	protected: 
	public: System::Windows::Forms::CheckBox^  LoopcheckBox;
	private: 
	public: System::Windows::Forms::Label^  AniNamelabel;

	public: 
	public: AnimationEventTabPage^	m_AnimationEventTabPage;
	public: AnimationLinkTestTabPage^ m_AnimationLinkTestTabPage;
	public: OverlayAnimationTestTabPage^ m_OverlayAnimationTestTabPage;
	public: AnimationHitSoundTabPage^ m_AnimationHitSoundTabPage;
	public: TreeNode^	m_selectAnimationNode;

	private: 
	public: System::Windows::Forms::TreeView^  AnitreeView;
	public: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  AniDeltoolStripMenuItem;
	public: 
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator11;


	private: System::Windows::Forms::ToolStripMenuItem^  aninamecopytoolStripMenuItem;
	public: System::Windows::Forms::TabControl^  Animation_tabControl;
	private: System::Windows::Forms::TabPage^  AnimationEvent_tabPage;
	private: System::Windows::Forms::TabPage^  AnimationLinkTest_tabPage;
	private: System::Windows::Forms::SplitContainer^  AnimationSplitContainer;
	public: System::Windows::Forms::Panel^  ErrorViewer_panel;
	private: System::Windows::Forms::TabPage^  OverlayAnimation_tabPage;
	private: System::Windows::Forms::ToolStripMenuItem^  AnimationBlendTableToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  OverlayAnimation_ToolStripMenuItem;
	private: System::Windows::Forms::TabPage^  HitSound_tabPage;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ToolStripMenuItem^  ReActionAnimation_ToolStripMenuItem;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button5;



	public: 

	public: 

	public: 

	private: 
	public: 
	private: 




	public: 
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::TreeNode^  treeNode1 = (gcnew System::Windows::Forms::TreeNode(L"none"));
			this->groupBox11 = (gcnew System::Windows::Forms::GroupBox());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->LoopcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->AniNamelabel = (gcnew System::Windows::Forms::Label());
			this->AnitreeView = (gcnew System::Windows::Forms::TreeView());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->AniDeltoolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ReActionAnimation_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->AnimationBlendTableToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->OverlayAnimation_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator11 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->aninamecopytoolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Animation_tabControl = (gcnew System::Windows::Forms::TabControl());
			this->AnimationEvent_tabPage = (gcnew System::Windows::Forms::TabPage());
			this->AnimationLinkTest_tabPage = (gcnew System::Windows::Forms::TabPage());
			this->OverlayAnimation_tabPage = (gcnew System::Windows::Forms::TabPage());
			this->HitSound_tabPage = (gcnew System::Windows::Forms::TabPage());
			this->AnimationSplitContainer = (gcnew System::Windows::Forms::SplitContainer());
			this->ErrorViewer_panel = (gcnew System::Windows::Forms::Panel());
			this->groupBox11->SuspendLayout();
			this->contextMenuStrip1->SuspendLayout();
			this->Animation_tabControl->SuspendLayout();
			this->AnimationSplitContainer->Panel1->SuspendLayout();
			this->AnimationSplitContainer->Panel2->SuspendLayout();
			this->AnimationSplitContainer->SuspendLayout();
			this->ErrorViewer_panel->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox11
			// 
			this->groupBox11->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox11->Controls->Add(this->button5);
			this->groupBox11->Controls->Add(this->button4);
			this->groupBox11->Controls->Add(this->button3);
			this->groupBox11->Controls->Add(this->button2);
			this->groupBox11->Controls->Add(this->button1);
			this->groupBox11->Controls->Add(this->LoopcheckBox);
			this->groupBox11->Controls->Add(this->AniNamelabel);
			this->groupBox11->Location = System::Drawing::Point(3, 229);
			this->groupBox11->Name = L"groupBox11";
			this->groupBox11->Size = System::Drawing::Size(252, 91);
			this->groupBox11->TabIndex = 9;
			this->groupBox11->TabStop = false;
			this->groupBox11->Text = L"Animation Info";
			// 
			// button5
			// 
			this->button5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button5->Font = (gcnew System::Drawing::Font(L"굴림", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->button5->Location = System::Drawing::Point(169, 33);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(75, 23);
			this->button5->TabIndex = 13;
			this->button5->Text = L"보스 테스트";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &AnimationTabPage::button5_Click);
			// 
			// button4
			// 
			this->button4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button4->Font = (gcnew System::Drawing::Font(L"굴림", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->button4->Location = System::Drawing::Point(89, 33);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(75, 23);
			this->button4->TabIndex = 12;
			this->button4->Text = L"리액션 테스트";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &AnimationTabPage::button4_Click);
			// 
			// button3
			// 
			this->button3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button3->Font = (gcnew System::Drawing::Font(L"굴림", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->button3->Location = System::Drawing::Point(89, 62);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 11;
			this->button3->Text = L"오버레이 설정";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &AnimationTabPage::button3_Click);
			// 
			// button2
			// 
			this->button2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button2->Font = (gcnew System::Drawing::Font(L"굴림", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->button2->Location = System::Drawing::Point(8, 62);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 10;
			this->button2->Text = L"블렌딩 설정";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &AnimationTabPage::button2_Click);
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button1->Font = (gcnew System::Drawing::Font(L"굴림", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->button1->Location = System::Drawing::Point(8, 33);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 9;
			this->button1->Text = L"리액션 설정";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &AnimationTabPage::button1_Click);
			// 
			// LoopcheckBox
			// 
			this->LoopcheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->LoopcheckBox->AutoSize = true;
			this->LoopcheckBox->Location = System::Drawing::Point(177, 16);
			this->LoopcheckBox->Name = L"LoopcheckBox";
			this->LoopcheckBox->Size = System::Drawing::Size(52, 16);
			this->LoopcheckBox->TabIndex = 7;
			this->LoopcheckBox->Text = L"Loop";
			this->LoopcheckBox->UseVisualStyleBackColor = true;
			this->LoopcheckBox->CheckedChanged += gcnew System::EventHandler(this, &AnimationTabPage::LoopcheckBox_CheckedChanged);
			// 
			// AniNamelabel
			// 
			this->AniNamelabel->AutoSize = true;
			this->AniNamelabel->Location = System::Drawing::Point(6, 17);
			this->AniNamelabel->Name = L"AniNamelabel";
			this->AniNamelabel->Size = System::Drawing::Size(57, 12);
			this->AniNamelabel->TabIndex = 8;
			this->AniNamelabel->Text = L"AniName";
			// 
			// AnitreeView
			// 
			this->AnitreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->AnitreeView->ContextMenuStrip = this->contextMenuStrip1;
			this->AnitreeView->HideSelection = false;
			this->AnitreeView->Location = System::Drawing::Point(0, 0);
			this->AnitreeView->Name = L"AnitreeView";
			treeNode1->Name = L"none";
			treeNode1->Text = L"none";
			this->AnitreeView->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(1) {treeNode1});
			this->AnitreeView->Size = System::Drawing::Size(258, 223);
			this->AnitreeView->TabIndex = 7;
			this->AnitreeView->NodeMouseDoubleClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &AnimationTabPage::AnitreeView_NodeMouseDoubleClick);
			this->AnitreeView->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &AnimationTabPage::AnitreeView_MouseUp);
			this->AnitreeView->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &AnimationTabPage::AnitreeView_MouseMove);
			this->AnitreeView->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &AnimationTabPage::AnitreeView_MouseDown);
			this->AnitreeView->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &AnimationTabPage::AnitreeView_NodeMouseClick);
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->AniDeltoolStripMenuItem, 
				this->ReActionAnimation_ToolStripMenuItem, this->AnimationBlendTableToolStripMenuItem, this->OverlayAnimation_ToolStripMenuItem, 
				this->toolStripSeparator11, this->aninamecopytoolStripMenuItem});
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(215, 120);
			this->contextMenuStrip1->UseWaitCursor = true;
			// 
			// AniDeltoolStripMenuItem
			// 
			this->AniDeltoolStripMenuItem->Name = L"AniDeltoolStripMenuItem";
			this->AniDeltoolStripMenuItem->Size = System::Drawing::Size(214, 22);
			this->AniDeltoolStripMenuItem->Text = L"애니메이션 삭제";
			this->AniDeltoolStripMenuItem->Click += gcnew System::EventHandler(this, &AnimationTabPage::AniDeltoolStripMenuItem_Click);
			// 
			// ReActionAnimation_ToolStripMenuItem
			// 
			this->ReActionAnimation_ToolStripMenuItem->Name = L"ReActionAnimation_ToolStripMenuItem";
			this->ReActionAnimation_ToolStripMenuItem->Size = System::Drawing::Size(214, 22);
			this->ReActionAnimation_ToolStripMenuItem->Text = L"리액션 애니메이션 설정";
			this->ReActionAnimation_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &AnimationTabPage::ReActionAnimation_ToolStripMenuItem_Click);
			// 
			// AnimationBlendTableToolStripMenuItem
			// 
			this->AnimationBlendTableToolStripMenuItem->Name = L"AnimationBlendTableToolStripMenuItem";
			this->AnimationBlendTableToolStripMenuItem->Size = System::Drawing::Size(214, 22);
			this->AnimationBlendTableToolStripMenuItem->Text = L"애니메이션 블렌딩 테이블";
			this->AnimationBlendTableToolStripMenuItem->Click += gcnew System::EventHandler(this, &AnimationTabPage::AnimationBlendTableToolStripMenuItem_Click);
			// 
			// OverlayAnimation_ToolStripMenuItem
			// 
			this->OverlayAnimation_ToolStripMenuItem->Name = L"OverlayAnimation_ToolStripMenuItem";
			this->OverlayAnimation_ToolStripMenuItem->Size = System::Drawing::Size(214, 22);
			this->OverlayAnimation_ToolStripMenuItem->Text = L"오버레이 애니메이션 설정";
			this->OverlayAnimation_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &AnimationTabPage::OverlayAnimation_ToolStripMenuItem_Click);
			// 
			// toolStripSeparator11
			// 
			this->toolStripSeparator11->Name = L"toolStripSeparator11";
			this->toolStripSeparator11->Size = System::Drawing::Size(211, 6);
			// 
			// aninamecopytoolStripMenuItem
			// 
			this->aninamecopytoolStripMenuItem->Name = L"aninamecopytoolStripMenuItem";
			this->aninamecopytoolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::C));
			this->aninamecopytoolStripMenuItem->Size = System::Drawing::Size(214, 22);
			this->aninamecopytoolStripMenuItem->Text = L"이름복사";
			this->aninamecopytoolStripMenuItem->Click += gcnew System::EventHandler(this, &AnimationTabPage::aninamecopytoolStripMenuItem_Click);
			// 
			// Animation_tabControl
			// 
			this->Animation_tabControl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->Animation_tabControl->Controls->Add(this->AnimationEvent_tabPage);
			this->Animation_tabControl->Controls->Add(this->AnimationLinkTest_tabPage);
			this->Animation_tabControl->Controls->Add(this->OverlayAnimation_tabPage);
			this->Animation_tabControl->Controls->Add(this->HitSound_tabPage);
			this->Animation_tabControl->Location = System::Drawing::Point(4, 7);
			this->Animation_tabControl->Name = L"Animation_tabControl";
			this->Animation_tabControl->SelectedIndex = 0;
			this->Animation_tabControl->Size = System::Drawing::Size(251, 182);
			this->Animation_tabControl->TabIndex = 10;
			this->Animation_tabControl->SelectedIndexChanged += gcnew System::EventHandler(this, &AnimationTabPage::Animation_tabControl_SelectedIndexChanged);
			// 
			// AnimationEvent_tabPage
			// 
			this->AnimationEvent_tabPage->Location = System::Drawing::Point(4, 22);
			this->AnimationEvent_tabPage->Name = L"AnimationEvent_tabPage";
			this->AnimationEvent_tabPage->Size = System::Drawing::Size(243, 156);
			this->AnimationEvent_tabPage->TabIndex = 0;
			this->AnimationEvent_tabPage->Text = L"이벤트";
			this->AnimationEvent_tabPage->UseVisualStyleBackColor = true;
			// 
			// AnimationLinkTest_tabPage
			// 
			this->AnimationLinkTest_tabPage->Location = System::Drawing::Point(4, 22);
			this->AnimationLinkTest_tabPage->Name = L"AnimationLinkTest_tabPage";
			this->AnimationLinkTest_tabPage->Size = System::Drawing::Size(243, 156);
			this->AnimationLinkTest_tabPage->TabIndex = 1;
			this->AnimationLinkTest_tabPage->Text = L"애니메이션 테스트";
			this->AnimationLinkTest_tabPage->UseVisualStyleBackColor = true;
			// 
			// OverlayAnimation_tabPage
			// 
			this->OverlayAnimation_tabPage->Location = System::Drawing::Point(4, 22);
			this->OverlayAnimation_tabPage->Name = L"OverlayAnimation_tabPage";
			this->OverlayAnimation_tabPage->Size = System::Drawing::Size(243, 156);
			this->OverlayAnimation_tabPage->TabIndex = 2;
			this->OverlayAnimation_tabPage->Text = L"상,하체 애니메이션 테스트";
			this->OverlayAnimation_tabPage->UseVisualStyleBackColor = true;
			// 
			// HitSound_tabPage
			// 
			this->HitSound_tabPage->Location = System::Drawing::Point(4, 22);
			this->HitSound_tabPage->Name = L"HitSound_tabPage";
			this->HitSound_tabPage->Size = System::Drawing::Size(243, 156);
			this->HitSound_tabPage->TabIndex = 3;
			this->HitSound_tabPage->Text = L"히트사운드";
			this->HitSound_tabPage->UseVisualStyleBackColor = true;
			// 
			// AnimationSplitContainer
			// 
			this->AnimationSplitContainer->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->AnimationSplitContainer->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->AnimationSplitContainer->Location = System::Drawing::Point(2, 1);
			this->AnimationSplitContainer->Name = L"AnimationSplitContainer";
			this->AnimationSplitContainer->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// AnimationSplitContainer.Panel1
			// 
			this->AnimationSplitContainer->Panel1->Controls->Add(this->groupBox11);
			this->AnimationSplitContainer->Panel1->Controls->Add(this->AnitreeView);
			// 
			// AnimationSplitContainer.Panel2
			// 
			this->AnimationSplitContainer->Panel2->Controls->Add(this->ErrorViewer_panel);
			this->AnimationSplitContainer->Size = System::Drawing::Size(260, 520);
			this->AnimationSplitContainer->SplitterDistance = 325;
			this->AnimationSplitContainer->TabIndex = 11;
			// 
			// ErrorViewer_panel
			// 
			this->ErrorViewer_panel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->ErrorViewer_panel->Controls->Add(this->Animation_tabControl);
			this->ErrorViewer_panel->Location = System::Drawing::Point(0, -3);
			this->ErrorViewer_panel->Name = L"ErrorViewer_panel";
			this->ErrorViewer_panel->Size = System::Drawing::Size(259, 232);
			this->ErrorViewer_panel->TabIndex = 11;
			// 
			// AnimationTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(262, 524);
			this->Controls->Add(this->AnimationSplitContainer);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"AnimationTabPage";
			this->Text = L"AnimationTabPage";
			this->Load += gcnew System::EventHandler(this, &AnimationTabPage::AnimationTabPage_Load);
			this->groupBox11->ResumeLayout(false);
			this->groupBox11->PerformLayout();
			this->contextMenuStrip1->ResumeLayout(false);
			this->Animation_tabControl->ResumeLayout(false);
			this->AnimationSplitContainer->Panel1->ResumeLayout(false);
			this->AnimationSplitContainer->Panel2->ResumeLayout(false);
			this->AnimationSplitContainer->ResumeLayout(false);
			this->ErrorViewer_panel->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void AnitreeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
					 //최상위노드이면 리턴
					 if( e->Node->Parent == nullptr ) return;

					 //트리뷰에서 애니메이션을 선택하면...
					 if(g_pMainApp->GetActor() == NULL) return;

					 g_pMainApp->AniTreeView_AnimationPlay(e->Node->Text);
					 g_pMainApp->m_TabAnimation.SetAniTreeViewName(e->Node->Text);
				 }
private: System::Void AnitreeView_NodeMouseDoubleClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
			 //최상위노드이면 리턴
			 if( e->Node->Parent == nullptr ) return;

			 String^ str = e->Node->Text;
			 RAnimation* pAnimation = g_pMainApp->m_TabAnimation.GetAnimation(str);
			 if( pAnimation )
			 {
				 g_pMainApp->GetActor()->InitMeshNodeNoiseRef();
				 g_pMainApp->GetActor()->StopAnimation();

				 EditAnimation pEdit;
				 pEdit.NametextBox->Text	= gcnew String(pAnimation->GetAliasName().c_str() );
				 pEdit.FileNametextBox->Text= gcnew String(pAnimation->GetResource()->GetName().c_str() );
				 pEdit.LoopcheckBox->Checked= (pAnimation->GetAnimationLoopType()==RAniLoopType_Loop)?true:false;
				 pEdit.ShowDialog();
				 System::Windows::Forms::DialogResult res = pEdit.DialogResult;
				 if (res == System::Windows::Forms::DialogResult::OK)
				 {
					 // TODO : 이벤트등을 계승시켜줘야 하나...

					 //지우고
					 g_pMainApp->GetActor()->m_pMesh->m_AniMgr.Delete(pAnimation->GetAliasName().c_str());
					 pAnimation = NULL;

					 const char* pAniName = MStringToCharPointer(pEdit.NametextBox->Text);
					 const char* pAniFileName = MStringToCharPointer(pEdit.FileNametextBox->Text);

					 //새로운이름으로 등록
					 pAnimation = g_pMainApp->GetActor()->m_pMesh->m_AniMgr.Add(pAniName, pAniFileName);
					 pAnimation->GetResource()->BuildAll();

					 pAnimation->SetAnimationLoopType( (pEdit.LoopcheckBox->Checked==true)?RAniLoopType_Loop:RAniLoopType_HoldLastFrame );

					 g_pMainApp->m_TabAnimation.InitAnitreeView();
					 AnitreeView->SelectedNode = g_pMainApp->SearchNode(AnitreeView->Nodes, pEdit.NametextBox->Text);				

					 //선택된 애니메이션 다시 적용
					 g_pMainApp->AniTreeView_AnimationPlay(pAniName);

					 MFreeCharPointer(pAniFileName);
					 MFreeCharPointer(pAniName);
				 }
			 }
		 }
private: System::Void LoopcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void AniDeltoolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_TabAnimation.DeleteAnimation();
		 }
private: System::Void aninamecopytoolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 Clipboard::SetText(AnitreeView->SelectedNode->Text);
		 }
private: System::Void AnitreeView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void AnitreeView_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void AnitreeView_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		 public: void SetCurrentAnimationLinkListIndex(int nIndex);
private: System::Void AnimationTabPage_Load(System::Object^  sender, System::EventArgs^  e) {
		 }
		 public: void SetPauseButtonDown_AnimationLinkTest();
		 public: void SetPlayButtonDown_AnimationLinkTest();

private: System::Void AnimationBlendTableToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void OverlayAnimation_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ReActionAnimation_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

private: System::Void Animation_tabControl_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);

		 public: LINK_TEST_STATE GetTestState() { return m_AnimationLinkTestTabPage->GetTestState(); }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->ReActionAnimationEditTool();
		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->AnimationBlendEditTool();
		 }
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->OvelayAnimationEditTool();
		 }
private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->ReActionAnimationNomalTest();
		 }
private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->ReActionAnimationBossTest();
		 }
};
}
