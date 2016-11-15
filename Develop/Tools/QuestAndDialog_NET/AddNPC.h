#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace QuestAndDialog_NET {

	/// <summary>
	/// AddNPC에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class AddNPC : public System::Windows::Forms::Form
	{
	public:
		AddNPC(void)
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
		~AddNPC()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::TextBox^  textBox_NPCID;
	protected: 

	private: System::Windows::Forms::Label^  Label_NPCID;
	private: System::Windows::Forms::Button^  button_OK;
	private: System::Windows::Forms::Button^  button_CANCEL;

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
			this->textBox_NPCID = (gcnew System::Windows::Forms::TextBox());
			this->Label_NPCID = (gcnew System::Windows::Forms::Label());
			this->button_OK = (gcnew System::Windows::Forms::Button());
			this->button_CANCEL = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// textBox_NPCID
			// 
			this->textBox_NPCID->Location = System::Drawing::Point(144, 21);
			this->textBox_NPCID->Name = L"textBox_NPCID";
			this->textBox_NPCID->Size = System::Drawing::Size(100, 21);
			this->textBox_NPCID->TabIndex = 0;
			// 
			// Label_NPCID
			// 
			this->Label_NPCID->AutoSize = true;
			this->Label_NPCID->Location = System::Drawing::Point(34, 21);
			this->Label_NPCID->Name = L"Label_NPCID";
			this->Label_NPCID->Size = System::Drawing::Size(42, 12);
			this->Label_NPCID->TabIndex = 1;
			this->Label_NPCID->Text = L"NPCID";
			// 
			// button_OK
			// 
			this->button_OK->Location = System::Drawing::Point(36, 59);
			this->button_OK->Name = L"button_OK";
			this->button_OK->Size = System::Drawing::Size(75, 23);
			this->button_OK->TabIndex = 2;
			this->button_OK->Text = L"확인";
			this->button_OK->UseVisualStyleBackColor = true;
			this->button_OK->Click += gcnew System::EventHandler(this, &AddNPC::button_OK_Click);
			// 
			// button_CANCEL
			// 
			this->button_CANCEL->Location = System::Drawing::Point(169, 59);
			this->button_CANCEL->Name = L"button_CANCEL";
			this->button_CANCEL->Size = System::Drawing::Size(75, 23);
			this->button_CANCEL->TabIndex = 3;
			this->button_CANCEL->Text = L"취소";
			this->button_CANCEL->UseVisualStyleBackColor = true;
			this->button_CANCEL->Click += gcnew System::EventHandler(this, &AddNPC::button_CANCEL_Click);
			// 
			// AddNPC
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 111);
			this->Controls->Add(this->button_CANCEL);
			this->Controls->Add(this->button_OK);
			this->Controls->Add(this->Label_NPCID);
			this->Controls->Add(this->textBox_NPCID);
			this->Name = L"AddNPC";
			this->Text = L"AddNPC";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button_OK_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->DialogResult = System::Windows::Forms::DialogResult::OK;
				 this->Close();
			 }
	private: System::Void button_CANCEL_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }
};
}
