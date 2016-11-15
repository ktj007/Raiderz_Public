#pragma once
#include "DialogForm.h"
#include "TQuestController.h"
#include "CSQuestInfo.h"
#include "InsertQuest.h"
#include "TQuestDBManager.h"

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Xml.dll>

//class DialogForm;

namespace QuestAndDialog_NET {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// QuestForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class QuestForm : public System::Windows::Forms::Form
	{
	public:
		QuestForm(TQuestController* pQc)
		{
			InitializeComponent();

			this->m_pQc = pQc;
		}


	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~QuestForm()
		{
			this->m_pQc->SetQuestForm(nullptr);

			if (components)
			{
				delete components;
			}
		}
	private: TQuestController* m_pQc;

	private: System::Windows::Forms::MenuStrip^  menuStrip_Quest;
	private: System::Windows::Forms::ToolStripMenuItem^  toolToolStripMenuItem_File;
	protected: 

	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Exit;
	private: System::Windows::Forms::ToolStripMenuItem^  ToolStripMenuItem_Tool;
	private: System::Windows::Forms::ToolStripMenuItem^  ToolStripMenuItem_RunDialog;
	











	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_SaveQuest;




























	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip_QuestList;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Insert;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Delete;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Cut;





	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Paste;









	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_AllOpen;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_AllClose;




	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_View;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_ReloadQuest;
	private: System::Windows::Forms::StatusStrip^  statusStrip_Quest;

	private: System::Windows::Forms::ToolStrip^  toolStrip_Quest;

	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	private: System::Windows::Forms::GroupBox^  groupBox_FindQuest;
	private: System::Windows::Forms::Button^  button_FindQuest;
	private: System::Windows::Forms::ComboBox^  comboBox_FindQuest;
	private: System::Windows::Forms::TextBox^  textBox_FindQuest;
	private: System::Windows::Forms::TreeView^  treeView_QuestList;
	private: System::Windows::Forms::SplitContainer^  splitContainer2;
	private: System::Windows::Forms::GroupBox^  groupBox_QuestUObjective;
	private: System::Windows::Forms::DataGridView^  dataGridView_QuestUObjective;
	private: System::Windows::Forms::GroupBox^  groupBox_QuestOObjective;
	private: System::Windows::Forms::DataGridView^  dataGridView_QuestOObjective;
	private: System::Windows::Forms::GroupBox^  groupBox_QuestPR;
	private: System::Windows::Forms::Label^  label_QuestPRFaction;
	private: System::Windows::Forms::Label^  label_QuestPR;
	private: System::Windows::Forms::DataGridView^  dataGridView_QuestPRFaction;
	private: System::Windows::Forms::DataGridView^  dataGridView_QuestPR;
	private: System::Windows::Forms::GroupBox^  groupBox_BastInfo;
	private: System::Windows::Forms::DataGridView^  dataGridView_QuestBaseInfo;
	private: System::Windows::Forms::Panel^  panel_ZoneDesc;
	private: System::Windows::Forms::Label^  label_ZoneDesc;
	private: System::Windows::Forms::Panel^  panel_ObjectiveType;
	private: System::Windows::Forms::Label^  label_ObjectiveType;
	private: System::Windows::Forms::Panel^  panel_PRSex;
	private: System::Windows::Forms::Label^  label_PRSex;
	private: System::Windows::Forms::Panel^  panel_PRRace;
	private: System::Windows::Forms::Label^  label_PRRace;
	private: System::Windows::Forms::Panel^  panel_QuestType;
	private: System::Windows::Forms::Label^  label_QuestType;
	private: System::Windows::Forms::Panel^  panel_Repeatable;
	private: System::Windows::Forms::Label^  label_Repeatable;
	private: System::Windows::Forms::Panel^  panel_Raidable;
	private: System::Windows::Forms::Label^  label_Raidable;
	private: System::Windows::Forms::Panel^  panel_QuestStingEditor;
	private: System::Windows::Forms::Button^  button_QuestStringEditorCancel;
	private: System::Windows::Forms::Button^  button_QuestStringEditorConfirm;
	private: System::Windows::Forms::GroupBox^  groupBox_QuestStringEditor;
	private: System::Windows::Forms::TextBox^  textBox_QuestStringEditor;
private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel_Quest;
private: System::Windows::Forms::Panel^  panel_HideTheNextStep;
private: System::Windows::Forms::Label^  label_HideTheNextStep;
private: System::Windows::Forms::Panel^  panel_DeadLine;
private: System::Windows::Forms::DateTimePicker^  dateTimePicker_DeadLine;




























































	private: System::ComponentModel::IContainer^  components;




























































	protected: 

	protected: 

	private:
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(QuestForm::typeid));
			this->menuStrip_Quest = (gcnew System::Windows::Forms::MenuStrip());
			this->toolToolStripMenuItem_File = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_SaveQuest = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Exit = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_View = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_ReloadQuest = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ToolStripMenuItem_Tool = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ToolStripMenuItem_RunDialog = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->contextMenuStrip_QuestList = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->toolStripMenuItem_Cut = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Paste = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Insert = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Delete = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_AllOpen = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_AllClose = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusStrip_Quest = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel_Quest = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStrip_Quest = (gcnew System::Windows::Forms::ToolStrip());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->groupBox_FindQuest = (gcnew System::Windows::Forms::GroupBox());
			this->button_FindQuest = (gcnew System::Windows::Forms::Button());
			this->comboBox_FindQuest = (gcnew System::Windows::Forms::ComboBox());
			this->textBox_FindQuest = (gcnew System::Windows::Forms::TextBox());
			this->treeView_QuestList = (gcnew System::Windows::Forms::TreeView());
			this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
			this->groupBox_QuestUObjective = (gcnew System::Windows::Forms::GroupBox());
			this->dataGridView_QuestUObjective = (gcnew System::Windows::Forms::DataGridView());
			this->groupBox_QuestOObjective = (gcnew System::Windows::Forms::GroupBox());
			this->dataGridView_QuestOObjective = (gcnew System::Windows::Forms::DataGridView());
			this->groupBox_QuestPR = (gcnew System::Windows::Forms::GroupBox());
			this->label_QuestPRFaction = (gcnew System::Windows::Forms::Label());
			this->label_QuestPR = (gcnew System::Windows::Forms::Label());
			this->dataGridView_QuestPRFaction = (gcnew System::Windows::Forms::DataGridView());
			this->dataGridView_QuestPR = (gcnew System::Windows::Forms::DataGridView());
			this->groupBox_BastInfo = (gcnew System::Windows::Forms::GroupBox());
			this->dataGridView_QuestBaseInfo = (gcnew System::Windows::Forms::DataGridView());
			this->panel_DeadLine = (gcnew System::Windows::Forms::Panel());
			this->dateTimePicker_DeadLine = (gcnew System::Windows::Forms::DateTimePicker());
			this->panel_HideTheNextStep = (gcnew System::Windows::Forms::Panel());
			this->label_HideTheNextStep = (gcnew System::Windows::Forms::Label());
			this->panel_ZoneDesc = (gcnew System::Windows::Forms::Panel());
			this->label_ZoneDesc = (gcnew System::Windows::Forms::Label());
			this->panel_ObjectiveType = (gcnew System::Windows::Forms::Panel());
			this->label_ObjectiveType = (gcnew System::Windows::Forms::Label());
			this->panel_PRSex = (gcnew System::Windows::Forms::Panel());
			this->label_PRSex = (gcnew System::Windows::Forms::Label());
			this->panel_PRRace = (gcnew System::Windows::Forms::Panel());
			this->label_PRRace = (gcnew System::Windows::Forms::Label());
			this->panel_QuestType = (gcnew System::Windows::Forms::Panel());
			this->label_QuestType = (gcnew System::Windows::Forms::Label());
			this->panel_Repeatable = (gcnew System::Windows::Forms::Panel());
			this->label_Repeatable = (gcnew System::Windows::Forms::Label());
			this->panel_Raidable = (gcnew System::Windows::Forms::Panel());
			this->label_Raidable = (gcnew System::Windows::Forms::Label());
			this->panel_QuestStingEditor = (gcnew System::Windows::Forms::Panel());
			this->button_QuestStringEditorCancel = (gcnew System::Windows::Forms::Button());
			this->button_QuestStringEditorConfirm = (gcnew System::Windows::Forms::Button());
			this->groupBox_QuestStringEditor = (gcnew System::Windows::Forms::GroupBox());
			this->textBox_QuestStringEditor = (gcnew System::Windows::Forms::TextBox());
			this->menuStrip_Quest->SuspendLayout();
			this->contextMenuStrip_QuestList->SuspendLayout();
			this->statusStrip_Quest->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->groupBox_FindQuest->SuspendLayout();
			this->splitContainer2->Panel1->SuspendLayout();
			this->splitContainer2->Panel2->SuspendLayout();
			this->splitContainer2->SuspendLayout();
			this->groupBox_QuestUObjective->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestUObjective))->BeginInit();
			this->groupBox_QuestOObjective->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestOObjective))->BeginInit();
			this->groupBox_QuestPR->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestPRFaction))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestPR))->BeginInit();
			this->groupBox_BastInfo->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestBaseInfo))->BeginInit();
			this->panel_DeadLine->SuspendLayout();
			this->panel_HideTheNextStep->SuspendLayout();
			this->panel_ZoneDesc->SuspendLayout();
			this->panel_ObjectiveType->SuspendLayout();
			this->panel_PRSex->SuspendLayout();
			this->panel_PRRace->SuspendLayout();
			this->panel_QuestType->SuspendLayout();
			this->panel_Repeatable->SuspendLayout();
			this->panel_Raidable->SuspendLayout();
			this->panel_QuestStingEditor->SuspendLayout();
			this->groupBox_QuestStringEditor->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip_Quest
			// 
			this->menuStrip_Quest->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->toolToolStripMenuItem_File, 
				this->toolStripMenuItem_View, this->ToolStripMenuItem_Tool});
			this->menuStrip_Quest->Location = System::Drawing::Point(0, 0);
			this->menuStrip_Quest->Name = L"menuStrip_Quest";
			this->menuStrip_Quest->Size = System::Drawing::Size(1146, 24);
			this->menuStrip_Quest->TabIndex = 0;
			this->menuStrip_Quest->Text = L"menuStrip1";
			// 
			// toolToolStripMenuItem_File
			// 
			this->toolToolStripMenuItem_File->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->toolStripMenuItem_SaveQuest, 
				this->toolStripMenuItem_Exit});
			this->toolToolStripMenuItem_File->Name = L"toolToolStripMenuItem_File";
			this->toolToolStripMenuItem_File->Size = System::Drawing::Size(58, 20);
			this->toolToolStripMenuItem_File->Text = L"파일(&F)";
			// 
			// toolStripMenuItem_SaveQuest
			// 
			this->toolStripMenuItem_SaveQuest->Name = L"toolStripMenuItem_SaveQuest";
			this->toolStripMenuItem_SaveQuest->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->toolStripMenuItem_SaveQuest->Size = System::Drawing::Size(168, 22);
			this->toolStripMenuItem_SaveQuest->Text = L"저장(&S)";
			this->toolStripMenuItem_SaveQuest->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_Click_SaveQuest);
			// 
			// toolStripMenuItem_Exit
			// 
			this->toolStripMenuItem_Exit->Name = L"toolStripMenuItem_Exit";
			this->toolStripMenuItem_Exit->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::F4));
			this->toolStripMenuItem_Exit->Size = System::Drawing::Size(168, 22);
			this->toolStripMenuItem_Exit->Text = L"끝내기(&X)";
			this->toolStripMenuItem_Exit->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_Click_Exit);
			// 
			// toolStripMenuItem_View
			// 
			this->toolStripMenuItem_View->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripMenuItem_ReloadQuest});
			this->toolStripMenuItem_View->Name = L"toolStripMenuItem_View";
			this->toolStripMenuItem_View->Size = System::Drawing::Size(59, 20);
			this->toolStripMenuItem_View->Text = L"보기(&V)";
			// 
			// toolStripMenuItem_ReloadQuest
			// 
			this->toolStripMenuItem_ReloadQuest->Name = L"toolStripMenuItem_ReloadQuest";
			this->toolStripMenuItem_ReloadQuest->ShortcutKeys = System::Windows::Forms::Keys::F5;
			this->toolStripMenuItem_ReloadQuest->Size = System::Drawing::Size(228, 22);
			this->toolStripMenuItem_ReloadQuest->Text = L"퀘스트 정보 새로고침(&R)";
			this->toolStripMenuItem_ReloadQuest->Click += gcnew System::EventHandler(this, &QuestForm::toolStripMenuItem_ReloadQuest_Click);
			// 
			// ToolStripMenuItem_Tool
			// 
			this->ToolStripMenuItem_Tool->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->ToolStripMenuItem_RunDialog});
			this->ToolStripMenuItem_Tool->Name = L"ToolStripMenuItem_Tool";
			this->ToolStripMenuItem_Tool->Size = System::Drawing::Size(59, 20);
			this->ToolStripMenuItem_Tool->Text = L"도구(&T)";
			// 
			// ToolStripMenuItem_RunDialog
			// 
			this->ToolStripMenuItem_RunDialog->Name = L"ToolStripMenuItem_RunDialog";
			this->ToolStripMenuItem_RunDialog->ShortcutKeys = System::Windows::Forms::Keys::F9;
			this->ToolStripMenuItem_RunDialog->Size = System::Drawing::Size(188, 22);
			this->ToolStripMenuItem_RunDialog->Text = L"다이얼로그 툴(&D)";
			this->ToolStripMenuItem_RunDialog->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_Click_RunDialog);
			// 
			// contextMenuStrip_QuestList
			// 
			this->contextMenuStrip_QuestList->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->toolStripMenuItem_Cut, 
				this->toolStripMenuItem_Paste, this->toolStripMenuItem_Insert, this->toolStripMenuItem_Delete, this->toolStripMenuItem_AllOpen, 
				this->toolStripMenuItem_AllClose});
			this->contextMenuStrip_QuestList->Name = L"contextMenuStrip_QuestList";
			this->contextMenuStrip_QuestList->Size = System::Drawing::Size(181, 136);
			// 
			// toolStripMenuItem_Cut
			// 
			this->toolStripMenuItem_Cut->Name = L"toolStripMenuItem_Cut";
			this->toolStripMenuItem_Cut->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::X));
			this->toolStripMenuItem_Cut->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_Cut->Text = L"잘라내기(&X)";
			this->toolStripMenuItem_Cut->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_Cut_Click);
			// 
			// toolStripMenuItem_Paste
			// 
			this->toolStripMenuItem_Paste->Name = L"toolStripMenuItem_Paste";
			this->toolStripMenuItem_Paste->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::V));
			this->toolStripMenuItem_Paste->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_Paste->Text = L"붙여넣기(&P)";
			this->toolStripMenuItem_Paste->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_Paste_Click);
			// 
			// toolStripMenuItem_Insert
			// 
			this->toolStripMenuItem_Insert->Name = L"toolStripMenuItem_Insert";
			this->toolStripMenuItem_Insert->ShortcutKeys = System::Windows::Forms::Keys::Insert;
			this->toolStripMenuItem_Insert->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_Insert->Text = L"추가(&A)";
			this->toolStripMenuItem_Insert->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_Insert_Click);
			// 
			// toolStripMenuItem_Delete
			// 
			this->toolStripMenuItem_Delete->Name = L"toolStripMenuItem_Delete";
			this->toolStripMenuItem_Delete->ShortcutKeys = System::Windows::Forms::Keys::Delete;
			this->toolStripMenuItem_Delete->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_Delete->Text = L"삭제(&D)";
			this->toolStripMenuItem_Delete->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_Delete_Click);
			// 
			// toolStripMenuItem_AllOpen
			// 
			this->toolStripMenuItem_AllOpen->Name = L"toolStripMenuItem_AllOpen";
			this->toolStripMenuItem_AllOpen->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_AllOpen->Text = L"전부열기(&O)";
			this->toolStripMenuItem_AllOpen->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_AllOpen_Click);
			// 
			// toolStripMenuItem_AllClose
			// 
			this->toolStripMenuItem_AllClose->Name = L"toolStripMenuItem_AllClose";
			this->toolStripMenuItem_AllClose->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_AllClose->Text = L"전부닫기(&C)";
			this->toolStripMenuItem_AllClose->Click += gcnew System::EventHandler(this, &QuestForm::ToolStripMenuItem_AllClose_Click);
			// 
			// statusStrip_Quest
			// 
			this->statusStrip_Quest->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripStatusLabel_Quest});
			this->statusStrip_Quest->Location = System::Drawing::Point(0, 817);
			this->statusStrip_Quest->Name = L"statusStrip_Quest";
			this->statusStrip_Quest->Size = System::Drawing::Size(1146, 22);
			this->statusStrip_Quest->TabIndex = 1;
			this->statusStrip_Quest->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel_Quest
			// 
			this->toolStripStatusLabel_Quest->Name = L"toolStripStatusLabel_Quest";
			this->toolStripStatusLabel_Quest->Size = System::Drawing::Size(155, 17);
			this->toolStripStatusLabel_Quest->Text = L"toolStripStatusLabel_Quest";
			// 
			// toolStrip_Quest
			// 
			this->toolStrip_Quest->Location = System::Drawing::Point(0, 24);
			this->toolStrip_Quest->Name = L"toolStrip_Quest";
			this->toolStrip_Quest->Size = System::Drawing::Size(1146, 25);
			this->toolStrip_Quest->TabIndex = 2;
			this->toolStrip_Quest->Text = L"toolStrip1";
			// 
			// splitContainer1
			// 
			this->splitContainer1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
			this->splitContainer1->Location = System::Drawing::Point(0, 49);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->groupBox_FindQuest);
			this->splitContainer1->Panel1->Controls->Add(this->treeView_QuestList);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->splitContainer2);
			this->splitContainer1->Size = System::Drawing::Size(1146, 768);
			this->splitContainer1->SplitterDistance = 280;
			this->splitContainer1->TabIndex = 3;
			// 
			// groupBox_FindQuest
			// 
			this->groupBox_FindQuest->Controls->Add(this->button_FindQuest);
			this->groupBox_FindQuest->Controls->Add(this->comboBox_FindQuest);
			this->groupBox_FindQuest->Controls->Add(this->textBox_FindQuest);
			this->groupBox_FindQuest->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->groupBox_FindQuest->Location = System::Drawing::Point(0, 716);
			this->groupBox_FindQuest->Name = L"groupBox_FindQuest";
			this->groupBox_FindQuest->Size = System::Drawing::Size(276, 48);
			this->groupBox_FindQuest->TabIndex = 7;
			this->groupBox_FindQuest->TabStop = false;
			this->groupBox_FindQuest->Text = L"Quest검색";
			// 
			// button_FindQuest
			// 
			this->button_FindQuest->Location = System::Drawing::Point(212, 18);
			this->button_FindQuest->Name = L"button_FindQuest";
			this->button_FindQuest->Size = System::Drawing::Size(49, 23);
			this->button_FindQuest->TabIndex = 6;
			this->button_FindQuest->Text = L"검색";
			this->button_FindQuest->UseVisualStyleBackColor = true;
			this->button_FindQuest->Click += gcnew System::EventHandler(this, &QuestForm::button_FindQuest_Click);
			// 
			// comboBox_FindQuest
			// 
			this->comboBox_FindQuest->FormattingEnabled = true;
			this->comboBox_FindQuest->Location = System::Drawing::Point(6, 19);
			this->comboBox_FindQuest->Name = L"comboBox_FindQuest";
			this->comboBox_FindQuest->Size = System::Drawing::Size(52, 20);
			this->comboBox_FindQuest->TabIndex = 5;
			// 
			// textBox_FindQuest
			// 
			this->textBox_FindQuest->Location = System::Drawing::Point(64, 18);
			this->textBox_FindQuest->Name = L"textBox_FindQuest";
			this->textBox_FindQuest->Size = System::Drawing::Size(142, 21);
			this->textBox_FindQuest->TabIndex = 4;
			this->textBox_FindQuest->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &QuestForm::TextBox_FindQuest_KeyDown);
			// 
			// treeView_QuestList
			// 
			this->treeView_QuestList->ContextMenuStrip = this->contextMenuStrip_QuestList;
			this->treeView_QuestList->Dock = System::Windows::Forms::DockStyle::Fill;
			this->treeView_QuestList->Location = System::Drawing::Point(0, 0);
			this->treeView_QuestList->Name = L"treeView_QuestList";
			this->treeView_QuestList->Size = System::Drawing::Size(276, 764);
			this->treeView_QuestList->TabIndex = 0;
			this->treeView_QuestList->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &QuestForm::treeView_QuestList_AfterSelect);
			this->treeView_QuestList->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &QuestForm::treeView_QuestList_NodeMouseClick);
			this->treeView_QuestList->BeforeSelect += gcnew System::Windows::Forms::TreeViewCancelEventHandler(this, &QuestForm::treeView_QuestList_BeforeSelect);
			// 
			// splitContainer2
			// 
			this->splitContainer2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer2->FixedPanel = System::Windows::Forms::FixedPanel::Panel2;
			this->splitContainer2->Location = System::Drawing::Point(0, 0);
			this->splitContainer2->Name = L"splitContainer2";
			this->splitContainer2->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer2.Panel1
			// 
			this->splitContainer2->Panel1->Controls->Add(this->groupBox_QuestUObjective);
			this->splitContainer2->Panel1->Controls->Add(this->groupBox_QuestOObjective);
			this->splitContainer2->Panel1->Controls->Add(this->groupBox_QuestPR);
			this->splitContainer2->Panel1->Controls->Add(this->groupBox_BastInfo);
			// 
			// splitContainer2.Panel2
			// 
			this->splitContainer2->Panel2->Controls->Add(this->panel_DeadLine);
			this->splitContainer2->Panel2->Controls->Add(this->panel_HideTheNextStep);
			this->splitContainer2->Panel2->Controls->Add(this->panel_ZoneDesc);
			this->splitContainer2->Panel2->Controls->Add(this->panel_ObjectiveType);
			this->splitContainer2->Panel2->Controls->Add(this->panel_PRSex);
			this->splitContainer2->Panel2->Controls->Add(this->panel_PRRace);
			this->splitContainer2->Panel2->Controls->Add(this->panel_QuestType);
			this->splitContainer2->Panel2->Controls->Add(this->panel_Repeatable);
			this->splitContainer2->Panel2->Controls->Add(this->panel_Raidable);
			this->splitContainer2->Panel2->Controls->Add(this->panel_QuestStingEditor);
			this->splitContainer2->Size = System::Drawing::Size(862, 768);
			this->splitContainer2->SplitterDistance = 542;
			this->splitContainer2->TabIndex = 0;
			// 
			// groupBox_QuestUObjective
			// 
			this->groupBox_QuestUObjective->Controls->Add(this->dataGridView_QuestUObjective);
			this->groupBox_QuestUObjective->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox_QuestUObjective->Location = System::Drawing::Point(0, 428);
			this->groupBox_QuestUObjective->Name = L"groupBox_QuestUObjective";
			this->groupBox_QuestUObjective->Size = System::Drawing::Size(858, 110);
			this->groupBox_QuestUObjective->TabIndex = 3;
			this->groupBox_QuestUObjective->TabStop = false;
			this->groupBox_QuestUObjective->Text = L"비순서형 목적";
			// 
			// dataGridView_QuestUObjective
			// 
			this->dataGridView_QuestUObjective->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
			this->dataGridView_QuestUObjective->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView_QuestUObjective->Dock = System::Windows::Forms::DockStyle::Fill;
			this->dataGridView_QuestUObjective->Location = System::Drawing::Point(3, 17);
			this->dataGridView_QuestUObjective->Name = L"dataGridView_QuestUObjective";
			this->dataGridView_QuestUObjective->RowTemplate->Height = 23;
			this->dataGridView_QuestUObjective->Size = System::Drawing::Size(852, 90);
			this->dataGridView_QuestUObjective->TabIndex = 3;
			this->dataGridView_QuestUObjective->UserAddedRow += gcnew System::Windows::Forms::DataGridViewRowEventHandler(this, &QuestForm::dataGridView_QuestUObjective_UserAddedRow);
			this->dataGridView_QuestUObjective->UserDeletingRow += gcnew System::Windows::Forms::DataGridViewRowCancelEventHandler(this, &QuestForm::dataGridView_QuestUObjective_UserDeletingRow);
			this->dataGridView_QuestUObjective->CellBeginEdit += gcnew System::Windows::Forms::DataGridViewCellCancelEventHandler(this, &QuestForm::dataGridView_QuestUObjective_CellBeginEdit);
			this->dataGridView_QuestUObjective->CellEndEdit += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestUObjective_CellEndEdit);
			this->dataGridView_QuestUObjective->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &QuestForm::dataGridView_QuestUObjective_DataError);
			this->dataGridView_QuestUObjective->CellEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestUObjective_CellEnter);
			// 
			// groupBox_QuestOObjective
			// 
			this->groupBox_QuestOObjective->Controls->Add(this->dataGridView_QuestOObjective);
			this->groupBox_QuestOObjective->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox_QuestOObjective->Location = System::Drawing::Point(0, 294);
			this->groupBox_QuestOObjective->Name = L"groupBox_QuestOObjective";
			this->groupBox_QuestOObjective->Size = System::Drawing::Size(858, 134);
			this->groupBox_QuestOObjective->TabIndex = 2;
			this->groupBox_QuestOObjective->TabStop = false;
			this->groupBox_QuestOObjective->Text = L"순서형 목적";
			// 
			// dataGridView_QuestOObjective
			// 
			this->dataGridView_QuestOObjective->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->dataGridView_QuestOObjective->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
			this->dataGridView_QuestOObjective->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView_QuestOObjective->Location = System::Drawing::Point(3, 20);
			this->dataGridView_QuestOObjective->Name = L"dataGridView_QuestOObjective";
			this->dataGridView_QuestOObjective->RowTemplate->Height = 23;
			this->dataGridView_QuestOObjective->Size = System::Drawing::Size(852, 100);
			this->dataGridView_QuestOObjective->TabIndex = 0;
			this->dataGridView_QuestOObjective->UserAddedRow += gcnew System::Windows::Forms::DataGridViewRowEventHandler(this, &QuestForm::dataGridView_QuestOObjective_UserAddedRow);
			this->dataGridView_QuestOObjective->UserDeletingRow += gcnew System::Windows::Forms::DataGridViewRowCancelEventHandler(this, &QuestForm::dataGridView_QuestOObjective_UserDeletingRow);
			this->dataGridView_QuestOObjective->CellBeginEdit += gcnew System::Windows::Forms::DataGridViewCellCancelEventHandler(this, &QuestForm::dataGridView_QuestOObjective_CellBeginEdit);
			this->dataGridView_QuestOObjective->CellEndEdit += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestOObjective_CellEndEdit);
			this->dataGridView_QuestOObjective->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &QuestForm::dataGridView_QuestOObjective_DataError);
			this->dataGridView_QuestOObjective->CellEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestOObjective_CellEnter);
			// 
			// groupBox_QuestPR
			// 
			this->groupBox_QuestPR->Controls->Add(this->label_QuestPRFaction);
			this->groupBox_QuestPR->Controls->Add(this->label_QuestPR);
			this->groupBox_QuestPR->Controls->Add(this->dataGridView_QuestPRFaction);
			this->groupBox_QuestPR->Controls->Add(this->dataGridView_QuestPR);
			this->groupBox_QuestPR->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox_QuestPR->Location = System::Drawing::Point(0, 90);
			this->groupBox_QuestPR->Name = L"groupBox_QuestPR";
			this->groupBox_QuestPR->Size = System::Drawing::Size(858, 204);
			this->groupBox_QuestPR->TabIndex = 1;
			this->groupBox_QuestPR->TabStop = false;
			this->groupBox_QuestPR->Text = L"선행조건";
			// 
			// label_QuestPRFaction
			// 
			this->label_QuestPRFaction->AutoSize = true;
			this->label_QuestPRFaction->Location = System::Drawing::Point(9, 106);
			this->label_QuestPRFaction->Name = L"label_QuestPRFaction";
			this->label_QuestPRFaction->Size = System::Drawing::Size(65, 12);
			this->label_QuestPRFaction->TabIndex = 5;
			this->label_QuestPRFaction->Text = L"가문우호도";
			// 
			// label_QuestPR
			// 
			this->label_QuestPR->AutoSize = true;
			this->label_QuestPR->Location = System::Drawing::Point(9, 17);
			this->label_QuestPR->Name = L"label_QuestPR";
			this->label_QuestPR->Size = System::Drawing::Size(53, 12);
			this->label_QuestPR->TabIndex = 4;
			this->label_QuestPR->Text = L"선행조건";
			// 
			// dataGridView_QuestPRFaction
			// 
			this->dataGridView_QuestPRFaction->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->dataGridView_QuestPRFaction->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
			this->dataGridView_QuestPRFaction->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView_QuestPRFaction->Location = System::Drawing::Point(3, 121);
			this->dataGridView_QuestPRFaction->Name = L"dataGridView_QuestPRFaction";
			this->dataGridView_QuestPRFaction->RowTemplate->Height = 23;
			this->dataGridView_QuestPRFaction->Size = System::Drawing::Size(852, 62);
			this->dataGridView_QuestPRFaction->TabIndex = 3;
			this->dataGridView_QuestPRFaction->UserAddedRow += gcnew System::Windows::Forms::DataGridViewRowEventHandler(this, &QuestForm::dataGridView_QuestPRFaction_UserAddedRow);
			this->dataGridView_QuestPRFaction->UserDeletingRow += gcnew System::Windows::Forms::DataGridViewRowCancelEventHandler(this, &QuestForm::dataGridView_QuestPRFaction_UserDeletingRow);
			this->dataGridView_QuestPRFaction->CellBeginEdit += gcnew System::Windows::Forms::DataGridViewCellCancelEventHandler(this, &QuestForm::dataGridView_QuestPRFaction_CellBeginEdit);
			this->dataGridView_QuestPRFaction->CellEndEdit += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestPRFaction_CellEndEdit);
			this->dataGridView_QuestPRFaction->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &QuestForm::dataGridView_QuestPRFaction_DataError);
			// 
			// dataGridView_QuestPR
			// 
			this->dataGridView_QuestPR->AllowUserToAddRows = false;
			this->dataGridView_QuestPR->AllowUserToDeleteRows = false;
			this->dataGridView_QuestPR->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->dataGridView_QuestPR->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
			this->dataGridView_QuestPR->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView_QuestPR->Location = System::Drawing::Point(3, 32);
			this->dataGridView_QuestPR->Name = L"dataGridView_QuestPR";
			this->dataGridView_QuestPR->RowTemplate->Height = 23;
			this->dataGridView_QuestPR->Size = System::Drawing::Size(852, 62);
			this->dataGridView_QuestPR->TabIndex = 0;
			this->dataGridView_QuestPR->CellBeginEdit += gcnew System::Windows::Forms::DataGridViewCellCancelEventHandler(this, &QuestForm::dataGridView_QuestPR_CellBeginEdit);
			this->dataGridView_QuestPR->CellEndEdit += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestPR_CellEndEdit);
			this->dataGridView_QuestPR->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &QuestForm::dataGridView_QuestPR_DataError);
			this->dataGridView_QuestPR->CellEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestPR_CellEnter);
			// 
			// groupBox_BastInfo
			// 
			this->groupBox_BastInfo->Controls->Add(this->dataGridView_QuestBaseInfo);
			this->groupBox_BastInfo->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox_BastInfo->Location = System::Drawing::Point(0, 0);
			this->groupBox_BastInfo->Name = L"groupBox_BastInfo";
			this->groupBox_BastInfo->Size = System::Drawing::Size(858, 90);
			this->groupBox_BastInfo->TabIndex = 0;
			this->groupBox_BastInfo->TabStop = false;
			this->groupBox_BastInfo->Text = L"기본정보";
			// 
			// dataGridView_QuestBaseInfo
			// 
			this->dataGridView_QuestBaseInfo->AllowUserToAddRows = false;
			this->dataGridView_QuestBaseInfo->AllowUserToDeleteRows = false;
			this->dataGridView_QuestBaseInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->dataGridView_QuestBaseInfo->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
			this->dataGridView_QuestBaseInfo->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView_QuestBaseInfo->Location = System::Drawing::Point(3, 20);
			this->dataGridView_QuestBaseInfo->Name = L"dataGridView_QuestBaseInfo";
			this->dataGridView_QuestBaseInfo->RowTemplate->Height = 23;
			this->dataGridView_QuestBaseInfo->Size = System::Drawing::Size(852, 62);
			this->dataGridView_QuestBaseInfo->TabIndex = 0;
			this->dataGridView_QuestBaseInfo->CellBeginEdit += gcnew System::Windows::Forms::DataGridViewCellCancelEventHandler(this, &QuestForm::dataGridView_QuestBaseInfo_CellBeginEdit);
			this->dataGridView_QuestBaseInfo->CellEndEdit += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestBaseInfo_CellEndEdit);
			this->dataGridView_QuestBaseInfo->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &QuestForm::dataGridView_QuestBaseInfo_DataError);
			this->dataGridView_QuestBaseInfo->CellEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &QuestForm::dataGridView_QuestBaseInfo_CellEnter);
			// 
			// panel_DeadLine
			// 
			this->panel_DeadLine->Controls->Add(this->dateTimePicker_DeadLine);
			this->panel_DeadLine->Location = System::Drawing::Point(5, 5);
			this->panel_DeadLine->Name = L"panel_DeadLine";
			this->panel_DeadLine->Size = System::Drawing::Size(207, 32);
			this->panel_DeadLine->TabIndex = 19;
			// 
			// dateTimePicker_DeadLine
			// 
			this->dateTimePicker_DeadLine->Format = System::Windows::Forms::DateTimePickerFormat::Custom;
			this->dateTimePicker_DeadLine->ImeMode = System::Windows::Forms::ImeMode::Disable;
			this->dateTimePicker_DeadLine->Location = System::Drawing::Point(4, 3);
			this->dateTimePicker_DeadLine->Name = L"dateTimePicker_DeadLine";
			this->dateTimePicker_DeadLine->Size = System::Drawing::Size(200, 21);
			this->dateTimePicker_DeadLine->TabIndex = 18;
			this->dateTimePicker_DeadLine->Value = System::DateTime(2007, 7, 20, 0, 0, 0, 0);
			this->dateTimePicker_DeadLine->ValueChanged += gcnew System::EventHandler(this, &QuestForm::dateTimePicker_DeadLine_ValueChanged);
			// 
			// panel_HideTheNextStep
			// 
			this->panel_HideTheNextStep->Controls->Add(this->label_HideTheNextStep);
			this->panel_HideTheNextStep->Location = System::Drawing::Point(5, 5);
			this->panel_HideTheNextStep->Name = L"panel_HideTheNextStep";
			this->panel_HideTheNextStep->Size = System::Drawing::Size(150, 150);
			this->panel_HideTheNextStep->TabIndex = 17;
			this->panel_HideTheNextStep->Visible = false;
			// 
			// label_HideTheNextStep
			// 
			this->label_HideTheNextStep->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->label_HideTheNextStep->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_HideTheNextStep->Location = System::Drawing::Point(10, 10);
			this->label_HideTheNextStep->Name = L"label_HideTheNextStep";
			this->label_HideTheNextStep->Size = System::Drawing::Size(130, 130);
			this->label_HideTheNextStep->TabIndex = 0;
			this->label_HideTheNextStep->Text = L"label_HideTheNextStep";
			// 
			// panel_ZoneDesc
			// 
			this->panel_ZoneDesc->Controls->Add(this->label_ZoneDesc);
			this->panel_ZoneDesc->Location = System::Drawing::Point(473, 6);
			this->panel_ZoneDesc->Name = L"panel_ZoneDesc";
			this->panel_ZoneDesc->Size = System::Drawing::Size(150, 150);
			this->panel_ZoneDesc->TabIndex = 17;
			this->panel_ZoneDesc->Visible = false;
			// 
			// label_ZoneDesc
			// 
			this->label_ZoneDesc->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->label_ZoneDesc->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_ZoneDesc->Location = System::Drawing::Point(10, 10);
			this->label_ZoneDesc->Name = L"label_ZoneDesc";
			this->label_ZoneDesc->Size = System::Drawing::Size(130, 130);
			this->label_ZoneDesc->TabIndex = 0;
			this->label_ZoneDesc->Text = L"label_ZoneDesc";
			// 
			// panel_ObjectiveType
			// 
			this->panel_ObjectiveType->Controls->Add(this->label_ObjectiveType);
			this->panel_ObjectiveType->Location = System::Drawing::Point(5, 5);
			this->panel_ObjectiveType->Name = L"panel_ObjectiveType";
			this->panel_ObjectiveType->Size = System::Drawing::Size(250, 100);
			this->panel_ObjectiveType->TabIndex = 16;
			this->panel_ObjectiveType->Visible = false;
			// 
			// label_ObjectiveType
			// 
			this->label_ObjectiveType->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->label_ObjectiveType->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_ObjectiveType->Location = System::Drawing::Point(10, 10);
			this->label_ObjectiveType->Name = L"label_ObjectiveType";
			this->label_ObjectiveType->Size = System::Drawing::Size(250, 100);
			this->label_ObjectiveType->TabIndex = 0;
			this->label_ObjectiveType->Text = L"label_ObjectiveType";
			// 
			// panel_PRSex
			// 
			this->panel_PRSex->Controls->Add(this->label_PRSex);
			this->panel_PRSex->Location = System::Drawing::Point(5, 5);
			this->panel_PRSex->Name = L"panel_PRSex";
			this->panel_PRSex->Size = System::Drawing::Size(150, 150);
			this->panel_PRSex->TabIndex = 15;
			this->panel_PRSex->Visible = false;
			// 
			// label_PRSex
			// 
			this->label_PRSex->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->label_PRSex->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_PRSex->Location = System::Drawing::Point(10, 10);
			this->label_PRSex->Name = L"label_PRSex";
			this->label_PRSex->Size = System::Drawing::Size(130, 130);
			this->label_PRSex->TabIndex = 0;
			this->label_PRSex->Text = L"label_PRSex";
			// 
			// panel_PRRace
			// 
			this->panel_PRRace->Controls->Add(this->label_PRRace);
			this->panel_PRRace->Location = System::Drawing::Point(5, 5);
			this->panel_PRRace->Name = L"panel_PRRace";
			this->panel_PRRace->Size = System::Drawing::Size(150, 150);
			this->panel_PRRace->TabIndex = 14;
			this->panel_PRRace->Visible = false;
			// 
			// label_PRRace
			// 
			this->label_PRRace->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->label_PRRace->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_PRRace->Location = System::Drawing::Point(10, 10);
			this->label_PRRace->Name = L"label_PRRace";
			this->label_PRRace->Size = System::Drawing::Size(130, 130);
			this->label_PRRace->TabIndex = 0;
			this->label_PRRace->Text = L"label_PRRace";
			// 
			// panel_QuestType
			// 
			this->panel_QuestType->Controls->Add(this->label_QuestType);
			this->panel_QuestType->Location = System::Drawing::Point(5, 5);
			this->panel_QuestType->Name = L"panel_QuestType";
			this->panel_QuestType->Size = System::Drawing::Size(150, 150);
			this->panel_QuestType->TabIndex = 13;
			this->panel_QuestType->Visible = false;
			// 
			// label_QuestType
			// 
			this->label_QuestType->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->label_QuestType->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_QuestType->Location = System::Drawing::Point(10, 10);
			this->label_QuestType->Name = L"label_QuestType";
			this->label_QuestType->Size = System::Drawing::Size(130, 130);
			this->label_QuestType->TabIndex = 0;
			this->label_QuestType->Text = L"label_QuestType";
			// 
			// panel_Repeatable
			// 
			this->panel_Repeatable->Controls->Add(this->label_Repeatable);
			this->panel_Repeatable->Location = System::Drawing::Point(5, 5);
			this->panel_Repeatable->Name = L"panel_Repeatable";
			this->panel_Repeatable->Size = System::Drawing::Size(150, 150);
			this->panel_Repeatable->TabIndex = 12;
			this->panel_Repeatable->Visible = false;
			// 
			// label_Repeatable
			// 
			this->label_Repeatable->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->label_Repeatable->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_Repeatable->Location = System::Drawing::Point(10, 10);
			this->label_Repeatable->Name = L"label_Repeatable";
			this->label_Repeatable->Size = System::Drawing::Size(130, 130);
			this->label_Repeatable->TabIndex = 0;
			this->label_Repeatable->Text = L"label_Repeatable";
			// 
			// panel_Raidable
			// 
			this->panel_Raidable->Controls->Add(this->label_Raidable);
			this->panel_Raidable->Location = System::Drawing::Point(5, 5);
			this->panel_Raidable->Name = L"panel_Raidable";
			this->panel_Raidable->Size = System::Drawing::Size(150, 150);
			this->panel_Raidable->TabIndex = 11;
			this->panel_Raidable->Visible = false;
			// 
			// label_Raidable
			// 
			this->label_Raidable->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->label_Raidable->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_Raidable->Location = System::Drawing::Point(10, 10);
			this->label_Raidable->Name = L"label_Raidable";
			this->label_Raidable->Size = System::Drawing::Size(130, 130);
			this->label_Raidable->TabIndex = 0;
			this->label_Raidable->Text = L"label_Raidable";
			// 
			// panel_QuestStingEditor
			// 
			this->panel_QuestStingEditor->Controls->Add(this->button_QuestStringEditorCancel);
			this->panel_QuestStingEditor->Controls->Add(this->button_QuestStringEditorConfirm);
			this->panel_QuestStingEditor->Controls->Add(this->groupBox_QuestStringEditor);
			this->panel_QuestStingEditor->Location = System::Drawing::Point(4, 3);
			this->panel_QuestStingEditor->Name = L"panel_QuestStingEditor";
			this->panel_QuestStingEditor->Size = System::Drawing::Size(465, 212);
			this->panel_QuestStingEditor->TabIndex = 5;
			this->panel_QuestStingEditor->Visible = false;
			// 
			// button_QuestStringEditorCancel
			// 
			this->button_QuestStringEditorCancel->Location = System::Drawing::Point(94, 186);
			this->button_QuestStringEditorCancel->Name = L"button_QuestStringEditorCancel";
			this->button_QuestStringEditorCancel->Size = System::Drawing::Size(75, 23);
			this->button_QuestStringEditorCancel->TabIndex = 10;
			this->button_QuestStringEditorCancel->Text = L"취소";
			this->button_QuestStringEditorCancel->UseVisualStyleBackColor = true;
			this->button_QuestStringEditorCancel->Click += gcnew System::EventHandler(this, &QuestForm::button_QuestStringEditorCancel_Click);
			// 
			// button_QuestStringEditorConfirm
			// 
			this->button_QuestStringEditorConfirm->Location = System::Drawing::Point(12, 186);
			this->button_QuestStringEditorConfirm->Name = L"button_QuestStringEditorConfirm";
			this->button_QuestStringEditorConfirm->Size = System::Drawing::Size(75, 23);
			this->button_QuestStringEditorConfirm->TabIndex = 9;
			this->button_QuestStringEditorConfirm->Text = L"확인";
			this->button_QuestStringEditorConfirm->UseVisualStyleBackColor = true;
			this->button_QuestStringEditorConfirm->Click += gcnew System::EventHandler(this, &QuestForm::button_QuestStringEditorConfirm_Click);
			// 
			// groupBox_QuestStringEditor
			// 
			this->groupBox_QuestStringEditor->Controls->Add(this->textBox_QuestStringEditor);
			this->groupBox_QuestStringEditor->Location = System::Drawing::Point(4, 4);
			this->groupBox_QuestStringEditor->Name = L"groupBox_QuestStringEditor";
			this->groupBox_QuestStringEditor->Size = System::Drawing::Size(457, 176);
			this->groupBox_QuestStringEditor->TabIndex = 8;
			this->groupBox_QuestStringEditor->TabStop = false;
			this->groupBox_QuestStringEditor->Text = L"String";
			// 
			// textBox_QuestStringEditor
			// 
			this->textBox_QuestStringEditor->Location = System::Drawing::Point(6, 19);
			this->textBox_QuestStringEditor->Multiline = true;
			this->textBox_QuestStringEditor->Name = L"textBox_QuestStringEditor";
			this->textBox_QuestStringEditor->Size = System::Drawing::Size(445, 151);
			this->textBox_QuestStringEditor->TabIndex = 1;
			// 
			// QuestForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1146, 839);
			this->Controls->Add(this->splitContainer1);
			this->Controls->Add(this->toolStrip_Quest);
			this->Controls->Add(this->statusStrip_Quest);
			this->Controls->Add(this->menuStrip_Quest);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->KeyPreview = true;
			this->MainMenuStrip = this->menuStrip_Quest;
			this->Name = L"QuestForm";
			this->Text = L"QuestForm";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &QuestForm::QuestForm_FormClosing);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &QuestForm::QuestForm_KeyDown);
			this->Load += gcnew System::EventHandler(this, &QuestForm::QuestForm_Load);
			this->menuStrip_Quest->ResumeLayout(false);
			this->menuStrip_Quest->PerformLayout();
			this->contextMenuStrip_QuestList->ResumeLayout(false);
			this->statusStrip_Quest->ResumeLayout(false);
			this->statusStrip_Quest->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->groupBox_FindQuest->ResumeLayout(false);
			this->groupBox_FindQuest->PerformLayout();
			this->splitContainer2->Panel1->ResumeLayout(false);
			this->splitContainer2->Panel2->ResumeLayout(false);
			this->splitContainer2->ResumeLayout(false);
			this->groupBox_QuestUObjective->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestUObjective))->EndInit();
			this->groupBox_QuestOObjective->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestOObjective))->EndInit();
			this->groupBox_QuestPR->ResumeLayout(false);
			this->groupBox_QuestPR->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestPRFaction))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestPR))->EndInit();
			this->groupBox_BastInfo->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView_QuestBaseInfo))->EndInit();
			this->panel_DeadLine->ResumeLayout(false);
			this->panel_HideTheNextStep->ResumeLayout(false);
			this->panel_ZoneDesc->ResumeLayout(false);
			this->panel_ObjectiveType->ResumeLayout(false);
			this->panel_PRSex->ResumeLayout(false);
			this->panel_PRRace->ResumeLayout(false);
			this->panel_QuestType->ResumeLayout(false);
			this->panel_Repeatable->ResumeLayout(false);
			this->panel_Raidable->ResumeLayout(false);
			this->panel_QuestStingEditor->ResumeLayout(false);
			this->groupBox_QuestStringEditor->ResumeLayout(false);
			this->groupBox_QuestStringEditor->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	/////////////////////////////////////////////////////////////////////////////////////
	// 임의로 추가한 함수들 시작

	public: bool Begin() {
				 this->m_pQc->SerializeTreeView_QuestList();
				 this->m_pQc->SerializeDataGridView();
				 return true;
			}

	public: void End() {
				this->treeView_QuestList->Nodes->Clear();
			}

	private: int GetQuestIDFromTVText_QL(String^ text) {
				if (0 >= text->Length) return -1;

				const char* szText = MStringToCharPointer(text->ToString());
				char szCopiedText[512];
				strncpy(szCopiedText, szText, 512);

				strtok(szCopiedText, "(");
				const char* szQuestID = strtok(nullptr, ")"); 
				int nQuestID = atoi(szQuestID);

				return nQuestID;
			}

	private: const char* GetQuestTitleFromTVText_QL(String^ text) {
				  if (0 >= text->Length) return nullptr;

				  const char* szText = MStringToCharPointer(text->ToString());
				  char szCopiedText[512];
				  strncpy(szCopiedText, szText, 512);

				  const char* szQuestTitle = strtok(szCopiedText, "(");

				  return szQuestTitle;
			}

	private: void CutAndPasteTVNode_QLByPrerequisitesQuest(TreeNode^ tnDestination, DataTable^ dtPR) {
				System::Collections::IEnumerator^ etor = this->treeView_QuestList->Nodes->GetEnumerator();

				while (etor->MoveNext())
				{
					TreeNode^ tnCurrent =  safe_cast<TreeNode^>(etor->Current);
					int nQuestID = GetQuestIDFromTVText_QL(tnCurrent->Text);

					String^ strFilter = String::Format("QID={0}", nQuestID);
					array<DataRow^>^ arrDr = dtPR->Select(strFilter);
					if (0 >= arrDr->Length) continue;

					int nPRQuestID = MStringToInt(arrDr[0][QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST]->ToString());

					if (this->GetQuestIDFromTVText_QL(tnDestination->Text) == nPRQuestID)
					{
						this->m_pQc->CutAndPasteTreeNode(tnDestination, tnCurrent);
						tnCurrent->Remove();
					}
				}
			}

	private: void ChangeBackColorOfTreeView_QuestList()
            {
				 int nQuestID = this->m_pQc->GetSelectedQuestID();
				 this->ChangeBackColorOfTreeView_QuestList(nQuestID, Color::Red);
			}

	private: void ChangeBackColorOfTreeView_QuestList(int nQuestID, Color color)
		    {
				String^ strText = this->m_pQc->MakeTreeView_QuestListText(nQuestID);
				TreeNode^ tnResult = this->m_pQc->SearchTreeNode(this->treeView_QuestList->Nodes, strText);

				if (nullptr == tnResult) return;
				tnResult->ForeColor = color;
		    }

	private: bool IsValidCell_QuestBaseInfo(DataGridViewCell^ dgvc)
			 {
				 String^ strColumnName = dgvc->DataGridView->Columns[dgvc->ColumnIndex]->Name;
				 if (QUEST_DB_TABLE_QUEST_COLUMN_RAIDABLE == strColumnName)
				 {
					 int nRaidable = MStringToInt(dgvc->Value->ToString());
					 if (1 < nRaidable )
					 {
						 return false;
					 }
				 }
				 else if (QUEST_DB_TABLE_QUEST_COLUMN_REPEATABLE == strColumnName)
				 {
					 int nRaidable = MStringToInt(dgvc->Value->ToString());
					 if (1 < nRaidable )
					 {
						 return false;
					 }
				 }
				 else if (QUEST_DB_TABLE_QUEST_COLUMN_TYPE == strColumnName)
				 {
					 int nType = MStringToInt(dgvc->Value->ToString());
					 if (QT_MAX <= nType)
					 {
						 return false;
					 }
				 }
				 else if (QUEST_DB_TABLE_QUEST_COLUMN_HIDETHENEXTSTEP == strColumnName)
				 {
					 int nHideTheNextStep = MStringToInt(dgvc->Value->ToString());
					 if (1 < nHideTheNextStep)
					 {
						 return false;
					 }
				 }
				 
				 return true;
			 }

	private: bool IsValidCell_QuestPR(DataGridViewCell^ dgvc)
			 {
				 String^ strColumnName = dgvc->DataGridView->Columns[dgvc->ColumnIndex]->Name;
				 if (QUEST_DB_TABLE_QUEST_COLUMN_PR_RACE == strColumnName)
				 {					 
					 int nRace = MStringToInt(dgvc->Value->ToString());
					 if (RACE_MAX <= nRace)
					 {
						 return false;
					 }
				 }
				 else if (QUEST_DB_TABLE_QUEST_COLUMN_PR_SEX == strColumnName)
				 {					 
					 int nSex = MStringToInt(dgvc->Value->ToString());
					 if (SEX_MALE <= nSex)
					 {
						 return false;
					 }
				 }

				 return true;
			 }

	private: bool IsValidCell_QuestPRFaction(DataGridViewCell^ dgvc)
			 {
				 return true;
			 }

	private: bool IsValidCell_QuestOObjective(DataGridViewCell^ dgvc)
			 {
				 String^ strColumnName = dgvc->DataGridView->Columns[dgvc->ColumnIndex]->Name;
				 if (QUEST_DB_TABLE_OBJECTIVE_COLUMN_TYPE == strColumnName)
				 {					 
					 int nType = MStringToInt(dgvc->Value->ToString());
					 if (QOT_MAX <= nType)
					 {
						 return false;
					 }
				 }

				 return true;
			 }

	private: bool IsValidCell_QuestUObjective(DataGridViewCell^ dgvc)
			 {
				 String^ strColumnName = dgvc->DataGridView->Columns[dgvc->ColumnIndex]->Name;
				 if (QUEST_DB_TABLE_OBJECTIVE_COLUMN_TYPE == strColumnName)
				 {					 
					 int nType = MStringToInt(dgvc->Value->ToString());
					 if (QOT_MAX <= nType)
					 {
						 return false;
					 }
				 }

				 return true;
			 }

	private: void SetLabelHelpText(String^ strKey, String^ strDelim, Label^ label_help)
			 {
				 label_help->Text = "";
				 String^ strValue = gcnew String(this->m_pQc->GetStringFromStrQuestToolTable(strKey));

				 const char* szValue = MStringToCharPointer(strValue);
				 const char* szDelim = MStringToCharPointer(strDelim);

				 char szCopiedValue[512];
				 strncpy(szCopiedValue, szValue, 512);

				 const char* szToken = strtok(szCopiedValue, szDelim);
				 while (nullptr != szToken)
				 {
					 label_help->Text = String::Format("{0}\n{1}", label_help->Text, gcnew String(szToken));
					 szToken = strtok(nullptr, szDelim);
				 }
			 }

	public: void HideAllPanel() {
				this->panel_QuestStingEditor->Visible = false;
				this->panel_ObjectiveType->Visible = false;
				this->panel_PRRace->Visible = false;
				this->panel_PRSex->Visible = false;
				this->panel_QuestType->Visible = false;
				this->panel_Raidable->Visible = false;
				this->panel_Repeatable->Visible = false;
				this->panel_ZoneDesc->Visible = false;
				this->panel_HideTheNextStep->Visible = false;
				this->panel_DeadLine->Visible = false;
			}

	public: String^ MakeTreeView_QuestListText(String^ strTitle, int nQuestID)
			{
				String^ strText = String::Format("{0}({1})", strTitle, nQuestID);

				return strText;
			}


	public: System::Void SerializeTreeView_QuestList(DataTable^& dtPR, DataTable^& dtBaseInfo) {
				 this->treeView_QuestList->Nodes->Clear();
				 this->treeView_QuestList->BeginUpdate();

				 array<DataRow^>^ arrDrPR = dtPR->Select();

				 for (int i = 0; arrDrPR->Length > i; i++)
				 {
					 DataRow^ drPR = arrDrPR[i];

					 int nQuestID = MStringToInt(drPR[QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
					 if (true == this->m_pQc->IsHidedQuest(nQuestID)) continue;

					 int nPRQuestID = MStringToInt(drPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST]->ToString());
					 String ^ strText = this->m_pQc->MakeTreeView_QuestListText(nQuestID);

					 TreeNode^ tnChild = nullptr;

					 if (0 >= nPRQuestID)
					 {
						 tnChild = this->treeView_QuestList->Nodes->Add(strText);
					 }
					 else
					 {
						 String^ strPRText = this->m_pQc->MakeTreeView_QuestListText(nPRQuestID);
						 TreeNode^ tnSearchedResult = this->m_pQc->SearchTreeNode(treeView_QuestList->Nodes, strPRText);

						 if (nullptr == tnSearchedResult)
						 {
							 tnChild = this->treeView_QuestList->Nodes->Add(strText);
						 }
						 else
						 {
							 tnChild = tnSearchedResult->Nodes->Add(strText);
						 }
					 }

					 if (true == this->m_pQc->IsModifiedQuest(nQuestID))
					 {
						 this->ChangeBackColorOfTreeView_QuestList(nQuestID, Color::Red);
					 }
					 
					 this->CutAndPasteTVNode_QLByPrerequisitesQuest(tnChild, dtPR);
				 }

				 int nCuttedQuestID = this->m_pQc->GetCuttedQuestID();
				 if (0 < nCuttedQuestID)
				 {
					 String^ strCuttedQuestText = this->m_pQc->MakeTreeView_QuestListText(nCuttedQuestID);
					 TreeNode^ tnSearchedResult = this->m_pQc->SearchTreeNode(treeView_QuestList->Nodes, strCuttedQuestText);

					 if (nullptr != tnSearchedResult)
					 {
						 tnSearchedResult->Remove();
					 }
				 }

				 this->treeView_QuestList->ExpandAll();
				 this->treeView_QuestList->EndUpdate();
			 }

	public: System::Void SerializeDataGridView_QuestBaseInfo(DataTable^ dt) {
				this->dataGridView_QuestBaseInfo->DataSource = dt;

				this->dataGridView_QuestBaseInfo->Columns[QUEST_DB_TABLE_QUEST_COLUMN_QID]->Visible = false; 
				this->dataGridView_QuestBaseInfo->Columns[QUEST_DB_TABLE_QUEST_COLUMN_TITLE]->ReadOnly = true; 
				this->dataGridView_QuestBaseInfo->Columns[QUEST_DB_TABLE_QUEST_COLUMN_ZONEDESCRIPTION]->ReadOnly = true; 
				this->dataGridView_QuestBaseInfo->Columns[QUEST_DB_TABLE_QUEST_COLUMN_DESCRIPTION]->ReadOnly = true;
				this->dataGridView_QuestBaseInfo->Columns[QUEST_DB_TABLE_QUEST_COLUMN_REGDATE]->ReadOnly = true;
				this->dataGridView_QuestBaseInfo->Columns[QUEST_DB_TABLE_QUEST_COLUMN_DEADLINE]->ReadOnly = true;
			}

	public: System::Void SerializeDataGridView_QuestPR(DataTable^ dt) {
				this->dataGridView_QuestPR->DataSource = dt;
				this->dataGridView_QuestPR->Columns[QUEST_DB_TABLE_QUEST_COLUMN_QID]->Visible = false; 
				this->dataGridView_QuestPR->Columns[QUEST_DB_TABLE_QUEST_COLUMN_PR_HaveFID]->Visible = false;
			}

	public: System::Void SerializeDataGridView_QuestPRFaction(DataTable^ dt) {
				this->dataGridView_QuestPRFaction->DataSource = dt;
				this->dataGridView_QuestPRFaction->Columns[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QRFID]->Visible = false; 
				this->dataGridView_QuestPRFaction->Columns[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID]->Visible = false; 
			}

	public: System::Void SerializeDataGridView_QuestOObjective(DataTable^ dt) {
				this->dataGridView_QuestOObjective->DataSource = dt;
				this->dataGridView_QuestOObjective->Columns[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->Visible = false; 
				this->dataGridView_QuestOObjective->Columns[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ReadOnly = true; 
				this->dataGridView_QuestOObjective->Columns[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE]->ReadOnly = true; 
				this->dataGridView_QuestOObjective->Columns[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION]->ReadOnly = true; 
			}

	public: System::Void SerializeDataGridView_QuestUObjective(DataTable^ dt) {
				this->dataGridView_QuestUObjective->DataSource = dt;
				this->dataGridView_QuestUObjective->Columns[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->Visible = false; 
				this->dataGridView_QuestUObjective->Columns[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ReadOnly = true; 
				this->dataGridView_QuestUObjective->Columns[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE]->ReadOnly = true; 
				this->dataGridView_QuestUObjective->Columns[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION]->ReadOnly = true; 
			}

	// 임의로 추가한 함수들 끝
	/////////////////////////////////////////////////////////////////////////////////////



	private: System::Void QuestForm_Load(System::Object^  sender, System::EventArgs^  e) {
				 this->comboBox_FindQuest->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"이름", L"ID"});
				 this->comboBox_FindQuest->SelectedIndex = 0;

				 this->dateTimePicker_DeadLine->Format = DateTimePickerFormat::Custom;
				 this->dateTimePicker_DeadLine->CustomFormat = "yyyy-MM-dd HH:mm";


				 String^ strKey;
				 String^ strDelim = "\\";

				 strKey = "QT_H_RAIDABLE";	 
				 this->SetLabelHelpText(strKey, strDelim, this->label_Raidable);
				 strKey = "QT_H_REPEATABLE";
				 this->SetLabelHelpText(strKey, strDelim, this->label_Repeatable);
				 strKey = "QT_H_QUESTTYPE";
				 this->SetLabelHelpText(strKey, strDelim, this->label_QuestType);
				 strKey = "QT_H_PRRACE";
				 this->SetLabelHelpText(strKey, strDelim, this->label_PRRace);
				 strKey = "QT_H_PRSEX";
				 this->SetLabelHelpText(strKey, strDelim, this->label_PRSex);
				 strKey = "QT_H_OBJECTIVETYPE";
				 this->SetLabelHelpText(strKey, strDelim, this->label_ObjectiveType);
				 strKey = "QT_H_ZONEDESC";
				 this->SetLabelHelpText(strKey, strDelim, this->label_ZoneDesc);
				 strKey = "QT_H_HIDETHENEXTSTEP";
				 this->SetLabelHelpText(strKey, strDelim, this->label_HideTheNextStep);
			 }


	private: System::Void ToolStripMenuItem_Click_RunDialog(System::Object^  sender, System::EventArgs^  e) {
				 this->m_pQc->RunDialogTool();
			 }

	private: System::Void ToolStripMenuItem_Click_Exit(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }

	private: System::Void ToolStripMenuItem_Click_SaveQuest(System::Object^  sender, System::EventArgs^  e) {
				 this->m_pQc->SaveQuests();
			 }

	private: System::Void treeView_QuestList_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
				 this->treeView_QuestList->SelectedNode = e->Node;

				 int nSelectedQuestID = this->GetQuestIDFromTVText_QL(this->treeView_QuestList->SelectedNode->Text);
				 this->m_pQc->SetSelectedQuestID(nSelectedQuestID);

				 this->treeView_QuestList->SelectedNode->ContextMenuStrip = this->contextMenuStrip_QuestList;

				 int nCuttedQuestID = this->m_pQc->GetCuttedQuestID();
				 if (0 < nCuttedQuestID)
				 {
					 this->toolStripMenuItem_Paste->Visible = true;
				 }
				 else
				 {
					 this->toolStripMenuItem_Paste->Visible = false;
				 }
			 }

	private: System::Void treeView_QuestList_BeforeSelect(System::Object^  sender, System::Windows::Forms::TreeViewCancelEventArgs^  e) {
				 if (nullptr == this->treeView_QuestList->SelectedNode) return;
				 this->treeView_QuestList->SelectedNode->BackColor = Color::White;
			 }

	private: System::Void treeView_QuestList_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
				 if (nullptr == this->treeView_QuestList->SelectedNode) return;
				 this->treeView_QuestList->SelectedNode->BackColor = Color::Gray;

				 int nSelectedQuestID = this->GetQuestIDFromTVText_QL(this->treeView_QuestList->SelectedNode->Text);
				 this->m_pQc->SetSelectedQuestID(nSelectedQuestID);
				 this->m_pQc->SerializeDataGridView();

				 if (nullptr == this->treeView_QuestList->SelectedNode) return;
				 this->treeView_QuestList->SelectedNode->ContextMenuStrip = this->contextMenuStrip_QuestList;

				 int nCuttedQuestID = this->m_pQc->GetCuttedQuestID();
				 if (0 < nCuttedQuestID)
				 {
					 this->toolStripMenuItem_Paste->Visible = true;
				 }
				 else
				 {
					 this->toolStripMenuItem_Paste->Visible = false;
				 }

				 this->toolStripStatusLabel_Quest->Text = String::Format("Quest : {0}", this->treeView_QuestList->SelectedNode->Text);

				 
				 this->HideAllPanel();
			 }

	private: System::Void dataGridView_QuestBaseInfo_CellBeginEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellCancelEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);
				 this->m_pQc->SetPrevSelectedCellValue(dgvcSelected->Value);
			 }

	private: System::Void dataGridView_QuestBaseInfo_CellEndEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);

				 if (false == this->IsValidCell_QuestBaseInfo(dgvcSelected))
				 {
					 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
					 dgvcSelected->Value = this->m_pQc->GetPrevSelectedCellValue();
					 return;
				 }
				 
				 int nQuestID = MStringToInt(this->dataGridView_QuestBaseInfo->Rows[e->RowIndex]->Cells[QUEST_DB_TABLE_QUEST_COLUMN_QID]->Value->ToString());
				 String^ strColumnName = this->dataGridView_QuestBaseInfo->Columns[e->ColumnIndex]->Name->ToString();
				 Object^ objValue = dgvcSelected->Value;

				 this->ChangeBackColorOfTreeView_QuestList();
				 this->m_pQc->SetModifiedQuestID(nQuestID);

				 this->m_pQc->UpdateQuestBaseInfo(nQuestID, strColumnName, objValue);
			 }

	private: System::Void dataGridView_QuestPR_CellBeginEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellCancelEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);
				 this->m_pQc->SetPrevSelectedCellValue(dgvcSelected->Value);
			 }

	private: System::Void dataGridView_QuestPR_CellEndEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);

				 if (false == this->IsValidCell_QuestPR(dgvcSelected))
				 {
					 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
					 dgvcSelected->Value = this->m_pQc->GetPrevSelectedCellValue();
					 return;
				 }

				 int nQuestID = MStringToInt(this->dataGridView_QuestPR->Rows[e->RowIndex]->Cells[QUEST_DB_TABLE_QUEST_COLUMN_QID]->Value->ToString());
				 String^ strColumnName = this->dataGridView_QuestPR->Columns[e->ColumnIndex]->Name->ToString();
				 Object^ objValue = this->dataGridView_QuestPR->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Value;

				 this->m_pQc->UpdateQuestPR(nQuestID, strColumnName, objValue);

				 this->ChangeBackColorOfTreeView_QuestList();
				 this->m_pQc->SetModifiedQuestID(nQuestID);
			 }

	private: System::Void dataGridView_QuestPRFaction_UserAddedRow(System::Object^  sender, System::Windows::Forms::DataGridViewRowEventArgs^  e) {
				 DataGridViewRow^ dgvr = this->dataGridView_QuestPRFaction->Rows[e->Row->Index-1];

				 int nSelectedQuestID = this->m_pQc->GetSelectedQuestID();
				 int nMaxQRFID = 0;

				 int nCnt = this->dataGridView_QuestPRFaction->Rows->Count-1;
				 for (int i = 0; nCnt > i; i++)
				 {
					 DataGridViewRow^ dgvr = this->dataGridView_QuestPRFaction->Rows[i];
					 int nQRFID = MStringToInt(dgvr->Cells[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QRFID]->Value->ToString());
					 int nQuestID = MStringToInt(dgvr->Cells[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID]->Value->ToString());

					 if (nMaxQRFID < nQRFID)
						 nMaxQRFID = nQRFID;
				 }
				 
				 String^ strQRFID = String::Format("{0}", nMaxQRFID + 1);
				 dgvr->Cells[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QRFID]->Value = strQRFID;
				 
				 String^ strQuestID = String::Format("{0}", nSelectedQuestID);
				 dgvr->Cells[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID]->Value = strQuestID;

				 int nQuestID = this->m_pQc->GetSelectedQuestID();

				 this->m_pQc->SetModifiedQuestID(nQuestID);
				 this->m_pQc->InsertQuestPRFaction(nMaxQRFID + 1);

				 if (1 == e->Row->Index)
				 {
					 this->ChangeBackColorOfTreeView_QuestList();
					 this->m_pQc->SetModifiedQuestID(nQuestID);
					 this->m_pQc->UpdateQuestPR(nQuestID, QUEST_DB_TABLE_QUEST_COLUMN_PR_HaveFID, 1);
				 }
			  }

	private: System::Void dataGridView_QuestPRFaction_CellBeginEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellCancelEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);
				 this->m_pQc->SetPrevSelectedCellValue(dgvcSelected->Value);
			 }

	private: System::Void dataGridView_QuestPRFaction_CellEndEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);

				 if (false == this->IsValidCell_QuestPRFaction(dgvcSelected))
				 {
					 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
					 dgvcSelected->Value = this->m_pQc->GetPrevSelectedCellValue();
					 return;
				 }

				 int nQRFID = MStringToInt(this->dataGridView_QuestPRFaction->Rows[e->RowIndex]->Cells[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QRFID]->Value->ToString());
				 String^ strColumnName = this->dataGridView_QuestPRFaction->Columns[e->ColumnIndex]->Name->ToString();
				 Object^ objValue = this->dataGridView_QuestPRFaction->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Value;

				 int nQuestID = this->m_pQc->GetSelectedQuestID();

				 this->ChangeBackColorOfTreeView_QuestList();
				 this->m_pQc->SetModifiedQuestID(nQuestID);
				 this->m_pQc->UpdateQuestPRFaction(nQRFID, strColumnName, objValue);
			 }

	private: System::Void dataGridView_QuestPRFaction_UserDeletingRow(System::Object^  sender, System::Windows::Forms::DataGridViewRowCancelEventArgs^  e) {
				 DataGridViewRow^ dgvr = e->Row;
				 int nQRFID = MStringToInt(e->Row->Cells[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QRFID]->Value->ToString());

				 this->m_pQc->DeleteQuestPRFaction(nQRFID);

				 if (2 == dataGridView_QuestPRFaction->Rows->Count)
				 {
					 int nQuestID = this->m_pQc->GetSelectedQuestID();

					 this->ChangeBackColorOfTreeView_QuestList();
					 this->m_pQc->SetModifiedQuestID(nQuestID);
					 this->m_pQc->UpdateQuestPR(nQuestID, QUEST_DB_TABLE_QUEST_COLUMN_PR_HaveFID, safe_cast<Object^>(0));
				 }
			 }

	private: System::Void dataGridView_QuestOObjective_UserAddedRow(System::Object^  sender, System::Windows::Forms::DataGridViewRowEventArgs^  e) {
				 DataGridViewRow^ dgvr = this->dataGridView_QuestOObjective->Rows[e->Row->Index-1];

				 int nQuestID = this->m_pQc->GetSelectedQuestID();
				 int nMaxObjectiveID = 0;
				 int nCnt = this->dataGridView_QuestOObjective->Rows->Count-1;
				 for (int i = 0; nCnt > i; i++)
				 {
					 DataGridViewRow^ dgvr = this->dataGridView_QuestOObjective->Rows[i];

					 int nObjectiveID = MStringToInt(dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value->ToString());
					 if (nObjectiveID > nMaxObjectiveID)
						 nMaxObjectiveID = nObjectiveID;
				 }

				 dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->Value = nQuestID;
				 dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value = nMaxObjectiveID + 1;
				 dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE]->Value = "NewTitle";
				 dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION]->Value = "NewDesc";

				 this->m_pQc->SetModifiedQuestID(nQuestID);
				 this->m_pQc->InsertQuestOObjective(nMaxObjectiveID + 1);
			 }

	private: System::Void dataGridView_QuestOObjective_CellBeginEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellCancelEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);
				 this->m_pQc->SetPrevSelectedCellValue(dgvcSelected->Value);
			 }

	private: System::Void dataGridView_QuestOObjective_CellEndEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);

				 if (false == this->IsValidCell_QuestOObjective(dgvcSelected))
				 {
					 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
					 dgvcSelected->Value = this->m_pQc->GetPrevSelectedCellValue();
					 return;
				 }

				 int nObjectiveID = MStringToInt(this->dataGridView_QuestOObjective->Rows[e->RowIndex]->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value->ToString());
				 String^ strColumnName = this->dataGridView_QuestOObjective->Columns[e->ColumnIndex]->Name->ToString();
				 Object^ objValue = this->dataGridView_QuestOObjective->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Value;
				 int nQuestID = this->m_pQc->GetSelectedQuestID();

				 this->ChangeBackColorOfTreeView_QuestList();
				 this->m_pQc->SetModifiedQuestID(nQuestID);
				 this->m_pQc->UpdateQuestOObjective(nQuestID, nObjectiveID, strColumnName, objValue);
			 }

	private: System::Void dataGridView_QuestOObjective_UserDeletingRow(System::Object^  sender, System::Windows::Forms::DataGridViewRowCancelEventArgs^  e) {
				 int nObjectiveiD = MStringToInt(e->Row->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value->ToString());
				 this->m_pQc->DeleteQuestOObjective(nObjectiveiD);
			 }

	private: System::Void dataGridView_QuestUObjective_UserAddedRow(System::Object^  sender, System::Windows::Forms::DataGridViewRowEventArgs^  e) {
				 DataGridViewRow^ dgvr = this->dataGridView_QuestUObjective->Rows[e->Row->Index-1];

				 int nQuestID = this->m_pQc->GetSelectedQuestID();
				 int nMaxObjectiveID = 0;
				 int nCnt = this->dataGridView_QuestUObjective->Rows->Count-1;
				 for (int i = 0; nCnt > i; i++)
				 {
					 DataGridViewRow^ dgvr = this->dataGridView_QuestUObjective->Rows[i];

					 int nObjectiveID = MStringToInt(dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value->ToString());
					 if (nObjectiveID > nMaxObjectiveID)
						 nMaxObjectiveID = nObjectiveID;
				 }

				 dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->Value = nQuestID;
				 dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value = nMaxObjectiveID + 1;

				 dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE]->Value = "NewTitle";
				 dgvr->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION]->Value = "NewDesc";

				 this->m_pQc->SetModifiedQuestID(nQuestID);
				 this->m_pQc->InsertQuestUObjective(nMaxObjectiveID + 1);
			 }

	private: System::Void dataGridView_QuestUObjective_CellBeginEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellCancelEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);
				 this->m_pQc->SetPrevSelectedCellValue(dgvcSelected->Value);
			 }

	private: System::Void dataGridView_QuestUObjective_CellEndEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);

				 if (false == this->IsValidCell_QuestUObjective(dgvcSelected))
				 {
					 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
					 dgvcSelected->Value = this->m_pQc->GetPrevSelectedCellValue();
					 return;
				 }

				 int nObjectiveID = MStringToInt(this->dataGridView_QuestUObjective->Rows[e->RowIndex]->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value->ToString());
				 String^ strColumnName = this->dataGridView_QuestUObjective->Columns[e->ColumnIndex]->Name->ToString();
				 Object^ objValue = this->dataGridView_QuestUObjective->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Value;

				 int nQuestID = this->m_pQc->GetSelectedQuestID();

				 this->ChangeBackColorOfTreeView_QuestList();
				 this->m_pQc->SetModifiedQuestID(nQuestID);
				 this->m_pQc->UpdateQuestUObjective(nQuestID, nObjectiveID, strColumnName, objValue);
			 }

	private: System::Void dataGridView_QuestUObjective_UserDeletingRow(System::Object^  sender, System::Windows::Forms::DataGridViewRowCancelEventArgs^  e) {
				 int nObjectiveID = MStringToInt(e->Row->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value->ToString());
				 this->m_pQc->DeleteQuestUObjective(nObjectiveID);
			 }


	private: System::Void dataGridView_QuestBaseInfo_CellEnter(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvc = this->dataGridView_QuestBaseInfo->Rows[e->RowIndex]->Cells[e->ColumnIndex];
				 this->m_pQc->SetSelectedCell(dgvc);

				 String^ strColumnName = this->dataGridView_QuestBaseInfo->Columns[e->ColumnIndex]->Name->ToString();

				 this->HideAllPanel();
				 
				 if (QUEST_DB_TABLE_QUEST_COLUMN_TITLE == strColumnName || 
					 QUEST_DB_TABLE_QUEST_COLUMN_DESCRIPTION == strColumnName || 
					 QUEST_DB_TABLE_QUEST_COLUMN_ZONEDESCRIPTION == strColumnName)
				 {
					 this->textBox_QuestStringEditor->Text = dgvc->Value->ToString();
					 this->panel_QuestStingEditor->Visible = true;

					 if (QUEST_DB_TABLE_QUEST_COLUMN_ZONEDESCRIPTION == strColumnName)
					 {
						 this->panel_ZoneDesc->Visible = true;
					 }
				 }

				 if (QUEST_DB_TABLE_QUEST_COLUMN_RAIDABLE == strColumnName)
				 {
					 this->panel_Raidable->Visible = true;
				 }

				 if (QUEST_DB_TABLE_QUEST_COLUMN_REPEATABLE == strColumnName)
				 {
					 this->panel_Repeatable->Visible = true;
				 }

				 if (QUEST_DB_TABLE_QUEST_COLUMN_TYPE == strColumnName)
				 {
					 this->panel_QuestType->Visible = true;
				 }
				 
				 if (QUEST_DB_TABLE_QUEST_COLUMN_HIDETHENEXTSTEP == strColumnName)
				 {
					 this->panel_HideTheNextStep->Visible = true;
				 }

				 if (QUEST_DB_TABLE_QUEST_COLUMN_DEADLINE == strColumnName)
				 {
					 this->panel_DeadLine->Visible = true;
				 }
			 }

	private: System::Void dataGridView_QuestPR_CellEnter(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvc = this->dataGridView_QuestPR->Rows[e->RowIndex]->Cells[e->ColumnIndex];
				 this->m_pQc->SetSelectedCell(dgvc);

				 String^ strColumnName = this->dataGridView_QuestPR->Columns[e->ColumnIndex]->Name->ToString();

				 this->HideAllPanel();
				 if (QUEST_DB_TABLE_QUEST_COLUMN_PR_RACE == strColumnName)
				 {
					 this->panel_PRRace->Visible = true;
				 }

				 if (QUEST_DB_TABLE_QUEST_COLUMN_PR_SEX == strColumnName)
				 {
					 this->panel_PRSex->Visible = true;
				 }
			 }

	private: System::Void dataGridView_QuestOObjective_CellEnter(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvc = this->dataGridView_QuestOObjective->Rows[e->RowIndex]->Cells[e->ColumnIndex];
				 this->m_pQc->SetSelectedCell(dgvc);

				 String^ strColumnName = this->dataGridView_QuestOObjective->Columns[e->ColumnIndex]->Name->ToString();

				 this->HideAllPanel();
				 if (QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE == strColumnName || 
					 QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION == strColumnName)
				 {
					 this->textBox_QuestStringEditor->Text = dgvc->Value->ToString();
					 this->panel_QuestStingEditor->Visible = true;
				 }

				 if (QUEST_DB_TABLE_OBJECTIVE_COLUMN_TYPE == strColumnName)
				 {
					 this->panel_ObjectiveType->Visible = true;
				 }
			 }

	private: System::Void dataGridView_QuestUObjective_CellEnter(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 DataGridViewCell^ dgvc = this->dataGridView_QuestUObjective->Rows[e->RowIndex]->Cells[e->ColumnIndex];
				 this->m_pQc->SetSelectedCell(dgvc);

				 this->HideAllPanel();
				 String^ strColumnName = this->dataGridView_QuestUObjective->Columns[e->ColumnIndex]->Name->ToString();
				 if (QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE == strColumnName || 
					 QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION == strColumnName)
				 {
					 this->textBox_QuestStringEditor->Text = dgvc->Value->ToString();
					 this->panel_QuestStingEditor->Visible = true;
				 }

				 if (QUEST_DB_TABLE_OBJECTIVE_COLUMN_TYPE == strColumnName)
				 {
					 this->panel_ObjectiveType->Visible = true;
				 }
			 }

	private: System::Void button_QuestStringEditorConfirm_Click(System::Object^  sender, System::EventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);

				 int nQuestID = this->m_pQc->GetSelectedQuestID();
				 String^ strColumnName = dgvcSelected->DataGridView->Columns[dgvcSelected->ColumnIndex]->Name->ToString();
				 Object^ objValue = this->textBox_QuestStringEditor->Text;

				 DataTable^ dt = safe_cast<DataTable^>(dgvcSelected->DataGridView->DataSource);
				 if (QUEST_DB_VTABLE_QUESTBASEINFO == dt->TableName->ToString())
				 {
					 this->ChangeBackColorOfTreeView_QuestList();
					 this->m_pQc->SetModifiedQuestID(nQuestID);
					 this->m_pQc->UpdateQuestBaseInfo(nQuestID, strColumnName, objValue);
					 if (QUEST_DB_TABLE_QUEST_COLUMN_TITLE == strColumnName)
					 {
						 String^ strTitleText = dgvcSelected->DataGridView->Rows[dgvcSelected->RowIndex]->Cells[dgvcSelected->ColumnIndex]->Value->ToString();
						 String^ strQuestListText = this->MakeTreeView_QuestListText(strTitleText, nQuestID);
						 TreeNode^ tnSearchedResult = this->m_pQc->SearchTreeNode(this->treeView_QuestList->Nodes, strQuestListText);

						 String^ strEditorText = this->textBox_QuestStringEditor->Text;
						 String^ strNewQuestListText = this->MakeTreeView_QuestListText(strEditorText, nQuestID);
						 tnSearchedResult->Text = strNewQuestListText;
					 }
				 }
				 else if (QUEST_DB_VTABLE_OOBJECTIVE == dt->TableName->ToString())
				 {
					 int nObjectiveID = MStringToInt(dgvcSelected->DataGridView->Rows[dgvcSelected->RowIndex]->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value->ToString());

					 this->ChangeBackColorOfTreeView_QuestList();
					 this->m_pQc->SetModifiedQuestID(nQuestID);
					 this->m_pQc->UpdateQuestOObjective(nQuestID, nObjectiveID, strColumnName, objValue);
				 }
				 else if (QUEST_DB_VTABLE_UOBJECTIVE == dt->TableName->ToString())
				 {
					 int nObjectiveID = MStringToInt(dgvcSelected->DataGridView->Rows[dgvcSelected->RowIndex]->Cells[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->Value->ToString());

					 this->ChangeBackColorOfTreeView_QuestList();
					 this->m_pQc->SetModifiedQuestID(nQuestID);
					 this->m_pQc->UpdateQuestUObjective(nQuestID, nObjectiveID, strColumnName, objValue);
				 }
				 dgvcSelected->DataGridView->Rows[dgvcSelected->RowIndex]->Cells[dgvcSelected->ColumnIndex]->Value = this->textBox_QuestStringEditor->Text;
			 }

	private: System::Void button_QuestStringEditorCancel_Click(System::Object^  sender, System::EventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);

				 textBox_QuestStringEditor->Text = dgvcSelected->Value->ToString();
			 }

    private: System::Void ToolStripMenuItem_Cut_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->m_pQc->CutQuests();
				 this->m_pQc->SetModifiedQuestID(this->m_pQc->GetSelectedQuestID());
				 this->m_pQc->SerializeTreeView_QuestList();
				 this->m_pQc->SerializeDataGridView();
			 }

	private: System::Void ToolStripMenuItem_Paste_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->m_pQc->PasteQuests();
				 this->m_pQc->SetModifiedQuestID(this->m_pQc->GetSelectedQuestID());
				 this->m_pQc->SerializeTreeView_QuestList();
				 this->m_pQc->SerializeDataGridView();
			 }

	private: System::Void ToolStripMenuItem_Insert_Click(System::Object^  sender, System::EventArgs^  e) {
				 InsertQuest iq;
				 iq.ShowDialog();
				 Windows::Forms::DialogResult dr = iq.DialogResult;
				 if (Windows::Forms::DialogResult::OK == dr)
				 {
					 int nQuestID = iq.GetQuestID();
					 if (false == this->m_pQc->InsertQuests(nQuestID))
					 {
						 Windows::Forms::MessageBox::Show("잘못된 퀘스트ID 입니다");
						 return;
					 }
					 this->m_pQc->SetModifiedQuestID(nQuestID);
					 this->m_pQc->SerializeTreeView_QuestList();
					 this->m_pQc->SerializeDataGridView();
				 }
			 }

	private: System::Void ToolStripMenuItem_Delete_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->m_pQc->DeleteQuests();
				 this->m_pQc->SetModifiedQuestID(this->m_pQc->GetSelectedQuestID());
				 this->m_pQc->SerializeTreeView_QuestList();
				 this->m_pQc->SerializeDataGridView();
				 this->treeView_QuestList->SelectedNode = nullptr;
			 }
	

	private: System::Void button_FindQuest_Click(System::Object^  sender, System::EventArgs^  e) {
				 if (0 == this->comboBox_FindQuest->SelectedIndex)
				 {
					 String^ strTitle = this->textBox_FindQuest->Text->ToString();
					 this->m_pQc->SetHidedQuestIDByTitle(strTitle);
				 }
				 else if (1 == this->comboBox_FindQuest->SelectedIndex)
				 {
					 int nQuestID = MStringToInt(this->textBox_FindQuest->Text->ToString());
					 this->m_pQc->SetHidedQuestIDByID(nQuestID);
				 }

				 this->m_pQc->SerializeTreeView_QuestList();
				 this->m_pQc->SerializeDataGridView();
			 }

	private: System::Void ToolStripMenuItem_AllOpen_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->m_pQc->SetSelectedQuestID(-1);
				 this->treeView_QuestList->ExpandAll();
				 this->m_pQc->SerializeDataGridView();
		 	 }

	private: System::Void ToolStripMenuItem_AllClose_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->m_pQc->SetSelectedQuestID(-1);
				 this->treeView_QuestList->CollapseAll();
				 this->m_pQc->SerializeDataGridView();
				 this->HideAllPanel();
			 }

	private: System::Void TextBox_FindQuest_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 if (Keys::Enter ==  e->KeyCode)
				 {
					 this->button_FindQuest_Click(nullptr, nullptr);
				 }
			 }

	private: System::Void toolStripMenuItem_ReloadQuest_Click(System::Object^  sender, System::EventArgs^  e) {
				 Windows::Forms::DialogResult dr = Windows::Forms::MessageBox::Show("변경된 사항들은 저장되지 않고 사라집니다.\n새로 고치시겠습니까? ", "퀘스트 새로고침", MessageBoxButtons::OKCancel);
				 if (Windows::Forms::DialogResult::OK == dr)
				 {
					 this->m_pQc->ReloadQuest();
				 }
			 }

	private: System::Void dataGridView_QuestBaseInfo_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
				 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
			 }
	private: System::Void dataGridView_QuestPR_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
				 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
			 }
	private: System::Void dataGridView_QuestPRFaction_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
				 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
			 }
	private: System::Void dataGridView_QuestOObjective_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
				 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
			 }
	private: System::Void dataGridView_QuestUObjective_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
				 Windows::Forms::MessageBox::Show("잘못된 cell값을 입력 하셨습니다.");
			 }

	private: System::Void QuestForm_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 switch (e->KeyCode)
				 {
				 case Keys::F:
					 {
						 if (true == e->Control)
						 {
							 this->textBox_FindQuest->Focus();
						 }
					 }
					 break;
				 }
			 }

	private: System::Void QuestForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 if (false == this->m_pQc->IsEmptyModifiedQuestID())
				 {
					 Windows::Forms::DialogResult dr = Windows::Forms::MessageBox::Show("변경 내용을 저장 하시겠습니까?", "QuestTool", MessageBoxButtons::OKCancel);
					 if (Windows::Forms::DialogResult::OK == dr)
					 {
						 this->m_pQc->SaveQuests();
					 }
				 }
			 }

	private: System::Void dateTimePicker_DeadLine_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 DataGridViewCell^ dgvcSelected;
				 this->m_pQc->GetSelectedCell(dgvcSelected);
				 dgvcSelected->Value = this->dateTimePicker_DeadLine->Value;

				 int nQuestID = this->m_pQc->GetSelectedQuestID();
				 String^ strColumnName = dgvcSelected->DataGridView->Columns[dgvcSelected->ColumnIndex]->Name->ToString();
				 array<String^>^ arrStrDatetime = this->dateTimePicker_DeadLine->Value.GetDateTimeFormats();

				 this->m_pQc->UpdateQuestBaseInfo(nQuestID, strColumnName, arrStrDatetime[60]);
			 }
};	
}

