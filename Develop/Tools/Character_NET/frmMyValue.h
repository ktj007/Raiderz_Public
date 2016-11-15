#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Windows::Forms::Design;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// frmMyValue에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class frmMyValue : public System::Windows::Forms::Form
	{
	public: bool	bAni;
	public: int MyValue;
	public: IWindowsFormsEditorService^ _wfes;

	public:
		frmMyValue(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
			TopLevel = false;
			bAni = false;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~frmMyValue()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	protected: 
	private: System::Windows::Forms::Label^  label1;
	public: System::Windows::Forms::TrackBar^  trackBar1;

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
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->BeginInit();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(99, 54);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 5;
			this->button1->Text = L"OK";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &frmMyValue::button1_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 59);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(38, 12);
			this->label1->TabIndex = 4;
			this->label1->Text = L"label1";
			// 
			// trackBar1
			// 
			this->trackBar1->Location = System::Drawing::Point(2, 3);
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(187, 45);
			this->trackBar1->TabIndex = 3;
			this->trackBar1->Scroll += gcnew System::EventHandler(this, &frmMyValue::trackBar1_Scroll);
			// 
			// frmMyValue
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(188, 84);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->trackBar1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"frmMyValue";
			this->ShowInTaskbar = false;
			this->Text = L"frmMyValue";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &frmMyValue::frmMyValue_FormClosed);
			this->Load += gcnew System::EventHandler(this, &frmMyValue::frmMyValue_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void frmMyValue_Load(System::Object^  sender, System::EventArgs^  e) {
				 label1->Text = "Value : " + MyValue;
				 if (bAni)
				 {
					 label1->Text = "Value : " + (int32)((float)MyValue/4.8f/(g_pMainApp->GetActor()->GetSpeed()*1000.f));
				 }
			 }
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				 Close();
			 }
	private: System::Void trackBar1_Scroll(System::Object^  sender, System::EventArgs^  e) {
				 MyValue = trackBar1->Value;
				 label1->Text = "Value : " + MyValue;

				 if (bAni)
				 {
					 GlobalObjects::g_pMainForm->AnitrackBar->Value = trackBar1->Value;
					 GlobalObjects::g_pMainForm->AnitrackBar_Scroll(nullptr, nullptr);
					 //GlobalObjects::g_pMainForm->MainLoop();

					 label1->Text = "Value : " + (int32)((float)MyValue/4.8f/(g_pMainApp->GetActor()->GetSpeed()*1000.f));
				 }
			 }
private: System::Void frmMyValue_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			 _wfes->CloseDropDown();
			}
};
}
