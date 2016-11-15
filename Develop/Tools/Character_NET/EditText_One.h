#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// EditText_One에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class EditText_One : public System::Windows::Forms::Form
	{
	public:
		EditText_One(void)
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
		~EditText_One()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::TextBox^  Edit_textBox;
	protected: 
	private: System::Windows::Forms::Button^  OK_button;
	public: 
	private: System::Windows::Forms::Button^  Cancle_button;

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
			this->Edit_textBox = (gcnew System::Windows::Forms::TextBox());
			this->OK_button = (gcnew System::Windows::Forms::Button());
			this->Cancle_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// Edit_textBox
			// 
			this->Edit_textBox->Location = System::Drawing::Point(55, 12);
			this->Edit_textBox->Name = L"Edit_textBox";
			this->Edit_textBox->Size = System::Drawing::Size(160, 21);
			this->Edit_textBox->TabIndex = 0;
			// 
			// OK_button
			// 
			this->OK_button->Location = System::Drawing::Point(45, 42);
			this->OK_button->Name = L"OK_button";
			this->OK_button->Size = System::Drawing::Size(75, 23);
			this->OK_button->TabIndex = 1;
			this->OK_button->Text = L"OK";
			this->OK_button->UseVisualStyleBackColor = true;
			this->OK_button->Click += gcnew System::EventHandler(this, &EditText_One::OK_button_Click);
			// 
			// Cancle_button
			// 
			this->Cancle_button->Location = System::Drawing::Point(151, 42);
			this->Cancle_button->Name = L"Cancle_button";
			this->Cancle_button->Size = System::Drawing::Size(75, 23);
			this->Cancle_button->TabIndex = 2;
			this->Cancle_button->Text = L"Cancle";
			this->Cancle_button->UseVisualStyleBackColor = true;
			this->Cancle_button->Click += gcnew System::EventHandler(this, &EditText_One::Cancle_button_Click);
			// 
			// EditText_One
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(269, 77);
			this->Controls->Add(this->Cancle_button);
			this->Controls->Add(this->OK_button);
			this->Controls->Add(this->Edit_textBox);
			this->Name = L"EditText_One";
			this->Text = L"EditText_One";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void OK_button_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->DialogResult = System::Windows::Forms::DialogResult::OK;
				 this->Close();
			 }
	private: System::Void Cancle_button_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }
	};
}
