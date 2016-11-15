#pragma once
#include "TNPCInfo.h"
#include "TDialogController.h"
#include "Tdef.h"
#include "InsertDialog.h"

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Xml.dll>

enum TREEVIEW_DIALOGLIST_NODETYPE
{
	TDN_NPC = 0,
	TDN_DIALOG,
	TDN_ON_SCREEN,
	TDN_OFF_SCREEN,
};

namespace QuestAndDialog_NET {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// DialogForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class DialogForm : public System::Windows::Forms::Form
	{
	public:
		DialogForm(TDialogController* pDc)
		{
			InitializeComponent();

			this->m_pDc = pDc;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~DialogForm()
		{
			this->m_pDc->SetDialogForm(nullptr);

			if (components)
			{
				delete components;
			}
		}

	private: TDialogController*  m_pDc;


	protected: 

	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip_treeView_DialogList;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDL_Add;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDL_Delete;

	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip_treeView_DialogInfo;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDI_Add;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDI_Delete;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDI_Cut;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDI_Copy;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDI_Paste;

	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Check;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_CheckFail;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Noexist;

	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Doing;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Final;

	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Completed;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Default;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Say;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Select;
	private: System::Windows::Forms::MenuStrip^  menuStrip_Dialog;


	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_File;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_SaveDialog;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDL_AddOnScreen;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_TVDL_AddOffScreen;
	private: System::Windows::Forms::ToolStripMenuItem^  ToolStripMenuItem_ExpandAll;
	private: System::Windows::Forms::ToolStripMenuItem^  ToolStripMenuItem_CollapseAll;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_View;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_ReloadNPC;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Tool;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_RunQuestTool;
	private: System::Windows::Forms::ToolStrip^  toolStrip1;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	private: System::Windows::Forms::GroupBox^  groupBox_FindNPC;
	private: System::Windows::Forms::Button^  button_FindNPC;
	private: System::Windows::Forms::ComboBox^  comboBox_FindNPC;
	private: System::Windows::Forms::TextBox^  textBox_FindNPC;
	private: System::Windows::Forms::TreeView^  treeView_DialogList;
	private: System::Windows::Forms::SplitContainer^  splitContainer2;
	private: System::Windows::Forms::TreeView^  treeView_DialogInfo;
	private: System::Windows::Forms::Panel^  panel_Default;
	private: System::Windows::Forms::Panel^  panel_Check;
	private: System::Windows::Forms::Button^  button_CheckCancel;
	private: System::Windows::Forms::TextBox^  textBox_CheckParam2;
	private: System::Windows::Forms::Label^  label_CheckParam2;
	private: System::Windows::Forms::Button^  button_CheckOk;
	private: System::Windows::Forms::Label^  label_CheckParam1;
	private: System::Windows::Forms::TextBox^  textBox_CheckParam1;
	private: System::Windows::Forms::Label^  label_CheckType;
	private: System::Windows::Forms::ComboBox^  comboBox_CheckType;
	private: System::Windows::Forms::Panel^  panel_Say;
	private: System::Windows::Forms::Button^  button_SayCancel;
	private: System::Windows::Forms::Button^  button_SayOk;
	private: System::Windows::Forms::TextBox^  textBox_SayText;
	private: System::Windows::Forms::Label^  label_SayText;
	private: System::Windows::Forms::Panel^  panel_Select;
	private: System::Windows::Forms::Button^  button_SelectCancel;
	private: System::Windows::Forms::Button^  button_SelectOk;
	private: System::Windows::Forms::TextBox^  textBox_SelectExit;
	private: System::Windows::Forms::Label^  label_SelectExit;
	private: System::Windows::Forms::TextBox^  textBox_SelectText;
	private: System::Windows::Forms::Label^  label_SelectText;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel_NPC;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel_Dialog;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem_Exit;


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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(DialogForm::typeid));
			this->contextMenuStrip_treeView_DialogInfo = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->toolStripMenuItem_TVDI_Add = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Check = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_CheckFail = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Noexist = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Doing = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Final = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Completed = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Default = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Say = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Select = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_TVDI_Delete = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_TVDI_Cut = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_TVDI_Copy = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_TVDI_Paste = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ToolStripMenuItem_ExpandAll = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ToolStripMenuItem_CollapseAll = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->contextMenuStrip_treeView_DialogList = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->toolStripMenuItem_TVDL_Add = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_TVDL_Delete = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_TVDL_AddOnScreen = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_TVDL_AddOffScreen = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuStrip_Dialog = (gcnew System::Windows::Forms::MenuStrip());
			this->toolStripMenuItem_File = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_SaveDialog = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Exit = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_View = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_ReloadNPC = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_Tool = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem_RunQuestTool = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel_NPC = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStripStatusLabel_Dialog = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->groupBox_FindNPC = (gcnew System::Windows::Forms::GroupBox());
			this->button_FindNPC = (gcnew System::Windows::Forms::Button());
			this->comboBox_FindNPC = (gcnew System::Windows::Forms::ComboBox());
			this->textBox_FindNPC = (gcnew System::Windows::Forms::TextBox());
			this->treeView_DialogList = (gcnew System::Windows::Forms::TreeView());
			this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
			this->treeView_DialogInfo = (gcnew System::Windows::Forms::TreeView());
			this->panel_Default = (gcnew System::Windows::Forms::Panel());
			this->panel_Check = (gcnew System::Windows::Forms::Panel());
			this->button_CheckCancel = (gcnew System::Windows::Forms::Button());
			this->textBox_CheckParam2 = (gcnew System::Windows::Forms::TextBox());
			this->label_CheckParam2 = (gcnew System::Windows::Forms::Label());
			this->button_CheckOk = (gcnew System::Windows::Forms::Button());
			this->label_CheckParam1 = (gcnew System::Windows::Forms::Label());
			this->textBox_CheckParam1 = (gcnew System::Windows::Forms::TextBox());
			this->label_CheckType = (gcnew System::Windows::Forms::Label());
			this->comboBox_CheckType = (gcnew System::Windows::Forms::ComboBox());
			this->panel_Say = (gcnew System::Windows::Forms::Panel());
			this->button_SayCancel = (gcnew System::Windows::Forms::Button());
			this->button_SayOk = (gcnew System::Windows::Forms::Button());
			this->textBox_SayText = (gcnew System::Windows::Forms::TextBox());
			this->label_SayText = (gcnew System::Windows::Forms::Label());
			this->panel_Select = (gcnew System::Windows::Forms::Panel());
			this->button_SelectCancel = (gcnew System::Windows::Forms::Button());
			this->button_SelectOk = (gcnew System::Windows::Forms::Button());
			this->textBox_SelectExit = (gcnew System::Windows::Forms::TextBox());
			this->label_SelectExit = (gcnew System::Windows::Forms::Label());
			this->textBox_SelectText = (gcnew System::Windows::Forms::TextBox());
			this->label_SelectText = (gcnew System::Windows::Forms::Label());
			this->contextMenuStrip_treeView_DialogInfo->SuspendLayout();
			this->contextMenuStrip_treeView_DialogList->SuspendLayout();
			this->menuStrip_Dialog->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->groupBox_FindNPC->SuspendLayout();
			this->splitContainer2->Panel1->SuspendLayout();
			this->splitContainer2->Panel2->SuspendLayout();
			this->splitContainer2->SuspendLayout();
			this->panel_Check->SuspendLayout();
			this->panel_Say->SuspendLayout();
			this->panel_Select->SuspendLayout();
			this->SuspendLayout();
			// 
			// contextMenuStrip_treeView_DialogInfo
			// 
			this->contextMenuStrip_treeView_DialogInfo->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->toolStripMenuItem_TVDI_Add, 
			this->toolStripMenuItem_TVDI_Delete, this->toolStripMenuItem_TVDI_Cut, this->toolStripMenuItem_TVDI_Copy, this->toolStripMenuItem_TVDI_Paste, 
			this->ToolStripMenuItem_ExpandAll, this->ToolStripMenuItem_CollapseAll});
			this->contextMenuStrip_treeView_DialogInfo->Name = L"contextMenuStrip_treeView_DialogInfo";
			this->contextMenuStrip_treeView_DialogInfo->Size = System::Drawing::Size(181, 158);
			// 
			// toolStripMenuItem_TVDI_Add
			// 
			this->toolStripMenuItem_TVDI_Add->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(9) {this->toolStripMenuItem_Check, 
			this->toolStripMenuItem_CheckFail, this->toolStripMenuItem_Noexist, this->toolStripMenuItem_Doing, this->toolStripMenuItem_Final, 
			this->toolStripMenuItem_Completed, this->toolStripMenuItem_Default, this->toolStripMenuItem_Say, this->toolStripMenuItem_Select});
			this->toolStripMenuItem_TVDI_Add->Name = L"toolStripMenuItem_TVDI_Add";
			this->toolStripMenuItem_TVDI_Add->ShortcutKeys = System::Windows::Forms::Keys::Insert;
			this->toolStripMenuItem_TVDI_Add->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_TVDI_Add->Text = L"추가(&A)";
			this->toolStripMenuItem_TVDI_Add->DropDownOpened += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_TVDI_Add_DropDownOpened);
			// 
			// toolStripMenuItem_Check
			// 
			this->toolStripMenuItem_Check->Name = L"toolStripMenuItem_Check";
			this->toolStripMenuItem_Check->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D0));
			this->toolStripMenuItem_Check->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_Check->Text = L"CHECK(&0)";
			this->toolStripMenuItem_Check->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Check_Click);
			// 
			// toolStripMenuItem_CheckFail
			// 
			this->toolStripMenuItem_CheckFail->Name = L"toolStripMenuItem_CheckFail";
			this->toolStripMenuItem_CheckFail->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D1));
			this->toolStripMenuItem_CheckFail->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_CheckFail->Text = L"CHECKFAIL(&1)";
			this->toolStripMenuItem_CheckFail->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_CheckFail_Click);
			// 
			// toolStripMenuItem_Noexist
			// 
			this->toolStripMenuItem_Noexist->Name = L"toolStripMenuItem_Noexist";
			this->toolStripMenuItem_Noexist->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D2));
			this->toolStripMenuItem_Noexist->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_Noexist->Text = L"NOEXIST(&2)";
			this->toolStripMenuItem_Noexist->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Noexist_Click);
			// 
			// toolStripMenuItem_Doing
			// 
			this->toolStripMenuItem_Doing->Name = L"toolStripMenuItem_Doing";
			this->toolStripMenuItem_Doing->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D3));
			this->toolStripMenuItem_Doing->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_Doing->Text = L"DOING(&3)";
			this->toolStripMenuItem_Doing->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Doing_Click);
			// 
			// toolStripMenuItem_Final
			// 
			this->toolStripMenuItem_Final->Name = L"toolStripMenuItem_Final";
			this->toolStripMenuItem_Final->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D4));
			this->toolStripMenuItem_Final->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_Final->Text = L"FINAL(&4)";
			this->toolStripMenuItem_Final->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Final_Click);
			// 
			// toolStripMenuItem_Completed
			// 
			this->toolStripMenuItem_Completed->Name = L"toolStripMenuItem_Completed";
			this->toolStripMenuItem_Completed->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D5));
			this->toolStripMenuItem_Completed->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_Completed->Text = L"COMPLETED(&5)";
			this->toolStripMenuItem_Completed->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Completed_Click);
			// 
			// toolStripMenuItem_Default
			// 
			this->toolStripMenuItem_Default->Name = L"toolStripMenuItem_Default";
			this->toolStripMenuItem_Default->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D6));
			this->toolStripMenuItem_Default->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_Default->Text = L"DEFAULT(&6)";
			this->toolStripMenuItem_Default->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Default_Click);
			// 
			// toolStripMenuItem_Say
			// 
			this->toolStripMenuItem_Say->Name = L"toolStripMenuItem_Say";
			this->toolStripMenuItem_Say->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D7));
			this->toolStripMenuItem_Say->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_Say->Text = L"SAY(&7)";
			this->toolStripMenuItem_Say->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Say_Click);
			// 
			// toolStripMenuItem_Select
			// 
			this->toolStripMenuItem_Select->Name = L"toolStripMenuItem_Select";
			this->toolStripMenuItem_Select->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D8));
			this->toolStripMenuItem_Select->Size = System::Drawing::Size(204, 22);
			this->toolStripMenuItem_Select->Text = L"SELECT(&8)";
			this->toolStripMenuItem_Select->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Select_Click);
			// 
			// toolStripMenuItem_TVDI_Delete
			// 
			this->toolStripMenuItem_TVDI_Delete->Name = L"toolStripMenuItem_TVDI_Delete";
			this->toolStripMenuItem_TVDI_Delete->ShortcutKeys = System::Windows::Forms::Keys::Delete;
			this->toolStripMenuItem_TVDI_Delete->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_TVDI_Delete->Text = L"삭제(&D)";
			this->toolStripMenuItem_TVDI_Delete->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_TVDI_Delete_Click);
			// 
			// toolStripMenuItem_TVDI_Cut
			// 
			this->toolStripMenuItem_TVDI_Cut->Name = L"toolStripMenuItem_TVDI_Cut";
			this->toolStripMenuItem_TVDI_Cut->Overflow = System::Windows::Forms::ToolStripItemOverflow::Always;
			this->toolStripMenuItem_TVDI_Cut->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::X));
			this->toolStripMenuItem_TVDI_Cut->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_TVDI_Cut->Text = L"잘라내기(&T)";
			this->toolStripMenuItem_TVDI_Cut->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_TVDI_Cut_Click);
			// 
			// toolStripMenuItem_TVDI_Copy
			// 
			this->toolStripMenuItem_TVDI_Copy->Name = L"toolStripMenuItem_TVDI_Copy";
			this->toolStripMenuItem_TVDI_Copy->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::C));
			this->toolStripMenuItem_TVDI_Copy->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_TVDI_Copy->Text = L"복사(&C)";
			this->toolStripMenuItem_TVDI_Copy->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_TVDI_Copy_Click);
			// 
			// toolStripMenuItem_TVDI_Paste
			// 
			this->toolStripMenuItem_TVDI_Paste->Name = L"toolStripMenuItem_TVDI_Paste";
			this->toolStripMenuItem_TVDI_Paste->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::V));
			this->toolStripMenuItem_TVDI_Paste->Size = System::Drawing::Size(180, 22);
			this->toolStripMenuItem_TVDI_Paste->Text = L"붙여넣기(&P)";
			this->toolStripMenuItem_TVDI_Paste->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_TVDI_Paste_Click);
			// 
			// ToolStripMenuItem_ExpandAll
			// 
			this->ToolStripMenuItem_ExpandAll->Name = L"ToolStripMenuItem_ExpandAll";
			this->ToolStripMenuItem_ExpandAll->Size = System::Drawing::Size(180, 22);
			this->ToolStripMenuItem_ExpandAll->Text = L"모두열기(&O)";
			this->ToolStripMenuItem_ExpandAll->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_ExpandAll_Click);
			// 
			// ToolStripMenuItem_CollapseAll
			// 
			this->ToolStripMenuItem_CollapseAll->Name = L"ToolStripMenuItem_CollapseAll";
			this->ToolStripMenuItem_CollapseAll->Size = System::Drawing::Size(180, 22);
			this->ToolStripMenuItem_CollapseAll->Text = L"모두닫기(&C)";
			this->ToolStripMenuItem_CollapseAll->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_CollapseAll_Click);
			// 
			// contextMenuStrip_treeView_DialogList
			// 
			this->contextMenuStrip_treeView_DialogList->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->toolStripMenuItem_TVDL_Add, 
				this->toolStripMenuItem_TVDL_Delete, this->toolStripMenuItem_TVDL_AddOnScreen, this->toolStripMenuItem_TVDL_AddOffScreen});
			this->contextMenuStrip_treeView_DialogList->Name = L"contextMenuStrip1";
			this->contextMenuStrip_treeView_DialogList->Size = System::Drawing::Size(180, 92);
			// 
			// toolStripMenuItem_TVDL_Add
			// 
			this->toolStripMenuItem_TVDL_Add->Name = L"toolStripMenuItem_TVDL_Add";
			this->toolStripMenuItem_TVDL_Add->ShortcutKeys = System::Windows::Forms::Keys::Insert;
			this->toolStripMenuItem_TVDL_Add->Size = System::Drawing::Size(179, 22);
			this->toolStripMenuItem_TVDL_Add->Text = L"추가(&A)";
			this->toolStripMenuItem_TVDL_Add->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_TVDL_Insert_Click);
			// 
			// toolStripMenuItem_TVDL_Delete
			// 
			this->toolStripMenuItem_TVDL_Delete->Name = L"toolStripMenuItem_TVDL_Delete";
			this->toolStripMenuItem_TVDL_Delete->ShortcutKeys = System::Windows::Forms::Keys::Delete;
			this->toolStripMenuItem_TVDL_Delete->Size = System::Drawing::Size(179, 22);
			this->toolStripMenuItem_TVDL_Delete->Text = L"삭제(&D)";
			this->toolStripMenuItem_TVDL_Delete->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_TVDL_Delete_Click);
			// 
			// toolStripMenuItem_TVDL_AddOnScreen
			// 
			this->toolStripMenuItem_TVDL_AddOnScreen->Name = L"toolStripMenuItem_TVDL_AddOnScreen";
			this->toolStripMenuItem_TVDL_AddOnScreen->Size = System::Drawing::Size(179, 22);
			this->toolStripMenuItem_TVDL_AddOnScreen->Text = L"추가 OnScreen(&N)";
			this->toolStripMenuItem_TVDL_AddOnScreen->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_TVDL_ADD_OnScreen_Click);
			// 
			// toolStripMenuItem_TVDL_AddOffScreen
			// 
			this->toolStripMenuItem_TVDL_AddOffScreen->Name = L"toolStripMenuItem_TVDL_AddOffScreen";
			this->toolStripMenuItem_TVDL_AddOffScreen->Size = System::Drawing::Size(179, 22);
			this->toolStripMenuItem_TVDL_AddOffScreen->Text = L"추가 OffScreen(&F)";
			this->toolStripMenuItem_TVDL_AddOffScreen->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_TVDL_ADD_OffScreen_Click);
			// 
			// menuStrip_Dialog
			// 
			this->menuStrip_Dialog->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->toolStripMenuItem_File, 
				this->toolStripMenuItem_View, this->toolStripMenuItem_Tool});
			this->menuStrip_Dialog->Location = System::Drawing::Point(0, 0);
			this->menuStrip_Dialog->Name = L"menuStrip_Dialog";
			this->menuStrip_Dialog->Size = System::Drawing::Size(1155, 24);
			this->menuStrip_Dialog->TabIndex = 2;
			this->menuStrip_Dialog->Text = L"menuStrip_Dialog";
			// 
			// toolStripMenuItem_File
			// 
			this->toolStripMenuItem_File->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->toolStripMenuItem_SaveDialog, 
				this->toolStripMenuItem_Exit});
			this->toolStripMenuItem_File->Name = L"toolStripMenuItem_File";
			this->toolStripMenuItem_File->Size = System::Drawing::Size(58, 20);
			this->toolStripMenuItem_File->Text = L"파일(&F)";
			// 
			// toolStripMenuItem_SaveDialog
			// 
			this->toolStripMenuItem_SaveDialog->Name = L"toolStripMenuItem_SaveDialog";
			this->toolStripMenuItem_SaveDialog->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->toolStripMenuItem_SaveDialog->Size = System::Drawing::Size(168, 22);
			this->toolStripMenuItem_SaveDialog->Text = L"저장(&S)";
			this->toolStripMenuItem_SaveDialog->Click += gcnew System::EventHandler(this, &DialogForm::ToolStripMenuItem_Click_SaveDialog);
			// 
			// toolStripMenuItem_Exit
			// 
			this->toolStripMenuItem_Exit->Name = L"toolStripMenuItem_Exit";
			this->toolStripMenuItem_Exit->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::F4));
			this->toolStripMenuItem_Exit->Size = System::Drawing::Size(168, 22);
			this->toolStripMenuItem_Exit->Text = L"끝내기(&X)";
			this->toolStripMenuItem_Exit->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_Exit_Click);
			// 
			// toolStripMenuItem_View
			// 
			this->toolStripMenuItem_View->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripMenuItem_ReloadNPC});
			this->toolStripMenuItem_View->Name = L"toolStripMenuItem_View";
			this->toolStripMenuItem_View->Size = System::Drawing::Size(59, 20);
			this->toolStripMenuItem_View->Text = L"보기(&V)";
			// 
			// toolStripMenuItem_ReloadNPC
			// 
			this->toolStripMenuItem_ReloadNPC->Name = L"toolStripMenuItem_ReloadNPC";
			this->toolStripMenuItem_ReloadNPC->ShortcutKeys = System::Windows::Forms::Keys::F5;
			this->toolStripMenuItem_ReloadNPC->Size = System::Drawing::Size(190, 22);
			this->toolStripMenuItem_ReloadNPC->Text = L"NPC 새로고침(&R)";
			this->toolStripMenuItem_ReloadNPC->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_ReloadNPC_Click);
			// 
			// toolStripMenuItem_Tool
			// 
			this->toolStripMenuItem_Tool->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripMenuItem_RunQuestTool});
			this->toolStripMenuItem_Tool->Name = L"toolStripMenuItem_Tool";
			this->toolStripMenuItem_Tool->Size = System::Drawing::Size(59, 20);
			this->toolStripMenuItem_Tool->Text = L"도구(&T)";
			// 
			// toolStripMenuItem_RunQuestTool
			// 
			this->toolStripMenuItem_RunQuestTool->Name = L"toolStripMenuItem_RunQuestTool";
			this->toolStripMenuItem_RunQuestTool->ShortcutKeys = System::Windows::Forms::Keys::F10;
			this->toolStripMenuItem_RunQuestTool->Size = System::Drawing::Size(171, 22);
			this->toolStripMenuItem_RunQuestTool->Text = L"퀘스트 툴(&Q)";
			this->toolStripMenuItem_RunQuestTool->Click += gcnew System::EventHandler(this, &DialogForm::toolStripMenuItem_RunQuestTool_Click);
			// 
			// toolStrip1
			// 
			this->toolStrip1->Location = System::Drawing::Point(0, 24);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(1155, 25);
			this->toolStrip1->TabIndex = 3;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->toolStripStatusLabel_NPC, 
				this->toolStripStatusLabel_Dialog});
			this->statusStrip1->Location = System::Drawing::Point(0, 732);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(1155, 22);
			this->statusStrip1->TabIndex = 4;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel_NPC
			// 
			this->toolStripStatusLabel_NPC->Name = L"toolStripStatusLabel_NPC";
			this->toolStripStatusLabel_NPC->Size = System::Drawing::Size(148, 17);
			this->toolStripStatusLabel_NPC->Text = L"toolStripStatusLabel_NPC";
			// 
			// toolStripStatusLabel_Dialog
			// 
			this->toolStripStatusLabel_Dialog->Name = L"toolStripStatusLabel_Dialog";
			this->toolStripStatusLabel_Dialog->Size = System::Drawing::Size(157, 17);
			this->toolStripStatusLabel_Dialog->Text = L"toolStripStatusLabel_Dialog";
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
			this->splitContainer1->Panel1->Controls->Add(this->groupBox_FindNPC);
			this->splitContainer1->Panel1->Controls->Add(this->treeView_DialogList);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->splitContainer2);
			this->splitContainer1->Size = System::Drawing::Size(1155, 683);
			this->splitContainer1->SplitterDistance = 275;
			this->splitContainer1->TabIndex = 5;
			// 
			// groupBox_FindNPC
			// 
			this->groupBox_FindNPC->Controls->Add(this->button_FindNPC);
			this->groupBox_FindNPC->Controls->Add(this->comboBox_FindNPC);
			this->groupBox_FindNPC->Controls->Add(this->textBox_FindNPC);
			this->groupBox_FindNPC->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->groupBox_FindNPC->Location = System::Drawing::Point(0, 631);
			this->groupBox_FindNPC->Name = L"groupBox_FindNPC";
			this->groupBox_FindNPC->Size = System::Drawing::Size(271, 48);
			this->groupBox_FindNPC->TabIndex = 6;
			this->groupBox_FindNPC->TabStop = false;
			this->groupBox_FindNPC->Text = L"NPC검색";
			// 
			// button_FindNPC
			// 
			this->button_FindNPC->Location = System::Drawing::Point(212, 18);
			this->button_FindNPC->Name = L"button_FindNPC";
			this->button_FindNPC->Size = System::Drawing::Size(49, 23);
			this->button_FindNPC->TabIndex = 6;
			this->button_FindNPC->Text = L"검색";
			this->button_FindNPC->UseVisualStyleBackColor = true;
			this->button_FindNPC->Click += gcnew System::EventHandler(this, &DialogForm::button_FindNPC_Click);
			// 
			// comboBox_FindNPC
			// 
			this->comboBox_FindNPC->FormattingEnabled = true;
			this->comboBox_FindNPC->Location = System::Drawing::Point(6, 19);
			this->comboBox_FindNPC->Name = L"comboBox_FindNPC";
			this->comboBox_FindNPC->Size = System::Drawing::Size(52, 20);
			this->comboBox_FindNPC->TabIndex = 5;
			// 
			// textBox_FindNPC
			// 
			this->textBox_FindNPC->Location = System::Drawing::Point(64, 18);
			this->textBox_FindNPC->Name = L"textBox_FindNPC";
			this->textBox_FindNPC->Size = System::Drawing::Size(142, 21);
			this->textBox_FindNPC->TabIndex = 4;
			this->textBox_FindNPC->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &DialogForm::textBox_FindNPC_KeyDown);
			// 
			// treeView_DialogList
			// 
			this->treeView_DialogList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeView_DialogList->Location = System::Drawing::Point(0, 0);
			this->treeView_DialogList->Name = L"treeView_DialogList";
			this->treeView_DialogList->Size = System::Drawing::Size(271, 625);
			this->treeView_DialogList->TabIndex = 1;
			this->treeView_DialogList->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &DialogForm::treeView_DialogList_AfterSelect);
			this->treeView_DialogList->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &DialogForm::treeView_DialogList_NodeMouseClick);
			this->treeView_DialogList->BeforeSelect += gcnew System::Windows::Forms::TreeViewCancelEventHandler(this, &DialogForm::treeView_DialogList_BeforeSelect);
			this->treeView_DialogList->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &DialogForm::treeView_DialogList_KeyDown);
			// 
			// splitContainer2
			// 
			this->splitContainer2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->splitContainer2->FixedPanel = System::Windows::Forms::FixedPanel::Panel2;
			this->splitContainer2->Location = System::Drawing::Point(0, 0);
			this->splitContainer2->Name = L"splitContainer2";
			this->splitContainer2->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer2.Panel1
			// 
			this->splitContainer2->Panel1->Controls->Add(this->treeView_DialogInfo);
			this->splitContainer2->Panel1->RightToLeft = System::Windows::Forms::RightToLeft::No;
			// 
			// splitContainer2.Panel2
			// 
			this->splitContainer2->Panel2->Controls->Add(this->panel_Default);
			this->splitContainer2->Panel2->Controls->Add(this->panel_Check);
			this->splitContainer2->Panel2->Controls->Add(this->panel_Say);
			this->splitContainer2->Panel2->Controls->Add(this->panel_Select);
			this->splitContainer2->Panel2->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->splitContainer2->Size = System::Drawing::Size(876, 679);
			this->splitContainer2->SplitterDistance = 418;
			this->splitContainer2->TabIndex = 6;
			// 
			// treeView_DialogInfo
			// 
			this->treeView_DialogInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeView_DialogInfo->Location = System::Drawing::Point(0, 0);
			this->treeView_DialogInfo->Name = L"treeView_DialogInfo";
			this->treeView_DialogInfo->Size = System::Drawing::Size(868, 411);
			this->treeView_DialogInfo->TabIndex = 0;
			this->treeView_DialogInfo->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &DialogForm::treeView_DialogInfo_AfterSelect);
			this->treeView_DialogInfo->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &DialogForm::treeView_DialogInfo_NodeMouseClick);
			this->treeView_DialogInfo->BeforeSelect += gcnew System::Windows::Forms::TreeViewCancelEventHandler(this, &DialogForm::treeView_DialogInfo_BeforeSelect);
			this->treeView_DialogInfo->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &DialogForm::treeView_DialogInfo_KeyDown);
			// 
			// panel_Default
			// 
			this->panel_Default->Location = System::Drawing::Point(1, 3);
			this->panel_Default->Name = L"panel_Default";
			this->panel_Default->Size = System::Drawing::Size(870, 250);
			this->panel_Default->TabIndex = 0;
			// 
			// panel_Check
			// 
			this->panel_Check->Controls->Add(this->button_CheckCancel);
			this->panel_Check->Controls->Add(this->textBox_CheckParam2);
			this->panel_Check->Controls->Add(this->label_CheckParam2);
			this->panel_Check->Controls->Add(this->button_CheckOk);
			this->panel_Check->Controls->Add(this->label_CheckParam1);
			this->panel_Check->Controls->Add(this->textBox_CheckParam1);
			this->panel_Check->Controls->Add(this->label_CheckType);
			this->panel_Check->Controls->Add(this->comboBox_CheckType);
			this->panel_Check->Location = System::Drawing::Point(1, 3);
			this->panel_Check->Name = L"panel_Check";
			this->panel_Check->Size = System::Drawing::Size(870, 250);
			this->panel_Check->TabIndex = 1;
			this->panel_Check->Visible = false;
			// 
			// button_CheckCancel
			// 
			this->button_CheckCancel->Location = System::Drawing::Point(141, 110);
			this->button_CheckCancel->Name = L"button_CheckCancel";
			this->button_CheckCancel->Size = System::Drawing::Size(75, 23);
			this->button_CheckCancel->TabIndex = 7;
			this->button_CheckCancel->Text = L"취소";
			this->button_CheckCancel->UseVisualStyleBackColor = true;
			this->button_CheckCancel->Click += gcnew System::EventHandler(this, &DialogForm::button_CheckCancel_Click);
			// 
			// textBox_CheckParam2
			// 
			this->textBox_CheckParam2->Location = System::Drawing::Point(60, 79);
			this->textBox_CheckParam2->Name = L"textBox_CheckParam2";
			this->textBox_CheckParam2->Size = System::Drawing::Size(100, 21);
			this->textBox_CheckParam2->TabIndex = 5;
			// 
			// label_CheckParam2
			// 
			this->label_CheckParam2->AutoSize = true;
			this->label_CheckParam2->Location = System::Drawing::Point(6, 79);
			this->label_CheckParam2->Name = L"label_CheckParam2";
			this->label_CheckParam2->Size = System::Drawing::Size(48, 12);
			this->label_CheckParam2->TabIndex = 4;
			this->label_CheckParam2->Text = L"Param2";
			// 
			// button_CheckOk
			// 
			this->button_CheckOk->Location = System::Drawing::Point(60, 110);
			this->button_CheckOk->Name = L"button_CheckOk";
			this->button_CheckOk->Size = System::Drawing::Size(75, 23);
			this->button_CheckOk->TabIndex = 6;
			this->button_CheckOk->Text = L"확인";
			this->button_CheckOk->UseVisualStyleBackColor = true;
			this->button_CheckOk->Click += gcnew System::EventHandler(this, &DialogForm::button_CheckOk_Click);
			// 
			// label_CheckParam1
			// 
			this->label_CheckParam1->AutoSize = true;
			this->label_CheckParam1->Location = System::Drawing::Point(6, 42);
			this->label_CheckParam1->Name = L"label_CheckParam1";
			this->label_CheckParam1->Size = System::Drawing::Size(48, 12);
			this->label_CheckParam1->TabIndex = 3;
			this->label_CheckParam1->Text = L"Param1";
			// 
			// textBox_CheckParam1
			// 
			this->textBox_CheckParam1->Location = System::Drawing::Point(60, 42);
			this->textBox_CheckParam1->Name = L"textBox_CheckParam1";
			this->textBox_CheckParam1->Size = System::Drawing::Size(100, 21);
			this->textBox_CheckParam1->TabIndex = 2;
			// 
			// label_CheckType
			// 
			this->label_CheckType->AutoSize = true;
			this->label_CheckType->Location = System::Drawing::Point(6, 7);
			this->label_CheckType->Name = L"label_CheckType";
			this->label_CheckType->Size = System::Drawing::Size(34, 12);
			this->label_CheckType->TabIndex = 1;
			this->label_CheckType->Text = L"Type";
			// 
			// comboBox_CheckType
			// 
			this->comboBox_CheckType->FormattingEnabled = true;
			this->comboBox_CheckType->Location = System::Drawing::Point(60, 7);
			this->comboBox_CheckType->Name = L"comboBox_CheckType";
			this->comboBox_CheckType->Size = System::Drawing::Size(100, 20);
			this->comboBox_CheckType->TabIndex = 0;
			// 
			// panel_Say
			// 
			this->panel_Say->Controls->Add(this->button_SayCancel);
			this->panel_Say->Controls->Add(this->button_SayOk);
			this->panel_Say->Controls->Add(this->textBox_SayText);
			this->panel_Say->Controls->Add(this->label_SayText);
			this->panel_Say->Location = System::Drawing::Point(1, 3);
			this->panel_Say->Name = L"panel_Say";
			this->panel_Say->Size = System::Drawing::Size(872, 250);
			this->panel_Say->TabIndex = 0;
			this->panel_Say->Visible = false;
			// 
			// button_SayCancel
			// 
			this->button_SayCancel->Location = System::Drawing::Point(141, 163);
			this->button_SayCancel->Name = L"button_SayCancel";
			this->button_SayCancel->Size = System::Drawing::Size(75, 23);
			this->button_SayCancel->TabIndex = 9;
			this->button_SayCancel->Text = L"취소";
			this->button_SayCancel->UseVisualStyleBackColor = true;
			this->button_SayCancel->Click += gcnew System::EventHandler(this, &DialogForm::button_SayCancel_Click);
			// 
			// button_SayOk
			// 
			this->button_SayOk->Location = System::Drawing::Point(60, 163);
			this->button_SayOk->Name = L"button_SayOk";
			this->button_SayOk->Size = System::Drawing::Size(75, 23);
			this->button_SayOk->TabIndex = 8;
			this->button_SayOk->Text = L"확인";
			this->button_SayOk->UseVisualStyleBackColor = true;
			this->button_SayOk->Click += gcnew System::EventHandler(this, &DialogForm::button_SayOk_Click);
			// 
			// textBox_SayText
			// 
			this->textBox_SayText->Location = System::Drawing::Point(60, 7);
			this->textBox_SayText->Multiline = true;
			this->textBox_SayText->Name = L"textBox_SayText";
			this->textBox_SayText->Size = System::Drawing::Size(803, 150);
			this->textBox_SayText->TabIndex = 1;
			// 
			// label_SayText
			// 
			this->label_SayText->AutoSize = true;
			this->label_SayText->Location = System::Drawing::Point(3, 9);
			this->label_SayText->Name = L"label_SayText";
			this->label_SayText->Size = System::Drawing::Size(30, 12);
			this->label_SayText->TabIndex = 0;
			this->label_SayText->Text = L"Text";
			// 
			// panel_Select
			// 
			this->panel_Select->Controls->Add(this->button_SelectCancel);
			this->panel_Select->Controls->Add(this->button_SelectOk);
			this->panel_Select->Controls->Add(this->textBox_SelectExit);
			this->panel_Select->Controls->Add(this->label_SelectExit);
			this->panel_Select->Controls->Add(this->textBox_SelectText);
			this->panel_Select->Controls->Add(this->label_SelectText);
			this->panel_Select->Location = System::Drawing::Point(1, 3);
			this->panel_Select->Name = L"panel_Select";
			this->panel_Select->Size = System::Drawing::Size(870, 250);
			this->panel_Select->TabIndex = 3;
			this->panel_Select->Visible = false;
			// 
			// button_SelectCancel
			// 
			this->button_SelectCancel->Location = System::Drawing::Point(141, 191);
			this->button_SelectCancel->Name = L"button_SelectCancel";
			this->button_SelectCancel->Size = System::Drawing::Size(75, 23);
			this->button_SelectCancel->TabIndex = 11;
			this->button_SelectCancel->Text = L"취소";
			this->button_SelectCancel->UseVisualStyleBackColor = true;
			this->button_SelectCancel->Click += gcnew System::EventHandler(this, &DialogForm::button_SelectCancel_Click);
			// 
			// button_SelectOk
			// 
			this->button_SelectOk->Location = System::Drawing::Point(60, 191);
			this->button_SelectOk->Name = L"button_SelectOk";
			this->button_SelectOk->Size = System::Drawing::Size(75, 23);
			this->button_SelectOk->TabIndex = 10;
			this->button_SelectOk->Text = L"확인";
			this->button_SelectOk->UseVisualStyleBackColor = true;
			this->button_SelectOk->Click += gcnew System::EventHandler(this, &DialogForm::button_SelectOk_Click);
			// 
			// textBox_SelectExit
			// 
			this->textBox_SelectExit->Location = System::Drawing::Point(60, 164);
			this->textBox_SelectExit->Name = L"textBox_SelectExit";
			this->textBox_SelectExit->Size = System::Drawing::Size(158, 21);
			this->textBox_SelectExit->TabIndex = 3;
			// 
			// label_SelectExit
			// 
			this->label_SelectExit->AutoSize = true;
			this->label_SelectExit->Location = System::Drawing::Point(5, 167);
			this->label_SelectExit->Name = L"label_SelectExit";
			this->label_SelectExit->Size = System::Drawing::Size(26, 12);
			this->label_SelectExit->TabIndex = 2;
			this->label_SelectExit->Text = L"Exit";
			// 
			// textBox_SelectText
			// 
			this->textBox_SelectText->Location = System::Drawing::Point(60, 7);
			this->textBox_SelectText->Multiline = true;
			this->textBox_SelectText->Name = L"textBox_SelectText";
			this->textBox_SelectText->Size = System::Drawing::Size(803, 150);
			this->textBox_SelectText->TabIndex = 1;
			// 
			// label_SelectText
			// 
			this->label_SelectText->AutoSize = true;
			this->label_SelectText->Location = System::Drawing::Point(5, 7);
			this->label_SelectText->Name = L"label_SelectText";
			this->label_SelectText->Size = System::Drawing::Size(30, 12);
			this->label_SelectText->TabIndex = 0;
			this->label_SelectText->Text = L"Text";
			// 
			// DialogForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1155, 754);
			this->Controls->Add(this->splitContainer1);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->menuStrip_Dialog);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->KeyPreview = true;
			this->MainMenuStrip = this->menuStrip_Dialog;
			this->Name = L"DialogForm";
			this->Text = L"DialogForm";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &DialogForm::DialogForm_FormClosing);
			this->Load += gcnew System::EventHandler(this, &DialogForm::DialogForm_Load);
			this->contextMenuStrip_treeView_DialogInfo->ResumeLayout(false);
			this->contextMenuStrip_treeView_DialogList->ResumeLayout(false);
			this->menuStrip_Dialog->ResumeLayout(false);
			this->menuStrip_Dialog->PerformLayout();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->groupBox_FindNPC->ResumeLayout(false);
			this->groupBox_FindNPC->PerformLayout();
			this->splitContainer2->Panel1->ResumeLayout(false);
			this->splitContainer2->Panel2->ResumeLayout(false);
			this->splitContainer2->ResumeLayout(false);
			this->panel_Check->ResumeLayout(false);
			this->panel_Check->PerformLayout();
			this->panel_Say->ResumeLayout(false);
			this->panel_Say->PerformLayout();
			this->panel_Select->ResumeLayout(false);
			this->panel_Select->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


		/////////////////////////////////////////////////////////////////////////////////////
		// 임의로 추가한 함수들 v

	public: bool Begin()
			{
				this->HideAllPanel();
				this->m_pDc->SerializeTreeView_DialogList();
				this->m_pDc->SerializeTreeView_DialogInfo();

				return true;
			}

	public: System::Void End()
			{
				this->treeView_DialogList->Nodes->Clear();
				this->treeView_DialogInfo->Nodes->Clear();
			}

	private: System::Void HideAllPanel() {
				this->panel_Check->Visible = false;
				this->panel_Default->Visible = false;
				this->panel_Say->Visible = false;
				this->panel_Select->Visible = false;
			}

	private: System::Void ChangeForeColorOfTreeView_DialogList() {
				 int nNPCID = this->m_pDc->GetSelectedNPCID();
				 bool bOnScreen = this->m_pDc->GetSelectedOnScreen();
				 int nDialogID = this->m_pDc->GetSelectedDialogID();

				 this->ChangeForeColorOfTreeView_DialogList(nNPCID, bOnScreen, nDialogID, Color::Red);
		    }

	private: System::Void ChangeForeColorOfTreeView_DialogList(int nNPCID, bool bOnScreen, int nDialogID, Color color) {
			 
				 String^ strNPCText = this->m_pDc->MakeTreeView_DilaogListNPCText(nNPCID);
				 TreeNode^ tnNPC = this->m_pDc->SearchTreeNode(this->treeView_DialogList->Nodes, strNPCText);

				 String^ strScreenText = this->MakeScreenTextOfTreeView_DilaogList(bOnScreen);
				 TreeNode^ tnScreen = this->m_pDc->SearchTreeNode(tnNPC->Nodes, strScreenText);

				 String^ strDialogText = this->MakeDialogTextOfTreeView_DilaogList(nDialogID);
				 TreeNode^ tnDialog = this->m_pDc->SearchTreeNode(tnScreen->Nodes, strDialogText);

				 tnDialog->ForeColor = color;
		    }

	private: System::String^ MakeScreenTextOfTreeView_DilaogList(bool bOnScreen)
			{
				String^ strScreenText = (true == bOnScreen) ? gcnew String("OnScreen") : gcnew String("OffScreen");

				return strScreenText;
			}

	private: System::String^ MakeDialogTextOfTreeView_DilaogList(int nDialogID)
			 {
				 String^ strDialogText = String::Format("{0}", nDialogID);

				 return strDialogText;
			 }
	
	private: System::String^ MakeTextOfTreeView_DialogInfo(TDialogNode* pDialogNode)
			 {
				 String^ strDialogInfoText = nullptr;

				 switch(pDialogNode->m_nType)
				 {
				 case DNT_CHECK:
					 {
						 TDialogCheckNode* pDialogChekNode = static_cast<TDialogCheckNode*>(pDialogNode);
						 switch (pDialogChekNode->m_nCheckType)
						 {
						 case DCT_QUEST	:
							 {
								 if (0 > pDialogChekNode->m_nParam2)
								 {
									 strDialogInfoText = String::Format("CHECK=Quest QuestID={0}", pDialogChekNode->m_nParam1);
								 }
								 else
								 {
									 strDialogInfoText = String::Format("CHECK=Quest QuestID={0} ObjectiveID={1}", pDialogChekNode->m_nParam1, pDialogChekNode->m_nParam2);
								 }
							 }
							 break;
						 case DCT_FACTION:
							 {
								 strDialogInfoText = "";
							 }
							 break;
						 };
					 }
					 break;
				 case DNT_CASE_CHECKFAIL:
					 strDialogInfoText = "CHECKFAIL";
					 break;
				 case DNT_CASE_NOEXIST:
					 strDialogInfoText = "NOEXIST";
					 break;
				 case DNT_CASE_DOING:	
					 strDialogInfoText = "DOING";
					 break;
				 case DNT_CASE_FINAL:	
					 strDialogInfoText = "FINAL";
					 break;
				 case DNT_CASE_COMPLETED:
					 strDialogInfoText = "COMPLETED";
					 break;
				 case DNT_CASE_DEFAULT:	
					 strDialogInfoText = "DEFAULT";
					 break;
				 case DNT_SAY:
					 {
						 TDialogSayNode* pDialogSayNode = static_cast<TDialogSayNode*>(pDialogNode);
						 strDialogInfoText = String::Format("NPC={0}",this->m_pDc->GetString(gcnew String(pDialogSayNode->m_strText.c_str())));
					 }
					 break;
				 case DNT_SELECT:
					 {
						 TDialogSelectNode* pDialogSelectNode = static_cast<TDialogSelectNode*>(pDialogNode);
						 strDialogInfoText = String::Format("PC={0} EXIT={1}", this->m_pDc->GetString(gcnew String(pDialogSelectNode->m_strText.c_str())), pDialogSelectNode->m_nExit);
					 }
					 break;

				 case DNT_INFO:
					 {
						 TDialogInfo* pDialogInfo = static_cast<TDialogInfo*>(pDialogNode);
						 strDialogInfoText = String::Format("DialogID={0}", pDialogInfo->m_nID);
					 }
					 break;
				 };

				 return  strDialogInfoText;
			 }

	private: int GetNPCIDOfTreeView_DilaogList(String^ strText)
			 {
				 array<String^>^ strsDelim = {"(", ")"};
				 array<String^>^ strsResult = strText->Split(strsDelim, StringSplitOptions::None);
				 int nNPCID = MStringToInt(strsResult[1]);

				 return nNPCID;
			 }

	public: System::String^ MakeNPCTextOfTreeView_DialogList(int nNPCID, String^ strNPCName)
			{
				String^ strNPCText = String::Format("{0} ({1})", strNPCName, nNPCID);

				return strNPCText;
			}

	public: System::Void SerializeTreeView_DialogList(TNPCInfoMgr* pNim) {
				this->treeView_DialogList->Nodes->Clear();
				this->treeView_DialogList->BeginUpdate();
				
				for(TNPCInfoMgr::iterator itor = pNim->begin(); pNim->end() != itor; ++itor)
				{
					TNPCInfo* pNPCInfo = (TNPCInfo*)(*itor).second;
					if (nullptr == pNPCInfo) continue;
					if (true == this->m_pDc->IsHidedNPC(pNPCInfo->nID)) continue;
					
					this->InsertTreeNodeIntoTreeView_DialogList(pNPCInfo);
				}
			
				this->treeView_DialogList->EndUpdate();
			}

	public: System::Void SerializeTreeView_DialogInfo(TNPCInfoMgr* pNim) 
			{
				int nSelectedNPCID = this->m_pDc->GetSelectedNPCID();
				int nSelectedDialogID = this->m_pDc->GetSelectedDialogID();
				bool bSelectedOnScreen = this->m_pDc->GetSelectedOnScreen();

				this->treeView_DialogInfo->Nodes->Clear();

				DIALOGINFOMAP::iterator itorDialogInfoMap;
				TNPCInfo* pNiSelectedNPC = pNim->Get(nSelectedNPCID);
				if (nullptr == pNiSelectedNPC) return;

				if (true == bSelectedOnScreen)
				{
					itorDialogInfoMap = pNiSelectedNPC->m_mapOnScreenDialog.find(nSelectedDialogID);
					if (pNiSelectedNPC->m_mapOnScreenDialog.end() == itorDialogInfoMap)
					{
						this->treeView_DialogInfo->Nodes->Clear();
						return;
					}
				}
				else
				{
					itorDialogInfoMap = pNiSelectedNPC->m_mapOffScreenDialog.find(nSelectedDialogID);
					if (pNiSelectedNPC->m_mapOffScreenDialog.end() == itorDialogInfoMap)
					{
						this->treeView_DialogInfo->Nodes->Clear();
						return;
					}
				}

				treeView_DialogInfo->BeginUpdate();

				TDialogInfo* pDiFindedResult = (*itorDialogInfoMap).second;
				String^ strText = this->MakeTextOfTreeView_DialogInfo(pDiFindedResult);

				TreeNode^ tnRoot = treeView_DialogInfo->Nodes->Add(strText);
				TDialogNodeAdapter^ dnaNew = gcnew TDialogNodeAdapter(pDiFindedResult);
				tnRoot->Tag = dnaNew;

				this->TVDI_SetTreeViewTextColor(tnRoot, pDiFindedResult);
				this->TVDI_AddNodes(tnRoot, pDiFindedResult);

				treeView_DialogInfo->EndUpdate();
			}

	private: System::Void InsertTreeNodeIntoTreeView_DialogList(TNPCInfo* pNPCInfo)
			 {
				 String^ strNPCText = this->MakeNPCTextOfTreeView_DialogList(pNPCInfo->nID, gcnew String(pNPCInfo->strName.c_str()));
				 TreeNode^ tnSearchedResult = this->m_pDc->SearchTreeNode(this->treeView_DialogList->Nodes, strNPCText);

				 if(nullptr == tnSearchedResult)
				 {
					 TreeNode^ tnNPC = treeView_DialogList->Nodes->Add(strNPCText);
					 tnNPC->Tag = safe_cast<int>(TDN_NPC);
					 
					 if (false == pNPCInfo->m_mapOnScreenDialog.empty())
					 {
						 String^ strScreenText = this->MakeScreenTextOfTreeView_DilaogList(true);
						 TreeNode^ tnScreen = tnNPC->Nodes->Add(strScreenText);
						 tnScreen->Tag = safe_cast<int>(TDN_ON_SCREEN);

						 for (DIALOGINFOMAP::iterator itorDialogInfo = pNPCInfo->m_mapOnScreenDialog.begin(); pNPCInfo->m_mapOnScreenDialog.end() != itorDialogInfo; ++itorDialogInfo)
						 {
							 int nDialogID = (*itorDialogInfo).second->m_nID;
							 String^ strDialogText = this->MakeDialogTextOfTreeView_DilaogList(nDialogID);

							 TreeNode^ tnDialog = tnScreen->Nodes->Add(strDialogText);
							 tnDialog->Tag = safe_cast<int>(TDN_DIALOG);

							 if (true == this->m_pDc->IsModifiedDialog(pNPCInfo->nID, true, nDialogID))
							 {
								 tnDialog->ForeColor = Color::Red;
							 }
						 }
					 }
					 
					 if (false == pNPCInfo->m_mapOffScreenDialog.empty())
					 {
						 String^ strScreenText = this->MakeScreenTextOfTreeView_DilaogList(false);
						 TreeNode^ tnScreen = tnNPC->Nodes->Add(strScreenText);
						 tnScreen->Tag = safe_cast<int>(TDN_OFF_SCREEN);
						 
						 for (DIALOGINFOMAP::iterator itorDialogInfo = pNPCInfo->m_mapOffScreenDialog.begin(); pNPCInfo->m_mapOffScreenDialog.end() != itorDialogInfo; ++itorDialogInfo)
						 {
							 int nDialogID = (*itorDialogInfo).second->m_nID;
							 String^ strDialogText = this->MakeDialogTextOfTreeView_DilaogList(nDialogID);

							 TreeNode^ tnDialog = tnScreen->Nodes->Add(strDialogText);
							 tnDialog->Tag = safe_cast<int>(TDN_DIALOG);
							 
							 if (true == this->m_pDc->IsModifiedDialog(pNPCInfo->nID, true, nDialogID))
							 {
								 tnDialog->ForeColor = Color::Red;
							 }
						 }
					 }
				 }
			 }

	
	private: System::Void TVDI_AddNodes(TreeNode^ tnDst, TDialogNode* pDnSrc)
			 {
				 size_t nSize = pDnSrc->m_vecpChildNode.size();
				 for (size_t i = 0; i < nSize; i++)
				 {
					 TDialogNode* pDnChild = pDnSrc->m_vecpChildNode[i];

					 TreeNode^ tnChild = TVDI_AddNode(tnDst, pDnChild);

					 TVDI_AddNodes(tnChild, pDnChild);
				 }
			 }
	private: TreeNode^ TVDI_AddNode(TreeNode^ tnDst, TDialogNode* pDnSrc)
			 {
				 String^ strText = this->MakeTextOfTreeView_DialogInfo(pDnSrc);
				 TreeNode^ tnChild = tnDst->Nodes->Add(strText);
				 tnChild->Tag = gcnew TDialogNodeAdapter(pDnSrc);
				 TVDI_SetTreeViewTextColor(tnChild, pDnSrc);

				 tnDst->Expand();

				 return tnChild;
			 }
	private: System::Void TVDI_SetTreeViewTextColor(TreeNode^ tn, TDialogNode* pDN)
			 {
				 switch (pDN->m_nType)
				 {
				 case DNT_CHECK:
				 case DNT_CASE_CHECKFAIL:
				 case DNT_CASE_NOEXIST:
				 case DNT_CASE_DOING:
				 case DNT_CASE_FINAL:
				 case DNT_CASE_COMPLETED:
				 case DNT_CASE_DEFAULT:
					 {
						 tn->ForeColor = System::Drawing::Color::Green;
					 }
					 break;
				 case DNT_SAY:
					 {
						 tn->ForeColor = System::Drawing::Color::Red;
					 }
					 break;
				 case DNT_SELECT:
					 {
						 tn->ForeColor = System::Drawing::Color::Blue;
					 }
					 break;
				 case DNT_INFO:
					 break;
				 }
			 }
			 /////////////////////////////////////////////////////////////////////////////////////
			 // 임의로 추가한 함수들 ^

	private: System::Void DialogForm_Load(System::Object^  sender, System::EventArgs^  e) {
				 this->comboBox_FindNPC->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"이름", L"ID"});
				 this->comboBox_FindNPC->SelectedIndex = 0;
				 // Dialog 정보에서 TreeView UI를 갱신한다.
			 }


	private: System::Void treeView_DialogList_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {				 
				 if (nullptr == treeView_DialogList->SelectedNode) return;

				 treeView_DialogList->SelectedNode = e->Node;
				 treeView_DialogList->SelectedNode->BackColor = System::Drawing::Color::White;
				 treeView_DialogList->SelectedNode->ContextMenuStrip = contextMenuStrip_treeView_DialogList;

				 TREEVIEW_DIALOGLIST_NODETYPE nTDN = safe_cast<TREEVIEW_DIALOGLIST_NODETYPE>(MStringToInt(e->Node->Tag->ToString()));

				 switch (nTDN)
				 {
				 case TDN_NPC:
					 {
						 toolStripMenuItem_TVDL_Add->Visible = false;
						 toolStripMenuItem_TVDL_Delete->Visible = false;

						 if (false == this->m_pDc->HaveOnScreenDialog())
						 {
							 toolStripMenuItem_TVDL_AddOnScreen->Visible = true;
						 }
						 else
						 {
							 toolStripMenuItem_TVDL_AddOnScreen->Visible = false;
						 }

						 if (false == this->m_pDc->HaveOffScreenDialog())
						 {
							 toolStripMenuItem_TVDL_AddOffScreen->Visible = true;
						 }
						 else
						 {
							 toolStripMenuItem_TVDL_AddOffScreen->Visible = false;
						 }
					 }
					 break;
				 case TDN_DIALOG:
					 {
						 toolStripMenuItem_TVDL_Add->Visible = false;
						 toolStripMenuItem_TVDL_Delete->Visible = true;
						 toolStripMenuItem_TVDL_AddOnScreen->Visible = false;
						 toolStripMenuItem_TVDL_AddOffScreen->Visible = false;
					 }
					 break;
				 case TDN_ON_SCREEN:
				 case TDN_OFF_SCREEN:
					 {
						 toolStripMenuItem_TVDL_Add->Visible = true;
						 toolStripMenuItem_TVDL_Delete->Visible = false;
						 toolStripMenuItem_TVDL_AddOnScreen->Visible = false;
						 toolStripMenuItem_TVDL_AddOffScreen->Visible = false;
					 }
					 break;
				 default:
					 {
						 _ASSERT(0);
					 }
					 break;
				 }
			 }
	private: System::Void ToolStripMenuItem_TVDL_Insert_Click(System::Object^  sender, System::EventArgs^  e) {

				 bool bOnScreen = false;

				 TREEVIEW_DIALOGLIST_NODETYPE nTDN = safe_cast<TREEVIEW_DIALOGLIST_NODETYPE>(MStringToInt(treeView_DialogList->SelectedNode->Tag->ToString()));

				 switch (nTDN)
				 {
				 case TDN_NPC:
				 case TDN_DIALOG:
					 {
						 return;
					 }
					 break;

				 case TDN_ON_SCREEN:
					 {
						 bOnScreen = true;
					 }
					 break;
				 case TDN_OFF_SCREEN:
					 {
						 bOnScreen = false;
					 }
					 break;
				 }

				 InsertDialog insertDialog;
				 insertDialog.ShowDialog();

				 System::Windows::Forms::DialogResult res = insertDialog.DialogResult;

				 int nDialogID = -1;
				 if (System::Windows::Forms::DialogResult::OK == res)
				 {
					 nDialogID = insertDialog.GetDialogID();
					 if (false == this->m_pDc->AddDialog(nDialogID, this->m_pDc->GetSelectedOnScreen()))
					 {
						 System::Windows::Forms::MessageBox::Show("잘못된 다이얼로그 ID 입니다");
						 return;
					 }

					 String^ strID = String::Format("{0}", nDialogID);
					 TreeNode^ tnChild = treeView_DialogList->SelectedNode->Nodes->Add(strID);
					 tnChild->Tag = safe_cast<int>(TDN_DIALOG);

					 treeView_DialogList->Update();
				 }
				 else
				 {
					 return;
				 }
				 
				 this->m_pDc->SerializeTreeView_DialogInfo();
				 int nNPCID = this->m_pDc->GetSelectedNPCID();
				 this->ChangeForeColorOfTreeView_DialogList(nNPCID, bOnScreen, nDialogID, Color::Red);
			 }

	private: System::Void ToolStripMenuItem_TVDL_Delete_Click(System::Object^  sender, System::EventArgs^  e) {

				 TREEVIEW_DIALOGLIST_NODETYPE nTDN = safe_cast<TREEVIEW_DIALOGLIST_NODETYPE>(MStringToInt(treeView_DialogList->SelectedNode->Tag->ToString()));
				 switch (nTDN)
				 {
				 case TDN_NPC:
				 case TDN_ON_SCREEN:
				 case TDN_OFF_SCREEN:
					 break;
				 case TDN_DIALOG:
					 {
						 this->m_pDc->DeleteDialog();
						 
						 if (1 < this->treeView_DialogList->SelectedNode->Parent->Nodes->Count)
						 {
							 this->treeView_DialogList->SelectedNode->Remove();
						 }
						 else
						 {
							 this->treeView_DialogList->SelectedNode->Parent->Remove();
						 }
					 }
					 break;
				 default:
					 {
						 _ASSERT(0);
					 }
					 break;
				 }

				 this->m_pDc->SerializeTreeView_DialogInfo();
			 }
    
	private: System::Void treeView_DialogList_BeforeSelect(System::Object^  sender, System::Windows::Forms::TreeViewCancelEventArgs^  e) {
				 if (nullptr == this->treeView_DialogList->SelectedNode) return;
				 this->m_pDc->SetPreSelectedDialogListTreeNode(this->treeView_DialogList->SelectedNode);
				 this->treeView_DialogList->SelectedNode->BackColor = Color::White;
			 }

	private: System::Void treeView_DialogList_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {

				 if (nullptr == this->treeView_DialogList->SelectedNode) return;
				 this->treeView_DialogList->SelectedNode->BackColor = Color::Gray;
				 this->treeView_DialogList->SelectedNode->ContextMenuStrip = this->contextMenuStrip_treeView_DialogList;

				 TREEVIEW_DIALOGLIST_NODETYPE nTDN = safe_cast<TREEVIEW_DIALOGLIST_NODETYPE>(MStringToInt(treeView_DialogList->SelectedNode->Tag->ToString()));
				 switch (nTDN)
				 {

				 case TDN_NPC: 
					 {
						 this->m_pDc->SetSelectedNPCID(this->GetNPCIDOfTreeView_DilaogList(treeView_DialogList->SelectedNode->Text));
						 this->m_pDc->SetSelectedDialogID(-1);

						 this->toolStripStatusLabel_NPC->Text = String::Format("NPC : {0}", this->treeView_DialogList->SelectedNode->Text);
						 this->toolStripStatusLabel_Dialog->Text = "";
					 }
					 break;
				 case TDN_ON_SCREEN: 
					 {
						 TreeNode^ tnNPC = treeView_DialogList->SelectedNode->Parent;
						 this->m_pDc->SetSelectedNPCID(this->GetNPCIDOfTreeView_DilaogList(tnNPC->Text));
						 this->m_pDc->SetSelectedOnScreen(true);
						 this->m_pDc->SetSelectedDialogID(-1);

						 this->toolStripStatusLabel_NPC->Text = String::Format("NPC : {0}", tnNPC->Text);
						 this->toolStripStatusLabel_Dialog->Text = "";
					 }
					 break;
				 case TDN_OFF_SCREEN: 
					 {
						 TreeNode^ tnNPC = treeView_DialogList->SelectedNode->Parent;
						 this->m_pDc->SetSelectedNPCID(this->GetNPCIDOfTreeView_DilaogList(tnNPC->Text));
						 this->m_pDc->SetSelectedOnScreen(false);
						 this->m_pDc->SetSelectedDialogID(-1);

						 this->toolStripStatusLabel_NPC->Text = String::Format("NPC : {0}", tnNPC->Text);
						 this->toolStripStatusLabel_Dialog->Text = "";
					 }
					 break;
				 case TDN_DIALOG:
					 {
						 TreeNode^ tnOnOffScreen = treeView_DialogList->SelectedNode->Parent;
						 if (nullptr == tnOnOffScreen) return;

						 TreeNode^ tnNPC = tnOnOffScreen->Parent;
						 this->m_pDc->SetSelectedNPCID(this->GetNPCIDOfTreeView_DilaogList(tnNPC->Text));

						 int nDialogID = MStringToInt(treeView_DialogList->SelectedNode->Text->ToString());
						 this->m_pDc->SetSelectedDialogID(nDialogID);

						 treeView_DialogList->SelectedNode->ContextMenuStrip = contextMenuStrip_treeView_DialogList;
						 this->toolStripStatusLabel_NPC->Text = String::Format("NPC : {0}", tnNPC->Text);
						 
						 TREEVIEW_DIALOGLIST_NODETYPE nTDN = safe_cast<TREEVIEW_DIALOGLIST_NODETYPE>(MStringToInt(tnOnOffScreen->Tag->ToString()));
						 if (TDN_ON_SCREEN == nTDN)
						 {
							 this->m_pDc->SetSelectedOnScreen(true);
							 this->toolStripStatusLabel_Dialog->Text = String::Format("OnScreenID : {0}", treeView_DialogList->SelectedNode->Text->ToString());
						 }							
						 else if (TDN_OFF_SCREEN == nTDN)
						 {
							 this->m_pDc->SetSelectedOnScreen(false);
							 this->toolStripStatusLabel_Dialog->Text = String::Format("OffScreenID : {0}", treeView_DialogList->SelectedNode->Text->ToString());
						 }
					 }
					 break;
				 }

				 this->m_pDc->SerializeTreeView_DialogInfo();
				 this->treeView_DialogInfo->ExpandAll();

				 this->HideAllPanel();
			 }

	private: System::Void treeView_DialogInfo_BeforeSelect(System::Object^  sender, System::Windows::Forms::TreeViewCancelEventArgs^  e) {
				 if (nullptr == treeView_DialogInfo->SelectedNode) return;
				 treeView_DialogInfo->SelectedNode->BackColor = Color::White;
			 }

	private: System::Void treeView_DialogInfo_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
				 this->treeView_DialogInfo->SelectedNode->BackColor = Color::Gray;
				 this->treeView_DialogInfo->SelectedNode->ContextMenuStrip = this->contextMenuStrip_treeView_DialogInfo;

				 TDialogNodeAdapter^ dialogNodeAdapter =	static_cast<TDialogNodeAdapter^>(treeView_DialogInfo->SelectedNode->Tag);
				 this->m_pDc->SetSelectedDialogNode(dialogNodeAdapter->m_pDialogNode);
				 TDialogNode* pDialogNode = dialogNodeAdapter->m_pDialogNode;
				 if (nullptr == pDialogNode) return;

				 switch (pDialogNode->m_nType)
				 {
				 case DNT_CHECK:
					 {
						 panel_Default->Visible = false;
						 panel_Check->Visible = true;
						 panel_Say->Visible = false;
						 panel_Select->Visible = false;

						 TDialogCheckNode* pDialogCheckNode = static_cast<TDialogCheckNode*>(pDialogNode);

						 comboBox_CheckType->Items->Clear();
						 comboBox_CheckType->Items->Add("퀘스트");
						 comboBox_CheckType->SelectedIndex = pDialogCheckNode->m_nCheckType;

						 switch (pDialogCheckNode->m_nCheckType)
						 {
						 case DCT_QUEST:
							 {
								 label_CheckType->Text = "퀘스트";
								 label_CheckParam1->Text = "퀘스트ID";
								 label_CheckParam2->Text = "목적ID";
							 }
							 break;
						 case DCT_FACTION:
							 {

							 }
							 break;
						 }

						 textBox_CheckParam1->Text = String::Format("{0}", pDialogCheckNode->m_nParam1);

						 if (0 >= pDialogCheckNode->m_nParam2)
						 {
							 textBox_CheckParam2->Text = "";
						 }
						 else
						 {
							 textBox_CheckParam2->Text = String::Format("{0}", pDialogCheckNode->m_nParam2);
						 }
					 }
					 break;
				 case DNT_SAY:
					 {
						 panel_Default->Visible = false;
						 panel_Check->Visible = false;
						 panel_Say->Visible = true;
						 panel_Select->Visible = false;

						 TDialogSayNode* pDialogSayNode = static_cast<TDialogSayNode*>(pDialogNode);
						 textBox_SayText->Text = gcnew String(this->m_pDc->GetString(gcnew String(pDialogSayNode->m_strText.c_str())));
					 }
					 break;
				 case DNT_SELECT:
					 {
						 panel_Default->Visible = false;
						 panel_Check->Visible = false;
						 panel_Say->Visible = false;
						 panel_Select->Visible = true;

						 TDialogSelectNode* pDialogSelectNode = static_cast<TDialogSelectNode*>(pDialogNode);

						 textBox_SelectText->Text = gcnew String(this->m_pDc->GetString(gcnew String(pDialogSelectNode->m_strText.c_str())));
						 textBox_SelectExit->Text = String::Format("{0}", pDialogSelectNode->m_nExit);
					 }
					 break;
				 case DNT_CASE_CHECKFAIL:
				 case DNT_CASE_NOEXIST:
				 case DNT_CASE_DOING:
				 case DNT_CASE_FINAL:
				 case DNT_CASE_COMPLETED:
				 case DNT_CASE_DEFAULT:
				 case DNT_INFO:
					 {
						 panel_Default->Visible = true;
						 panel_Check->Visible = false;
						 panel_Say->Visible = false;
						 panel_Select->Visible = false;
					 }
					 break;
				 };
			 }

	private: System::Void button_CheckOk_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogCheckNode* pDialogCheckNode = static_cast<TDialogCheckNode*>(this->m_pDc->GetSelectedDialogNode());
				 if (nullptr == pDialogCheckNode) return;

				 pDialogCheckNode->m_nCheckType = comboBox_CheckType->SelectedIndex;

				 int nParam1 = MStringToInt(textBox_CheckParam1->Text->ToString());
				 int nParam2 = MStringToInt(textBox_CheckParam2->Text->ToString());

				 switch (comboBox_CheckType->SelectedIndex)
				 {
				 case DCT_QUEST:
					 {
						 if (false == this->m_pDc->IsValidQuest(nParam1))
						 {
							 Windows::Forms::MessageBox::Show("잘못된 퀘스트 ID입니다.");
							 return;
						 }
						 else 
						 {
							 if (0 == textBox_CheckParam2->Text->Length)
							 {
								 nParam2 = -1;
							 }
							 else if (false == this->m_pDc->IsValidQuestObjective(nParam1, nParam2))
							 {
								 Windows::Forms::MessageBox::Show("잘못된 퀘스트 목적 ID입니다.");
								 return;
							 }
						 }
					 }
					 break;
				 case DCT_FACTION:
					 break;
				 }

				 pDialogCheckNode->m_nParam1 = nParam1;
				 pDialogCheckNode->m_nParam2 = nParam2;

				 
				 treeView_DialogInfo->SelectedNode->Text = this->MakeTextOfTreeView_DialogInfo(pDialogCheckNode);
				 treeView_DialogInfo->Update();

				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }

	private: System::Void button_CheckCancel_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogCheckNode* pDialogCheckNode = static_cast<TDialogCheckNode*>(this->m_pDc->GetSelectedDialogNode());
				 if (nullptr == pDialogCheckNode) return;

				 comboBox_CheckType->SelectedIndex = pDialogCheckNode->m_nCheckType;

				 textBox_CheckParam1->Text = String::Format("{0}", pDialogCheckNode->m_nParam1);
				 textBox_CheckParam2->Text = String::Format("{0}", pDialogCheckNode->m_nParam2);
			 }

	private: System::Void button_SayOk_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogSayNode* pDialogSayNode = static_cast<TDialogSayNode*>(this->m_pDc->GetSelectedDialogNode());
				 if (nullptr == pDialogSayNode) return;

				 String^ strText = textBox_SayText->Text->ToString();
				 bool bOnScreen = this->m_pDc->GetSelectedOnScreen();
				 if (true == bOnScreen)
				 {
					 this->m_pDc->SetStringToStrOnScreenDialogTable(gcnew String(pDialogSayNode->m_strText.c_str()), strText);
				 }
				 else
				 {
					 this->m_pDc->SetStringToStrOffScreenDialogTable(gcnew String(pDialogSayNode->m_strText.c_str()), strText);
				 }
				 

				 treeView_DialogInfo->SelectedNode->Text = this->MakeTextOfTreeView_DialogInfo(pDialogSayNode);
				 treeView_DialogInfo->Update();

				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }

	private: System::Void button_SayCancel_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogSayNode* pDialogSayNode = static_cast<TDialogSayNode*>(this->m_pDc->GetSelectedDialogNode());
				 if (nullptr == pDialogSayNode) return;

				 textBox_SayText->Text = gcnew String(this->m_pDc->GetString(gcnew String(pDialogSayNode->m_strText.c_str())));
			 }

	private: System::Void button_SelectOk_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogSelectNode* pDialogSelectNode = static_cast<TDialogSelectNode*>(this->m_pDc->GetSelectedDialogNode());
				 TDialogInfo* pDialogInfo = this->m_pDc->GetSelectedDialogRootNode();

				 int nExit = -1;
				 if (false == String::IsNullOrEmpty(textBox_SelectExit->Text))
				 {
					 nExit = MStringToInt(textBox_SelectExit->Text->ToString());
				 }
				 int nSize = pDialogInfo->m_vecChildSelectNodeExit.size();

				 if (0 < nExit && nExit != pDialogSelectNode->m_nExit)
				 {
					 for (int i = 0; nSize > i; i++)
					 {
						 if (nExit == pDialogInfo->m_vecChildSelectNodeExit[i])
						 {
							 System::Windows::Forms::MessageBox::Show(L"다른 Exit값을 입력해주세요. 이미 사용중입니다.");
							 return;
						 }
					 }
				 }
				 else if(0 > nExit)
				 {
					 System::Windows::Forms::MessageBox::Show(L"다른 Exit값을 입력해주세요. 음수값은 사용할 수 없습니다.");
					 return;
				 }

				 if (0 < nExit)
				 {
					 if (0 < pDialogSelectNode->m_nExit)
					 {
						 for (int i = 0; nSize > i; i++)
						 {
							 if (pDialogSelectNode->m_nExit == pDialogInfo->m_vecChildSelectNodeExit[i])
							 {
								 pDialogInfo->m_vecChildSelectNodeExit[i] = nExit;
							 }
						 }
					 }
					 else
					 {
						 pDialogInfo->m_vecChildSelectNodeExit.push_back(nExit);
					 }
				 }
				 else
				 {
					 if (0 < pDialogSelectNode->m_nExit)
					 {
						 for (vector<int>::iterator itor = pDialogInfo->m_vecChildSelectNodeExit.begin(); pDialogInfo->m_vecChildSelectNodeExit.end() != itor; ++itor)
						 {
							 if (pDialogSelectNode->m_nExit == (*itor))
							 {
								 pDialogInfo->m_vecChildSelectNodeExit.erase(itor);
								 break;
							 }
						 }
					 }
				 }

				 String^ strText = textBox_SelectText->Text->ToString();
				 bool bOnScreen = this->m_pDc->GetSelectedOnScreen();
				 if (true == bOnScreen)
				 {
					 this->m_pDc->SetStringToStrOnScreenDialogTable(gcnew String(pDialogSelectNode->m_strText.c_str()), strText);
				 }
				 else
				 {
					 this->m_pDc->SetStringToStrOffScreenDialogTable(gcnew String(pDialogSelectNode->m_strText.c_str()), strText);
				 }
				 

				 pDialogSelectNode->m_nExit = nExit;

				 treeView_DialogInfo->SelectedNode->Text = this->MakeTextOfTreeView_DialogInfo(pDialogSelectNode);
				 treeView_DialogInfo->Update();

				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void button_SelectCancel_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogSelectNode* pDialogSelectNode = static_cast<TDialogSelectNode*>(this->m_pDc->GetSelectedDialogNode());
				 if (nullptr == pDialogSelectNode) return;

				 textBox_SelectText->Text = gcnew String(this->m_pDc->GetString(gcnew String(pDialogSelectNode->m_strText.c_str())));
				 textBox_SelectExit->Text = String::Format("{0}", pDialogSelectNode->m_nExit);
			 }
	private: System::Void treeView_DialogInfo_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
				 TREEVIEW_DIALOGLIST_NODETYPE nTDN = safe_cast<TREEVIEW_DIALOGLIST_NODETYPE>(MStringToInt(treeView_DialogList->SelectedNode->Tag->ToString()));
				 if (TDN_DIALOG == nTDN)
				 {
					 treeView_DialogList->SelectedNode->BackColor = System::Drawing::Color::Gray;
				 }

				 treeView_DialogInfo->SelectedNode = e->Node;
				 TDialogNodeAdapter^ pDialogNodeAdapter = static_cast<TDialogNodeAdapter^>(treeView_DialogInfo->SelectedNode->Tag);
				 this->m_pDc->SetSelectedDialogNode(pDialogNodeAdapter->m_pDialogNode);

				 TDialogNode* pDialogNode = pDialogNodeAdapter->m_pDialogNode;

				 if (nullptr == pDialogNode) return;

				 this->toolStripMenuItem_TVDI_Add->Visible = true;
				 this->toolStripMenuItem_TVDI_Paste->Visible = false;
		 
				 if (DNT_INFO == pDialogNode->m_nType)
				 {
					 this->toolStripMenuItem_TVDI_Delete->Visible = false;
					 this->toolStripMenuItem_TVDI_Cut->Visible = false;
					 this->toolStripMenuItem_TVDI_Copy->Visible = false;
				 }
				 else
				 {
					 this->toolStripMenuItem_TVDI_Delete->Visible = true;
					 this->toolStripMenuItem_TVDI_Cut->Visible = true;
					 this->toolStripMenuItem_TVDI_Copy->Visible = true;
				 }
				 
				 if (nullptr != this->m_pDc->GetDialogNodeClip())
				 {
					 switch (pDialogNode->m_nType)
					 {
					 case DNT_INFO:
						 {
							 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
							 {
							 case DNT_CHECK:
							 case DNT_SAY:
								 {
									 this->toolStripMenuItem_TVDI_Paste->Visible = true;
								 }
								 break;
							 }
						 }
						 break;
					 case DNT_CHECK:
						 {
							 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
							 {							 
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
								 {
									 this->toolStripMenuItem_TVDI_Paste->Visible = true;
								 }
								 break;
							 }
						 }
						 break;
					 case DNT_CASE_CHECKFAIL:
					 case DNT_CASE_NOEXIST:
					 case DNT_CASE_DOING:
					 case DNT_CASE_FINAL:
					 case DNT_CASE_COMPLETED:
					 case DNT_CASE_DEFAULT:					 
						 {
							 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
							 {
							 case DNT_SELECT:
							 case DNT_CHECK:
							 case DNT_SAY:
								 {
									 this->toolStripMenuItem_TVDI_Paste->Visible = true;
								 }
								 break;
							 }
						 }
						 break;
					 case DNT_SAY:
						 {
							 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
							 {
							 case DNT_CHECK:
								 {

									 this->toolStripMenuItem_TVDI_Paste->Visible = true;
								 }
								 break;
							 case DNT_SELECT:
								 {
									 if (false == this->m_pDc->GetSelectedOnScreen())
									 {
										 this->toolStripMenuItem_TVDI_Paste->Visible = true;
									 }
								 }
								 break;
							 }
						 }
						 break;
					 case DNT_SELECT:
						 {
							 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
							 {
							 case DNT_CHECK:
							 case DNT_SAY:
								 {
									 this->toolStripMenuItem_TVDI_Paste->Visible = true;
								 }
								 break;
							 }
						 }
						 break;
					 }

				 }
				 else
				 {
					 this->toolStripMenuItem_TVDI_Paste->Visible = false;
				 }

				 treeView_DialogInfo->SelectedNode->ContextMenuStrip = contextMenuStrip_treeView_DialogInfo;
			 }
	private: System::Void toolStripMenuItem_TVDI_Add_DropDownOpened(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 this->toolStripMenuItem_Check->Visible = false;
				 this->toolStripMenuItem_CheckFail->Visible  = false;
				 this->toolStripMenuItem_Noexist->Visible  = false;
				 this->toolStripMenuItem_Doing->Visible = false;
				 this->toolStripMenuItem_Final->Visible  = false;
				 this->toolStripMenuItem_Completed->Visible  = false;
				 this->toolStripMenuItem_Default->Visible  = false;
				 this->toolStripMenuItem_Say->Visible  = false;
				 this->toolStripMenuItem_Select->Visible  = false;

				 switch (pDialogNode->m_nType)
				 {
				 case DNT_INFO:
					 {
						 this->toolStripMenuItem_Check->Visible = true;
						 this->toolStripMenuItem_Say->Visible  = true;
					 }
					 break;
				 case DNT_CHECK:
					 {
						 this->toolStripMenuItem_CheckFail->Visible  = true;
						 this->toolStripMenuItem_Noexist->Visible  = true;
						 this->toolStripMenuItem_Doing->Visible = true;
						 this->toolStripMenuItem_Final->Visible  = true;
						 this->toolStripMenuItem_Completed->Visible  = true;
						 this->toolStripMenuItem_Default->Visible  = true;
					 }
					 break;
				 case DNT_CASE_CHECKFAIL:
				 case DNT_CASE_NOEXIST:
				 case DNT_CASE_DOING:
				 case DNT_CASE_FINAL:
				 case DNT_CASE_COMPLETED:
				 case DNT_CASE_DEFAULT:
					 {
						 this->toolStripMenuItem_Check->Visible = true;
						 this->toolStripMenuItem_Say->Visible  = true;
						 this->toolStripMenuItem_Select->Visible  = true;
					 }
					 break;
				 case DNT_SAY:
					 {						 
						 if (false == this->m_pDc->GetSelectedOnScreen())
						 {
							 this->toolStripMenuItem_Select->Visible  = true;
						 }
						 this->toolStripMenuItem_Check->Visible = true;
					 }
					 break;
				 case DNT_SELECT:
					 {
						 this->toolStripMenuItem_Check->Visible = true;
						 this->toolStripMenuItem_Say->Visible  = true;
					 }
					 break;
				 }

			 }
	private: System::Void ToolStripMenuItem_TVDI_Delete_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 this->m_pDc->DeleteDialogNode(pDialogNode);

				 treeView_DialogInfo->SelectedNode->Remove();
				 treeView_DialogInfo->Update();

				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void ToolStripMenuItem_TVDI_Cut_Click(System::Object^  sender, System::EventArgs^  e) {

				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pParentDialogNode = pDialogNode->m_pParentNode;
				 TreeNode^ tnParent = treeView_DialogInfo->SelectedNode->Parent;

				 this->m_pDc->CutDialogNode(pDialogNode);

				 treeView_DialogInfo->SelectedNode->Remove();
				 treeView_DialogInfo->Update();

				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void ToolStripMenuItem_TVDI_Copy_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 this->m_pDc->CopyDialogNode(pDialogNode);
			 }
	private: System::Void ToolStripMenuItem_TVDI_Paste_Click(System::Object^  sender, System::EventArgs^  e) {

				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->PasteDialogNode(pDialogNode);
				 TreeNode^ tnChild = TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 TVDI_AddNodes(tnChild, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 treeView_DialogInfo->SelectedNode->ExpandAll();

				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_Check_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogCheckNode(pDialogNode);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_CheckFail_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogCaseNode(pDialogNode, DNT_CASE_CHECKFAIL);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_Noexist_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogCaseNode(pDialogNode, DNT_CASE_NOEXIST);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_Doing_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogCaseNode(pDialogNode, DNT_CASE_DOING);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_Final_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogCaseNode(pDialogNode, DNT_CASE_FINAL);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_Completed_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogCaseNode(pDialogNode, DNT_CASE_COMPLETED);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_Default_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogCaseNode(pDialogNode, DNT_CASE_DEFAULT);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_Say_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogSayNode(pDialogNode);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void toolStripMenuItem_Select_Click(System::Object^  sender, System::EventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr == pDialogNode) return;

				 TDialogNode* pNewDialogNode = this->m_pDc->AddDialogSelectNode(pDialogNode);
				 TVDI_AddNode(treeView_DialogInfo->SelectedNode, pNewDialogNode);

				 treeView_DialogInfo->Update();
				 this->m_pDc->SetModifiedDialogID();
				 this->ChangeForeColorOfTreeView_DialogList();
			 }
	private: System::Void button_Expand_Click(System::Object^  sender, System::EventArgs^  e) {
				 treeView_DialogInfo->ExpandAll();
			 }
	private: System::Void button_Collapse_Click(System::Object^  sender, System::EventArgs^  e) {
				 treeView_DialogInfo->CollapseAll();
				 this->HideAllPanel();
			 }

	private: System::Void DialogForm_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 switch (e->KeyCode)
				 {
				 case Keys::F:
					 {
						 if (true == e->Control)
						 {
							 this->textBox_FindNPC->Focus();
						 }
					 }
					 break;
				 }
			 }


    private: System::Void treeView_DialogList_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 switch (e->KeyCode)
				 {
				 case Keys::Insert:
					 {
						 this->ToolStripMenuItem_TVDL_Insert_Click(nullptr, nullptr);
					 }
					 break;

				 case Keys::Delete:					 
					 {
						 this->ToolStripMenuItem_TVDL_Delete_Click(nullptr, nullptr);
					 }
					 break;
				 }
			 }

	private: System::Void treeView_DialogInfo_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 TDialogNode* pDialogNode = this->m_pDc->GetSelectedDialogNode();
				 if (nullptr ==  pDialogNode) return;

				 switch (e->KeyCode)
				 {
				 case Keys::Delete:
					 {
						 if (DNT_INFO != pDialogNode->m_nType)
						 {
							 this->ToolStripMenuItem_TVDI_Delete_Click(nullptr, nullptr);
						 }
					 }
					 break;
				 case Keys::D0:
					 {
						 if (true == e->Control)
						 {
							 switch (pDialogNode->m_nType)
							 {
							 case DNT_CHECK:
								 {
									 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
								 }
								 break;
							 case DNT_INFO:
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
							 case DNT_SAY:
							 case DNT_SELECT:
								 {
									 this->toolStripMenuItem_Check_Click(nullptr, nullptr);
								 }
								 break;
							 }
						 }
					 }
					 break;
				 case Keys::D1:
					 {
						 if (true == e->Control)
						 {
							 switch (pDialogNode->m_nType)
							 {
							 case DNT_CHECK:
								 {
									 this->toolStripMenuItem_CheckFail_Click(nullptr, nullptr);
								 }
								 break;
							 case DNT_INFO:
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
							 case DNT_SAY:
							 case DNT_SELECT:
								 {
									 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
								 }
								 break;
							 }
						 }
					 }
					 break;
				 case Keys::D2:
					 {
						 if (true == e->Control)
						 {
							 switch (pDialogNode->m_nType)
							 {
							 case DNT_CHECK:
								 {
									 this->toolStripMenuItem_Noexist_Click(nullptr, nullptr);
								 }
								 break;
							 case DNT_INFO:
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
							 case DNT_SAY:
							 case DNT_SELECT:
								 {
									 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
								 }
								 break;
							 }
						 }
					 }
					 break;
				 case Keys::D3:
					 {
						 if (true == e->Control)
						 {
							 switch (pDialogNode->m_nType)
							 {
							 case DNT_CHECK:
								 {
									 this->toolStripMenuItem_Doing_Click(nullptr, nullptr);
								 }
								 break;
							 case DNT_INFO:
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
							 case DNT_SAY:
							 case DNT_SELECT:
								 {
									 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
								 }
								 break;
							 }
						 }
					 }
					 break;
				 case Keys::D4:
					 {
						 if (true == e->Control)
						 {
							 switch (pDialogNode->m_nType)
							 {
							 case DNT_CHECK:
								 {
									 this->toolStripMenuItem_Final_Click(nullptr, nullptr);
								 }
								 break;
							 case DNT_INFO:
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
							 case DNT_SAY:
							 case DNT_SELECT:
								 {
									 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
								 }
								 break;
							 }
						 }
					 }
					 break;
				 case Keys::D5:
					 {
						 if (true == e->Control)
						 {
							 switch (pDialogNode->m_nType)
							 {
							 case DNT_CHECK:
								 {
									 this->toolStripMenuItem_Completed_Click(nullptr, nullptr);
								 }
								 break;
							 case DNT_INFO:
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
							 case DNT_SAY:
							 case DNT_SELECT:
								 {
									 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
								 }
								 break;
							 }
						 }
					 }
					 break;
				 case Keys::D6:
					 {
						 if (true == e->Control)
						 {
							 switch (pDialogNode->m_nType)
							 {
							 case DNT_CHECK:
								 {
									 this->toolStripMenuItem_Default_Click(nullptr, nullptr);
								 }
								 break;
							 case DNT_INFO:
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
							 case DNT_SAY:
							 case DNT_SELECT:
								 {
									 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
								 }
								 break;
							 }
						 }
					 }
					 break;
				 case Keys::D7:
					 {
						 switch (pDialogNode->m_nType)
						 {
						 case DNT_SAY:
						 case DNT_CHECK:
							 {
								 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");				
							 }
							 break;
						 case DNT_INFO:
						 case DNT_CASE_CHECKFAIL:
						 case DNT_CASE_NOEXIST:
						 case DNT_CASE_DOING:
						 case DNT_CASE_FINAL:
						 case DNT_CASE_COMPLETED:
						 case DNT_CASE_DEFAULT:
						 case DNT_SELECT:
							 {
								 this->toolStripMenuItem_Say_Click(nullptr, nullptr);
							 }
							 break;
						 }
					 }
					 break;
				 case Keys::D8:
					 {
						 switch (pDialogNode->m_nType)
						 {
						 case DNT_INFO:
						 case DNT_CHECK:
						 case DNT_SELECT:
							 {
								 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
							 }
							 break;
						 case DNT_CASE_CHECKFAIL:
						 case DNT_CASE_NOEXIST:
						 case DNT_CASE_DOING:
						 case DNT_CASE_FINAL:
						 case DNT_CASE_COMPLETED:
						 case DNT_CASE_DEFAULT:
						 case DNT_SAY:	
							 {
								 if (true == this->m_pDc->GetSelectedOnScreen())
								 {
									 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
								 }
								 else
								 {
									 this->toolStripMenuItem_Select_Click(nullptr, nullptr);
								 }
							 }
							 break;	
						 }
					 }
					 break;
				 case Keys::X:
					 {
						 if (true == e->Control && DNT_INFO != pDialogNode->m_nType)
						 {
							 this->ToolStripMenuItem_TVDI_Cut_Click(nullptr, nullptr);							 
						 }
					 }
					 break;
				 case Keys::C:
					 {
						 if (true == e->Control && DNT_INFO != pDialogNode->m_nType)
						 {
							 this->ToolStripMenuItem_TVDI_Copy_Click(nullptr, nullptr);
						 }
					 }
					 break;
				 case Keys::V:
					 {
						 if (true == e->Control && nullptr != this->m_pDc->GetDialogNodeClip())
						 {
							 switch (pDialogNode->m_nType)
							 {
							 case DNT_INFO:
								 {
									 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
									 {
									 case DNT_CHECK:
									 case DNT_SAY:
										 {
											 this->ToolStripMenuItem_TVDI_Paste_Click(nullptr, nullptr);
										 }
										 break;
									 case DNT_CASE_CHECKFAIL:
									 case DNT_CASE_NOEXIST:
									 case DNT_CASE_DOING:
									 case DNT_CASE_FINAL:
									 case DNT_CASE_COMPLETED:
									 case DNT_CASE_DEFAULT:
									 case DNT_SELECT:
										 {
											 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
										 }
										 break;
									 }
								 }
								 break;
							 case DNT_CHECK:
								 {
									 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
									 {									 
									 case DNT_CASE_CHECKFAIL:
									 case DNT_CASE_NOEXIST:
									 case DNT_CASE_DOING:
									 case DNT_CASE_FINAL:
									 case DNT_CASE_COMPLETED:
									 case DNT_CASE_DEFAULT:
										 {
											 this->ToolStripMenuItem_TVDI_Paste_Click(nullptr, nullptr);
										 }
										 break;
									 case DNT_CHECK:
									 case DNT_SAY:
									 case DNT_SELECT:
										 {
											 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
										 }
										 break;
									 }
								 }
								 break;
							 case DNT_CASE_CHECKFAIL:
							 case DNT_CASE_NOEXIST:
							 case DNT_CASE_DOING:
							 case DNT_CASE_FINAL:
							 case DNT_CASE_COMPLETED:
							 case DNT_CASE_DEFAULT:
								 {
									 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
									 {									 
									 case DNT_CASE_CHECKFAIL:
									 case DNT_CASE_NOEXIST:
									 case DNT_CASE_DOING:
									 case DNT_CASE_FINAL:
									 case DNT_CASE_COMPLETED:
									 case DNT_CASE_DEFAULT:
										 {
											 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
										 }
										 break;
									 case DNT_CHECK:
									 case DNT_SAY:
									 case DNT_SELECT:
										 {
											 this->ToolStripMenuItem_TVDI_Paste_Click(nullptr, nullptr);
										 }
										 break;
									 }
								 }
								 break;
							 case DNT_SAY:
								 {
									 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
									 {
									 case DNT_CASE_CHECKFAIL:
									 case DNT_CASE_NOEXIST:
									 case DNT_CASE_DOING:
									 case DNT_CASE_FINAL:
									 case DNT_CASE_COMPLETED:
									 case DNT_CASE_DEFAULT:
									 case DNT_SAY:
										 {
											 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
										 }
										 break;
									 case DNT_SELECT:
										 {
											 if (true == this->m_pDc->GetSelectedOnScreen())
											 {
												 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
											 }
											 else
											 {
												 this->ToolStripMenuItem_TVDI_Paste_Click(nullptr, nullptr);
											 }
										 }
										 break;
									 case DNT_CHECK:
										 {
											 this->ToolStripMenuItem_TVDI_Paste_Click(nullptr, nullptr);
										 }
										 break;
									 }
								 }
								 break;
							 case DNT_SELECT:
								 {
									 switch (this->m_pDc->GetDialogNodeClip()->m_nType)
									 {									 
									 case DNT_CASE_CHECKFAIL:
									 case DNT_CASE_NOEXIST:
									 case DNT_CASE_DOING:
									 case DNT_CASE_FINAL:
									 case DNT_CASE_COMPLETED:
									 case DNT_CASE_DEFAULT:
									 case DNT_SELECT:
										 {
											 System::Windows::Forms::MessageBox::Show("붙여넣기할 노드가 적합하지 않습니다.");
										 }
										 break;
									 case DNT_CHECK:
									 case DNT_SAY:
										 {
											 this->ToolStripMenuItem_TVDI_Paste_Click(nullptr, nullptr);
										 }
										 break;
									 }
								 }
								 break;
							 }
						 }
					 }
					 break;
				 }
			 }

	private: System::Void button_FindNPC_Click(System::Object^  sender, System::EventArgs^  e) {
				 
				 if (0 == this->comboBox_FindNPC->SelectedIndex)
				 {
					 this->m_pDc->SetHidedNPCIDByName(this->textBox_FindNPC->Text);
				 }
				 else
				 {
					 int nNPCID = MStringToInt(this->textBox_FindNPC->Text);
					 this->m_pDc->SetHidedNPCIDByID(nNPCID);
				 }

				 this->m_pDc->SetSelectedNPCID(-1);
				 this->End();
				 this->Begin();
			 }
	
	private: System::Void toolStripMenuItem_TVDL_ADD_OnScreen_Click(System::Object^  sender, System::EventArgs^  e) {
				 TreeNode^ tnOnScreen = treeView_DialogList->SelectedNode->Nodes->Add("OnScreen");
				 tnOnScreen->Tag = safe_cast<int>(TDN_ON_SCREEN);

				 InsertDialog insertDialog;
				 insertDialog.ShowDialog();
				 
				 System::Windows::Forms::DialogResult res = insertDialog.DialogResult;

				 int nDialogID = -1;
				 if (System::Windows::Forms::DialogResult::OK == res)
				 {
					 nDialogID = insertDialog.GetDialogID();
					 if (false == this->m_pDc->AddDialog(nDialogID, true))
					 {
						 System::Windows::Forms::MessageBox::Show("잘못된 다이얼로그ID 입니다");
						 return;
					 }

					 String^ strID = String::Format("{0}", nDialogID);
					 TreeNode^ tnChild = tnOnScreen->Nodes->Add(strID);
					 tnChild->Tag = safe_cast<int>(TDN_DIALOG);

					 treeView_DialogList->Update();
				 }
				 else
				 {
					 return;
				 }

				 this->m_pDc->SerializeTreeView_DialogInfo();

				 int nNPCID = this->m_pDc->GetSelectedNPCID();
				 this->ChangeForeColorOfTreeView_DialogList(nNPCID, true, nDialogID, Color::Red);
			 }
	private: System::Void toolStripMenuItem_TVDL_ADD_OffScreen_Click(System::Object^  sender, System::EventArgs^  e) {
				 TreeNode^ tnOffScreen = treeView_DialogList->SelectedNode->Nodes->Add("OffScreen");
				 tnOffScreen->Tag = safe_cast<int>(TDN_OFF_SCREEN);

				 InsertDialog insertDialog;
				 insertDialog.ShowDialog();

				 System::Windows::Forms::DialogResult res = insertDialog.DialogResult;

				 int nDialogID = -1;
				 if (System::Windows::Forms::DialogResult::OK == res)
				 {
					 nDialogID = insertDialog.GetDialogID();
					 if (false == this->m_pDc->AddDialog(nDialogID, false))
					 {
						 System::Windows::Forms::MessageBox::Show("잘못된 다이얼로그 ID 입니다");
						 return;
					 }

					 String^ strID = String::Format("{0}", nDialogID);
					 TreeNode^ tnChild = tnOffScreen->Nodes->Add(strID);
					 tnChild->Tag = safe_cast<int>(TDN_DIALOG);

					 treeView_DialogList->Update();
				 }
				 else
				 {
					 return;
				 }
				 
				 this->m_pDc->SerializeTreeView_DialogInfo();

				 int nNPCID = this->m_pDc->GetSelectedNPCID();
				 this->ChangeForeColorOfTreeView_DialogList(nNPCID, false, nDialogID, Color::Red);
			 }

	private: System::Void ToolStripMenuItem_Click_SaveDialog(System::Object^  sender, System::EventArgs^  e) {
				 if (0 > this->m_pDc->GetSelectedDialogID() || true == this->m_pDc->GetSelectedOnScreen())
				 {
					 this->m_pDc->SaveDialogToXml();
				 }
				 else
				 {
					 vector<TDialogNode*> vecDialogNode;

					 TreeNode^ tnRoot = treeView_DialogInfo->SelectedNode;
					 if (nullptr != tnRoot)
					 {
						 while (nullptr != tnRoot->Parent)
						 {
							 tnRoot = tnRoot->Parent;
						 }

						 TDialogNodeAdapter^ dialogNodeAdapter = static_cast<TDialogNodeAdapter^>(tnRoot->Tag);

						 this->m_pDc->GetInvalidDialogNode(dialogNodeAdapter->m_pDialogNode, vecDialogNode);
					 }
					 
					 if (true == vecDialogNode.empty())
					 {
						 this->m_pDc->SaveDialogToXml();
					 }
					 else
					 {
						 System::Windows::Forms::MessageBox::Show("유효성 검사에 실패했습니다. 배경색깔이 검정인 노드를 수정해 주세요.");
					 }
				 }
			 }

	private: System::Void ToolStripMenuItem_ExpandAll_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->treeView_DialogInfo->ExpandAll();
			 }

	private: System::Void ToolStripMenuItem_CollapseAll_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->treeView_DialogInfo->CollapseAll();
			 }

	private: System::Void textBox_FindNPC_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 if (Keys::Enter ==  e->KeyCode)
				 {
					 this->button_FindNPC_Click(nullptr, nullptr);
				 }
			 }
	private: System::Void toolStripMenuItem_ReloadNPC_Click(System::Object^  sender, System::EventArgs^  e) {
				 Windows::Forms::DialogResult dr = Windows::Forms::MessageBox::Show("변경 내용은 저장되지 않고 사라집니다.\n새로 고치시겠습니까? ", "DialogTool", MessageBoxButtons::OKCancel);
				 if (Windows::Forms::DialogResult::OK == dr)
				 {
					 this->m_pDc->ReloadDialog();
				 }
			 }

	private: System::Void toolStripMenuItem_RunQuestTool_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->m_pDc->RunQuestTool();
			 }

	private: System::Void DialogForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 if (false == this->m_pDc->IsEmptyModifiedDialogID())
				 {
					 Windows::Forms::DialogResult dr = Windows::Forms::MessageBox::Show("변경 내용을 저장 하시겠습니까?", "DialogTool", MessageBoxButtons::OKCancel);
					 if (Windows::Forms::DialogResult::OK == dr)
					 {
						 this->m_pDc->SaveDialogToXml();
					 }
				 }
			 }

	private: System::Void toolStripMenuItem_Exit_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }
};
}
