#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// ReActionAnimationInputForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class ReActionAnimationInputForm : public System::Windows::Forms::Form
	{
	private:
		bool 	CheckInputForNumber(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
		bool 	CheckInputForEnterOrEsc(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);

	public:
		void	SetReActionAnimationList(vector<string>& vecReActionAnimationList);

	public:
		ReActionAnimationInputForm(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//

			m_bUsableWeight = false;
			m_bUsableSpeed = false;
			m_bUsableDisable = false;

		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~ReActionAnimationInputForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	public: System::Windows::Forms::ComboBox^  reAction_comboBox;
	private: 
	public: System::Windows::Forms::TextBox^  weight_textBox;
	public: System::Windows::Forms::TextBox^  speed_textBox;

	private: System::Windows::Forms::Label^  label4;
	public: System::Windows::Forms::CheckBox^  Disable_checkBox;
	private: 


	private: System::Windows::Forms::Button^  OK_button;
	private: System::Windows::Forms::Button^  cancel_button;

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

	public:
		String^			m_strReActionAnimationName;
		String^			m_strWeight;
		String^			m_strSpeed;

		bool			m_bUsableWeight;
		bool			m_bUsableSpeed;
	public: System::Windows::Forms::ComboBox^  weaponBoneName_comboBox;
	private: System::Windows::Forms::Label^  label5;
	public: 
		bool			m_bUsableDisable;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->reAction_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->weight_textBox = (gcnew System::Windows::Forms::TextBox());
			this->speed_textBox = (gcnew System::Windows::Forms::TextBox());
			this->OK_button = (gcnew System::Windows::Forms::Button());
			this->cancel_button = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->Disable_checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->weaponBoneName_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(33, 24);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(105, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"리액션 애니메이션";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(33, 52);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(42, 12);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Weight";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(33, 90);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(41, 12);
			this->label3->TabIndex = 2;
			this->label3->Text = L"Speed";
			// 
			// reAction_comboBox
			// 
			this->reAction_comboBox->FormattingEnabled = true;
			this->reAction_comboBox->Location = System::Drawing::Point(158, 18);
			this->reAction_comboBox->Name = L"reAction_comboBox";
			this->reAction_comboBox->Size = System::Drawing::Size(213, 20);
			this->reAction_comboBox->TabIndex = 3;
			this->reAction_comboBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ReActionAnimationInputForm::reAction_comboBox_KeyDown);
			// 
			// weight_textBox
			// 
			this->weight_textBox->Location = System::Drawing::Point(158, 49);
			this->weight_textBox->Name = L"weight_textBox";
			this->weight_textBox->Size = System::Drawing::Size(212, 21);
			this->weight_textBox->TabIndex = 4;
			this->weight_textBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ReActionAnimationInputForm::weight_textBox_KeyDown);
			this->weight_textBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &ReActionAnimationInputForm::weight_textBox_KeyPress);
			// 
			// speed_textBox
			// 
			this->speed_textBox->Location = System::Drawing::Point(158, 82);
			this->speed_textBox->Name = L"speed_textBox";
			this->speed_textBox->Size = System::Drawing::Size(212, 21);
			this->speed_textBox->TabIndex = 5;
			this->speed_textBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ReActionAnimationInputForm::speed_textBox_KeyDown);
			this->speed_textBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &ReActionAnimationInputForm::speed_textBox_KeyPress);
			// 
			// OK_button
			// 
			this->OK_button->Location = System::Drawing::Point(145, 188);
			this->OK_button->Name = L"OK_button";
			this->OK_button->Size = System::Drawing::Size(67, 27);
			this->OK_button->TabIndex = 6;
			this->OK_button->Text = L"완료";
			this->OK_button->UseVisualStyleBackColor = true;
			this->OK_button->Click += gcnew System::EventHandler(this, &ReActionAnimationInputForm::OK_button_Click);
			// 
			// cancel_button
			// 
			this->cancel_button->Location = System::Drawing::Point(231, 188);
			this->cancel_button->Name = L"cancel_button";
			this->cancel_button->Size = System::Drawing::Size(67, 27);
			this->cancel_button->TabIndex = 7;
			this->cancel_button->Text = L"취소";
			this->cancel_button->UseVisualStyleBackColor = true;
			this->cancel_button->Click += gcnew System::EventHandler(this, &ReActionAnimationInputForm::cancel_button_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(33, 147);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(47, 12);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Disable";
			// 
			// Disable_checkBox
			// 
			this->Disable_checkBox->AutoSize = true;
			this->Disable_checkBox->Location = System::Drawing::Point(161, 144);
			this->Disable_checkBox->Name = L"Disable_checkBox";
			this->Disable_checkBox->Size = System::Drawing::Size(15, 14);
			this->Disable_checkBox->TabIndex = 9;
			this->Disable_checkBox->UseVisualStyleBackColor = true;
			this->Disable_checkBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ReActionAnimationInputForm::Disable_checkBox_KeyDown);
			// 
			// weaponBoneName_comboBox
			// 
			this->weaponBoneName_comboBox->FormattingEnabled = true;
			this->weaponBoneName_comboBox->Location = System::Drawing::Point(158, 111);
			this->weaponBoneName_comboBox->Name = L"weaponBoneName_comboBox";
			this->weaponBoneName_comboBox->Size = System::Drawing::Size(213, 20);
			this->weaponBoneName_comboBox->TabIndex = 11;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(33, 117);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(73, 12);
			this->label5->TabIndex = 10;
			this->label5->Text = L"무기 본 이름";
			// 
			// ReActionAnimationInputForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(428, 226);
			this->Controls->Add(this->weaponBoneName_comboBox);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->Disable_checkBox);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->cancel_button);
			this->Controls->Add(this->OK_button);
			this->Controls->Add(this->speed_textBox);
			this->Controls->Add(this->weight_textBox);
			this->Controls->Add(this->reAction_comboBox);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Name = L"ReActionAnimationInputForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"ReActionAnimationInputForm";
			this->Load += gcnew System::EventHandler(this, &ReActionAnimationInputForm::ReActionAnimationInputForm_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void ReActionAnimationInputForm_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void weight_textBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
private: System::Void speed_textBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
private: System::Void OK_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->DialogResult = System::Windows::Forms::DialogResult::OK;
			 this->Close();
		 }
private: System::Void cancel_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void reAction_comboBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if(CheckInputForEnterOrEsc(sender, e))
				this->Close();
		 }
private: System::Void weight_textBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if(CheckInputForEnterOrEsc(sender, e))
				this->Close();
		 }
private: System::Void speed_textBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if(CheckInputForEnterOrEsc(sender, e))
				this->Close();
		 }
private: System::Void Disable_checkBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if(CheckInputForEnterOrEsc(sender, e))
				this->Close();
		 }

};
}
