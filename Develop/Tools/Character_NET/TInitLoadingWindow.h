#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// TInitLoadingWindow에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class TInitLoadingWindow : public System::Windows::Forms::Form
	{
	private:
		System::Drawing::Graphics^		m_formGraphics;
		System::Drawing::SolidBrush^	m_fontBrush;	
		System::Drawing::Font^			m_drawFont;			
		System::Drawing::StringFormat^	m_drawFormat;
		System::Drawing::RectangleF		m_drawRect;


	public:
		TInitLoadingWindow(void)
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
		~TInitLoadingWindow()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^  InitLoadingPictureBox;
	protected: 

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;
	private: System::Windows::Forms::Label^  InitLoadInfoLabel;


		Image^ m_imgInitLoading;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->InitLoadingPictureBox = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->InitLoadingPictureBox))->BeginInit();
			this->SuspendLayout();
			// 
			// InitLoadingPictureBox
			// 
			this->InitLoadingPictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->InitLoadingPictureBox->Location = System::Drawing::Point(0, 0);
			this->InitLoadingPictureBox->Name = L"InitLoadingPictureBox";
			this->InitLoadingPictureBox->Size = System::Drawing::Size(292, 266);
			this->InitLoadingPictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::CenterImage;
			this->InitLoadingPictureBox->TabIndex = 0;
			this->InitLoadingPictureBox->TabStop = false;
			this->InitLoadingPictureBox->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &TInitLoadingWindow::InitLoadingPictureBox_Paint);
			// 
			// TInitLoadingWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 266);
			this->ControlBox = false;
			this->Controls->Add(this->InitLoadingPictureBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"TInitLoadingWindow";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Loading";
			this->Load += gcnew System::EventHandler(this, &TInitLoadingWindow::TInitLoadingWindow_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->InitLoadingPictureBox))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void TInitLoadingWindow_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void InitLoadingPictureBox_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
	public:  System::Void InitLoadingInfoText(string strLoadingIndo);
	};
}
