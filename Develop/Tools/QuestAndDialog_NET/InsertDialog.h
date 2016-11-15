#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace QuestAndDialog_NET {

	/// <summary>
	/// InsertDialog에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class InsertDialog : public System::Windows::Forms::Form
	{
	public:
		InsertDialog(void)
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
		~InsertDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button_Cancel;
	protected: 
	private: System::Windows::Forms::Button^  button_Confirm;
	private: System::Windows::Forms::TextBox^  textBox_InsertDialog;
	private: System::Windows::Forms::Label^  label_InsertDialog;

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
			this->button_Cancel = (gcnew System::Windows::Forms::Button());
			this->button_Confirm = (gcnew System::Windows::Forms::Button());
			this->textBox_InsertDialog = (gcnew System::Windows::Forms::TextBox());
			this->label_InsertDialog = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// button_Cancel
			// 
			this->button_Cancel->Location = System::Drawing::Point(129, 69);
			this->button_Cancel->Name = L"button_Cancel";
			this->button_Cancel->Size = System::Drawing::Size(75, 23);
			this->button_Cancel->TabIndex = 7;
			this->button_Cancel->Text = L"취소";
			this->button_Cancel->UseVisualStyleBackColor = true;
			this->button_Cancel->Click += gcnew System::EventHandler(this, &InsertDialog::button_Cancel_Click);
			// 
			// button_Confirm
			// 
			this->button_Confirm->Location = System::Drawing::Point(41, 69);
			this->button_Confirm->Name = L"button_Confirm";
			this->button_Confirm->Size = System::Drawing::Size(75, 23);
			this->button_Confirm->TabIndex = 6;
			this->button_Confirm->Text = L"확인";
			this->button_Confirm->UseVisualStyleBackColor = true;
			this->button_Confirm->Click += gcnew System::EventHandler(this, &InsertDialog::button_Confirm_Click);
			// 
			// textBox_InsertDialog
			// 
			this->textBox_InsertDialog->Location = System::Drawing::Point(104, 25);
			this->textBox_InsertDialog->Name = L"textBox_InsertDialog";
			this->textBox_InsertDialog->Size = System::Drawing::Size(100, 21);
			this->textBox_InsertDialog->TabIndex = 5;
			this->textBox_InsertDialog->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &InsertDialog::textBox_InsertDialog_KeyDown);
			// 
			// label_InsertDialog
			// 
			this->label_InsertDialog->AutoSize = true;
			this->label_InsertDialog->Location = System::Drawing::Point(39, 28);
			this->label_InsertDialog->Name = L"label_InsertDialog";
			this->label_InsertDialog->Size = System::Drawing::Size(51, 12);
			this->label_InsertDialog->TabIndex = 4;
			this->label_InsertDialog->Text = L"DialogID";
			// 
			// InsertDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(242, 116);
			this->Controls->Add(this->button_Cancel);
			this->Controls->Add(this->button_Confirm);
			this->Controls->Add(this->textBox_InsertDialog);
			this->Controls->Add(this->label_InsertDialog);
			this->Name = L"InsertDialog";
			this->Text = L"InsertDialog";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button_Confirm_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->DialogResult = Windows::Forms::DialogResult::OK;
				 this->Close();
			 }

	private: System::Void button_Cancel_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }

	private: System::Void textBox_InsertDialog_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 if (Keys::Enter == e->KeyCode)
				 {
					 this->button_Confirm_Click(sender, e);
				 }
				 else if (Keys::Escape == e->KeyCode)
				 {
					 this->Close();
				 }
			 }
	public: int GetDialogID()
			{
				int nDialogID = MStringToInt(this->textBox_InsertDialog->Text->ToString());
				return nDialogID;
			}
};
}
