#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// ItemTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class ItemTabPage : public System::Windows::Forms::Form
	{
	public:
		ItemTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

		void InitItemList();

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~ItemTabPage()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  ItemInfolistBox;
	protected: 
	public: System::Windows::Forms::TreeView^  ItemtreeView;
	private: 

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
			this->ItemInfolistBox = (gcnew System::Windows::Forms::ListBox());
			this->ItemtreeView = (gcnew System::Windows::Forms::TreeView());
			this->SuspendLayout();
			// 
			// ItemInfolistBox
			// 
			this->ItemInfolistBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->ItemInfolistBox->FormattingEnabled = true;
			this->ItemInfolistBox->ItemHeight = 12;
			this->ItemInfolistBox->Location = System::Drawing::Point(6, 454);
			this->ItemInfolistBox->Name = L"ItemInfolistBox";
			this->ItemInfolistBox->Size = System::Drawing::Size(251, 52);
			this->ItemInfolistBox->TabIndex = 5;
			// 
			// ItemtreeView
			// 
			this->ItemtreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->ItemtreeView->HideSelection = false;
			this->ItemtreeView->Location = System::Drawing::Point(4, 18);
			this->ItemtreeView->Name = L"ItemtreeView";
			this->ItemtreeView->Size = System::Drawing::Size(254, 425);
			this->ItemtreeView->TabIndex = 4;
			this->ItemtreeView->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &ItemTabPage::ItemtreeView_NodeMouseClick);
			// 
			// ItemTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(262, 524);
			this->Controls->Add(this->ItemInfolistBox);
			this->Controls->Add(this->ItemtreeView);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"ItemTabPage";
			this->Text = L"ItemTabPage";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void ItemtreeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) {
				 if( e->Node->Parent == nullptr ) return;

				 Object^ ptag = e->Node->Tag;
				 int itemid;
				 if( ptag!= nullptr)
					 itemid = (int)ptag;

				 g_pMainApp->SelectItemInItemList(e->Node->Parent->Index);
				 bool bEquipItem = g_pMainApp->m_EquipItems.EquipItemID(itemid, e->Node->Parent->Index);

				 if( bEquipItem)
					 ItemInfolistBox->Items->Add(e->Node->Text);
				 else
					 ItemInfolistBox->Items->Add("장착실패");

				 if( ptag!= nullptr)
				 {
					 if(g_pMainApp->GetActor())
					 {
						 if( bEquipItem )	// 장착성공 일때만 들어온다
						 {
							 // 현재 선택한 BaseMesh의 재질 리스트를 읽어 들인다.
							 g_pMainApp->m_Mtrl.SetCurrentMaterialList(itemid);
							 g_pMainApp->m_Mtrl.SetItemDataSubMtrl(itemid);
							 g_pMainApp->m_Mtrl.SetMeshTreeView();
							 g_pMainApp->m_Mtrl.SetMtrlList();
						 }
					 }
				 }
			 }
	};
}
