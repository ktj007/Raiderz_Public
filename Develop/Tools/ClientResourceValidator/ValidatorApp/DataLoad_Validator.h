#pragma once

#include "VErrorMsg.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace ValidatorApp {

	/// <summary>
	/// DataLoad_Validator에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class DataLoad_Validator : public System::Windows::Forms::Form
	{
	public:
		DataLoad_Validator(void)
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
		~DataLoad_Validator()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  OK_button;
	protected: 
	private: System::Windows::Forms::ListBox^  LoadingErrorResult_listBox;

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

	public:
		void						AddErrorMsg(VErrorMsg& errorMsg);

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->OK_button = (gcnew System::Windows::Forms::Button());
			this->LoadingErrorResult_listBox = (gcnew System::Windows::Forms::ListBox());
			this->SuspendLayout();
			// 
			// OK_button
			// 
			this->OK_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->OK_button->Location = System::Drawing::Point(203, 150);
			this->OK_button->Name = L"OK_button";
			this->OK_button->Size = System::Drawing::Size(76, 25);
			this->OK_button->TabIndex = 0;
			this->OK_button->Text = L"확인";
			this->OK_button->UseVisualStyleBackColor = true;
			this->OK_button->Click += gcnew System::EventHandler(this, &DataLoad_Validator::OK_button_Click);
			// 
			// LoadingErrorResult_listBox
			// 
			this->LoadingErrorResult_listBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->LoadingErrorResult_listBox->Font = (gcnew System::Drawing::Font(L"굴림", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->LoadingErrorResult_listBox->FormattingEnabled = true;
			this->LoadingErrorResult_listBox->HorizontalScrollbar = true;
			this->LoadingErrorResult_listBox->Location = System::Drawing::Point(9, 6);
			this->LoadingErrorResult_listBox->Name = L"LoadingErrorResult_listBox";
			this->LoadingErrorResult_listBox->Size = System::Drawing::Size(478, 134);
			this->LoadingErrorResult_listBox->TabIndex = 1;
			// 
			// DataLoad_Validator
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(496, 187);
			this->Controls->Add(this->LoadingErrorResult_listBox);
			this->Controls->Add(this->OK_button);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"DataLoad_Validator";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"로딩 에러 리포트";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void OK_button_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }

	};
}
