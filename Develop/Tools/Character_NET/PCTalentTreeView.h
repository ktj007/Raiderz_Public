#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

struct TALENT_ANIMATION_INFO;

namespace Character_NET {


	/// <summary>
	/// PCTalentTreeView에 대한 요약입니다.
	/// </summary>
	public ref class PCTalentTreeView : public System::Windows::Forms::UserControl
	{
	public:
		PCTalentTreeView(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}
		void FindTalent(System::String^ Talent);
		void ReLoadPCTalentInfo();

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~PCTalentTreeView()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  FindTalentNametextBox;

	private: System::Windows::Forms::Label^  label3;
	public: System::Windows::Forms::TreeView^  treeView_TalentAnimationList;
	private: System::Windows::Forms::GroupBox^  TalentGroup;
	public: System::Windows::Forms::TreeView^  treeView1;
	private: 
	public: 

	public: 

	public: 
	private: 



			 System::Windows::Forms::TreeView^ get() { return treeView1; }
			 void set(System::Windows::Forms::TreeView^ val) { treeView1 = val; }
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::ComboBox^  comboBox2;
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
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->FindTalentNametextBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->treeView_TalentAnimationList = (gcnew System::Windows::Forms::TreeView());
			this->TalentGroup = (gcnew System::Windows::Forms::GroupBox());
			this->TalentGroup->SuspendLayout();
			this->SuspendLayout();
			// 
			// treeView1
			// 
			this->treeView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeView1->HideSelection = false;
			this->treeView1->Location = System::Drawing::Point(0, 20);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(198, 359);
			this->treeView1->TabIndex = 0;
			this->treeView1->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &PCTalentTreeView::treeView1_NodeMouseClick);
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(74, 3);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(121, 20);
			this->comboBox1->TabIndex = 1;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &PCTalentTreeView::comboBox1_SelectedIndexChanged);
			// 
			// comboBox2
			// 
			this->comboBox2->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox2->FormattingEnabled = true;
			this->comboBox2->Location = System::Drawing::Point(74, 29);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(121, 20);
			this->comboBox2->TabIndex = 2;
			this->comboBox2->SelectedIndexChanged += gcnew System::EventHandler(this, &PCTalentTreeView::comboBox2_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(15, 6);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(37, 12);
			this->label1->TabIndex = 3;
			this->label1->Text = L"성별 :";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(15, 32);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(37, 12);
			this->label2->TabIndex = 4;
			this->label2->Text = L"무기 :";
			// 
			// FindTalentNametextBox
			// 
			this->FindTalentNametextBox->Location = System::Drawing::Point(82, 55);
			this->FindTalentNametextBox->Name = L"FindTalentNametextBox";
			this->FindTalentNametextBox->Size = System::Drawing::Size(100, 21);
			this->FindTalentNametextBox->TabIndex = 5;
			this->FindTalentNametextBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &PCTalentTreeView::FindTalentNametextBox_KeyDown);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(3, 58);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(73, 12);
			this->label3->TabIndex = 6;
			this->label3->Text = L"탈렌트검색 :";
			// 
			// treeView_TalentAnimationList
			// 
			this->treeView_TalentAnimationList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeView_TalentAnimationList->Location = System::Drawing::Point(0, 474);
			this->treeView_TalentAnimationList->Name = L"treeView_TalentAnimationList";
			this->treeView_TalentAnimationList->Size = System::Drawing::Size(198, 99);
			this->treeView_TalentAnimationList->TabIndex = 7;
			// 
			// TalentGroup
			// 
			this->TalentGroup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TalentGroup->Controls->Add(this->treeView1);
			this->TalentGroup->Location = System::Drawing::Point(0, 89);
			this->TalentGroup->Name = L"TalentGroup";
			this->TalentGroup->Size = System::Drawing::Size(197, 382);
			this->TalentGroup->TabIndex = 8;
			this->TalentGroup->TabStop = false;
			this->TalentGroup->Text = L"Talent";
			// 
			// PCTalentTreeView
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->TalentGroup);
			this->Controls->Add(this->treeView_TalentAnimationList);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->FindTalentNametextBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->comboBox2);
			this->Controls->Add(this->comboBox1);
			this->Name = L"PCTalentTreeView";
			this->Size = System::Drawing::Size(198, 573);
			this->Load += gcnew System::EventHandler(this, &PCTalentTreeView::PCTalentTreeView_Load);
			this->TalentGroup->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	public: System::Void TreeInfoClear();
	private: System::Void PCTalentTreeClear();
	private: System::Void InitPCTalentTree();
	private: System::Void InitTalentMode( CSTalentInfo* pTalentInfo, TreeNode^ pTalentNode );
	private: System::Void treeView1_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e);
private: System::Void PCTalentTreeView_Load(System::Object^  sender, System::EventArgs^  e) {
			 //for( int i = 0;i < SEX_MAX-1; ++i)
			 //{
				// String^ str = gcnew String(TStrings::SexTypeToString(SEX(i)));
				// this->comboBox1->Items->Add(str);
			 //}
			 this->comboBox1->Items->Add("남자");
			 this->comboBox1->Items->Add("여자");

			 Init();
			
			 for( int i = WEAPON_1H_SLASH;i < WEAPON_MAX; ++i)
			 {
				 String^ str = gcnew String(TStrings::WeaponType(i));
				 this->comboBox2->Items->Add(str);
			 }

			 this->comboBox2->SelectedIndex = 0;

			 TreeInfoClear();
		 }
	 public: void PlayerModelLoad(SEX nIndex)
			 {
				 string NextSex = TStrings::SexTypeToString(nIndex);
				 if (g_pMainApp->GetActor())
				 {
					 string sex = g_pMainApp->GetActor()->CurrentSexType();
					 if (sex == NextSex) return;
				 }

				 String^ str = gcnew String(NextSex.c_str());
				 g_pMainApp->PlayerModelLoad(str);
			 }
	 public: void Init() {
				 int nSelectIndex = SEX_MALE;

				 if(g_pMainApp->IsDefaultPlayerModelLoad())
				 {
					 nSelectIndex = SEX_MALE;
				 }
				 else
				 {
					 string sex = g_pMainApp->GetActor()->CurrentSexType();
					 nSelectIndex = TStrings::StringToSexType(sex.c_str());
					 if(nSelectIndex == -1)
						nSelectIndex = SEX_MALE;
					 this->comboBox1->SelectedIndex = nSelectIndex;
				 }

				 if(nSelectIndex == this->comboBox1->SelectedIndex)
				 {
					 PlayerModelLoad(SEX(nSelectIndex));
					 return;
				 }

				 this->comboBox1->SelectedIndex = nSelectIndex;

				 g_pMainApp->m_NPC.SetNPCID(0);
			 }
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 int index = ((System::Windows::Forms::ComboBox^)sender)->SelectedIndex;
			 PlayerModelLoad(SEX(index));
		 }
private: System::Void comboBox2_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 int index = ((System::Windows::Forms::ComboBox^)sender)->SelectedIndex + WEAPON_1H_SLASH;			

			 if(IsEqualWeapon(index)) return;

			 ChangeWeapon(WEAPON_TYPE(index));
		 }

	public: bool IsEqualWeapon( int index )
		 {
			 WEAPON_TYPE weapon_type = WEAPON_NONE;
			 weapon_type = g_pMainApp->GetActor()->GetCurrentWeaponType();
			 //string str = TStrings::WeaponType(weapon_type);
			 if (weapon_type == WEAPON_TYPE(index)) return true;

			 return false;
		 }
	 public: void ChangeWeapon(WEAPON_TYPE nWeaponType) {
				 CSItemData* pItemData = GetItemDataByWeaponType(nWeaponType);
				 if(pItemData == NULL) return;

				 g_pMainApp->SelectItemInItemList(pItemData->m_nItemSlot);
				 bool bEquipItem = g_pMainApp->m_EquipItems.EquipItemID(pItemData->m_nID, pItemData->m_nItemSlot);
			 }
private: CSItemData* GetItemDataByWeaponType(WEAPON_TYPE weapontype) {
			 TItemManager* pItemMgr = g_pMainApp->GetItemMgr();
			 TItemManager::CSItemDataMap& dataMap = pItemMgr->GetDataMap();
			 for(TItemManager::CSItemDataMap::iterator itr = dataMap.begin();itr!=dataMap.end();itr++)
			 {
				 CSItemData* data = itr->second;
				 if( data != NULL &&
					 data->m_nWeaponType == weapontype)
					 return data;
			 }

			 return NULL;
		 }
	public: void SetWeapon(int nTalentID, int nTalentMode);
	private: void FindTalentName(System::String^ TalentText)
			 {
				 if (TalentText != "")
				 {
					 FindTalent(TalentText);
				 }
				 else
					 TreeInfoClear();
			 }
	private: System::Void InitPCTalentAnimationListTree(int nTalentID, int nTalentMode);
	private: System::Void SetPCTalentAnimationList(TALENT_ANIMATION_INFO& talentAnimationInfo, TreeNode^ tn);
private: System::Void FindTalentNametextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if(e->KeyCode == Keys::Enter)
			 {
				 FindTalentName(((TextBox^)sender)->Text);
			 }
		 }
};
}
