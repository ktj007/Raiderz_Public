#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "ProgressBarForm.h"
#include <string>

namespace Character_NET {

	/// <summary>
	/// AnimationBlendInputForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class AnimationBlendInputForm : public System::Windows::Forms::Form
	{
	public: System::Windows::Forms::TextBox^  BlendAnimation_textBox;
	private: System::Windows::Forms::Button^  TargetAnimationSelect_button;
	public: 

	public: 


	private:
		Array^		m_arrSourceAniName;

	private:
		void		SetTargetAnimationText(vector<string>& vecTargetAniList);

		bool		CheckDuplicationSourceAnimation(vector<string>& vecTargetAniList);

	public:
		void		SetAnimationBlendData(vector<string>& vecSourceAniName, vector<string>& vecTargetAniName, float fBlendTime);
		void		SetDiableForm(bool bDisable);

		void		GetTargetAnimation(vector<string>& vecTargetAniList);

	public:
		AnimationBlendInputForm(void)
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
		~AnimationBlendInputForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  Cancel_button;
	protected: 
	private: System::Windows::Forms::Button^  OK_button;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label3;

	private: 


	public: System::Windows::Forms::TextBox^  BlendValue_textBox;
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
			this->Cancel_button = (gcnew System::Windows::Forms::Button());
			this->OK_button = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->BlendValue_textBox = (gcnew System::Windows::Forms::TextBox());
			this->BlendAnimation_textBox = (gcnew System::Windows::Forms::TextBox());
			this->TargetAnimationSelect_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// Cancel_button
			// 
			this->Cancel_button->Location = System::Drawing::Point(204, 99);
			this->Cancel_button->Name = L"Cancel_button";
			this->Cancel_button->Size = System::Drawing::Size(75, 23);
			this->Cancel_button->TabIndex = 9;
			this->Cancel_button->Text = L"취소";
			this->Cancel_button->UseVisualStyleBackColor = true;
			this->Cancel_button->Click += gcnew System::EventHandler(this, &AnimationBlendInputForm::Cancel_button_Click);
			// 
			// OK_button
			// 
			this->OK_button->Location = System::Drawing::Point(90, 99);
			this->OK_button->Name = L"OK_button";
			this->OK_button->Size = System::Drawing::Size(75, 23);
			this->OK_button->TabIndex = 8;
			this->OK_button->Text = L"설정";
			this->OK_button->UseVisualStyleBackColor = true;
			this->OK_button->Click += gcnew System::EventHandler(this, &AnimationBlendInputForm::OK_button_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 15);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(105, 12);
			this->label1->TabIndex = 6;
			this->label1->Text = L"블랜딩 애니메이션";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(12, 61);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(69, 12);
			this->label3->TabIndex = 10;
			this->label3->Text = L"블랜딩 시간";
			// 
			// BlendValue_textBox
			// 
			this->BlendValue_textBox->Location = System::Drawing::Point(138, 54);
			this->BlendValue_textBox->Name = L"BlendValue_textBox";
			this->BlendValue_textBox->Size = System::Drawing::Size(68, 21);
			this->BlendValue_textBox->TabIndex = 13;
			this->BlendValue_textBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &AnimationBlendInputForm::BlendValue_textBox_KeyPress);
			// 
			// BlendAnimation_textBox
			// 
			this->BlendAnimation_textBox->Location = System::Drawing::Point(137, 11);
			this->BlendAnimation_textBox->Name = L"BlendAnimation_textBox";
			this->BlendAnimation_textBox->Size = System::Drawing::Size(208, 21);
			this->BlendAnimation_textBox->TabIndex = 14;
			// 
			// TargetAnimationSelect_button
			// 
			this->TargetAnimationSelect_button->Location = System::Drawing::Point(348, 12);
			this->TargetAnimationSelect_button->Name = L"TargetAnimationSelect_button";
			this->TargetAnimationSelect_button->Size = System::Drawing::Size(25, 19);
			this->TargetAnimationSelect_button->TabIndex = 15;
			this->TargetAnimationSelect_button->Text = L"...";
			this->TargetAnimationSelect_button->UseVisualStyleBackColor = true;
			this->TargetAnimationSelect_button->Click += gcnew System::EventHandler(this, &AnimationBlendInputForm::TargetAnimationSelect_button_Click);
			// 
			// AnimationBlendInputForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(379, 135);
			this->Controls->Add(this->TargetAnimationSelect_button);
			this->Controls->Add(this->BlendAnimation_textBox);
			this->Controls->Add(this->BlendValue_textBox);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->Cancel_button);
			this->Controls->Add(this->OK_button);
			this->Controls->Add(this->label1);
			this->Name = L"AnimationBlendInputForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"AnimationBlendInputForm";
			this->Shown += gcnew System::EventHandler(this, &AnimationBlendInputForm::AnimationBlendInputForm_Shown);
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
private: System::Void AnimationBlendInputForm_Shown(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void BlendValue_textBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
		 }
private: System::Void TargetAnimationSelect_button_Click(System::Object^  sender, System::EventArgs^  e);
};
}
