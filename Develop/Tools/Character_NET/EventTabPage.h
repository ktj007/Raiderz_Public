#pragma once

#include "TTalentEventController.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// EventTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class EventTabPage : public System::Windows::Forms::Form
	{
	public:
		EventTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
			m_TalentEventController = gcnew TTalentEventController(this);
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~EventTabPage()
		{
			if (components)
			{
				delete components;
			}
		}
	public: TTalentEventController^ m_TalentEventController;
	public: System::Windows::Forms::Button^  TalentEventDeleteBtn;
	public: System::Windows::Forms::Button^  TalentEventInsertBtn;

	public: System::Windows::Forms::Button^  TalentEventCopy;
	public: System::Windows::Forms::Button^  TalentEventPaste;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	public: 
	private: System::Windows::Forms::Label^  label9;

	private: 
	private: System::Windows::Forms::Label^  label17;
	public: 
	public: System::Windows::Forms::TextBox^  EventRefTextBox;
	private: 
	public: System::Windows::Forms::Button^  EventRefSelectBtn;
	public: System::Windows::Forms::PropertyGrid^  TalentEventAtt;
	private: System::Windows::Forms::GroupBox^  groupBox9;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	public: System::Windows::Forms::TextBox^  CurrentTalentID2;
	public: ::TreeViewMS::TreeViewMS^  TalentEventTreeView;
	public: System::Windows::Forms::Button^  Deref_button;

	public: 

	public: 
	private: 
	public: 

	public: 

	private: 


	public: 

	private: 

	public: 


	private: 





	public: 

	private: 

	protected: 


	public: 

	private: 

	public: 


	private: 





	public: 


	public: 
	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(EventTabPage::typeid));
			this->TalentEventDeleteBtn = (gcnew System::Windows::Forms::Button());
			this->TalentEventInsertBtn = (gcnew System::Windows::Forms::Button());
			this->TalentEventCopy = (gcnew System::Windows::Forms::Button());
			this->TalentEventPaste = (gcnew System::Windows::Forms::Button());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->TalentEventTreeView = (gcnew ::TreeViewMS::TreeViewMS());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->EventRefTextBox = (gcnew System::Windows::Forms::TextBox());
			this->EventRefSelectBtn = (gcnew System::Windows::Forms::Button());
			this->TalentEventAtt = (gcnew System::Windows::Forms::PropertyGrid());
			this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->Deref_button = (gcnew System::Windows::Forms::Button());
			this->CurrentTalentID2 = (gcnew System::Windows::Forms::TextBox());
			this->groupBox6->SuspendLayout();
			this->groupBox9->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// TalentEventDeleteBtn
			// 
			this->TalentEventDeleteBtn->Location = System::Drawing::Point(78, 16);
			this->TalentEventDeleteBtn->Name = L"TalentEventDeleteBtn";
			this->TalentEventDeleteBtn->Size = System::Drawing::Size(46, 23);
			this->TalentEventDeleteBtn->TabIndex = 16;
			this->TalentEventDeleteBtn->Text = L"삭제";
			this->TalentEventDeleteBtn->UseVisualStyleBackColor = true;
			this->TalentEventDeleteBtn->Click += gcnew System::EventHandler(this, &EventTabPage::TalentEventDeleteBtn_Click);
			// 
			// TalentEventInsertBtn
			// 
			this->TalentEventInsertBtn->Location = System::Drawing::Point(26, 16);
			this->TalentEventInsertBtn->Name = L"TalentEventInsertBtn";
			this->TalentEventInsertBtn->Size = System::Drawing::Size(46, 23);
			this->TalentEventInsertBtn->TabIndex = 15;
			this->TalentEventInsertBtn->Text = L"추가";
			this->TalentEventInsertBtn->UseVisualStyleBackColor = true;
			this->TalentEventInsertBtn->Click += gcnew System::EventHandler(this, &EventTabPage::TalentEventInsertBtn_Click);
			// 
			// TalentEventCopy
			// 
			this->TalentEventCopy->Location = System::Drawing::Point(130, 16);
			this->TalentEventCopy->Name = L"TalentEventCopy";
			this->TalentEventCopy->Size = System::Drawing::Size(46, 23);
			this->TalentEventCopy->TabIndex = 18;
			this->TalentEventCopy->Text = L"복사";
			this->TalentEventCopy->UseVisualStyleBackColor = true;
			this->TalentEventCopy->Click += gcnew System::EventHandler(this, &EventTabPage::TalentEventCopy_Click);
			// 
			// TalentEventPaste
			// 
			this->TalentEventPaste->Location = System::Drawing::Point(183, 16);
			this->TalentEventPaste->Name = L"TalentEventPaste";
			this->TalentEventPaste->Size = System::Drawing::Size(46, 23);
			this->TalentEventPaste->TabIndex = 19;
			this->TalentEventPaste->Text = L"붙여넣기";
			this->TalentEventPaste->UseVisualStyleBackColor = true;
			this->TalentEventPaste->Click += gcnew System::EventHandler(this, &EventTabPage::TalentEventPaste_Click);
			// 
			// groupBox6
			// 
			this->groupBox6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox6->Controls->Add(this->TalentEventTreeView);
			this->groupBox6->Controls->Add(this->TalentEventPaste);
			this->groupBox6->Controls->Add(this->TalentEventCopy);
			this->groupBox6->Controls->Add(this->TalentEventInsertBtn);
			this->groupBox6->Controls->Add(this->TalentEventDeleteBtn);
			this->groupBox6->Location = System::Drawing::Point(3, 72);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(251, 185);
			this->groupBox6->TabIndex = 26;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"탤런트 이벤트";
			// 
			// TalentEventTreeView
			// 
			this->TalentEventTreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TalentEventTreeView->Location = System::Drawing::Point(6, 45);
			this->TalentEventTreeView->Name = L"TalentEventTreeView";
			this->TalentEventTreeView->SelectedNodes = (cli::safe_cast<System::Collections::ArrayList^  >(resources->GetObject(L"TalentEventTreeView.SelectedNodes")));
			this->TalentEventTreeView->Size = System::Drawing::Size(239, 134);
			this->TalentEventTreeView->TabIndex = 20;
			this->TalentEventTreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &EventTabPage::TalentEventTreeView_AfterSelect);
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(12, 13);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(55, 12);
			this->label9->TabIndex = 24;
			this->label9->Text = L"Talent ID";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(29, 39);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(38, 12);
			this->label17->TabIndex = 28;
			this->label17->Text = L"Ref ID";
			// 
			// EventRefTextBox
			// 
			this->EventRefTextBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->EventRefTextBox->ForeColor = System::Drawing::Color::DarkRed;
			this->EventRefTextBox->Location = System::Drawing::Point(73, 34);
			this->EventRefTextBox->Name = L"EventRefTextBox";
			this->EventRefTextBox->ReadOnly = true;
			this->EventRefTextBox->Size = System::Drawing::Size(82, 21);
			this->EventRefTextBox->TabIndex = 29;
			// 
			// EventRefSelectBtn
			// 
			this->EventRefSelectBtn->Location = System::Drawing::Point(158, 34);
			this->EventRefSelectBtn->Name = L"EventRefSelectBtn";
			this->EventRefSelectBtn->Size = System::Drawing::Size(25, 21);
			this->EventRefSelectBtn->TabIndex = 30;
			this->EventRefSelectBtn->Text = L"...";
			this->EventRefSelectBtn->UseVisualStyleBackColor = true;
			this->EventRefSelectBtn->Click += gcnew System::EventHandler(this, &EventTabPage::EventRefSelectBtn_Click);
			// 
			// TalentEventAtt
			// 
			this->TalentEventAtt->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TalentEventAtt->Location = System::Drawing::Point(6, 18);
			this->TalentEventAtt->Name = L"TalentEventAtt";
			this->TalentEventAtt->PropertySort = System::Windows::Forms::PropertySort::Categorized;
			this->TalentEventAtt->Size = System::Drawing::Size(243, 226);
			this->TalentEventAtt->TabIndex = 18;
			this->TalentEventAtt->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &EventTabPage::TalentEventAtt_PropertyValueChanged);
			// 
			// groupBox9
			// 
			this->groupBox9->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox9->Controls->Add(this->TalentEventAtt);
			this->groupBox9->Location = System::Drawing::Point(3, 3);
			this->groupBox9->Name = L"groupBox9";
			this->groupBox9->Size = System::Drawing::Size(251, 250);
			this->groupBox9->TabIndex = 27;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = L"탤런트 이벤트 속성";
			// 
			// splitContainer1
			// 
			this->splitContainer1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->Deref_button);
			this->splitContainer1->Panel1->Controls->Add(this->CurrentTalentID2);
			this->splitContainer1->Panel1->Controls->Add(this->groupBox6);
			this->splitContainer1->Panel1->Controls->Add(this->EventRefSelectBtn);
			this->splitContainer1->Panel1->Controls->Add(this->label9);
			this->splitContainer1->Panel1->Controls->Add(this->EventRefTextBox);
			this->splitContainer1->Panel1->Controls->Add(this->label17);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->groupBox9);
			this->splitContainer1->Size = System::Drawing::Size(262, 524);
			this->splitContainer1->SplitterDistance = 262;
			this->splitContainer1->TabIndex = 31;
			// 
			// Deref_button
			// 
			this->Deref_button->Enabled = false;
			this->Deref_button->Location = System::Drawing::Point(158, 9);
			this->Deref_button->Name = L"Deref_button";
			this->Deref_button->Size = System::Drawing::Size(49, 21);
			this->Deref_button->TabIndex = 32;
			this->Deref_button->Text = L"역참조";
			this->Deref_button->UseVisualStyleBackColor = true;
			this->Deref_button->Click += gcnew System::EventHandler(this, &EventTabPage::Deref_button_Click);
			// 
			// CurrentTalentID2
			// 
			this->CurrentTalentID2->BackColor = System::Drawing::SystemColors::Control;
			this->CurrentTalentID2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->CurrentTalentID2->Location = System::Drawing::Point(73, 9);
			this->CurrentTalentID2->Name = L"CurrentTalentID2";
			this->CurrentTalentID2->ReadOnly = true;
			this->CurrentTalentID2->Size = System::Drawing::Size(82, 21);
			this->CurrentTalentID2->TabIndex = 31;
			this->CurrentTalentID2->DoubleClick += gcnew System::EventHandler(this, &EventTabPage::CurrentTalentID2_DoubleClick);
			// 
			// EventTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(262, 524);
			this->Controls->Add(this->splitContainer1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"EventTabPage";
			this->Text = L"EventTabPage";
			this->groupBox6->ResumeLayout(false);
			this->groupBox9->ResumeLayout(false);
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel1->PerformLayout();
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void CurrentTalentID2_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
			 CharacterToolForms::TextOneBox AddEventIDTextBox;

			 AddEventIDTextBox.TitleLabel->Text = gcnew String("탤런트 ID");
			 AddEventIDTextBox.ShowDialog();

			 System::Windows::Forms::DialogResult res = AddEventIDTextBox.DialogResult;
			 if (res == System::Windows::Forms::DialogResult::OK)
			 {
				 int nTalentID = atoi(MStringToCharPointer(AddEventIDTextBox.TextValue->Text));

				 CSTalentInfo* pTalentInfo = TTalentHelper::GetTalentInfo(nTalentID, 0);
				 // 이벤트 정보 보여주기
				 m_TalentEventController->SetTalentEvent(pTalentInfo);
			 }
		 }
public: System::Void EventRefSelectBtn_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEventController->OnTalentEventRefSelect(sender, e);
		 }
private: System::Void TalentEventInsertBtn_Click(System::Object^  sender, System::EventArgs^  e) {
			 // 탤런트가 선택이 되었는가?
			 if(m_TalentEventController->IsExistenceTalentEvent() == false)
			 {
				 // 아무것도 없다.
				 System::Windows::Forms::MessageBox::Show(L"탤런트를 선택하세요!", "에러");
				 return;
			 }

			 // 이벤트 트리 뷰어에 선택이 되어 있는가?
			 TreeNode^ tn = TalentEventTreeView->SelectedNode;

			 int nStartIndex;

			 // 이벤트 트리 뷰어에 선택이 없다.
			 // 창을 띄워서 선택을 하게 한다.
			 nStartIndex = 0;
			 if(tn == nullptr)
			 {
				 System::Windows::Forms::MessageBox::Show(L"이벤트 종류를 선택하세요(Normal, Act)!", "에러");
				 return;
			 }

			 // 자식인가?
			 if(tn->Parent != nullptr)
			 {
				 tn = tn->Parent;
			 }

			 // 이벤트 트리 뷰어에 선택이 되어 있다면...
			 if(tn->Text == "Normal")
			 {
				 nStartIndex = 0;
			 }
			 else if(tn->Text == "Act")
			 {
				 nStartIndex = 1;
			 }

			 // 정보 생성
			 m_TalentEventController->CreateTalentEvent(nStartIndex);
		 }
private: System::Void TalentEventDeleteBtn_Click(System::Object^  sender, System::EventArgs^  e) {
			 // 탤런트가 선택이 되었는가?
			 if(m_TalentEventController->IsExistenceTalentEvent() == false)
			 {
				 // 아무것도 없다.
				 System::Windows::Forms::MessageBox::Show(L"탤런트를 선택하세요!", "에러");
				 return;
			 }

			 m_TalentEventController->DeleteTalentEvent();
		 }
private: System::Void TalentEventCopy_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEventController->OnTalentEventCopy();
		 }
private: System::Void TalentEventPaste_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEventController->OnTalentEventPasete();
		 }
private: System::Void TalentEventTreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 // 탤런트가 선택이 되었는가?
			 if(m_TalentEventController->IsExistenceTalentEvent() == false)
			 {
				 // 아무것도 없다.
				 System::Windows::Forms::MessageBox::Show(L"탤런트를 선택하세요!", "에러");
				 return;
			 }

			 //g_pMainApp->SetPropertyTalentEvent(false);
			 int nEventType = m_TalentEventController->GetCurrentTalentEventType();
			 if(nEventType >= 0)
			 {
				 m_TalentEventController->SetCurrentPropertyTalentEvent(true);
			 }
			 else
			 {
				 TalentEventAtt->SelectedObject = nullptr;
			 }
		 }
private: System::Void TalentEventAtt_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e) {
			 // 탤런트가 선택이 되었는가?
			 if(m_TalentEventController->IsExistenceTalentEvent() == false)
			 {
				 // 아무것도 없다.
				 System::Windows::Forms::MessageBox::Show(L"탤런트를 선택하세요!", "에러");
				 return;
			 }

			 m_TalentEventController->SetCurrentPropertyTalentEvent(false);

			 m_TalentEventController->CheckSegmentIndexCountAndCapsuleIndexCount();
		 }
private: System::Void Deref_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEventController->OnDeDereferenceTalentEffectIDViewer();
		 }
};
}
