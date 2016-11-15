#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// OverlayAnimationTestTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class OverlayAnimationTestTabPage : public System::Windows::Forms::Form
	{
	public:
		OverlayAnimationTestTabPage(void)
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
		~OverlayAnimationTestTabPage()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	public: System::Windows::Forms::TextBox^  UpperPartAnimation_textBox;
	private: 
	protected: 

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  LowerPartAnimation_textBox;
	private: System::Windows::Forms::Button^  Play_button;
	private: System::Windows::Forms::Button^  Stop_button;

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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->UpperPartAnimation_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->LowerPartAnimation_textBox = (gcnew System::Windows::Forms::TextBox());
			this->Play_button = (gcnew System::Windows::Forms::Button());
			this->Stop_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 22);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(29, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"상체";
			// 
			// UpperPartAnimation_textBox
			// 
			this->UpperPartAnimation_textBox->AllowDrop = true;
			this->UpperPartAnimation_textBox->Location = System::Drawing::Point(52, 16);
			this->UpperPartAnimation_textBox->Name = L"UpperPartAnimation_textBox";
			this->UpperPartAnimation_textBox->Size = System::Drawing::Size(211, 21);
			this->UpperPartAnimation_textBox->TabIndex = 1;
			this->UpperPartAnimation_textBox->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &OverlayAnimationTestTabPage::UpperPartAnimationTest_textBox_DragDrop);
			this->UpperPartAnimation_textBox->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &OverlayAnimationTestTabPage::UpperPartAnimationTest_textBox_DragEnter);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 59);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(29, 12);
			this->label2->TabIndex = 2;
			this->label2->Text = L"하체";
			// 
			// LowerPartAnimation_textBox
			// 
			this->LowerPartAnimation_textBox->AllowDrop = true;
			this->LowerPartAnimation_textBox->Location = System::Drawing::Point(52, 56);
			this->LowerPartAnimation_textBox->Name = L"LowerPartAnimation_textBox";
			this->LowerPartAnimation_textBox->Size = System::Drawing::Size(210, 21);
			this->LowerPartAnimation_textBox->TabIndex = 3;
			this->LowerPartAnimation_textBox->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &OverlayAnimationTestTabPage::LowerPartAnimationTest_textBox_DragDrop);
			this->LowerPartAnimation_textBox->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &OverlayAnimationTestTabPage::LowerPartAnimationTest_textBox_DragEnter);
			// 
			// Play_button
			// 
			this->Play_button->Location = System::Drawing::Point(17, 97);
			this->Play_button->Name = L"Play_button";
			this->Play_button->Size = System::Drawing::Size(75, 23);
			this->Play_button->TabIndex = 4;
			this->Play_button->Text = L"Play";
			this->Play_button->UseVisualStyleBackColor = true;
			this->Play_button->Click += gcnew System::EventHandler(this, &OverlayAnimationTestTabPage::Play_button_Click);
			// 
			// Stop_button
			// 
			this->Stop_button->Location = System::Drawing::Point(17, 126);
			this->Stop_button->Name = L"Stop_button";
			this->Stop_button->Size = System::Drawing::Size(75, 23);
			this->Stop_button->TabIndex = 5;
			this->Stop_button->Text = L"Stop";
			this->Stop_button->UseVisualStyleBackColor = true;
			this->Stop_button->Click += gcnew System::EventHandler(this, &OverlayAnimationTestTabPage::Stop_button_Click);
			// 
			// OverlayAnimationTestTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(292, 266);
			this->Controls->Add(this->Stop_button);
			this->Controls->Add(this->Play_button);
			this->Controls->Add(this->LowerPartAnimation_textBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->UpperPartAnimation_textBox);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"OverlayAnimationTestTabPage";
			this->Text = L"OverlayAnimationTestTabPage";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion




		private: System::Void UpperPartAnimationTest_textBox_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
		private: System::Void UpperPartAnimationTest_textBox_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
		private: System::Void LowerPartAnimationTest_textBox_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
		private: System::Void LowerPartAnimationTest_textBox_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);

		public: System::Void SetLowerPartAnimation(string strAniName);

		private: System::Void Play_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void Stop_button_Click(System::Object^  sender, System::EventArgs^  e);
};
}
