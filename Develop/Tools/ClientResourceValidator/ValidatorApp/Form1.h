#pragma once

#include "ClientValidator.h"
#include "VReportMgr.h"
#include "MMaietXmlTestReporter.h"

namespace ValidatorApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	enum TAB_PAGE
	{
		TP_ANIMATION,
		TP_ANIMATION_EVENT,
		TP_ANIMATION_INFO,
		TP_TALENT_EVENT,
		TP_EFFECT,
		TP_TALENT_EFFECT,
		TP_BUFF_EFFECT,
		TP_NPC_INFO,
		TP_ITEM_INFO,
		TP_SOUND_INFO,

		TP_MAX
	};

	/// <summary>
	/// Form1에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
			m_bOneExecute = false;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::TabControl^  ResourceValidator_tabControl;
	private: System::Windows::Forms::Button^  Start_button;
	public: 
		bool							m_bOneExecute;

	protected: 
		const char*						GetOutputFileName() { return "ClientValidatorResult.xml"; }

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

	private: System::Windows::Forms::TabPage^  AnimationResult_tabPage;
	private: System::Windows::Forms::TabPage^  AnimationEventResult_tabPage;
	private: System::Windows::Forms::TabPage^  AnimationInfoResult_tabPage;
	private: System::Windows::Forms::TabPage^  TalentEventResult_tabPage;
	private: System::Windows::Forms::TabPage^  EffectResult_tabPage;
	private: System::Windows::Forms::TabPage^  TalentEffectResult_tabPage;
	private: System::Windows::Forms::TabPage^  BuffEffectResult_tabPage;
	private: System::Windows::Forms::TabPage^  NpcInfoResult_tabPage;
	private: System::Windows::Forms::TabPage^  ItemInfoResult_tabPage;
	private: System::Windows::Forms::TabPage^  SoundInfoResult_tabPage;


	private: System::Windows::Forms::ListBox^  AnimationResult_listBox;
	private: System::Windows::Forms::ListBox^  AnimationEventResult_listBox;
	private: System::Windows::Forms::ListBox^  AnimationInfoResult_listBox;
	private: System::Windows::Forms::ListBox^  TalentEventResult_listBox;
	private: System::Windows::Forms::ListBox^  EffectResult_listBox;
	private: System::Windows::Forms::ListBox^  TalentEffectResult_listBox;
	private: System::Windows::Forms::ListBox^  BuffEffectResult_listBox;
	private: System::Windows::Forms::ListBox^  NpcInfoResult_listBox;
	private: System::Windows::Forms::ListBox^  ItemInfoResult_listBox;
	private: System::Windows::Forms::ListBox^  SoundInfoResult_listBox;

	private: System::Void AddTabPage(string strTabPageName, [Runtime::InteropServices::Out] System::Windows::Forms::TabPage^ %tabPage, [Runtime::InteropServices::Out] System::Windows::Forms::ListBox^ %listBox, TAB_PAGE eType);
	private: System::Void SetData([Runtime::InteropServices::Out] System::Windows::Forms::ListBox^ %listBox, VErrorMsg& errorMsg, MMaietXmlTestReporter& maietReporter);
	private: System::Void ReportLogFirstInfo(VReportMgr * pReport, MMaietXmlTestReporter& maietReporter);

	public:
		void						SetValidatorReport(VReportMgr * pReport);

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->ResourceValidator_tabControl = (gcnew System::Windows::Forms::TabControl());
			this->Start_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// ResourceValidator_tabControl
			// 
			this->ResourceValidator_tabControl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->ResourceValidator_tabControl->Location = System::Drawing::Point(6, 6);
			this->ResourceValidator_tabControl->Name = L"ResourceValidator_tabControl";
			this->ResourceValidator_tabControl->SelectedIndex = 0;
			this->ResourceValidator_tabControl->Size = System::Drawing::Size(615, 468);
			this->ResourceValidator_tabControl->TabIndex = 0;
			// 
			// Start_button
			// 
			this->Start_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Start_button->Location = System::Drawing::Point(51, 484);
			this->Start_button->Name = L"Start_button";
			this->Start_button->Size = System::Drawing::Size(95, 23);
			this->Start_button->TabIndex = 1;
			this->Start_button->Text = L"다시검사시작";
			this->Start_button->UseVisualStyleBackColor = true;
			this->Start_button->Click += gcnew System::EventHandler(this, &Form1::Start_button_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(626, 515);
			this->Controls->Add(this->Start_button);
			this->Controls->Add(this->ResourceValidator_tabControl);
			this->Name = L"Form1";
			this->Text = L"ClientResourceValidator";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
				 g_pValidatorApp = new ClientValidator();
			 }
	private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {
				 g_pValidatorApp->Start();

				 if(m_bOneExecute)
				 {
					 this->Close();
				 }
			 }
	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 SAFE_DELETE(g_pValidatorApp);
			 }
	private: System::Void Start_button_Click(System::Object^  sender, System::EventArgs^  e) {
				 g_pValidatorApp->Start();
			 }
	};

	enum CCNET_RESULT
	{
		CCNET_SUCCESS	= 0,
		CCNET_FAILED	= 1
	};

	ref class GlobalObjects
	{
	public:
		static Form1^		g_pMainForm;

		static bool			g_ValidatorCCNet;
		static bool			g_ValidatorResult;

		GlobalObjects()
		{
			g_ValidatorResult	= CCNET_SUCCESS;
			g_ValidatorCCNet	= false;
		}
	};
}