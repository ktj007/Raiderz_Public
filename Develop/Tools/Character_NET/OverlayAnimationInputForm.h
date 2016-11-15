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
	/// OverlayAnimationInputForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class OverlayAnimationInputForm : public System::Windows::Forms::Form
	{
	private:
		ProgressBarForm	m_loadProgressBar;
		int				m_nFullCount;
		int				m_nCurrCount;

	private:
		void		SetProgressBar(int nCount);

	public:
		void		SetAnimation();
		void		SetBoneName();

		void		SetOverlayAnimationData(String^ strOverlayAniName, String^ strBoneName);

		void		OverlayAnimationInputForm_DataLoad();

		OverlayAnimationInputForm(void)
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
		~OverlayAnimationInputForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::ComboBox^  OverlayAnimation_comboBox;
	protected: 
	public: System::Windows::Forms::ComboBox^  BoneSelect_comboBox;
	private: System::Windows::Forms::Label^  label1;
	public: 
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  OK_button;
	private: System::Windows::Forms::Button^  Cancel_button;

	protected: 

	protected: 

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
			this->OverlayAnimation_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->BoneSelect_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->OK_button = (gcnew System::Windows::Forms::Button());
			this->Cancel_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// OverlayAnimation_comboBox
			// 
			this->OverlayAnimation_comboBox->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->OverlayAnimation_comboBox->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->OverlayAnimation_comboBox->FormattingEnabled = true;
			this->OverlayAnimation_comboBox->Location = System::Drawing::Point(154, 22);
			this->OverlayAnimation_comboBox->Name = L"OverlayAnimation_comboBox";
			this->OverlayAnimation_comboBox->Size = System::Drawing::Size(212, 20);
			this->OverlayAnimation_comboBox->Sorted = true;
			this->OverlayAnimation_comboBox->TabIndex = 0;
			// 
			// BoneSelect_comboBox
			// 
			this->BoneSelect_comboBox->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->BoneSelect_comboBox->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->BoneSelect_comboBox->FormattingEnabled = true;
			this->BoneSelect_comboBox->Location = System::Drawing::Point(154, 57);
			this->BoneSelect_comboBox->Name = L"BoneSelect_comboBox";
			this->BoneSelect_comboBox->Size = System::Drawing::Size(212, 20);
			this->BoneSelect_comboBox->Sorted = true;
			this->BoneSelect_comboBox->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 25);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(117, 12);
			this->label1->TabIndex = 2;
			this->label1->Text = L"오버레이 애니메이션";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 60);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(45, 12);
			this->label2->TabIndex = 3;
			this->label2->Text = L"본 선택";
			// 
			// OK_button
			// 
			this->OK_button->Location = System::Drawing::Point(90, 99);
			this->OK_button->Name = L"OK_button";
			this->OK_button->Size = System::Drawing::Size(75, 23);
			this->OK_button->TabIndex = 4;
			this->OK_button->Text = L"설정";
			this->OK_button->UseVisualStyleBackColor = true;
			this->OK_button->Click += gcnew System::EventHandler(this, &OverlayAnimationInputForm::OK_button_Click);
			// 
			// Cancel_button
			// 
			this->Cancel_button->Location = System::Drawing::Point(204, 99);
			this->Cancel_button->Name = L"Cancel_button";
			this->Cancel_button->Size = System::Drawing::Size(75, 23);
			this->Cancel_button->TabIndex = 5;
			this->Cancel_button->Text = L"취소";
			this->Cancel_button->UseVisualStyleBackColor = true;
			this->Cancel_button->Click += gcnew System::EventHandler(this, &OverlayAnimationInputForm::Cancel_button_Click);
			// 
			// OverlayAnimationInputForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(379, 136);
			this->Controls->Add(this->Cancel_button);
			this->Controls->Add(this->OK_button);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->BoneSelect_comboBox);
			this->Controls->Add(this->OverlayAnimation_comboBox);
			this->Name = L"OverlayAnimationInputForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"OverlayAnimationInputForm";
			this->Shown += gcnew System::EventHandler(this, &OverlayAnimationInputForm::OverlayAnimationInputForm_Shown);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void OK_button_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->DialogResult = System::Windows::Forms::DialogResult::OK;
				 this->Close();
			 }
private: System::Void Cancel_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void OverlayAnimationInputForm_Shown(System::Object^  sender, System::EventArgs^  e) {
			 m_loadProgressBar.Hide();
		 }
};
}
