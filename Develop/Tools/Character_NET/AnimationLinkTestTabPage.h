#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// AnimationLinkTestTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	enum LINK_TEST_STATE
	{
		LTS_STOP,
		LTS_PLAY,
		LTS_PAUSE
	};

	public ref class AnimationLinkTestTabPage : public System::Windows::Forms::Form
	{
	private:
		void	CheckAnimationBlend();

	public:
		AnimationLinkTestTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//

			m_eAnimationLinkTestState = LTS_STOP;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~AnimationLinkTestTabPage()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::ListBox^  AnimationLinkTest_listBox;
	private: System::Windows::Forms::Button^  Clear_button;
	private: System::Windows::Forms::Button^  Up_button;
	private: System::Windows::Forms::Button^  Down_button;

	private: System::Windows::Forms::Button^  Play_button;
	private: System::Windows::Forms::Button^  Stop_button;
	private: System::Windows::Forms::NumericUpDown^  AnimationLinkTestSpeed_UpDown;
	private: System::Windows::Forms::Button^  Pause_button;
	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  AnimationCutTime_ToolStripMenuItem;

	private: System::ComponentModel::IContainer^  components;
	public: 
	protected: 

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		LINK_TEST_STATE	m_eAnimationLinkTestState;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->AnimationLinkTest_listBox = (gcnew System::Windows::Forms::ListBox());
			this->Clear_button = (gcnew System::Windows::Forms::Button());
			this->Up_button = (gcnew System::Windows::Forms::Button());
			this->Down_button = (gcnew System::Windows::Forms::Button());
			this->Play_button = (gcnew System::Windows::Forms::Button());
			this->Stop_button = (gcnew System::Windows::Forms::Button());
			this->AnimationLinkTestSpeed_UpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->Pause_button = (gcnew System::Windows::Forms::Button());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->AnimationCutTime_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AnimationLinkTestSpeed_UpDown))->BeginInit();
			this->contextMenuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// AnimationLinkTest_listBox
			// 
			this->AnimationLinkTest_listBox->AllowDrop = true;
			this->AnimationLinkTest_listBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->AnimationLinkTest_listBox->ContextMenuStrip = this->contextMenuStrip1;
			this->AnimationLinkTest_listBox->FormattingEnabled = true;
			this->AnimationLinkTest_listBox->ItemHeight = 12;
			this->AnimationLinkTest_listBox->Location = System::Drawing::Point(48, 7);
			this->AnimationLinkTest_listBox->Name = L"AnimationLinkTest_listBox";
			this->AnimationLinkTest_listBox->Size = System::Drawing::Size(242, 256);
			this->AnimationLinkTest_listBox->TabIndex = 0;
			this->AnimationLinkTest_listBox->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &AnimationLinkTestTabPage::AnimationLinkTest_listBox_DragDrop);
			this->AnimationLinkTest_listBox->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &AnimationLinkTestTabPage::AnimationLinkTest_listBox_DragEnter);
			this->AnimationLinkTest_listBox->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &AnimationLinkTestTabPage::AnimationLinkTest_listBox_KeyUp);
			// 
			// Clear_button
			// 
			this->Clear_button->Font = (gcnew System::Drawing::Font(L"굴림", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->Clear_button->Location = System::Drawing::Point(-1, 7);
			this->Clear_button->Name = L"Clear_button";
			this->Clear_button->Size = System::Drawing::Size(48, 25);
			this->Clear_button->TabIndex = 1;
			this->Clear_button->Text = L"Clear";
			this->Clear_button->UseVisualStyleBackColor = true;
			this->Clear_button->Click += gcnew System::EventHandler(this, &AnimationLinkTestTabPage::Clear_button_Click);
			// 
			// Up_button
			// 
			this->Up_button->Location = System::Drawing::Point(-1, 88);
			this->Up_button->Name = L"Up_button";
			this->Up_button->Size = System::Drawing::Size(23, 23);
			this->Up_button->TabIndex = 2;
			this->Up_button->Text = L"▲";
			this->Up_button->UseVisualStyleBackColor = true;
			this->Up_button->Click += gcnew System::EventHandler(this, &AnimationLinkTestTabPage::Up_button_Click);
			// 
			// Down_button
			// 
			this->Down_button->Location = System::Drawing::Point(24, 88);
			this->Down_button->Name = L"Down_button";
			this->Down_button->Size = System::Drawing::Size(23, 23);
			this->Down_button->TabIndex = 3;
			this->Down_button->Text = L"▼";
			this->Down_button->UseVisualStyleBackColor = true;
			this->Down_button->Click += gcnew System::EventHandler(this, &AnimationLinkTestTabPage::Down_button_Click);
			// 
			// Play_button
			// 
			this->Play_button->Font = (gcnew System::Drawing::Font(L"굴림", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->Play_button->Location = System::Drawing::Point(-1, 34);
			this->Play_button->Name = L"Play_button";
			this->Play_button->Size = System::Drawing::Size(48, 25);
			this->Play_button->TabIndex = 4;
			this->Play_button->Text = L"Play";
			this->Play_button->UseVisualStyleBackColor = true;
			this->Play_button->Click += gcnew System::EventHandler(this, &AnimationLinkTestTabPage::Play_button_Click);
			// 
			// Stop_button
			// 
			this->Stop_button->Font = (gcnew System::Drawing::Font(L"굴림", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->Stop_button->Location = System::Drawing::Point(-1, 61);
			this->Stop_button->Name = L"Stop_button";
			this->Stop_button->Size = System::Drawing::Size(48, 25);
			this->Stop_button->TabIndex = 5;
			this->Stop_button->Text = L"Stop";
			this->Stop_button->UseVisualStyleBackColor = true;
			this->Stop_button->Click += gcnew System::EventHandler(this, &AnimationLinkTestTabPage::Stop_button_Click);
			// 
			// AnimationLinkTestSpeed_UpDown
			// 
			this->AnimationLinkTestSpeed_UpDown->DecimalPlaces = 1;
			this->AnimationLinkTestSpeed_UpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->AnimationLinkTestSpeed_UpDown->Location = System::Drawing::Point(1, 119);
			this->AnimationLinkTestSpeed_UpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			this->AnimationLinkTestSpeed_UpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->AnimationLinkTestSpeed_UpDown->Name = L"AnimationLinkTestSpeed_UpDown";
			this->AnimationLinkTestSpeed_UpDown->Size = System::Drawing::Size(45, 21);
			this->AnimationLinkTestSpeed_UpDown->TabIndex = 6;
			this->AnimationLinkTestSpeed_UpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 65536});
			this->AnimationLinkTestSpeed_UpDown->ValueChanged += gcnew System::EventHandler(this, &AnimationLinkTestTabPage::AnimationLinkTestSpeed_UpDown_ValueChanged);
			// 
			// Pause_button
			// 
			this->Pause_button->Font = (gcnew System::Drawing::Font(L"굴림", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->Pause_button->Location = System::Drawing::Point(0, 34);
			this->Pause_button->Name = L"Pause_button";
			this->Pause_button->Size = System::Drawing::Size(48, 25);
			this->Pause_button->TabIndex = 7;
			this->Pause_button->Text = L"Pause";
			this->Pause_button->UseVisualStyleBackColor = true;
			this->Pause_button->Visible = false;
			this->Pause_button->Click += gcnew System::EventHandler(this, &AnimationLinkTestTabPage::Pause_button_Click);
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->AnimationCutTime_ToolStripMenuItem});
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(181, 48);
			// 
			// AnimationCutTime_ToolStripMenuItem
			// 
			this->AnimationCutTime_ToolStripMenuItem->Name = L"AnimationCutTime_ToolStripMenuItem";
			this->AnimationCutTime_ToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->AnimationCutTime_ToolStripMenuItem->Text = L"애니메이션 컷 시간";
			this->AnimationCutTime_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &AnimationLinkTestTabPage::AnimationCutTime_ToolStripMenuItem_Click);
			// 
			// AnimationLinkTestTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(292, 266);
			this->Controls->Add(this->Pause_button);
			this->Controls->Add(this->AnimationLinkTestSpeed_UpDown);
			this->Controls->Add(this->Stop_button);
			this->Controls->Add(this->Play_button);
			this->Controls->Add(this->Down_button);
			this->Controls->Add(this->Up_button);
			this->Controls->Add(this->Clear_button);
			this->Controls->Add(this->AnimationLinkTest_listBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"AnimationLinkTestTabPage";
			this->Text = L"AnimationLinkTestTabPage";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AnimationLinkTestSpeed_UpDown))->EndInit();
			this->contextMenuStrip1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void AnimationLinkTest_Stop();
	private: System::Void AnimationLinkTest_listBox_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void AnimationLinkTest_listBox_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void AnimationLinkTest_listBox_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void Clear_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Play_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Down_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Up_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Stop_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Pause_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void AnimationLinkTestSpeed_UpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	public: void PauseButtonDown();
	public: void PlayButtonDown();
private: System::Void AnimationCutTime_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	public: LINK_TEST_STATE GetTestState() { return m_eAnimationLinkTestState; }
};
}
