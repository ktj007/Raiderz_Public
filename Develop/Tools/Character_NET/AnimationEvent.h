#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// AnimationEvent에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class AnimationEvent : public System::Windows::Forms::Form
	{
	public:
		AnimationEvent(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~AnimationEvent()
		{
			if (components)
			{
				delete components;
			}
		}

		void SearchEffectList();

	public: System::Windows::Forms::TabControl^  AnimationEventTab;
	protected: 

	private: System::Windows::Forms::TabPage^  CustomEvent;
	private: System::Windows::Forms::TabPage^  BasicEvent;


	protected: 

	protected: 

	protected: 


	private: System::Windows::Forms::Label^  label1;
	public: System::Windows::Forms::TextBox^  FrametextBox;
	public: System::Windows::Forms::TextBox^  CustomBoneNametextBox;
	public: System::Windows::Forms::TextBox^  CustomParamtextBox;

	public: 


	public: System::Windows::Forms::TextBox^  EffectNameTextBox;
	private: 
	public: 




	private: System::Windows::Forms::Label^  CustomBoneName;
	private: System::Windows::Forms::Label^  CustomParam;
	private: System::Windows::Forms::Label^  CustomEffectName;

	public: System::Windows::Forms::CheckBox^  unconditionalcheckBox;
	public: System::Windows::Forms::ListBox^  CustomEffectNameListBox;

	public: 

	public: 
	private: System::Windows::Forms::Button^  Cancelbutton;
	private: System::Windows::Forms::Button^  Okbutton;
	private: System::Windows::Forms::Label^  EventName;


	public: System::Windows::Forms::TextBox^  Param1textBox;
	private: 
	private: System::Windows::Forms::Button^  searchbutton;
	public: 
	public: System::Windows::Forms::TextBox^  Param2textBox;
	private: 
	private: System::Windows::Forms::Label^  label4;
	public: 
	private: System::Windows::Forms::Label^  label3;
	public: System::Windows::Forms::TextBox^  EventNameTextBox;
	private: 

	private: 

	private: 
	private: System::Windows::Forms::ListBox^  EventNameListBox;
	public: System::Windows::Forms::TextBox^  Param3textBox;
	private: 

	private: 
	private: System::Windows::Forms::Label^  labelParameter3;
	public: System::Windows::Forms::ComboBox^  Param1comboBox;
	private: System::Windows::Forms::CheckBox^  Param2checkBox;
	private: System::Windows::Forms::Button^  search_button;
	private: System::Windows::Forms::TextBox^  search_textBox;

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
			this->AnimationEventTab = (gcnew System::Windows::Forms::TabControl());
			this->CustomEvent = (gcnew System::Windows::Forms::TabPage());
			this->CustomEffectNameListBox = (gcnew System::Windows::Forms::ListBox());
			this->CustomBoneNametextBox = (gcnew System::Windows::Forms::TextBox());
			this->CustomParamtextBox = (gcnew System::Windows::Forms::TextBox());
			this->EffectNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->CustomBoneName = (gcnew System::Windows::Forms::Label());
			this->CustomParam = (gcnew System::Windows::Forms::Label());
			this->CustomEffectName = (gcnew System::Windows::Forms::Label());
			this->BasicEvent = (gcnew System::Windows::Forms::TabPage());
			this->Param2checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->Param1comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->Param3textBox = (gcnew System::Windows::Forms::TextBox());
			this->labelParameter3 = (gcnew System::Windows::Forms::Label());
			this->EventNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->EventNameListBox = (gcnew System::Windows::Forms::ListBox());
			this->Param1textBox = (gcnew System::Windows::Forms::TextBox());
			this->searchbutton = (gcnew System::Windows::Forms::Button());
			this->Param2textBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->EventName = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->FrametextBox = (gcnew System::Windows::Forms::TextBox());
			this->unconditionalcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->Cancelbutton = (gcnew System::Windows::Forms::Button());
			this->Okbutton = (gcnew System::Windows::Forms::Button());
			this->search_textBox = (gcnew System::Windows::Forms::TextBox());
			this->search_button = (gcnew System::Windows::Forms::Button());
			this->AnimationEventTab->SuspendLayout();
			this->CustomEvent->SuspendLayout();
			this->BasicEvent->SuspendLayout();
			this->SuspendLayout();
			// 
			// AnimationEventTab
			// 
			this->AnimationEventTab->Controls->Add(this->CustomEvent);
			this->AnimationEventTab->Controls->Add(this->BasicEvent);
			this->AnimationEventTab->Location = System::Drawing::Point(8, 50);
			this->AnimationEventTab->Name = L"AnimationEventTab";
			this->AnimationEventTab->SelectedIndex = 0;
			this->AnimationEventTab->Size = System::Drawing::Size(464, 217);
			this->AnimationEventTab->TabIndex = 0;
			this->AnimationEventTab->SelectedIndexChanged += gcnew System::EventHandler(this, &AnimationEvent::AnimationEventTab_SelectedIndexChanged);
			// 
			// CustomEvent
			// 
			this->CustomEvent->Controls->Add(this->search_button);
			this->CustomEvent->Controls->Add(this->search_textBox);
			this->CustomEvent->Controls->Add(this->CustomEffectNameListBox);
			this->CustomEvent->Controls->Add(this->CustomBoneNametextBox);
			this->CustomEvent->Controls->Add(this->CustomParamtextBox);
			this->CustomEvent->Controls->Add(this->EffectNameTextBox);
			this->CustomEvent->Controls->Add(this->CustomBoneName);
			this->CustomEvent->Controls->Add(this->CustomParam);
			this->CustomEvent->Controls->Add(this->CustomEffectName);
			this->CustomEvent->Location = System::Drawing::Point(4, 22);
			this->CustomEvent->Name = L"CustomEvent";
			this->CustomEvent->Padding = System::Windows::Forms::Padding(3);
			this->CustomEvent->Size = System::Drawing::Size(456, 191);
			this->CustomEvent->TabIndex = 0;
			this->CustomEvent->Text = L"Custom";
			this->CustomEvent->UseVisualStyleBackColor = true;
			// 
			// CustomEffectNameListBox
			// 
			this->CustomEffectNameListBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->CustomEffectNameListBox->FormattingEnabled = true;
			this->CustomEffectNameListBox->ItemHeight = 12;
			this->CustomEffectNameListBox->Location = System::Drawing::Point(257, 27);
			this->CustomEffectNameListBox->Name = L"CustomEffectNameListBox";
			this->CustomEffectNameListBox->Size = System::Drawing::Size(193, 160);
			this->CustomEffectNameListBox->TabIndex = 8;
			this->CustomEffectNameListBox->DoubleClick += gcnew System::EventHandler(this, &AnimationEvent::CustomEffectNameListBox_DoubleClick);
			// 
			// CustomBoneNametextBox
			// 
			this->CustomBoneNametextBox->Location = System::Drawing::Point(115, 107);
			this->CustomBoneNametextBox->Name = L"CustomBoneNametextBox";
			this->CustomBoneNametextBox->Size = System::Drawing::Size(136, 21);
			this->CustomBoneNametextBox->TabIndex = 7;
			// 
			// CustomParamtextBox
			// 
			this->CustomParamtextBox->Location = System::Drawing::Point(115, 64);
			this->CustomParamtextBox->Name = L"CustomParamtextBox";
			this->CustomParamtextBox->Size = System::Drawing::Size(136, 21);
			this->CustomParamtextBox->TabIndex = 6;
			// 
			// EffectNameTextBox
			// 
			this->EffectNameTextBox->Location = System::Drawing::Point(115, 21);
			this->EffectNameTextBox->Name = L"EffectNameTextBox";
			this->EffectNameTextBox->Size = System::Drawing::Size(136, 21);
			this->EffectNameTextBox->TabIndex = 5;
			// 
			// CustomBoneName
			// 
			this->CustomBoneName->AutoSize = true;
			this->CustomBoneName->Location = System::Drawing::Point(22, 113);
			this->CustomBoneName->Name = L"CustomBoneName";
			this->CustomBoneName->Size = System::Drawing::Size(68, 12);
			this->CustomBoneName->TabIndex = 3;
			this->CustomBoneName->Text = L"BoneName";
			// 
			// CustomParam
			// 
			this->CustomParam->AutoSize = true;
			this->CustomParam->Location = System::Drawing::Point(22, 69);
			this->CustomParam->Name = L"CustomParam";
			this->CustomParam->Size = System::Drawing::Size(63, 12);
			this->CustomParam->TabIndex = 2;
			this->CustomParam->Text = L"Parameter";
			// 
			// CustomEffectName
			// 
			this->CustomEffectName->AutoSize = true;
			this->CustomEffectName->Location = System::Drawing::Point(22, 25);
			this->CustomEffectName->Name = L"CustomEffectName";
			this->CustomEffectName->Size = System::Drawing::Size(70, 12);
			this->CustomEffectName->TabIndex = 1;
			this->CustomEffectName->Text = L"EffectName";
			// 
			// BasicEvent
			// 
			this->BasicEvent->Controls->Add(this->Param2checkBox);
			this->BasicEvent->Controls->Add(this->Param1comboBox);
			this->BasicEvent->Controls->Add(this->Param3textBox);
			this->BasicEvent->Controls->Add(this->labelParameter3);
			this->BasicEvent->Controls->Add(this->EventNameTextBox);
			this->BasicEvent->Controls->Add(this->EventNameListBox);
			this->BasicEvent->Controls->Add(this->Param1textBox);
			this->BasicEvent->Controls->Add(this->searchbutton);
			this->BasicEvent->Controls->Add(this->Param2textBox);
			this->BasicEvent->Controls->Add(this->label4);
			this->BasicEvent->Controls->Add(this->label3);
			this->BasicEvent->Controls->Add(this->EventName);
			this->BasicEvent->Location = System::Drawing::Point(4, 22);
			this->BasicEvent->Name = L"BasicEvent";
			this->BasicEvent->Padding = System::Windows::Forms::Padding(3);
			this->BasicEvent->Size = System::Drawing::Size(436, 191);
			this->BasicEvent->TabIndex = 1;
			this->BasicEvent->Text = L"Normal";
			this->BasicEvent->UseVisualStyleBackColor = true;
			// 
			// Param2checkBox
			// 
			this->Param2checkBox->AutoSize = true;
			this->Param2checkBox->Location = System::Drawing::Point(15, 111);
			this->Param2checkBox->Name = L"Param2checkBox";
			this->Param2checkBox->Size = System::Drawing::Size(15, 14);
			this->Param2checkBox->TabIndex = 24;
			this->Param2checkBox->UseVisualStyleBackColor = true;
			// 
			// Param1comboBox
			// 
			this->Param1comboBox->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->Param1comboBox->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->Param1comboBox->FormattingEnabled = true;
			this->Param1comboBox->Location = System::Drawing::Point(114, 69);
			this->Param1comboBox->Name = L"Param1comboBox";
			this->Param1comboBox->Size = System::Drawing::Size(138, 20);
			this->Param1comboBox->Sorted = true;
			this->Param1comboBox->TabIndex = 23;
			// 
			// Param3textBox
			// 
			this->Param3textBox->Location = System::Drawing::Point(115, 149);
			this->Param3textBox->Name = L"Param3textBox";
			this->Param3textBox->Size = System::Drawing::Size(136, 21);
			this->Param3textBox->TabIndex = 5;
			// 
			// labelParameter3
			// 
			this->labelParameter3->AutoSize = true;
			this->labelParameter3->Location = System::Drawing::Point(1, 152);
			this->labelParameter3->Name = L"labelParameter3";
			this->labelParameter3->Size = System::Drawing::Size(69, 12);
			this->labelParameter3->TabIndex = 22;
			this->labelParameter3->Text = L"Parameter3";
			// 
			// EventNameTextBox
			// 
			this->EventNameTextBox->Location = System::Drawing::Point(115, 25);
			this->EventNameTextBox->Name = L"EventNameTextBox";
			this->EventNameTextBox->Size = System::Drawing::Size(137, 21);
			this->EventNameTextBox->TabIndex = 1;
			this->EventNameTextBox->TextChanged += gcnew System::EventHandler(this, &AnimationEvent::EventNameTextBox_TextChanged);
			// 
			// EventNameListBox
			// 
			this->EventNameListBox->FormattingEnabled = true;
			this->EventNameListBox->ItemHeight = 12;
			this->EventNameListBox->Location = System::Drawing::Point(259, 11);
			this->EventNameListBox->Name = L"EventNameListBox";
			this->EventNameListBox->Size = System::Drawing::Size(166, 172);
			this->EventNameListBox->TabIndex = 20;
			this->EventNameListBox->DoubleClick += gcnew System::EventHandler(this, &AnimationEvent::EventNameListBox_DoubleClick);
			// 
			// Param1textBox
			// 
			this->Param1textBox->Location = System::Drawing::Point(115, 68);
			this->Param1textBox->Name = L"Param1textBox";
			this->Param1textBox->Size = System::Drawing::Size(113, 21);
			this->Param1textBox->TabIndex = 2;
			// 
			// searchbutton
			// 
			this->searchbutton->Location = System::Drawing::Point(228, 68);
			this->searchbutton->Name = L"searchbutton";
			this->searchbutton->Size = System::Drawing::Size(25, 23);
			this->searchbutton->TabIndex = 3;
			this->searchbutton->Text = L"...";
			this->searchbutton->UseVisualStyleBackColor = true;
			this->searchbutton->Click += gcnew System::EventHandler(this, &AnimationEvent::searchbutton_Click);
			// 
			// Param2textBox
			// 
			this->Param2textBox->Location = System::Drawing::Point(115, 109);
			this->Param2textBox->Name = L"Param2textBox";
			this->Param2textBox->Size = System::Drawing::Size(137, 21);
			this->Param2textBox->TabIndex = 4;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(0, 114);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(69, 12);
			this->label4->TabIndex = 15;
			this->label4->Text = L"Parameter2";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(0, 72);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(69, 12);
			this->label3->TabIndex = 14;
			this->label3->Text = L"Parameter1";
			// 
			// EventName
			// 
			this->EventName->AutoSize = true;
			this->EventName->Location = System::Drawing::Point(0, 28);
			this->EventName->Name = L"EventName";
			this->EventName->Size = System::Drawing::Size(70, 12);
			this->EventName->TabIndex = 6;
			this->EventName->Text = L"EventName";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(10, 22);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(41, 12);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Frame";
			// 
			// FrametextBox
			// 
			this->FrametextBox->ForeColor = System::Drawing::Color::Green;
			this->FrametextBox->Location = System::Drawing::Point(59, 19);
			this->FrametextBox->Name = L"FrametextBox";
			this->FrametextBox->Size = System::Drawing::Size(54, 21);
			this->FrametextBox->TabIndex = 10;
			// 
			// unconditionalcheckBox
			// 
			this->unconditionalcheckBox->AutoSize = true;
			this->unconditionalcheckBox->CheckAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->unconditionalcheckBox->Location = System::Drawing::Point(12, 280);
			this->unconditionalcheckBox->Name = L"unconditionalcheckBox";
			this->unconditionalcheckBox->Size = System::Drawing::Size(84, 16);
			this->unconditionalcheckBox->TabIndex = 15;
			this->unconditionalcheckBox->Text = L"무조건실행";
			this->unconditionalcheckBox->UseVisualStyleBackColor = true;
			// 
			// Cancelbutton
			// 
			this->Cancelbutton->Location = System::Drawing::Point(364, 273);
			this->Cancelbutton->Name = L"Cancelbutton";
			this->Cancelbutton->Size = System::Drawing::Size(75, 23);
			this->Cancelbutton->TabIndex = 17;
			this->Cancelbutton->Text = L"취소";
			this->Cancelbutton->UseVisualStyleBackColor = true;
			this->Cancelbutton->Click += gcnew System::EventHandler(this, &AnimationEvent::Cancelbutton_Click);
			// 
			// Okbutton
			// 
			this->Okbutton->Location = System::Drawing::Point(260, 273);
			this->Okbutton->Name = L"Okbutton";
			this->Okbutton->Size = System::Drawing::Size(75, 23);
			this->Okbutton->TabIndex = 16;
			this->Okbutton->Text = L"확인";
			this->Okbutton->UseVisualStyleBackColor = true;
			this->Okbutton->Click += gcnew System::EventHandler(this, &AnimationEvent::Okbutton_Click);
			// 
			// search_textBox
			// 
			this->search_textBox->Location = System::Drawing::Point(257, 3);
			this->search_textBox->Name = L"search_textBox";
			this->search_textBox->Size = System::Drawing::Size(154, 21);
			this->search_textBox->TabIndex = 9;
			this->search_textBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &AnimationEvent::search_textBox_KeyDown);
			// 
			// search_button
			// 
			this->search_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->search_button->Location = System::Drawing::Point(412, 3);
			this->search_button->Name = L"search_button";
			this->search_button->Size = System::Drawing::Size(38, 21);
			this->search_button->TabIndex = 10;
			this->search_button->Text = L"찾기";
			this->search_button->UseVisualStyleBackColor = true;
			this->search_button->Click += gcnew System::EventHandler(this, &AnimationEvent::search_button_Click);
			// 
			// AnimationEvent
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(484, 309);
			this->ControlBox = false;
			this->Controls->Add(this->Cancelbutton);
			this->Controls->Add(this->Okbutton);
			this->Controls->Add(this->unconditionalcheckBox);
			this->Controls->Add(this->FrametextBox);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->AnimationEventTab);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"AnimationEvent";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"AnimationEvent";
			this->Load += gcnew System::EventHandler(this, &AnimationEvent::AnimationEvent_Load);
			this->AnimationEventTab->ResumeLayout(false);
			this->CustomEvent->ResumeLayout(false);
			this->CustomEvent->PerformLayout();
			this->BasicEvent->ResumeLayout(false);
			this->BasicEvent->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void AnimationEvent_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void AnimationEventTab_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void Okbutton_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->DialogResult = System::Windows::Forms::DialogResult::OK;
			 this->Close();
		 }
private: System::Void Cancelbutton_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void EventNameListBox_DoubleClick(System::Object^  sender, System::EventArgs^  e);
private: System::Void CustomEffectNameListBox_DoubleClick(System::Object^  sender, System::EventArgs^  e);
private: System::Void searchbutton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Setting_AnimationEventParam();
private: System::Void Param1Show(bool bShow);
private: System::Void Param1NotSerchShow(bool bShow);
private: System::Void Param2Show(bool bShow);
private: System::Void Param3Show(bool bShow);
private: System::Void Param1ComboBoxShow(bool bShow);
private: System::Void Param2CheckBoxShow(bool bShow);
private: System::Void EventNameTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e);

private: System::Void SetFacialAniamtionList();
private: System::Void SetEventFacialAniamtionList();
public: System::String^ GetParam1Text();
public: System::Void SetParam1Text(System::String^ strText);
public: System::String^ GetParam2Text();
public: System::Void SetParam2Text(System::String^ strText);
private: System::Void search_textBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if(e->KeyCode == Keys::Enter)
			 {
				 SearchEffectList();
			 }
		 }
private: System::Void search_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 SearchEffectList();
		 }
};
}
