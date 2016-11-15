#pragma once

#include "TTalentHitTabController.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// TalentTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class TalentTabPage : public System::Windows::Forms::Form
	{
	public:
		TalentTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
			m_TalentHitTabController = new TTalentHitTabController();
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~TalentTabPage()
		{
			if (components)
			{
				delete components;
			}
			if (m_TalentHitTabController)
			{
				delete m_TalentHitTabController;
			}
		}
	public: TTalentHitTabController* m_TalentHitTabController;
	public: System::Windows::Forms::PropertyGrid^  Capsule_propertyGrid;
	protected: 
	public: System::Windows::Forms::GroupBox^  groupBox8;
	private: System::Windows::Forms::Label^  label8;
	public: 
	public: System::Windows::Forms::TextBox^  TalentIDtextbox;
	private: 
	public: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::Label^  label11;
	public: 
	private: System::Windows::Forms::Button^  btnHitReference;
	public: System::Windows::Forms::TextBox^  tbHitReference;
	private: 
	public: System::Windows::Forms::Button^  CapsulePaste;
	public: System::Windows::Forms::Button^  AddCapsule;
	public: System::Windows::Forms::Button^  CapsuleCopy;

	public: System::Windows::Forms::Button^  AddSegment;
	public: System::Windows::Forms::Button^  DeleteSegment;
	private: System::Windows::Forms::CheckBox^  AniSync_checkBox;
	private: System::Windows::Forms::CheckBox^  AllView_checkBox;
	public: System::Windows::Forms::Button^  Deref_button;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	public: 
	public: ::TreeViewMS::TreeViewMS^  HitSegmenttreeView;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(TalentTabPage::typeid));
			this->Capsule_propertyGrid = (gcnew System::Windows::Forms::PropertyGrid());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->TalentIDtextbox = (gcnew System::Windows::Forms::TextBox());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->HitSegmenttreeView = (gcnew ::TreeViewMS::TreeViewMS());
			this->Deref_button = (gcnew System::Windows::Forms::Button());
			this->AllView_checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->AniSync_checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->btnHitReference = (gcnew System::Windows::Forms::Button());
			this->tbHitReference = (gcnew System::Windows::Forms::TextBox());
			this->CapsulePaste = (gcnew System::Windows::Forms::Button());
			this->AddCapsule = (gcnew System::Windows::Forms::Button());
			this->CapsuleCopy = (gcnew System::Windows::Forms::Button());
			this->AddSegment = (gcnew System::Windows::Forms::Button());
			this->DeleteSegment = (gcnew System::Windows::Forms::Button());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->groupBox8->SuspendLayout();
			this->groupBox7->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// Capsule_propertyGrid
			// 
			this->Capsule_propertyGrid->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->Capsule_propertyGrid->Location = System::Drawing::Point(3, 3);
			this->Capsule_propertyGrid->Name = L"Capsule_propertyGrid";
			this->Capsule_propertyGrid->Size = System::Drawing::Size(248, 270);
			this->Capsule_propertyGrid->TabIndex = 5;
			this->Capsule_propertyGrid->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &TalentTabPage::Capsule_propertyGrid_PropertyValueChanged);
			// 
			// groupBox8
			// 
			this->groupBox8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox8->Controls->Add(this->label8);
			this->groupBox8->Controls->Add(this->TalentIDtextbox);
			this->groupBox8->Location = System::Drawing::Point(4, 3);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(247, 45);
			this->groupBox8->TabIndex = 6;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = L"탈렌트 정보";
			this->groupBox8->Visible = false;
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(15, 17);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(64, 12);
			this->label8->TabIndex = 11;
			this->label8->Text = L"탈렌트 ID :";
			// 
			// TalentIDtextbox
			// 
			this->TalentIDtextbox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->TalentIDtextbox->Location = System::Drawing::Point(82, 14);
			this->TalentIDtextbox->Name = L"TalentIDtextbox";
			this->TalentIDtextbox->ReadOnly = true;
			this->TalentIDtextbox->Size = System::Drawing::Size(118, 21);
			this->TalentIDtextbox->TabIndex = 10;
			// 
			// groupBox7
			// 
			this->groupBox7->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox7->Controls->Add(this->HitSegmenttreeView);
			this->groupBox7->Controls->Add(this->Deref_button);
			this->groupBox7->Controls->Add(this->AllView_checkBox);
			this->groupBox7->Controls->Add(this->AniSync_checkBox);
			this->groupBox7->Controls->Add(this->label11);
			this->groupBox7->Controls->Add(this->btnHitReference);
			this->groupBox7->Controls->Add(this->tbHitReference);
			this->groupBox7->Controls->Add(this->CapsulePaste);
			this->groupBox7->Controls->Add(this->AddCapsule);
			this->groupBox7->Controls->Add(this->CapsuleCopy);
			this->groupBox7->Controls->Add(this->AddSegment);
			this->groupBox7->Controls->Add(this->DeleteSegment);
			this->groupBox7->Location = System::Drawing::Point(3, 54);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(248, 182);
			this->groupBox7->TabIndex = 4;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = L"판정 세그먼트";
			this->groupBox7->Visible = false;
			// 
			// HitSegmenttreeView
			// 
			this->HitSegmenttreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->HitSegmenttreeView->Location = System::Drawing::Point(4, 56);
			this->HitSegmenttreeView->Name = L"HitSegmenttreeView";
			this->HitSegmenttreeView->SelectedNodes = (cli::safe_cast<System::Collections::ArrayList^  >(resources->GetObject(L"HitSegmenttreeView.SelectedNodes")));
			this->HitSegmenttreeView->Size = System::Drawing::Size(238, 71);
			this->HitSegmenttreeView->TabIndex = 34;
			this->HitSegmenttreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &TalentTabPage::HitSegmenttreeView_AfterSelect);
			this->HitSegmenttreeView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &TalentTabPage::HitSegmenttreeView_KeyDown);
			// 
			// Deref_button
			// 
			this->Deref_button->Enabled = false;
			this->Deref_button->Location = System::Drawing::Point(191, 21);
			this->Deref_button->Name = L"Deref_button";
			this->Deref_button->Size = System::Drawing::Size(49, 21);
			this->Deref_button->TabIndex = 33;
			this->Deref_button->Text = L"역참조";
			this->Deref_button->UseVisualStyleBackColor = true;
			this->Deref_button->Click += gcnew System::EventHandler(this, &TalentTabPage::Deref_button_Click);
			// 
			// AllView_checkBox
			// 
			this->AllView_checkBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AllView_checkBox->AutoSize = true;
			this->AllView_checkBox->Location = System::Drawing::Point(180, 137);
			this->AllView_checkBox->Name = L"AllView_checkBox";
			this->AllView_checkBox->Size = System::Drawing::Size(70, 16);
			this->AllView_checkBox->TabIndex = 27;
			this->AllView_checkBox->Text = L"All View";
			this->AllView_checkBox->UseVisualStyleBackColor = true;
			this->AllView_checkBox->CheckedChanged += gcnew System::EventHandler(this, &TalentTabPage::AllView_checkBox_CheckedChanged);
			// 
			// AniSync_checkBox
			// 
			this->AniSync_checkBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AniSync_checkBox->AutoSize = true;
			this->AniSync_checkBox->Location = System::Drawing::Point(156, 160);
			this->AniSync_checkBox->Name = L"AniSync_checkBox";
			this->AniSync_checkBox->Size = System::Drawing::Size(84, 16);
			this->AniSync_checkBox->TabIndex = 26;
			this->AniSync_checkBox->Text = L"애니동기화";
			this->AniSync_checkBox->UseVisualStyleBackColor = true;
			this->AniSync_checkBox->CheckedChanged += gcnew System::EventHandler(this, &TalentTabPage::AniSync_checkBox_CheckedChanged);
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(10, 25);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(23, 12);
			this->label11->TabIndex = 25;
			this->label11->Text = L"Ref";
			// 
			// btnHitReference
			// 
			this->btnHitReference->Location = System::Drawing::Point(154, 21);
			this->btnHitReference->Name = L"btnHitReference";
			this->btnHitReference->Size = System::Drawing::Size(31, 21);
			this->btnHitReference->TabIndex = 24;
			this->btnHitReference->Text = L"...";
			this->btnHitReference->UseVisualStyleBackColor = true;
			this->btnHitReference->Click += gcnew System::EventHandler(this, &TalentTabPage::btnHitReference_Click);
			// 
			// tbHitReference
			// 
			this->tbHitReference->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbHitReference->ForeColor = System::Drawing::Color::DarkRed;
			this->tbHitReference->Location = System::Drawing::Point(40, 21);
			this->tbHitReference->Name = L"tbHitReference";
			this->tbHitReference->ReadOnly = true;
			this->tbHitReference->Size = System::Drawing::Size(107, 21);
			this->tbHitReference->TabIndex = 23;
			// 
			// CapsulePaste
			// 
			this->CapsulePaste->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->CapsulePaste->Location = System::Drawing::Point(82, 157);
			this->CapsulePaste->Name = L"CapsulePaste";
			this->CapsulePaste->Size = System::Drawing::Size(72, 20);
			this->CapsulePaste->TabIndex = 20;
			this->CapsulePaste->Text = L"붙여넣기";
			this->CapsulePaste->UseVisualStyleBackColor = true;
			this->CapsulePaste->Click += gcnew System::EventHandler(this, &TalentTabPage::CapsulePaste_Click);
			// 
			// AddCapsule
			// 
			this->AddCapsule->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AddCapsule->Location = System::Drawing::Point(65, 134);
			this->AddCapsule->Name = L"AddCapsule";
			this->AddCapsule->Size = System::Drawing::Size(61, 20);
			this->AddCapsule->TabIndex = 17;
			this->AddCapsule->Text = L"캡슐추가";
			this->AddCapsule->UseVisualStyleBackColor = true;
			this->AddCapsule->Click += gcnew System::EventHandler(this, &TalentTabPage::AddCapsule_Click);
			// 
			// CapsuleCopy
			// 
			this->CapsuleCopy->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->CapsuleCopy->Location = System::Drawing::Point(4, 157);
			this->CapsuleCopy->Name = L"CapsuleCopy";
			this->CapsuleCopy->Size = System::Drawing::Size(72, 20);
			this->CapsuleCopy->TabIndex = 20;
			this->CapsuleCopy->Text = L"복사";
			this->CapsuleCopy->UseVisualStyleBackColor = true;
			this->CapsuleCopy->Click += gcnew System::EventHandler(this, &TalentTabPage::CapsuleCopy_Click);
			// 
			// AddSegment
			// 
			this->AddSegment->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AddSegment->Location = System::Drawing::Point(1, 134);
			this->AddSegment->Name = L"AddSegment";
			this->AddSegment->Size = System::Drawing::Size(61, 20);
			this->AddSegment->TabIndex = 14;
			this->AddSegment->Text = L"세그추가";
			this->AddSegment->UseVisualStyleBackColor = true;
			this->AddSegment->Click += gcnew System::EventHandler(this, &TalentTabPage::AddSegment_Click);
			// 
			// DeleteSegment
			// 
			this->DeleteSegment->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->DeleteSegment->Location = System::Drawing::Point(129, 134);
			this->DeleteSegment->Name = L"DeleteSegment";
			this->DeleteSegment->Size = System::Drawing::Size(49, 20);
			this->DeleteSegment->TabIndex = 15;
			this->DeleteSegment->Text = L"삭제";
			this->DeleteSegment->UseVisualStyleBackColor = true;
			this->DeleteSegment->Click += gcnew System::EventHandler(this, &TalentTabPage::DeleteSegment_Click);
			// 
			// splitContainer1
			// 
			this->splitContainer1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer1->Location = System::Drawing::Point(5, 3);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->groupBox8);
			this->splitContainer1->Panel1->Controls->Add(this->groupBox7);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->Capsule_propertyGrid);
			this->splitContainer1->Size = System::Drawing::Size(254, 519);
			this->splitContainer1->SplitterDistance = 239;
			this->splitContainer1->TabIndex = 7;
			// 
			// TalentTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(262, 524);
			this->Controls->Add(this->splitContainer1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"TalentTabPage";
			this->Text = L"TalentTabPage";
			this->groupBox8->ResumeLayout(false);
			this->groupBox8->PerformLayout();
			this->groupBox7->ResumeLayout(false);
			this->groupBox7->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void btnHitReference_Click(System::Object^  sender, System::EventArgs^  e) {
			 CharacterToolForms::TalentEffectRegisterEdit^ regEdit = gcnew CharacterToolForms::TalentEffectRegisterEdit();
			 regEdit->TalentModecomboBox->Items->Clear();
			 for (int i = 0; i < WEAPON_MAX; ++i)
			 {
				 string strWeapon = TStrings::WeaponType(i);
				 String^ str = String::Format("{0}", gcnew String(strWeapon.c_str()));
				 regEdit->TalentModecomboBox->Items->Add(str);
			 }

			 int nTalentID = atoi(MStringToCharPointer(tbHitReference->Text));
			 regEdit->SetText(nTalentID, 0);
			 regEdit->ShowDialog();

			 System::Windows::Forms::DialogResult res = regEdit->DialogResult;
			 if (res == System::Windows::Forms::DialogResult::OK)
			 {
				 int nSelectTalentID = 0;
				 int nSelectTalentMode = 0;

				 // 자신 탤런트 번호인가?
				 int nCurrTalentID = atoi(MStringToCharPointer(TalentIDtextbox->Text));
				 if(regEdit->m_nTalentID == nCurrTalentID)
				 {
					System::Windows::Forms::MessageBox::Show(regEdit, L"자신의 탤런트 ID를 입력할 수 없습니다.", "에러");
					return;
				 }

				 // 레퍼런스 탤런트
				 TTalentInfo* refTalent = TTalentHelper::GetTalentInfo(regEdit->m_nTalentID, regEdit->m_nTalentMode);
				 if(refTalent)
				 {
					 nSelectTalentID = regEdit->m_nTalentID;
					 nSelectTalentMode = regEdit->m_nTalentMode;
				 }
				 else
				 {
					 if (regEdit->m_nTalentID != 0
						 || regEdit->m_nTalentMode != 0)
					 {
						 if(System::Windows::Forms::MessageBox::Show(regEdit, L"탤런트가 존재하지 않습니다.", "에러")
							 == System::Windows::Forms::DialogResult::OK)
						 {
							 btnHitReference_Click(sender, e);
						 }

						 return;
					 }
				 }

				 m_TalentHitTabController->OnHitReferenceTextChanged(nSelectTalentID, nSelectTalentMode);
			 }
		 }
private: System::Void HitSegmenttreeView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if (e->Control)
			 {
				 if (e->KeyCode == Keys::C)
				 {
					 m_TalentHitTabController->CopyCapsule();
				 }
				 else if (e->KeyCode == Keys::V)
				 {
					 m_TalentHitTabController->PasteCapsule();
				 }
			 }
		 }
private: System::Void HitSegmenttreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 m_TalentHitTabController->OnHitTreeViewAfterSelect(e);
		 }
private: System::Void AddSegment_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentHitTabController->OnAddSegmentClick();
		 }
private: System::Void AddCapsule_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentHitTabController->OnAddCapsuleClick();
		 }
private: System::Void DeleteSegment_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentHitTabController->OnDeleteSegmentClick();
		 }
private: System::Void CapsuleCopy_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentHitTabController->CopyCapsule();
		 }
private: System::Void CapsulePaste_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentHitTabController->PasteCapsule();
		 }
private: System::Void Capsule_propertyGrid_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e) {
			 m_TalentHitTabController->OnChangeSelSegmentTop();
			 m_TalentHitTabController->SetSelCapsuleProperty();
		 }
private: System::Void AniSync_checkBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentHitTabController->OnAniSync_checkBox_CheckedChanged(AniSync_checkBox->Checked);
		 }
private: System::Void AllView_checkBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentHitTabController->OnHitTreeViewAllSelect(AllView_checkBox->Checked);
		 }
private: System::Void Deref_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentHitTabController->OnDeDereferenceTalentEffectIDViewer();
		 }
};
}
