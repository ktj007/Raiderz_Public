#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// OverlayAnimationTable에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class OverlayAnimationTable : public System::Windows::Forms::Form
	{
	private:
		String^	m_strSelectAniName;			// 오직 한개만 선택한 애니 이름
		bool	m_bSave;

	private:
		void	FindAnimation(System::String^ strAnimation);
		int		FindAnimationItemIndexAnimation(String^ strAnimation);

		OVERLAY_ANIMATION_LIST * GetOverlayAniList(string strAni);
		OVERLAY_ANIMATION_INFO * GetOverlayAniInfo(string strAni, string strOverlayAni, string strOverlayAniBone);

		bool	Show_OverlayAnimationInputForm(string& strOutOverlayAni, string& strOutOverlayAniBone);
		bool	AddOverlayAnimationInfo(string strAniName, string strOverlayAni, string strOverlayAniBone);
		bool	DelOverlayAnimationInfo(string strAniName, string strOverlayAni, string strOverlayAniBone);
			
		void	Load_AnimationList();
		void	Load_OverlayAniList();

	public:
		void	InitSetAnimationInfo(String^ strAni);

		OverlayAnimationTable(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//

			m_bSave = false;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~OverlayAnimationTable()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 

	private: System::Windows::Forms::TextBox^  SearchAnimation_textBox;
	private: System::Windows::Forms::Button^  Finish_button;
	private: System::Windows::Forms::ListView^  Animation_listView;
	private: System::Windows::Forms::ColumnHeader^  AniName_columnHeader;
	private: System::Windows::Forms::ColumnHeader^  OverlayCount_columnHeader;
	private: System::Windows::Forms::Button^  AddOverlay_button;




	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ListView^  OverlayAnimation_listView;
	private: System::Windows::Forms::ColumnHeader^  OverlayAnimation_columnHeader;
	private: System::Windows::Forms::ColumnHeader^  BoneName_columnHeader;


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
			this->SearchAnimation_textBox = (gcnew System::Windows::Forms::TextBox());
			this->Finish_button = (gcnew System::Windows::Forms::Button());
			this->Animation_listView = (gcnew System::Windows::Forms::ListView());
			this->AniName_columnHeader = (gcnew System::Windows::Forms::ColumnHeader());
			this->OverlayCount_columnHeader = (gcnew System::Windows::Forms::ColumnHeader());
			this->AddOverlay_button = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->OverlayAnimation_listView = (gcnew System::Windows::Forms::ListView());
			this->OverlayAnimation_columnHeader = (gcnew System::Windows::Forms::ColumnHeader());
			this->BoneName_columnHeader = (gcnew System::Windows::Forms::ColumnHeader());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// SearchAnimation_textBox
			// 
			this->SearchAnimation_textBox->Location = System::Drawing::Point(6, 19);
			this->SearchAnimation_textBox->Name = L"SearchAnimation_textBox";
			this->SearchAnimation_textBox->Size = System::Drawing::Size(204, 21);
			this->SearchAnimation_textBox->TabIndex = 2;
			this->SearchAnimation_textBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &OverlayAnimationTable::SearchAnimation_textBox_KeyDown);
			// 
			// Finish_button
			// 
			this->Finish_button->Location = System::Drawing::Point(193, 383);
			this->Finish_button->Name = L"Finish_button";
			this->Finish_button->Size = System::Drawing::Size(75, 23);
			this->Finish_button->TabIndex = 3;
			this->Finish_button->Text = L"완료";
			this->Finish_button->UseVisualStyleBackColor = true;
			this->Finish_button->Click += gcnew System::EventHandler(this, &OverlayAnimationTable::Finish_button_Click);
			// 
			// Animation_listView
			// 
			this->Animation_listView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->AniName_columnHeader, 
				this->OverlayCount_columnHeader});
			this->Animation_listView->FullRowSelect = true;
			this->Animation_listView->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->Animation_listView->HideSelection = false;
			this->Animation_listView->Location = System::Drawing::Point(6, 44);
			this->Animation_listView->Name = L"Animation_listView";
			this->Animation_listView->ShowItemToolTips = true;
			this->Animation_listView->Size = System::Drawing::Size(205, 316);
			this->Animation_listView->Sorting = System::Windows::Forms::SortOrder::Ascending;
			this->Animation_listView->TabIndex = 4;
			this->Animation_listView->UseCompatibleStateImageBehavior = false;
			this->Animation_listView->View = System::Windows::Forms::View::Details;
			this->Animation_listView->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &OverlayAnimationTable::Animation_listView_MouseDoubleClick);
			this->Animation_listView->SelectedIndexChanged += gcnew System::EventHandler(this, &OverlayAnimationTable::Animation_listView_SelectedIndexChanged);
			// 
			// AniName_columnHeader
			// 
			this->AniName_columnHeader->Text = L"";
			this->AniName_columnHeader->Width = 150;
			// 
			// OverlayCount_columnHeader
			// 
			this->OverlayCount_columnHeader->Text = L"";
			this->OverlayCount_columnHeader->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->OverlayCount_columnHeader->Width = 34;
			// 
			// AddOverlay_button
			// 
			this->AddOverlay_button->Font = (gcnew System::Drawing::Font(L"굴림", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->AddOverlay_button->Location = System::Drawing::Point(217, 191);
			this->AddOverlay_button->Name = L"AddOverlay_button";
			this->AddOverlay_button->Size = System::Drawing::Size(26, 23);
			this->AddOverlay_button->TabIndex = 5;
			this->AddOverlay_button->Text = L"╋";
			this->AddOverlay_button->UseVisualStyleBackColor = true;
			this->AddOverlay_button->Click += gcnew System::EventHandler(this, &OverlayAnimationTable::AddOverlay_button_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->Animation_listView);
			this->groupBox1->Controls->Add(this->SearchAnimation_textBox);
			this->groupBox1->Location = System::Drawing::Point(3, 6);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(219, 368);
			this->groupBox1->TabIndex = 7;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"애니메이션";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->OverlayAnimation_listView);
			this->groupBox2->Location = System::Drawing::Point(239, 6);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(216, 367);
			this->groupBox2->TabIndex = 8;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"오버레이 애니메이션";
			// 
			// OverlayAnimation_listView
			// 
			this->OverlayAnimation_listView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->OverlayAnimation_columnHeader, 
				this->BoneName_columnHeader});
			this->OverlayAnimation_listView->FullRowSelect = true;
			this->OverlayAnimation_listView->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->OverlayAnimation_listView->Location = System::Drawing::Point(7, 44);
			this->OverlayAnimation_listView->Name = L"OverlayAnimation_listView";
			this->OverlayAnimation_listView->ShowItemToolTips = true;
			this->OverlayAnimation_listView->Size = System::Drawing::Size(203, 315);
			this->OverlayAnimation_listView->Sorting = System::Windows::Forms::SortOrder::Ascending;
			this->OverlayAnimation_listView->TabIndex = 7;
			this->OverlayAnimation_listView->UseCompatibleStateImageBehavior = false;
			this->OverlayAnimation_listView->View = System::Windows::Forms::View::Details;
			this->OverlayAnimation_listView->DoubleClick += gcnew System::EventHandler(this, &OverlayAnimationTable::OverlayAnimation_listView_DoubleClick);
			this->OverlayAnimation_listView->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &OverlayAnimationTable::OverlayAnimation_listView_KeyUp);
			// 
			// OverlayAnimation_columnHeader
			// 
			this->OverlayAnimation_columnHeader->Width = 111;
			// 
			// BoneName_columnHeader
			// 
			this->BoneName_columnHeader->Width = 77;
			// 
			// OverlayAnimationTable
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(458, 414);
			this->Controls->Add(this->AddOverlay_button);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->Finish_button);
			this->Name = L"OverlayAnimationTable";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"OverlayAnimationTable";
			this->Load += gcnew System::EventHandler(this, &OverlayAnimationTable::OverlayAnimationTable_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void SearchAnimation_textBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 if(e->KeyCode == Keys::Enter)
				 {
					 FindAnimation(SearchAnimation_textBox->Text);
				 }
			 }
	private: System::Void OverlayAnimationTable_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Finish_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Animation_listView_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 Load_OverlayAniList();
		 }
private: System::Void OverlayAnimation_listView_DoubleClick(System::Object^  sender, System::EventArgs^  e);
private: System::Void Animation_listView_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e){
			 ListView::SelectedListViewItemCollection^ selectOne = Animation_listView->SelectedItems;
			 System::Collections::IEnumerator^ myEnum = selectOne->GetEnumerator();
			 myEnum->MoveNext();
			 ListViewItem^ item = safe_cast<ListViewItem^>(myEnum->Current);
			 
			 string strAniName = MStringToCharPointer(item->Text);
			 string strOverlayAniName;
			 string strOverlayAniBone;
			 if(Show_OverlayAnimationInputForm(strOverlayAniName, strOverlayAniBone))
			 {				
				 if(AddOverlayAnimationInfo(strAniName, strOverlayAniName, strOverlayAniBone))
				 {
					 m_bSave = true;

					 // 갱신
					 Load_OverlayAniList();
					 Load_AnimationList();
				 }
			 }
		 }
private: System::Void AddOverlay_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void OverlayAnimation_listView_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
};
}
