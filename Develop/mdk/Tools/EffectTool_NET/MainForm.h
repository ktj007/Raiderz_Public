#pragma once

#include "MainApp.h"
#include "PicForm.h"
#include "RConfiguration.h"
#include "ToolConfig.h"
#include "EffectTabForm.h"
#include "About.h"
#include "PropertyForm.h"
#include "RPointLightSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "PointLightPropertyReflection.h"
#include "SpotLightPropertyReflection.h"

namespace EffectTool_NET {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace ReflectivePropertyFormNameSpace;

	/// <summary>
	/// Form1에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();

			// tab-page form을 붙인다.
			childTabForm = gcnew EffectTabForm();
			childTabForm->TopLevel = false;
			childTabForm->Show();
			childTabForm->Dock = System::Windows::Forms::DockStyle::Fill;

			IPanel1->Controls->Add(childTabForm);

			GlobalObjects::g_pTrackBar = TimeTrackBar;
			GlobalObjects::g_fTrackBarValue = 0.0f;

			m_propertyForm = gcnew PropertyForm();
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public: EffectTabForm^ childTabForm;
	public: PropertyForm^	m_propertyForm;
	public: System::Windows::Forms::PictureBox^  MainView;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	public: System::Windows::Forms::TrackBar^  TimeTrackBar;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::TabControl^  tabControl2;
	private: System::Windows::Forms::Button^  stopbutton;
	private: System::Windows::Forms::Button^  Startbutton;
	private: System::Windows::Forms::Button^  Pausebutton;
	private: System::Windows::Forms::Label^  EmitterLifelabel;
	private: System::Windows::Forms::VScrollBar^  vScrollBar1;
	public: System::Windows::Forms::TextBox^  LogtextBox;
	private: System::Windows::Forms::SplitContainer^  splitContainer2;
	private: System::Windows::Forms::SplitContainer^  splitContainer3;
	private: System::Windows::Forms::MenuStrip^  menuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  FileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  NewToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  OpenToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  eluOpenToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SaveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SaveAsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  eXitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ToolToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  backgroundColorToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  FeedBackToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  GoMDNToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  HelpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  InformationToolStripMenuItem;
	private: System::Windows::Forms::ToolStrip^  toolStrip1;
	private: System::Windows::Forms::ToolStripButton^  NewToolStripButton;
	private: System::Windows::Forms::ToolStripButton^  OpenToolStripButton;
	private: System::Windows::Forms::ToolStripButton^  SaveToolStripButton;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator;
	private: System::Windows::Forms::ToolStripButton^  GridtoolStripButton;
	private: System::Windows::Forms::ToolStripButton^  AxistoolStripButton;
	private: System::Windows::Forms::ToolStripButton^  ShowEmitterRangetoolStripButton1;
	private: System::Windows::Forms::ToolStripButton^  PathtoolStripButton;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ToolStripButton^  HelpToolStripButton;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripCameraReset;
	private: System::Windows::Forms::Panel^  IPanel1;
	private: System::Windows::Forms::ToolStripMenuItem^  fogEnableToolStripMenuItem;
	private: System::Windows::Forms::TabPage^  tabPage1;
	public: System::Windows::Forms::TreeView^  SceneNodetreeView;
	public: System::Windows::Forms::ContextMenuStrip^  sceneTreeMenu;
	private: System::Windows::Forms::ToolStripMenuItem^  sceneTreeMenuCreate;
	private: System::Windows::Forms::ToolStripMenuItem^  sceneTreeMenuImport;
	private: System::Windows::Forms::ToolStripMenuItem^  sceneTreeMenuDelete;
	private: System::Windows::Forms::ToolStripMenuItem^  emitterToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  sphereToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  boxToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ringToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  trailToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  propertyToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  applyAllSameNamedToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  moveUpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  moveDownToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  다시열기RToolStripMenuItem;
	public: System::Windows::Forms::ImageList^  imageListSceneNodeType;
	private: System::Windows::Forms::ToolStripMenuItem^  changeTypeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ChangeEmitterTypeSphereToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ChangeEmitterTypeBoxToolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  ChangeEmitterTypeRingToolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  EmitterName_AutoUpdate;
	private: System::Windows::Forms::ToolStripMenuItem^  lightToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  pointLightToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  spotLightToolStripMenuItem;
	private: System::Windows::Forms::ToolStripButton^  ShowLightRangetoolStripButton;
	private: System::Windows::Forms::ToolStripMenuItem^  CopyToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  PasteToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  effectGroupToolStripMenuItem;
	public: System::Windows::Forms::ToolStripMenuItem^  LightOnOffMenuItem;
	public: System::Windows::Forms::ToolStripMenuItem^  LightToCameraPosMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  HDREnableToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  lightAmbientMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  MRUEluFileListToolStripMenuItem;
	private: System::ComponentModel::IContainer^  components;
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::ToolStripMenuItem^  라이팅LToolStripMenuItem;
			System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
			this->LightOnOffMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LightToCameraPosMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->lightAmbientMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->HDREnableToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MainView = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->EmitterLifelabel = (gcnew System::Windows::Forms::Label());
			this->Pausebutton = (gcnew System::Windows::Forms::Button());
			this->Startbutton = (gcnew System::Windows::Forms::Button());
			this->stopbutton = (gcnew System::Windows::Forms::Button());
			this->TimeTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->SceneNodetreeView = (gcnew System::Windows::Forms::TreeView());
			this->sceneTreeMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->sceneTreeMenuCreate = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->emitterToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->sphereToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->boxToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ringToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->trailToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->lightToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pointLightToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->spotLightToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->effectGroupToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->changeTypeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ChangeEmitterTypeSphereToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ChangeEmitterTypeBoxToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ChangeEmitterTypeRingToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->CopyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->PasteToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->sceneTreeMenuImport = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->sceneTreeMenuDelete = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->propertyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->applyAllSameNamedToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->moveUpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->moveDownToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->imageListSceneNodeType = (gcnew System::Windows::Forms::ImageList(this->components));
			this->LogtextBox = (gcnew System::Windows::Forms::TextBox());
			this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
			this->IPanel1 = (gcnew System::Windows::Forms::Panel());
			this->splitContainer3 = (gcnew System::Windows::Forms::SplitContainer());
			this->menuStrip = (gcnew System::Windows::Forms::MenuStrip());
			this->FileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->NewToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->OpenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->다시열기RToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->eluOpenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MRUEluFileListToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SaveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SaveAsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->EmitterName_AutoUpdate = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->eXitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ToolToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripCameraReset = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->backgroundColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->fogEnableToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->FeedBackToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->GoMDNToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->HelpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->InformationToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->NewToolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->OpenToolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->SaveToolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->GridtoolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->AxistoolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->ShowEmitterRangetoolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
			this->ShowLightRangetoolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->PathtoolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->HelpToolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			라이팅LToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MainView))->BeginInit();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TimeTrackBar))->BeginInit();
			this->tabControl2->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->sceneTreeMenu->SuspendLayout();
			this->splitContainer2->Panel1->SuspendLayout();
			this->splitContainer2->Panel2->SuspendLayout();
			this->splitContainer2->SuspendLayout();
			this->splitContainer3->Panel1->SuspendLayout();
			this->splitContainer3->Panel2->SuspendLayout();
			this->splitContainer3->SuspendLayout();
			this->menuStrip->SuspendLayout();
			this->toolStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// 라이팅LToolStripMenuItem
			// 
			라이팅LToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->LightOnOffMenuItem, 
				this->LightToCameraPosMenuItem, this->lightAmbientMenuItem, this->HDREnableToolStripMenuItem});
			라이팅LToolStripMenuItem->Name = L"라이팅LToolStripMenuItem";
			라이팅LToolStripMenuItem->Size = System::Drawing::Size(69, 20);
			라이팅LToolStripMenuItem->Text = L"라이팅(&L)";
			// 
			// LightOnOffMenuItem
			// 
			this->LightOnOffMenuItem->Checked = true;
			this->LightOnOffMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->LightOnOffMenuItem->Name = L"LightOnOffMenuItem";
			this->LightOnOffMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::L));
			this->LightOnOffMenuItem->Size = System::Drawing::Size(215, 22);
			this->LightOnOffMenuItem->Text = L"Light On/Off";
			this->LightOnOffMenuItem->Click += gcnew System::EventHandler(this, &MainForm::LightOnOffMenuItem_Click);
			// 
			// LightToCameraPosMenuItem
			// 
			this->LightToCameraPosMenuItem->Checked = true;
			this->LightToCameraPosMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->LightToCameraPosMenuItem->Name = L"LightToCameraPosMenuItem";
			this->LightToCameraPosMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::P));
			this->LightToCameraPosMenuItem->Size = System::Drawing::Size(215, 22);
			this->LightToCameraPosMenuItem->Text = L"LightToCameraPos";
			this->LightToCameraPosMenuItem->Click += gcnew System::EventHandler(this, &MainForm::LightToCameraPosMenuItem_Click);
			// 
			// lightAmbientMenuItem
			// 
			this->lightAmbientMenuItem->Name = L"lightAmbientMenuItem";
			this->lightAmbientMenuItem->Size = System::Drawing::Size(215, 22);
			this->lightAmbientMenuItem->Text = L"LightAmbient";
			this->lightAmbientMenuItem->Click += gcnew System::EventHandler(this, &MainForm::lightAmbientMenuItem_Click);
			// 
			// HDREnableToolStripMenuItem
			// 
			this->HDREnableToolStripMenuItem->Checked = true;
			this->HDREnableToolStripMenuItem->CheckOnClick = true;
			this->HDREnableToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->HDREnableToolStripMenuItem->Name = L"HDREnableToolStripMenuItem";
			this->HDREnableToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::H));
			this->HDREnableToolStripMenuItem->Size = System::Drawing::Size(215, 22);
			this->HDREnableToolStripMenuItem->Text = L"&HDR Enable";
			this->HDREnableToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::HDREnableToolStripMenuItem_Click);
			// 
			// toolStripSeparator4
			// 
			toolStripSeparator4->Name = L"toolStripSeparator4";
			toolStripSeparator4->Size = System::Drawing::Size(217, 6);
			// 
			// MainView
			// 
			this->MainView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->MainView->Location = System::Drawing::Point(0, 52);
			this->MainView->Name = L"MainView";
			this->MainView->Size = System::Drawing::Size(905, 661);
			this->MainView->TabIndex = 0;
			this->MainView->TabStop = false;
			this->MainView->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::MainView_MouseMove);
			this->MainView->Click += gcnew System::EventHandler(this, &MainForm::MainView_Click);
			this->MainView->Resize += gcnew System::EventHandler(this, &MainForm::MainView_Resize);
			// 
			// groupBox3
			// 
			this->groupBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox3->Controls->Add(this->EmitterLifelabel);
			this->groupBox3->Controls->Add(this->Pausebutton);
			this->groupBox3->Controls->Add(this->Startbutton);
			this->groupBox3->Controls->Add(this->stopbutton);
			this->groupBox3->Controls->Add(this->TimeTrackBar);
			this->groupBox3->Location = System::Drawing::Point(3, 719);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(899, 74);
			this->groupBox3->TabIndex = 10;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"TimeSlider";
			// 
			// EmitterLifelabel
			// 
			this->EmitterLifelabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->EmitterLifelabel->AutoSize = true;
			this->EmitterLifelabel->Location = System::Drawing::Point(36, 56);
			this->EmitterLifelabel->Name = L"EmitterLifelabel";
			this->EmitterLifelabel->Size = System::Drawing::Size(76, 12);
			this->EmitterLifelabel->TabIndex = 10;
			this->EmitterLifelabel->Text = L"Emitter Life :";
			// 
			// Pausebutton
			// 
			this->Pausebutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->Pausebutton->Location = System::Drawing::Point(760, 51);
			this->Pausebutton->Name = L"Pausebutton";
			this->Pausebutton->Size = System::Drawing::Size(55, 23);
			this->Pausebutton->TabIndex = 9;
			this->Pausebutton->Text = L"Pause";
			this->Pausebutton->UseVisualStyleBackColor = true;
			this->Pausebutton->Click += gcnew System::EventHandler(this, &MainForm::Pausebutton_Click);
			// 
			// Startbutton
			// 
			this->Startbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->Startbutton->Location = System::Drawing::Point(699, 51);
			this->Startbutton->Name = L"Startbutton";
			this->Startbutton->Size = System::Drawing::Size(55, 23);
			this->Startbutton->TabIndex = 8;
			this->Startbutton->Text = L"Start";
			this->Startbutton->UseVisualStyleBackColor = true;
			this->Startbutton->Click += gcnew System::EventHandler(this, &MainForm::Startbutton_Click);
			// 
			// stopbutton
			// 
			this->stopbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->stopbutton->Location = System::Drawing::Point(821, 51);
			this->stopbutton->Name = L"stopbutton";
			this->stopbutton->Size = System::Drawing::Size(55, 23);
			this->stopbutton->TabIndex = 7;
			this->stopbutton->Text = L"Stop";
			this->stopbutton->UseVisualStyleBackColor = true;
			this->stopbutton->Click += gcnew System::EventHandler(this, &MainForm::stopbutton_Click);
			// 
			// TimeTrackBar
			// 
			this->TimeTrackBar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TimeTrackBar->Enabled = false;
			this->TimeTrackBar->LargeChange = 50;
			this->TimeTrackBar->Location = System::Drawing::Point(6, 23);
			this->TimeTrackBar->Maximum = 100;
			this->TimeTrackBar->Name = L"TimeTrackBar";
			this->TimeTrackBar->Size = System::Drawing::Size(887, 45);
			this->TimeTrackBar->SmallChange = 50;
			this->TimeTrackBar->TabIndex = 5;
			this->TimeTrackBar->TickStyle = System::Windows::Forms::TickStyle::TopLeft;
			this->TimeTrackBar->Scroll += gcnew System::EventHandler(this, &MainForm::TimeTrackBar_Scroll);
			// 
			// statusStrip1
			// 
			this->statusStrip1->Location = System::Drawing::Point(0, 881);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(1158, 22);
			this->statusStrip1->TabIndex = 12;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// tabControl2
			// 
			this->tabControl2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl2->Controls->Add(this->tabPage1);
			this->tabControl2->Location = System::Drawing::Point(0, 50);
			this->tabControl2->Name = L"tabControl2";
			this->tabControl2->SelectedIndex = 0;
			this->tabControl2->Size = System::Drawing::Size(245, 213);
			this->tabControl2->TabIndex = 0;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->SceneNodetreeView);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(237, 187);
			this->tabPage1->TabIndex = 1;
			this->tabPage1->Text = L"sceneNodes";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// SceneNodetreeView
			// 
			this->SceneNodetreeView->AllowDrop = true;
			this->SceneNodetreeView->CheckBoxes = true;
			this->SceneNodetreeView->ContextMenuStrip = this->sceneTreeMenu;
			this->SceneNodetreeView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->SceneNodetreeView->HideSelection = false;
			this->SceneNodetreeView->ImageIndex = 0;
			this->SceneNodetreeView->ImageList = this->imageListSceneNodeType;
			this->SceneNodetreeView->Indent = 14;
			this->SceneNodetreeView->LabelEdit = true;
			this->SceneNodetreeView->Location = System::Drawing::Point(3, 3);
			this->SceneNodetreeView->Name = L"SceneNodetreeView";
			this->SceneNodetreeView->SelectedImageIndex = 0;
			this->SceneNodetreeView->Size = System::Drawing::Size(231, 181);
			this->SceneNodetreeView->TabIndex = 0;
			this->SceneNodetreeView->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &MainForm::SceneNodetreeView_AfterCheck);
			this->SceneNodetreeView->AfterLabelEdit += gcnew System::Windows::Forms::NodeLabelEditEventHandler(this, &MainForm::SceneNodetreeView_AfterLabelEdit);
			this->SceneNodetreeView->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::SceneNodetreeView_DragDrop);
			this->SceneNodetreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &MainForm::SceneNodetreeView_AfterSelect);
			this->SceneNodetreeView->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::SceneNodetreeView_DragEnter);
			this->SceneNodetreeView->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &MainForm::SceneNodetreeView_NodeMouseClick);
			this->SceneNodetreeView->ItemDrag += gcnew System::Windows::Forms::ItemDragEventHandler(this, &MainForm::SceneNodetreeView_ItemDrag);
			this->SceneNodetreeView->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::SceneNodetreeView_DragOver);
			// 
			// sceneTreeMenu
			// 
			this->sceneTreeMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(10) {this->sceneTreeMenuCreate, 
				this->changeTypeToolStripMenuItem, this->CopyToolStripMenuItem, this->PasteToolStripMenuItem, this->sceneTreeMenuImport, this->sceneTreeMenuDelete, 
				this->propertyToolStripMenuItem, this->applyAllSameNamedToolStripMenuItem, this->moveUpToolStripMenuItem, this->moveDownToolStripMenuItem});
			this->sceneTreeMenu->Name = L"contextMenuStrip1";
			this->sceneTreeMenu->Size = System::Drawing::Size(210, 224);
			this->sceneTreeMenu->Opening += gcnew System::ComponentModel::CancelEventHandler(this, &MainForm::sceneTreeMenu_Opening);
			// 
			// sceneTreeMenuCreate
			// 
			this->sceneTreeMenuCreate->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->emitterToolStripMenuItem, 
				this->trailToolStripMenuItem, this->lightToolStripMenuItem, this->effectGroupToolStripMenuItem});
			this->sceneTreeMenuCreate->Name = L"sceneTreeMenuCreate";
			this->sceneTreeMenuCreate->Size = System::Drawing::Size(209, 22);
			this->sceneTreeMenuCreate->Text = L"Create";
			// 
			// emitterToolStripMenuItem
			// 
			this->emitterToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->sphereToolStripMenuItem, 
				this->boxToolStripMenuItem, this->ringToolStripMenuItem});
			this->emitterToolStripMenuItem->Name = L"emitterToolStripMenuItem";
			this->emitterToolStripMenuItem->Size = System::Drawing::Size(137, 22);
			this->emitterToolStripMenuItem->Text = L"Emitter";
			// 
			// sphereToolStripMenuItem
			// 
			this->sphereToolStripMenuItem->Name = L"sphereToolStripMenuItem";
			this->sphereToolStripMenuItem->Size = System::Drawing::Size(111, 22);
			this->sphereToolStripMenuItem->Text = L"Sphere";
			this->sphereToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::sphereToolStripMenuItem_Click);
			// 
			// boxToolStripMenuItem
			// 
			this->boxToolStripMenuItem->Name = L"boxToolStripMenuItem";
			this->boxToolStripMenuItem->Size = System::Drawing::Size(111, 22);
			this->boxToolStripMenuItem->Text = L"Box";
			this->boxToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::boxToolStripMenuItem_Click);
			// 
			// ringToolStripMenuItem
			// 
			this->ringToolStripMenuItem->Name = L"ringToolStripMenuItem";
			this->ringToolStripMenuItem->Size = System::Drawing::Size(111, 22);
			this->ringToolStripMenuItem->Text = L"Ring";
			this->ringToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::ringToolStripMenuItem_Click);
			// 
			// trailToolStripMenuItem
			// 
			this->trailToolStripMenuItem->Name = L"trailToolStripMenuItem";
			this->trailToolStripMenuItem->Size = System::Drawing::Size(137, 22);
			this->trailToolStripMenuItem->Text = L"Trail";
			this->trailToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::trailToolStripMenuItem_Click);
			// 
			// lightToolStripMenuItem
			// 
			this->lightToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->pointLightToolStripMenuItem, 
				this->spotLightToolStripMenuItem});
			this->lightToolStripMenuItem->Name = L"lightToolStripMenuItem";
			this->lightToolStripMenuItem->Size = System::Drawing::Size(137, 22);
			this->lightToolStripMenuItem->Text = L"Light";
			// 
			// pointLightToolStripMenuItem
			// 
			this->pointLightToolStripMenuItem->Name = L"pointLightToolStripMenuItem";
			this->pointLightToolStripMenuItem->Size = System::Drawing::Size(129, 22);
			this->pointLightToolStripMenuItem->Text = L"PointLight";
			this->pointLightToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::pointLightToolStripMenuItem_Click);
			// 
			// spotLightToolStripMenuItem
			// 
			this->spotLightToolStripMenuItem->Name = L"spotLightToolStripMenuItem";
			this->spotLightToolStripMenuItem->Size = System::Drawing::Size(129, 22);
			this->spotLightToolStripMenuItem->Text = L"SpotLight";
			this->spotLightToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::spotLightToolStripMenuItem_Click);
			// 
			// effectGroupToolStripMenuItem
			// 
			this->effectGroupToolStripMenuItem->Name = L"effectGroupToolStripMenuItem";
			this->effectGroupToolStripMenuItem->Size = System::Drawing::Size(137, 22);
			this->effectGroupToolStripMenuItem->Text = L"EffectGroup";
			this->effectGroupToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::effectGroupToolStripMenuItem_Click);
			// 
			// changeTypeToolStripMenuItem
			// 
			this->changeTypeToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->ChangeEmitterTypeSphereToolStripMenuItem, 
				this->ChangeEmitterTypeBoxToolStripMenuItem1, this->ChangeEmitterTypeRingToolStripMenuItem1});
			this->changeTypeToolStripMenuItem->Name = L"changeTypeToolStripMenuItem";
			this->changeTypeToolStripMenuItem->Size = System::Drawing::Size(209, 22);
			this->changeTypeToolStripMenuItem->Text = L"Change type";
			// 
			// ChangeEmitterTypeSphereToolStripMenuItem
			// 
			this->ChangeEmitterTypeSphereToolStripMenuItem->Name = L"ChangeEmitterTypeSphereToolStripMenuItem";
			this->ChangeEmitterTypeSphereToolStripMenuItem->Size = System::Drawing::Size(111, 22);
			this->ChangeEmitterTypeSphereToolStripMenuItem->Text = L"Sphere";
			this->ChangeEmitterTypeSphereToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::ChangeEmitterTypeSphereToolStripMenuItem_Click);
			// 
			// ChangeEmitterTypeBoxToolStripMenuItem1
			// 
			this->ChangeEmitterTypeBoxToolStripMenuItem1->Name = L"ChangeEmitterTypeBoxToolStripMenuItem1";
			this->ChangeEmitterTypeBoxToolStripMenuItem1->Size = System::Drawing::Size(111, 22);
			this->ChangeEmitterTypeBoxToolStripMenuItem1->Text = L"Box";
			this->ChangeEmitterTypeBoxToolStripMenuItem1->Click += gcnew System::EventHandler(this, &MainForm::ChangeEmitterTypeBoxToolStripMenuItem1_Click);
			// 
			// ChangeEmitterTypeRingToolStripMenuItem1
			// 
			this->ChangeEmitterTypeRingToolStripMenuItem1->Name = L"ChangeEmitterTypeRingToolStripMenuItem1";
			this->ChangeEmitterTypeRingToolStripMenuItem1->Size = System::Drawing::Size(111, 22);
			this->ChangeEmitterTypeRingToolStripMenuItem1->Text = L"Ring";
			this->ChangeEmitterTypeRingToolStripMenuItem1->Click += gcnew System::EventHandler(this, &MainForm::ChangeEmitterTypeRingToolStripMenuItem1_Click);
			// 
			// CopyToolStripMenuItem
			// 
			this->CopyToolStripMenuItem->Name = L"CopyToolStripMenuItem";
			this->CopyToolStripMenuItem->Size = System::Drawing::Size(209, 22);
			this->CopyToolStripMenuItem->Text = L"Copy";
			this->CopyToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::CopyToolStripMenuItem_Click);
			// 
			// PasteToolStripMenuItem
			// 
			this->PasteToolStripMenuItem->Name = L"PasteToolStripMenuItem";
			this->PasteToolStripMenuItem->Size = System::Drawing::Size(209, 22);
			this->PasteToolStripMenuItem->Text = L"Paste";
			this->PasteToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::PasteToolStripMenuItem_Click);
			// 
			// sceneTreeMenuImport
			// 
			this->sceneTreeMenuImport->Name = L"sceneTreeMenuImport";
			this->sceneTreeMenuImport->Size = System::Drawing::Size(209, 22);
			this->sceneTreeMenuImport->Text = L"Import";
			this->sceneTreeMenuImport->Click += gcnew System::EventHandler(this, &MainForm::sceneTreeMenuImport_Click);
			// 
			// sceneTreeMenuDelete
			// 
			this->sceneTreeMenuDelete->Name = L"sceneTreeMenuDelete";
			this->sceneTreeMenuDelete->Size = System::Drawing::Size(209, 22);
			this->sceneTreeMenuDelete->Text = L"Delete";
			this->sceneTreeMenuDelete->Click += gcnew System::EventHandler(this, &MainForm::sceneTreeMenuDelete_Click);
			// 
			// propertyToolStripMenuItem
			// 
			this->propertyToolStripMenuItem->Name = L"propertyToolStripMenuItem";
			this->propertyToolStripMenuItem->Size = System::Drawing::Size(209, 22);
			this->propertyToolStripMenuItem->Text = L"Property";
			this->propertyToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::propertyToolStripMenuItem_Click);
			// 
			// applyAllSameNamedToolStripMenuItem
			// 
			this->applyAllSameNamedToolStripMenuItem->Name = L"applyAllSameNamedToolStripMenuItem";
			this->applyAllSameNamedToolStripMenuItem->Size = System::Drawing::Size(209, 22);
			this->applyAllSameNamedToolStripMenuItem->Text = L"Apply to all same named";
			this->applyAllSameNamedToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::applyAllSameNamedToolStripMenuItem_Click);
			// 
			// moveUpToolStripMenuItem
			// 
			this->moveUpToolStripMenuItem->Name = L"moveUpToolStripMenuItem";
			this->moveUpToolStripMenuItem->Size = System::Drawing::Size(209, 22);
			this->moveUpToolStripMenuItem->Text = L"MoveUp";
			this->moveUpToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::moveUpToolStripMenuItem_Click);
			// 
			// moveDownToolStripMenuItem
			// 
			this->moveDownToolStripMenuItem->Name = L"moveDownToolStripMenuItem";
			this->moveDownToolStripMenuItem->Size = System::Drawing::Size(209, 22);
			this->moveDownToolStripMenuItem->Text = L"MoveDown";
			this->moveDownToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::moveDownToolStripMenuItem_Click);
			// 
			// imageListSceneNodeType
			// 
			this->imageListSceneNodeType->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"imageListSceneNodeType.ImageStream")));
			this->imageListSceneNodeType->TransparentColor = System::Drawing::Color::Transparent;
			this->imageListSceneNodeType->Images->SetKeyName(0, L"effect.png");
			this->imageListSceneNodeType->Images->SetKeyName(1, L"actor.png");
			this->imageListSceneNodeType->Images->SetKeyName(2, L"actorNode.png");
			this->imageListSceneNodeType->Images->SetKeyName(3, L"trail.png");
			this->imageListSceneNodeType->Images->SetKeyName(4, L"sphere.png");
			this->imageListSceneNodeType->Images->SetKeyName(5, L"box.png");
			this->imageListSceneNodeType->Images->SetKeyName(6, L"ring.png");
			this->imageListSceneNodeType->Images->SetKeyName(7, L"Light.png");
			this->imageListSceneNodeType->Images->SetKeyName(8, L"Spotlight.png");
			// 
			// LogtextBox
			// 
			this->LogtextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->LogtextBox->Location = System::Drawing::Point(3, 799);
			this->LogtextBox->Multiline = true;
			this->LogtextBox->Name = L"LogtextBox";
			this->LogtextBox->ReadOnly = true;
			this->LogtextBox->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->LogtextBox->Size = System::Drawing::Size(899, 79);
			this->LogtextBox->TabIndex = 16;
			// 
			// splitContainer2
			// 
			this->splitContainer2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer2->Location = System::Drawing::Point(0, 0);
			this->splitContainer2->Name = L"splitContainer2";
			this->splitContainer2->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer2.Panel1
			// 
			this->splitContainer2->Panel1->Controls->Add(this->tabControl2);
			// 
			// splitContainer2.Panel2
			// 
			this->splitContainer2->Panel2->Controls->Add(this->IPanel1);
			this->splitContainer2->Size = System::Drawing::Size(249, 903);
			this->splitContainer2->SplitterDistance = 267;
			this->splitContainer2->TabIndex = 98;
			// 
			// IPanel1
			// 
			this->IPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->IPanel1->Location = System::Drawing::Point(0, 0);
			this->IPanel1->Margin = System::Windows::Forms::Padding(1);
			this->IPanel1->Name = L"IPanel1";
			this->IPanel1->Size = System::Drawing::Size(245, 628);
			this->IPanel1->TabIndex = 0;
			// 
			// splitContainer3
			// 
			this->splitContainer3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer3->FixedPanel = System::Windows::Forms::FixedPanel::Panel2;
			this->splitContainer3->Location = System::Drawing::Point(0, 0);
			this->splitContainer3->Name = L"splitContainer3";
			// 
			// splitContainer3.Panel1
			// 
			this->splitContainer3->Panel1->Controls->Add(this->MainView);
			this->splitContainer3->Panel1->Controls->Add(this->groupBox3);
			this->splitContainer3->Panel1->Controls->Add(this->LogtextBox);
			// 
			// splitContainer3.Panel2
			// 
			this->splitContainer3->Panel2->Controls->Add(this->splitContainer2);
			this->splitContainer3->Size = System::Drawing::Size(1158, 903);
			this->splitContainer3->SplitterDistance = 905;
			this->splitContainer3->TabIndex = 99;
			// 
			// menuStrip
			// 
			this->menuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->FileToolStripMenuItem, 
				this->ToolToolStripMenuItem, 라이팅LToolStripMenuItem, this->HelpToolStripMenuItem});
			this->menuStrip->Location = System::Drawing::Point(0, 0);
			this->menuStrip->Name = L"menuStrip";
			this->menuStrip->Size = System::Drawing::Size(1158, 24);
			this->menuStrip->TabIndex = 100;
			this->menuStrip->Text = L"menuStrip";
			// 
			// FileToolStripMenuItem
			// 
			this->FileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(11) {this->NewToolStripMenuItem, 
				this->OpenToolStripMenuItem, this->다시열기RToolStripMenuItem, this->eluOpenToolStripMenuItem, this->MRUEluFileListToolStripMenuItem, 
				toolStripSeparator2, this->SaveToolStripMenuItem, this->SaveAsToolStripMenuItem, this->EmitterName_AutoUpdate, toolStripSeparator4, 
				this->eXitToolStripMenuItem});
			this->FileToolStripMenuItem->Name = L"FileToolStripMenuItem";
			this->FileToolStripMenuItem->Size = System::Drawing::Size(57, 20);
			this->FileToolStripMenuItem->Text = L"파일(&F)";
			// 
			// NewToolStripMenuItem
			// 
			this->NewToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"NewToolStripMenuItem.Image")));
			this->NewToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->NewToolStripMenuItem->Name = L"NewToolStripMenuItem";
			this->NewToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::N));
			this->NewToolStripMenuItem->Size = System::Drawing::Size(220, 22);
			this->NewToolStripMenuItem->Text = L"새로 만들기(&N)";
			this->NewToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::NewToolStripMenuItem_Click);
			// 
			// OpenToolStripMenuItem
			// 
			this->OpenToolStripMenuItem->Enabled = false;
			this->OpenToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"OpenToolStripMenuItem.Image")));
			this->OpenToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->OpenToolStripMenuItem->Name = L"OpenToolStripMenuItem";
			this->OpenToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->OpenToolStripMenuItem->Size = System::Drawing::Size(220, 22);
			this->OpenToolStripMenuItem->Text = L"열기(&O)";
			// 
			// 다시열기RToolStripMenuItem
			// 
			this->다시열기RToolStripMenuItem->Name = L"다시열기RToolStripMenuItem";
			this->다시열기RToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::R));
			this->다시열기RToolStripMenuItem->Size = System::Drawing::Size(220, 22);
			this->다시열기RToolStripMenuItem->Text = L"다시열기(&R)";
			this->다시열기RToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::다시열기RToolStripMenuItem_Click);
			// 
			// eluOpenToolStripMenuItem
			// 
			this->eluOpenToolStripMenuItem->Name = L"eluOpenToolStripMenuItem";
			this->eluOpenToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::M));
			this->eluOpenToolStripMenuItem->Size = System::Drawing::Size(220, 22);
			this->eluOpenToolStripMenuItem->Text = L"모델파일 열기(&M)";
			this->eluOpenToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::eluOpenToolStripMenuItem_Click);
			// 
			// MRUEluFileListToolStripMenuItem
			// 
			this->MRUEluFileListToolStripMenuItem->Name = L"MRUEluFileListToolStripMenuItem";
			this->MRUEluFileListToolStripMenuItem->Size = System::Drawing::Size(220, 22);
			this->MRUEluFileListToolStripMenuItem->Text = L"최근에 사용한 모델 파일(&J)";
			// 
			// toolStripSeparator2
			// 
			toolStripSeparator2->Name = L"toolStripSeparator2";
			toolStripSeparator2->Size = System::Drawing::Size(217, 6);
			// 
			// SaveToolStripMenuItem
			// 
			this->SaveToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"SaveToolStripMenuItem.Image")));
			this->SaveToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->SaveToolStripMenuItem->Name = L"SaveToolStripMenuItem";
			this->SaveToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->SaveToolStripMenuItem->Size = System::Drawing::Size(220, 22);
			this->SaveToolStripMenuItem->Text = L"저장(&S)";
			this->SaveToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::SaveToolStripMenuItem_Click);
			// 
			// SaveAsToolStripMenuItem
			// 
			this->SaveAsToolStripMenuItem->Name = L"SaveAsToolStripMenuItem";
			this->SaveAsToolStripMenuItem->Size = System::Drawing::Size(220, 22);
			this->SaveAsToolStripMenuItem->Text = L"다른 이름으로 저장(&A)";
			this->SaveAsToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::SaveasToolStripMenuItem_Click);
			// 
			// EmitterName_AutoUpdate
			// 
			this->EmitterName_AutoUpdate->Checked = true;
			this->EmitterName_AutoUpdate->CheckOnClick = true;
			this->EmitterName_AutoUpdate->CheckState = System::Windows::Forms::CheckState::Checked;
			this->EmitterName_AutoUpdate->Name = L"EmitterName_AutoUpdate";
			this->EmitterName_AutoUpdate->Size = System::Drawing::Size(220, 22);
			this->EmitterName_AutoUpdate->Text = L"Emitter 이름 자동 업데이트";
			this->EmitterName_AutoUpdate->Click += gcnew System::EventHandler(this, &MainForm::EmitterName_AutoUpdate_Click);
			// 
			// eXitToolStripMenuItem
			// 
			this->eXitToolStripMenuItem->Name = L"eXitToolStripMenuItem";
			this->eXitToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::X));
			this->eXitToolStripMenuItem->Size = System::Drawing::Size(220, 22);
			this->eXitToolStripMenuItem->Text = L"끝내기(&X)";
			this->eXitToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::eXitToolStripMenuItem_Click);
			// 
			// ToolToolStripMenuItem
			// 
			this->ToolToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->toolStripCameraReset, 
				this->backgroundColorToolStripMenuItem, this->fogEnableToolStripMenuItem, this->FeedBackToolStripMenuItem, this->GoMDNToolStripMenuItem});
			this->ToolToolStripMenuItem->Name = L"ToolToolStripMenuItem";
			this->ToolToolStripMenuItem->Size = System::Drawing::Size(57, 20);
			this->ToolToolStripMenuItem->Text = L"도구(&T)";
			// 
			// toolStripCameraReset
			// 
			this->toolStripCameraReset->Name = L"toolStripCameraReset";
			this->toolStripCameraReset->Size = System::Drawing::Size(171, 22);
			this->toolStripCameraReset->Text = L"카메라 리셋";
			this->toolStripCameraReset->Click += gcnew System::EventHandler(this, &MainForm::toolStripCameraReset_Click);
			// 
			// backgroundColorToolStripMenuItem
			// 
			this->backgroundColorToolStripMenuItem->Name = L"backgroundColorToolStripMenuItem";
			this->backgroundColorToolStripMenuItem->Size = System::Drawing::Size(171, 22);
			this->backgroundColorToolStripMenuItem->Text = L"Background Color";
			this->backgroundColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::backgroundColorToolStripMenuItem_Click);
			// 
			// fogEnableToolStripMenuItem
			// 
			this->fogEnableToolStripMenuItem->CheckOnClick = true;
			this->fogEnableToolStripMenuItem->Name = L"fogEnableToolStripMenuItem";
			this->fogEnableToolStripMenuItem->Size = System::Drawing::Size(171, 22);
			this->fogEnableToolStripMenuItem->Text = L"Fog Enable";
			this->fogEnableToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::fogEnableToolStripMenuItem_Click);
			// 
			// FeedBackToolStripMenuItem
			// 
			this->FeedBackToolStripMenuItem->Name = L"FeedBackToolStripMenuItem";
			this->FeedBackToolStripMenuItem->Size = System::Drawing::Size(171, 22);
			this->FeedBackToolStripMenuItem->Text = L"Trac 홈페이지";
			this->FeedBackToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::FeedBackToolStripMenuItem_Click);
			// 
			// GoMDNToolStripMenuItem
			// 
			this->GoMDNToolStripMenuItem->Name = L"GoMDNToolStripMenuItem";
			this->GoMDNToolStripMenuItem->Size = System::Drawing::Size(171, 22);
			this->GoMDNToolStripMenuItem->Text = L"MDN으로 가기";
			this->GoMDNToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::GoMDNToolStripMenuItem_Click);
			// 
			// HelpToolStripMenuItem
			// 
			this->HelpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->InformationToolStripMenuItem});
			this->HelpToolStripMenuItem->Name = L"HelpToolStripMenuItem";
			this->HelpToolStripMenuItem->Size = System::Drawing::Size(72, 20);
			this->HelpToolStripMenuItem->Text = L"도움말(&H)";
			// 
			// InformationToolStripMenuItem
			// 
			this->InformationToolStripMenuItem->Name = L"InformationToolStripMenuItem";
			this->InformationToolStripMenuItem->Size = System::Drawing::Size(123, 22);
			this->InformationToolStripMenuItem->Text = L"정보(&A)...";
			this->InformationToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::InformationToolStripMenuItem_Click);
			// 
			// toolStrip1
			// 
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(11) {this->NewToolStripButton, 
				this->OpenToolStripButton, this->SaveToolStripButton, this->toolStripSeparator, this->GridtoolStripButton, this->AxistoolStripButton, 
				this->ShowEmitterRangetoolStripButton1, this->ShowLightRangetoolStripButton, this->PathtoolStripButton, this->toolStripSeparator1, 
				this->HelpToolStripButton});
			this->toolStrip1->Location = System::Drawing::Point(0, 24);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(1158, 25);
			this->toolStrip1->TabIndex = 101;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// NewToolStripButton
			// 
			this->NewToolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->NewToolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"NewToolStripButton.Image")));
			this->NewToolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->NewToolStripButton->Name = L"NewToolStripButton";
			this->NewToolStripButton->Size = System::Drawing::Size(23, 22);
			this->NewToolStripButton->Text = L"새로 만들기(&N)";
			this->NewToolStripButton->Click += gcnew System::EventHandler(this, &MainForm::NewToolStripMenuItem_Click);
			// 
			// OpenToolStripButton
			// 
			this->OpenToolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->OpenToolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"OpenToolStripButton.Image")));
			this->OpenToolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->OpenToolStripButton->Name = L"OpenToolStripButton";
			this->OpenToolStripButton->Size = System::Drawing::Size(23, 22);
			this->OpenToolStripButton->Text = L"열기(&O)";
			this->OpenToolStripButton->Click += gcnew System::EventHandler(this, &MainForm::NewToolStripMenuItem_Click);
			// 
			// SaveToolStripButton
			// 
			this->SaveToolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->SaveToolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"SaveToolStripButton.Image")));
			this->SaveToolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->SaveToolStripButton->Name = L"SaveToolStripButton";
			this->SaveToolStripButton->Size = System::Drawing::Size(23, 22);
			this->SaveToolStripButton->Text = L"저장(&S)";
			this->SaveToolStripButton->Click += gcnew System::EventHandler(this, &MainForm::SaveasToolStripMenuItem_Click);
			// 
			// toolStripSeparator
			// 
			this->toolStripSeparator->Name = L"toolStripSeparator";
			this->toolStripSeparator->Size = System::Drawing::Size(6, 25);
			// 
			// GridtoolStripButton
			// 
			this->GridtoolStripButton->CheckOnClick = true;
			this->GridtoolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->GridtoolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"GridtoolStripButton.Image")));
			this->GridtoolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->GridtoolStripButton->Name = L"GridtoolStripButton";
			this->GridtoolStripButton->Size = System::Drawing::Size(23, 22);
			this->GridtoolStripButton->Text = L"toolStripButton1";
			this->GridtoolStripButton->ToolTipText = L"그리드 on/off";
			this->GridtoolStripButton->Click += gcnew System::EventHandler(this, &MainForm::GridtoolStripButton_Click);
			// 
			// AxistoolStripButton
			// 
			this->AxistoolStripButton->CheckOnClick = true;
			this->AxistoolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->AxistoolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"AxistoolStripButton.Image")));
			this->AxistoolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->AxistoolStripButton->Name = L"AxistoolStripButton";
			this->AxistoolStripButton->Size = System::Drawing::Size(23, 22);
			this->AxistoolStripButton->Text = L"toolStripButton1";
			this->AxistoolStripButton->ToolTipText = L"좌표축을 on/off 합니다.";
			this->AxistoolStripButton->Click += gcnew System::EventHandler(this, &MainForm::AxistoolStripButton_Click);
			// 
			// ShowEmitterRangetoolStripButton1
			// 
			this->ShowEmitterRangetoolStripButton1->CheckOnClick = true;
			this->ShowEmitterRangetoolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->ShowEmitterRangetoolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"ShowEmitterRangetoolStripButton1.Image")));
			this->ShowEmitterRangetoolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->ShowEmitterRangetoolStripButton1->Name = L"ShowEmitterRangetoolStripButton1";
			this->ShowEmitterRangetoolStripButton1->Size = System::Drawing::Size(23, 22);
			this->ShowEmitterRangetoolStripButton1->Text = L"toolStripButton1";
			this->ShowEmitterRangetoolStripButton1->ToolTipText = L"이미터 보기.";
			this->ShowEmitterRangetoolStripButton1->Click += gcnew System::EventHandler(this, &MainForm::ShowEmitterRangetoolStripButton1_Click);
			// 
			// ShowLightRangetoolStripButton
			// 
			this->ShowLightRangetoolStripButton->CheckOnClick = true;
			this->ShowLightRangetoolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->ShowLightRangetoolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"ShowLightRangetoolStripButton.Image")));
			this->ShowLightRangetoolStripButton->ImageTransparentColor = System::Drawing::Color::White;
			this->ShowLightRangetoolStripButton->Name = L"ShowLightRangetoolStripButton";
			this->ShowLightRangetoolStripButton->Size = System::Drawing::Size(23, 22);
			this->ShowLightRangetoolStripButton->Text = L"toolStripButton1";
			this->ShowLightRangetoolStripButton->ToolTipText = L"Light 범위 보기";
			this->ShowLightRangetoolStripButton->Click += gcnew System::EventHandler(this, &MainForm::ShowLightRangetoolStripButton_Click);
			// 
			// PathtoolStripButton
			// 
			this->PathtoolStripButton->CheckOnClick = true;
			this->PathtoolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->PathtoolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"PathtoolStripButton.Image")));
			this->PathtoolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->PathtoolStripButton->Name = L"PathtoolStripButton";
			this->PathtoolStripButton->Size = System::Drawing::Size(23, 22);
			this->PathtoolStripButton->Text = L"toolStripButton1";
			this->PathtoolStripButton->ToolTipText = L"이미터가 움직이는 길을 보여준다.";
			this->PathtoolStripButton->Click += gcnew System::EventHandler(this, &MainForm::PathtoolStripButton_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(6, 25);
			// 
			// HelpToolStripButton
			// 
			this->HelpToolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->HelpToolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"HelpToolStripButton.Image")));
			this->HelpToolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->HelpToolStripButton->Name = L"HelpToolStripButton";
			this->HelpToolStripButton->Size = System::Drawing::Size(23, 22);
			this->HelpToolStripButton->Text = L"도움말(&L)";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1158, 903);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->menuStrip);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->splitContainer3);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"MainForm";
			this->Text = L"EffectTool_NET";
			this->Deactivate += gcnew System::EventHandler(this, &MainForm::MainForm_OnDeactivated);
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::MainForm_MouseWheel);
			this->Activated += gcnew System::EventHandler(this, &MainForm::MainForm_OnActivated);
			this->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::MainForm_DragDrop);
			this->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::MainForm_DragEnter);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MainForm::MainForm_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MainView))->EndInit();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TimeTrackBar))->EndInit();
			this->tabControl2->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->sceneTreeMenu->ResumeLayout(false);
			this->splitContainer2->Panel1->ResumeLayout(false);
			this->splitContainer2->Panel2->ResumeLayout(false);
			this->splitContainer2->ResumeLayout(false);
			this->splitContainer3->Panel1->ResumeLayout(false);
			this->splitContainer3->Panel1->PerformLayout();
			this->splitContainer3->Panel2->ResumeLayout(false);
			this->splitContainer3->ResumeLayout(false);
			this->menuStrip->ResumeLayout(false);
			this->menuStrip->PerformLayout();
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e) {
				 assert(g_pMainApp==NULL);
				 HWND hWnd = (HWND)(MainView->Handle.ToPointer());
				 assert(hWnd!=NULL);

				 g_pMainApp = new CMainApplication;
				 if ( !g_pMainApp->Init(hWnd))
				 {
					 Close();
					 return;
				 }

				 g_pMainApp->InitState(MainView->Width,MainView->Height);
				 this->Bounds = System::Drawing::Rectangle(System::Drawing::Point(CToolConfig::GetInstance().start_X, CToolConfig::GetInstance().start_Y),
					 System::Drawing::Size(CToolConfig::GetInstance().WinForm_width, CToolConfig::GetInstance().WinForm_height));

				 // config 내용 반영해서 초기화
				 if( CToolConfig::GetInstance().maximize)
					 this->WindowState = FormWindowState::Maximized;

				 RefreshMRUEluFileList();

				 // 기본 애니메이션 모델 로드
				 g_pMainApp->DefaultMeshFileOpen();
			 }

	public: System::Void MainLoop() {
				if( this->ActiveForm)
					g_pMainApp->OnUpdateKeys();

				g_pMainApp->OnUpdate();
				if ( !g_pMainApp->OnDraw())
				{
					g_pMainApp->OnLostDevice();
					g_pMainApp->OnResetDevice();
				}

				//탭 구분
				String ^tabText = childTabForm->tabControl1->SelectedTab->Text;
				if ( tabText == L"Emitter" )
				{
					TimeTrackBar_Display();
				}
				else if ( tabText == L"Particle" )
				{
					ParticleTimeTrackBar_Display();
				}
			}

	private: System::Void TimeTrackBar_Display() {
				 if(g_pMainApp->GetSelectedEmitter())
				 {
					 float mf = g_pMainApp->GetSelectedEmitter()->GetEmitLife() * 30;
					 TimeTrackBar->SetRange(0, (int)ceilf(mf));

					 EmitterLifelabel->Text = String::Format("Emitter Life : {0}/{1}(sec:{2})",
						 TimeTrackBar->Value, TimeTrackBar->Maximum, g_pMainApp->GetSelectedEmitter()->GetEmitLife());
				 }
			 }
	private: System::Void ParticleTimeTrackBar_Display() {
				 if(g_pMainApp->GetSelectedEmitter())
				 {
					 float mf = g_pMainApp->GetSelectedEmitter()->GetParticleLife().m_Max * 30;
					 TimeTrackBar->SetRange(0, (int)ceilf(mf));

					 EmitterLifelabel->Text = String::Format("Particle Life : {0}/{1}(sec:{2})",
						 TimeTrackBar->Value, TimeTrackBar->Maximum, g_pMainApp->GetSelectedEmitter()->GetParticleLife().m_Max);
				 }
			 }
	private: System::Void TimeTrackBar_PlayPosition() {
				 // 사용되지 않음.
			 }

	private: System::Void MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {

				 // 상태 저장
				 CToolConfig::GetInstance().start_X = this->Bounds.X;
				 CToolConfig::GetInstance().start_Y = this->Bounds.Y;
				 CToolConfig::GetInstance().maximize = ( this->WindowState == FormWindowState::Maximized ? true : false );
				 CToolConfig::GetInstance().WinForm_width = this->Bounds.Width;
				 CToolConfig::GetInstance().WinForm_height= this->Bounds.Height;
				 CToolConfig::GetInstance().split2 = this->splitContainer2->SplitterDistance;
				 CToolConfig::GetInstance().split3 = this->splitContainer3->SplitterDistance;
				 CToolConfig::GetInstance().SaveConfig();

				 if( ConfirmSave())	// 취소 추가 - 090603, OZ
				 {
					 assert(g_pMainApp!=NULL);
					 SAFE_DELETE(g_pMainApp);
				 }
				 else
				 {
					 // by pok. 만일 창 닫는것을 취소하고 싶다면.
					 e->Cancel = true;
					 //return;
				 }
			 }

			 // madduck [3/2/2007] - 클릭할때마다 들어온다. 
	private: System::Void EffectListtreeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {

				 //저장확인
				 if( ConfirmSave()) // 090603, OZ
				 {
					 if( e->Node->Parent != nullptr )
					 {
						 String^ parentName(e->Node->Parent->Text);
						 return;
					 }
					 String^ nodeName(e->Node->Text);
					 String^ FileName = String::Format("{0}", nodeName);

					 //effectmgr에서 로드 하도록
					 bool bOldModifiedInfo = g_bModified;
					 g_pMainApp->LoadEffectFile(FileName);
					 g_bModified = bOldModifiedInfo;	// 이때 뭔가 편집이 바뀌는것은 아니다.
				 }
			 }
	private: System::Void MainView_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {

				 Point mouseDownLocation = Point(e->X,e->Y);
				 int istat = 0;
				 switch ( e->Button )
				 {
				 case System::Windows::Forms::MouseButtons::Left:
					 istat = 1;
					 break;
				 case System::Windows::Forms::MouseButtons::Right:
					 istat = 2;
					 break;
				 case System::Windows::Forms::MouseButtons::Middle:
					 istat = 3;
					 break;
				 case System::Windows::Forms::MouseButtons::XButton1:
					 istat = 4;
					 break;
				 case System::Windows::Forms::MouseButtons::XButton2:
					 istat = 5;
					 break;
				 case System::Windows::Forms::MouseButtons::None:
				 default:
					 break;
				 }

				 g_pMainApp->OnMouseMove(istat, RVector2((float)e->X, (float)e->Y), e->Delta);
			 }
	private: System::Void MainForm_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 g_pMainApp->OnMouseMove(0, RVector2(0,0), e->Delta);
			 }
	private: System::Void MainForm_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
				 bool dragObjectIsMyClass = (e->Data->GetDataPresent(DataFormats::FileDrop) == true);
				 if (dragObjectIsMyClass)
				 {
					 e->Effect = DragDropEffects::Copy;
				 }
			 }
	private: System::Void MainForm_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
				 System::Array^ a = (System::Array^)e->Data->GetData(DataFormats::FileDrop);

				 int count = 0;
				 while(a->GetValue(count))
				 {
					 String^ strFileName = a->GetValue(count)->ToString();
					 count++;
				 }
			 }
	private: System::Void TimeTrackBar_Scroll(System::Object^  sender, System::EventArgs^  e) {
				 //탭구분
				 String ^tabText = childTabForm->tabControl1->SelectedTab->Text;
				 if ( tabText == L"Emitter" )
					 childTabForm->EmitterTimePosnumericUpDown->Value = TimeTrackBar->Value;
				 else if ( tabText == L"Particle" )
					 childTabForm->ParticleTimePosnumericUpDown->Value = TimeTrackBar->Value;

				 GlobalObjects::g_fTrackBarValue = Decimal::ToSingle(TimeTrackBar->Value);
			 }
	private: System::Void stopbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->StopButton();
				 TimeTrackBar->Value = 0;
			 }
	private: System::Void Startbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->StartButton();
			 }
	private: System::Void Pausebutton_Click(System::Object^  sender, System::EventArgs^  e) {		
				 g_pMainApp->PauseButton();
			 }

	private: System::Void SaveToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 if( g_pMainApp->GetCurrentEffectSceneNode() )
				 {
					 g_pMainApp->SaveEffectFile();
					 g_bModified = false;
				 }
			 }
	private: System::Void SaveasToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->SaveAs();
			 }

	private: System::Void backgroundColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 ColorDialog^ colorDialog = gcnew System::Windows::Forms::ColorDialog();
				 if(colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 g_pMainApp->SetClearColor(D3DCOLOR_ARGB(colorDialog->Color.A,colorDialog->Color.R,colorDialog->Color.G,colorDialog->Color.B));
				 }
			 }

	private: System::Void eluOpenToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

				 openFileDialog1->Filter = "xml files (*.xml)|*.xml|elu files (*.elu)|*.elu|ani files (*.ani)|*.ani|All files (*.*)|*.*";
				 openFileDialog1->FilterIndex = 2;
				 openFileDialog1->RestoreDirectory = true;
				 openFileDialog1->Multiselect		= true;

				 openFileDialog1->InitialDirectory = gcnew String( CToolConfig::GetInstance().m_strEluPath.c_str() );

				 if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 String^ FileName = openFileDialog1->FileName;	//path까지 다 들어온다.
					 g_pMainApp->MeshFileOpen(FileName);
				 }

				 delete openFileDialog1;
			 }
	private: System::Void MainView_Resize(System::Object^  sender, System::EventArgs^  e) {
				 REngine::GetConfig().m_nWidth = MainView->Width;
				 REngine::GetConfig().m_nHeight = MainView->Height;

				 if(g_pMainApp)
				 {
					 g_pMainApp->InitState(MainView->Width,MainView->Height, false);
					 g_pMainApp->OnResetDevice();
				 }
			 }
	private: System::Void MainView_Click(System::Object^  sender, System::EventArgs^  e) {
				 MainView->Focus();
			 }
	private: System::Void eXitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 Close();
			 }
	private: System::Void GridtoolStripButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->m_bGrid = !g_pMainApp->m_bGrid;
			 }
	private: System::Void AxistoolStripButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->m_bAxis = !g_pMainApp->m_bAxis;
			 }
	private: System::Void ShowEmitterRangetoolStripButton1_Click(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->m_bShowEmitter = !g_pMainApp->m_bShowEmitter;
			 }
	private: System::Void NodeListtreeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
				 g_pMainApp->SelectedNode(e->Node->Text);
			 }
	private: System::Void AniLoopcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->OnLoopAnimation();
			 }

	private: System::Void PathtoolStripButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 if( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter())
					 pEmitter->m_bPath = !g_pMainApp->GetSelectedEmitter()->m_bPath;
			 }
	private: System::Void NewToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 // 새로 하기 전 다이어로그를.. - 090602, OZ(Engine Ticket #650)
				 if( g_pMainApp->ConfirmSave())
				 {
					g_pMainApp->OnNew();
				 }
			 }
	private: System::Void GoMDNToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 System::Diagnostics::Process::Start("IExplore.exe", "http://iworks.maiet.net/mdn/wiki.php/EffectTool_Net");
			 }
	private: System::Void FeedBackToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 System::Diagnostics::Process::Start("IExplore.exe", "http://dev:8181/projects/engine");
			 }
	
	private: System::Void EffectNametextBox_Leave(System::Object^  sender, System::EventArgs^  e) {
			 }

	private: System::Void EmitterTypecomboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 // 딱히 해주는건 없고 생성시 type으로 건낼 멤버만 바꿔준다.
			 }
	private: System::Void toolStripCameraReset_Click(System::Object^  sender, System::EventArgs^  e) {
				 assert( g_pMainApp != NULL );
				 g_pMainApp->CameraReset();
			 }

	//private: System::Void ConfirmSave() {
	private: bool ConfirmSave() {
				 return g_pMainApp->ConfirmSave();
			 }

	private: System::Void InformationToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 About^ aboutDialog = gcnew About();
				 aboutDialog->ShowDialog();
			 }
	private: System::Void fogEnableToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 if ( fogEnableToolStripMenuItem->Checked )
					 g_pMainApp->SetFogSettingEnable(true);
				 else
					 g_pMainApp->SetFogSettingEnable(false);
			 }

			 public: bool m_bActive;
private: System::Void MainForm_OnActivated(System::Object^  sender, System::EventArgs^  e) {
			 m_bActive = true;
		 }
private: System::Void MainForm_OnDeactivated(System::Object^  sender, System::EventArgs^  e) {
			 m_bActive = false;
		 }
		 //////////////////////////////////////////////////////////////////////////
		 // right click
private: System::Void SceneNodetreeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
			 // right click - context menu 부른 노드가 어느것인지 기록해둔다
			 if (e->Button == ::MouseButtons::Right)
				 SceneNodetreeView->Tag = e->Node;
		 }
private: System::Void SceneNodetreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( e->Node );
			 g_pMainApp->SetChild( pSceneNode );
		 }
private: System::Void sceneTreeMenuImport_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
		//	 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode(treeNode);
			 OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

			 openFileDialog1->InitialDirectory = gcnew String( CToolConfig::GetInstance().m_strEffectPath.c_str() );
			 openFileDialog1->Filter = "Scene XML Files (*.scene.xml)|*.scene.xml|elu files (*.elu)|*.elu|All files (*.*)|*.*";
			 openFileDialog1->FilterIndex = 0;
			 openFileDialog1->RestoreDirectory = true;
			 openFileDialog1->Multiselect		= true;

			 if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 //다중선택.
				 array<String^>^ FileNames = openFileDialog1->FileNames;
				 int size = FileNames->Length;

				 for each ( String^ FileName in FileNames )
				 {
					 const char* filename= MStringToCharPointer(FileName);
					 string str = GetRemovePathName(filename);
					 g_pMainApp->OnChildFileOpen( treeNode, str.c_str() );
					 MFreeCharPointer(filename);
					 g_bModified = true;
				 }
			 }
			 delete openFileDialog1;
		 }
private: System::Void trailToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->CreateTrail( treeNode );
		 }
private: System::Void sphereToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->CreateEmitter( treeNode, EMITTER_SPHERE);
		 }
private: System::Void effectGroupToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->CreateEffectSceneNode( treeNode);
		 }
private: System::Void boxToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->CreateEmitter( treeNode, EMITTER_BOX);
		 }
private: System::Void ringToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->CreateEmitter( treeNode, EMITTER_RING);
		 }
private: System::Void sceneTreeMenuDelete_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->DeleteChild( treeNode );
		}
private: System::Void propertyToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode(treeNode);

			 if( MDynamicCast(RLightSceneNode, pSceneNode) )
			 {
				 ReflectivePropertyForm^ reflectivePropertyForm = gcnew ReflectivePropertyForm;
				 reflectivePropertyForm->InitializeComponent();

				 if( RPointLightSceneNode* pPointLight = MDynamicCast(RPointLightSceneNode, pSceneNode) )
				 {
					 static CPointLightPropertyRef reflectivePointLight;
					 reflectivePointLight.SetSceneNode(pPointLight);
					 reflectivePropertyForm->setText("PointLight Property");	
					 reflectivePropertyForm->setBaseObject((void*)&reflectivePointLight);

					 reflectivePropertyForm->addAllReflection();

					 reflectivePropertyForm->Show();
					 reflectivePropertyForm->CompleteToInitializeComponent();
				 }
				 else if( RSpotLightSceneNode* pSpotLight = MDynamicCast(RSpotLightSceneNode, pSceneNode) )
				 {
					 static CSpotLightPropertyRef reflectiveSpotLight;
					 reflectiveSpotLight.SetSceneNode(pSpotLight);
					 reflectivePropertyForm->setText("SpotLight Property");	
					 reflectivePropertyForm->setBaseObject((void*)&reflectiveSpotLight);

					 reflectivePropertyForm->addAllReflection();

					 reflectivePropertyForm->Show();
					 reflectivePropertyForm->CompleteToInitializeComponent();
				 }

				 g_bModified = true;
			 }
			 else
			 {
				 m_propertyForm->Show();
				 m_propertyForm->sceneNodePropertyGrid->SelectedObject = gcnew RSceneNodeProperty(pSceneNode);
				 m_propertyForm->sceneNodePropertyGrid->Update();
			 }
		 }
private: System::Void applyAllSameNamedToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( treeNode );
			 g_pMainApp->ApplyToAllSameNamed( pSceneNode );
		 }
private: System::Void moveUpToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->MoveUpDownSceneNode(treeNode, true);
		 }
private: System::Void moveDownToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->MoveUpDownSceneNode(treeNode, false);
		 }

		 //////////////////////////////////////////////////////////////////////////
		 // Drag & Drop
private: System::Void SceneNodetreeView_ItemDrag(System::Object^  sender, System::Windows::Forms::ItemDragEventArgs^  e) {
			 SceneNodetreeView->DoDragDrop(e->Item, DragDropEffects::Move | DragDropEffects::Copy | DragDropEffects::Link );
		 }
private: System::Void SceneNodetreeView_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
			 e->Effect = DragDropEffects::Move;
		 }
private: System::Void SceneNodetreeView_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {

			 Point loc = ((TreeView^)sender)->PointToClient(Point(e->X, e->Y));
  			 
			 // 데이터를 가지고 있는지 확인하는 법 : if ( e->Data->GetDataPresent( TreeNode::typeid ) )
			 TreeNode^ node = (TreeNode^)e->Data->GetData( TreeNode::typeid );
  			 TreeNode^ destNode = ((TreeView^)sender)->GetNodeAt(loc);

			 if( node==nullptr || destNode==nullptr ) return;

			 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( node );
			 RSceneNode* pDestSceneNode = GetSceneNodeFromTreeNode( destNode );
			 if( pSceneNode==NULL || pDestSceneNode==NULL ) return;
			 if( pSceneNode == pDestSceneNode ) return;	// 자기자신 ?

			 // 딸린 노드는 움직이거나 복사할 수 없다
			 if( pSceneNode->QueryAttribute(RSNA_DO_NOT_SAVE) ) return;

			 switch(e->Effect)
			 {
			 case DragDropEffects::Move :	g_pMainApp->MoveSceneNode( node, destNode );break;
			 case DragDropEffects::Copy :	g_pMainApp->CopySceneNode( node, destNode );break;
			 case DragDropEffects::Link :	g_pMainApp->LinkSceneNode( node, destNode );break;
			 default : _ASSERT(FALSE);
			 }
		 }
private: System::Void SceneNodetreeView_AfterLabelEdit(System::Object^  sender, System::Windows::Forms::NodeLabelEditEventArgs^  e) {
			 
			 if( e->Label==nullptr ) return;

			 // 신노드 이름 변경
			 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( e->Node );

			 const char* pstr= MStringToCharPointer( e->Label );
			 pSceneNode->SetNodeName( pstr );
			 g_pMainApp->MarkSameNamedNodes();
			 MFreeCharPointer( pstr );

			 g_bModified = true;
		 }
private: System::Void SceneNodetreeView_AfterCheck(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( e->Node );
			 if( REffectActor* pEffectActor = MDynamicCast(REffectActor, pSceneNode) )
				 pEffectActor->SetVisibleSwitch( e->Node->Checked );
			 else
			 if( REffectBase* pEffectBase = MDynamicCast(REffectBase, pSceneNode) )
				 pEffectBase->SetVisibleSwitch( e->Node->Checked );
			 else
				 pSceneNode->SetVisible( e->Node->Checked );

			 for( int i = 0; i < e->Node->Nodes->Count; ++i )
				 e->Node->Nodes[i]->Checked = e->Node->Checked;
		 }
private: System::Void SceneNodetreeView_DragOver(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
			 if ( (e->KeyState & 4) == 4 && ((e->AllowedEffect & DragDropEffects::Move) == DragDropEffects::Move) )
			 {
				 // SHIFT KeyState for move.
				 e->Effect = DragDropEffects::Link;
			 }else 
			 if ( (e->KeyState & 8) == 8 && ((e->AllowedEffect & DragDropEffects::Copy) == DragDropEffects::Copy) )
			 {
				 // CTL KeyState for copy.
				 e->Effect = DragDropEffects::Copy;
			 }
			else
				e->Effect = DragDropEffects::Move;
		 }
private: System::Void 다시열기RToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 // 저장확인 : 취소 추가 - 090602, OZ
			 if( g_pMainApp->ConfirmSave())
			 {
				g_pMainApp->ReloadEffect();
			 }
		 }
private: System::Void ChangeEmitterTypeSphereToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
 			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
		//	 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( treeNode );
			 g_bModified = g_pMainApp->ChangeEmitterType( treeNode, EMITTER_SPHERE );
		 }
private: System::Void ChangeEmitterTypeBoxToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
 			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
		//	 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( treeNode );
			 g_bModified = g_pMainApp->ChangeEmitterType( treeNode, EMITTER_BOX );
		 }
private: System::Void ChangeEmitterTypeRingToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
 			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
		//	 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( treeNode );
			 g_bModified = g_pMainApp->ChangeEmitterType( treeNode, EMITTER_RING );
		 }
private: System::Void sceneTreeMenu_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
 			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 RSceneNode* pSceneNode = GetSceneNodeFromTreeNode( treeNode );
			 REmitter* pEmitter = MDynamicCast(REmitter, pSceneNode);
			 changeTypeToolStripMenuItem->Enabled = ( pEmitter ? true : false );

			 RActorNode* pActorNode = MDynamicCast(RActorNode, pSceneNode);
			 bool bEnable = ( pActorNode ? false : true );
			 sceneTreeMenuDelete->Enabled = bEnable;
			 moveUpToolStripMenuItem->Enabled = bEnable;
			 moveDownToolStripMenuItem->Enabled = bEnable;
			 applyAllSameNamedToolStripMenuItem->Enabled = bEnable;
			 CopyToolStripMenuItem->Enabled = bEnable;
		 }
private: System::Void EmitterName_AutoUpdate_Click(System::Object^  sender, System::EventArgs^  e) {
			 if ( EmitterName_AutoUpdate->Checked )
				 g_pMainApp->SetEmitterNameAutoUpdate(true);
			 else
				 g_pMainApp->SetEmitterNameAutoUpdate(false);
		 }
private: System::Void HDREnableToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if ( HDREnableToolStripMenuItem->Checked )
				 g_pMainApp->SetHDREnable(true);
			 else
				 g_pMainApp->SetHDREnable(false);
		 }
private: System::Void pointLightToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->CreateLight( treeNode, false);
		 }
private: System::Void spotLightToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->CreateLight( treeNode, true);
		 }
private: System::Void ShowLightRangetoolStripButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_bShowLight = !g_pMainApp->m_bShowLight;
		 }
private: System::Void CopyToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->CopySceneNode(treeNode);
		 }
private: System::Void PasteToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 TreeNode^ treeNode = (TreeNode^)(SceneNodetreeView->Tag);
			 g_pMainApp->PasteSceneNode(treeNode);
		 }
private: System::Void LightOnOffMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->SetLightToggle();
		 }
private: System::Void LightToCameraPosMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->SetLightToCameraToggle();
		 }
private: System::Void lightAmbientMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 ColorDialog^ colorDialog = gcnew System::Windows::Forms::ColorDialog();
			 if(colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 DWORD dwColor = D3DCOLOR_ARGB(colorDialog->Color.A,colorDialog->Color.R,colorDialog->Color.G,colorDialog->Color.B);
				 RVector4 ambient = RVector4(dwColor);
				 g_pMainApp->SetLightAmbient(ambient);
			 }
		 }
private: System::Void MRUEluFileListToolStripMenuItemSub_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 ToolStripMenuItem^ menuItem = (ToolStripMenuItem^)sender;
			 if( nullptr != menuItem )
			 {
				 g_pMainApp->MeshFileOpenFromMRUList( (int)menuItem->Tag );
			 }
		 }
public: System::Void RefreshMRUEluFileList()
		 {
			 MRUEluFileListToolStripMenuItem->DropDownItems->Clear();

			 CMostRecentlyUsedList& eluFileMRUList = CToolConfig::GetInstance().m_eluFileMRUList;
			 for( size_t i = 0; i < eluFileMRUList.GetCount(); ++i )
			 {
				 // "&1: " 형식 만들기
				 String^ lableText = gcnew String(L"");
				 System::Int32 index = i;
				 if( i < 9 )
				 {
					 lableText += "&";
				 }
				 lableText += (System::Int32)( i + 1 );
				 lableText += L": ";

				 string purefilename = eluFileMRUList.Get(i);
				 purefilename = MGetPureFileNameExt(purefilename);

				 String^ fileName = gcnew String(purefilename.c_str());
				 lableText += fileName;

				 ToolStripMenuItem^ menuItem = gcnew ToolStripMenuItem();
				 menuItem->Name = L"MRUEluFileListToolStripMenuItem" + (System::Int32)(i);
				 menuItem->Text = lableText;
				 menuItem->Tag = (System::Int32)i;
				 menuItem->Click += gcnew System::EventHandler(this, &MainForm::MRUEluFileListToolStripMenuItemSub_Click);

				 MRUEluFileListToolStripMenuItem->DropDownItems->Add( menuItem );
			 }
		 }
};	// end of ref class MainForm
} // end of namespace EffectTool_NET