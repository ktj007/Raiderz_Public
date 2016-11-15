#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "ProgressBarForm.h"

namespace Character_NET {

	/// <summary>
	/// EffectSelectForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class EffectSelectForm : public System::Windows::Forms::Form
	{
	private:
		ProgressBarForm	m_loadProgressBar;

	public:
		EffectSelectForm(void)
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
		~EffectSelectForm()
		{
			if (components)
			{
				delete components;
			}
		}
	protected: 
	private: System::Windows::Forms::Button^  ok_button;
	private: System::Windows::Forms::Button^  cancel_button;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::ComboBox^  effectSelect_comboBox;
	private: System::Windows::Forms::ComboBox^  effectSelect_comboBox1;
	private: System::Windows::Forms::ComboBox^  effectSelect_comboBox2;
	private: System::Windows::Forms::ComboBox^  effectSelect_comboBox3;
	private: System::Windows::Forms::ComboBox^  effectSelect_comboBox4;
	private: System::Windows::Forms::ComboBox^  effectSelect_comboBox5;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::ComboBox^  EnchantBuffEffectID_comboBox;
	private: System::Windows::Forms::ComboBox^  EquipType_comboBox;


	private: System::Windows::Forms::Label^  label8;



	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

	public: String^ GetSelectEffectName(int nIndex);
	public:	int GetSelectEnchantBuffEffectID();
	public: String^ GetSelectEnchantEquipType();
	public:	void SetEffectName(String^ strEffectName, int nIndex);
	public:	void SetEffectBuffID(int nBuffID);
	public:	void SetEffectEquipType(String^ strEquipType);

	private: void EffectSelectComboBox();

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->effectSelect_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->ok_button = (gcnew System::Windows::Forms::Button());
			this->cancel_button = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->effectSelect_comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->effectSelect_comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->effectSelect_comboBox3 = (gcnew System::Windows::Forms::ComboBox());
			this->effectSelect_comboBox4 = (gcnew System::Windows::Forms::ComboBox());
			this->effectSelect_comboBox5 = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->EnchantBuffEffectID_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->EquipType_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// effectSelect_comboBox
			// 
			this->effectSelect_comboBox->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->effectSelect_comboBox->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->effectSelect_comboBox->FormattingEnabled = true;
			this->effectSelect_comboBox->Location = System::Drawing::Point(155, 18);
			this->effectSelect_comboBox->MaxDropDownItems = 12;
			this->effectSelect_comboBox->Name = L"effectSelect_comboBox";
			this->effectSelect_comboBox->Size = System::Drawing::Size(229, 20);
			this->effectSelect_comboBox->Sorted = true;
			this->effectSelect_comboBox->TabIndex = 0;
			this->effectSelect_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectSelectForm::effectSelect_comboBox_SelectedIndexChanged);
			// 
			// ok_button
			// 
			this->ok_button->Location = System::Drawing::Point(101, 285);
			this->ok_button->Name = L"ok_button";
			this->ok_button->Size = System::Drawing::Size(71, 21);
			this->ok_button->TabIndex = 1;
			this->ok_button->Text = L"선택";
			this->ok_button->UseVisualStyleBackColor = true;
			this->ok_button->Click += gcnew System::EventHandler(this, &EffectSelectForm::ok_button_Click);
			// 
			// cancel_button
			// 
			this->cancel_button->Location = System::Drawing::Point(206, 285);
			this->cancel_button->Name = L"cancel_button";
			this->cancel_button->Size = System::Drawing::Size(71, 21);
			this->cancel_button->TabIndex = 2;
			this->cancel_button->Text = L"취소";
			this->cancel_button->UseVisualStyleBackColor = true;
			this->cancel_button->Click += gcnew System::EventHandler(this, &EffectSelectForm::cancel_button_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(16, 21);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(133, 12);
			this->label1->TabIndex = 3;
			this->label1->Text = L"dummy_ef_enchant_00";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(16, 171);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(133, 12);
			this->label2->TabIndex = 4;
			this->label2->Text = L"dummy_ef_enchant_05";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(16, 141);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(133, 12);
			this->label3->TabIndex = 5;
			this->label3->Text = L"dummy_ef_enchant_04";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(16, 111);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(133, 12);
			this->label4->TabIndex = 6;
			this->label4->Text = L"dummy_ef_enchant_03";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(16, 81);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(133, 12);
			this->label5->TabIndex = 7;
			this->label5->Text = L"dummy_ef_enchant_02";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(16, 51);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(133, 12);
			this->label6->TabIndex = 8;
			this->label6->Text = L"dummy_ef_enchant_01";
			// 
			// effectSelect_comboBox1
			// 
			this->effectSelect_comboBox1->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->effectSelect_comboBox1->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->effectSelect_comboBox1->FormattingEnabled = true;
			this->effectSelect_comboBox1->Location = System::Drawing::Point(155, 48);
			this->effectSelect_comboBox1->MaxDropDownItems = 12;
			this->effectSelect_comboBox1->Name = L"effectSelect_comboBox1";
			this->effectSelect_comboBox1->Size = System::Drawing::Size(229, 20);
			this->effectSelect_comboBox1->Sorted = true;
			this->effectSelect_comboBox1->TabIndex = 9;
			this->effectSelect_comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectSelectForm::effectSelect_comboBox1_SelectedIndexChanged);
			// 
			// effectSelect_comboBox2
			// 
			this->effectSelect_comboBox2->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->effectSelect_comboBox2->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->effectSelect_comboBox2->FormattingEnabled = true;
			this->effectSelect_comboBox2->Location = System::Drawing::Point(155, 78);
			this->effectSelect_comboBox2->MaxDropDownItems = 12;
			this->effectSelect_comboBox2->Name = L"effectSelect_comboBox2";
			this->effectSelect_comboBox2->Size = System::Drawing::Size(229, 20);
			this->effectSelect_comboBox2->Sorted = true;
			this->effectSelect_comboBox2->TabIndex = 10;
			this->effectSelect_comboBox2->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectSelectForm::effectSelect_comboBox2_SelectedIndexChanged);
			// 
			// effectSelect_comboBox3
			// 
			this->effectSelect_comboBox3->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->effectSelect_comboBox3->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->effectSelect_comboBox3->FormattingEnabled = true;
			this->effectSelect_comboBox3->Location = System::Drawing::Point(155, 108);
			this->effectSelect_comboBox3->MaxDropDownItems = 12;
			this->effectSelect_comboBox3->Name = L"effectSelect_comboBox3";
			this->effectSelect_comboBox3->Size = System::Drawing::Size(229, 20);
			this->effectSelect_comboBox3->Sorted = true;
			this->effectSelect_comboBox3->TabIndex = 11;
			this->effectSelect_comboBox3->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectSelectForm::effectSelect_comboBox3_SelectedIndexChanged);
			// 
			// effectSelect_comboBox4
			// 
			this->effectSelect_comboBox4->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->effectSelect_comboBox4->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->effectSelect_comboBox4->FormattingEnabled = true;
			this->effectSelect_comboBox4->Location = System::Drawing::Point(155, 138);
			this->effectSelect_comboBox4->MaxDropDownItems = 12;
			this->effectSelect_comboBox4->Name = L"effectSelect_comboBox4";
			this->effectSelect_comboBox4->Size = System::Drawing::Size(229, 20);
			this->effectSelect_comboBox4->Sorted = true;
			this->effectSelect_comboBox4->TabIndex = 12;
			this->effectSelect_comboBox4->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectSelectForm::effectSelect_comboBox4_SelectedIndexChanged);
			// 
			// effectSelect_comboBox5
			// 
			this->effectSelect_comboBox5->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->effectSelect_comboBox5->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->effectSelect_comboBox5->FormattingEnabled = true;
			this->effectSelect_comboBox5->Location = System::Drawing::Point(155, 168);
			this->effectSelect_comboBox5->MaxDropDownItems = 12;
			this->effectSelect_comboBox5->Name = L"effectSelect_comboBox5";
			this->effectSelect_comboBox5->Size = System::Drawing::Size(229, 20);
			this->effectSelect_comboBox5->Sorted = true;
			this->effectSelect_comboBox5->TabIndex = 13;
			this->effectSelect_comboBox5->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectSelectForm::effectSelect_comboBox5_SelectedIndexChanged);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->EquipType_comboBox);
			this->groupBox1->Controls->Add(this->label8);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->EnchantBuffEffectID_comboBox);
			this->groupBox1->Location = System::Drawing::Point(14, 207);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(369, 63);
			this->groupBox1->TabIndex = 14;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Enchant Buff Effect";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(16, 27);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(41, 12);
			this->label7->TabIndex = 15;
			this->label7->Text = L"Buff ID";
			// 
			// EnchantBuffEffectID_comboBox
			// 
			this->EnchantBuffEffectID_comboBox->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->EnchantBuffEffectID_comboBox->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->EnchantBuffEffectID_comboBox->FormattingEnabled = true;
			this->EnchantBuffEffectID_comboBox->Location = System::Drawing::Point(69, 24);
			this->EnchantBuffEffectID_comboBox->MaxDropDownItems = 12;
			this->EnchantBuffEffectID_comboBox->Name = L"EnchantBuffEffectID_comboBox";
			this->EnchantBuffEffectID_comboBox->Size = System::Drawing::Size(89, 20);
			this->EnchantBuffEffectID_comboBox->Sorted = true;
			this->EnchantBuffEffectID_comboBox->TabIndex = 15;
			this->EnchantBuffEffectID_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectSelectForm::EnchantBuffEffectID_comboBox_SelectedIndexChanged);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(176, 27);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(74, 12);
			this->label8->TabIndex = 16;
			this->label8->Text = L"EQUIP Type";
			// 
			// EquipType_comboBox
			// 
			this->EquipType_comboBox->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
			this->EquipType_comboBox->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->EquipType_comboBox->FormattingEnabled = true;
			this->EquipType_comboBox->Location = System::Drawing::Point(256, 24);
			this->EquipType_comboBox->MaxDropDownItems = 12;
			this->EquipType_comboBox->Name = L"EquipType_comboBox";
			this->EquipType_comboBox->Size = System::Drawing::Size(107, 20);
			this->EquipType_comboBox->Sorted = true;
			this->EquipType_comboBox->TabIndex = 17;
			// 
			// EffectSelectForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(396, 318);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->effectSelect_comboBox5);
			this->Controls->Add(this->effectSelect_comboBox4);
			this->Controls->Add(this->effectSelect_comboBox3);
			this->Controls->Add(this->effectSelect_comboBox2);
			this->Controls->Add(this->effectSelect_comboBox1);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->cancel_button);
			this->Controls->Add(this->ok_button);
			this->Controls->Add(this->effectSelect_comboBox);
			this->Name = L"EffectSelectForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"EffectSelectForm";
			this->Load += gcnew System::EventHandler(this, &EffectSelectForm::EffectSelectForm_Load);
			this->Shown += gcnew System::EventHandler(this, &EffectSelectForm::EffectSelectForm_Shown);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void EffectSelectForm_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ok_button_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->DialogResult = System::Windows::Forms::DialogResult::OK;
				 this->Close();
			 }
	private: System::Void cancel_button_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }
	private: System::Void EnchantBuffEffectID_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void effectSelect_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 EffectSelectComboBox();
		 }
private: System::Void effectSelect_comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 EffectSelectComboBox();
		 }
private: System::Void effectSelect_comboBox2_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 EffectSelectComboBox();
		 }
private: System::Void effectSelect_comboBox3_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 EffectSelectComboBox();
		 }
private: System::Void effectSelect_comboBox4_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 EffectSelectComboBox();
		 }
private: System::Void effectSelect_comboBox5_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 EffectSelectComboBox();
		 }
private: System::Void EffectSelectForm_Shown(System::Object^  sender, System::EventArgs^  e) {
			 m_loadProgressBar.Hide();
		 }
};
}
