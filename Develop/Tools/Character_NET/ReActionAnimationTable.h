#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// ReActionAnimationTable에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class ReActionAnimationTable : public System::Windows::Forms::Form
	{
	public:
		ReActionAnimationTable(void)
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
		~ReActionAnimationTable()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::DataGridView^  ReActionAniamtion_dataGridView;





	private: System::Windows::Forms::Button^  Cancel_button;
	private: System::Windows::Forms::Button^  modify_button;





	private: System::Windows::Forms::Button^  Del_button;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column1;
	private: System::Windows::Forms::DataGridViewComboBoxColumn^  Column2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column3;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column4;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column6;
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  Column5;




















































	private: System::Windows::Forms::Button^  button1;
































	private:
		void	Load_AnimationList();

		//DataTable^ GetReActionList();

		int 	GetSelectSourceAniList(vector<string>& vecSelectAni);
		String^ GetSelectCellData(int nRow, int nColumn);
		void	GetReActionDataToArray(String^ strAni, array<String^>^ arrayData);
		void	GetSourceAniList(vector<string>& vecAni);
		void	GetSelectRowData(int nRow, ReAction_DATA& reAction_Data);

		void	SetSelectCellData(int nRow, int nColumn, String^ strData);

		bool	CheckReActionDataChange(ReAction_DATA& sourceData, ReAction_DATA& targetData);
		void	CheckEffectiveReActionData(int nRow, ReAction_DATA& checkData);

		void	CopyReActionData();
		void	PasteReActionData();
		void	DelReActionData();

		int		FindAnimationItemIndexAnimation(String^ strAnimation);

	private:
		String^	m_strSelectAniName;			// 오직 한개만 선택한 애니 이름
		bool	m_bSave;


	public:
		void	InitSetAnimationInfo(String^ strAni);

	protected: 

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
			this->ReActionAniamtion_dataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column2 = (gcnew System::Windows::Forms::DataGridViewComboBoxColumn());
			this->Column3 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column4 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column6 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column5 = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->Cancel_button = (gcnew System::Windows::Forms::Button());
			this->modify_button = (gcnew System::Windows::Forms::Button());
			this->Del_button = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ReActionAniamtion_dataGridView))->BeginInit();
			this->SuspendLayout();
			// 
			// ReActionAniamtion_dataGridView
			// 
			this->ReActionAniamtion_dataGridView->AllowUserToAddRows = false;
			this->ReActionAniamtion_dataGridView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->ReActionAniamtion_dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->ReActionAniamtion_dataGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(6) {this->Column1, 
				this->Column2, this->Column3, this->Column4, this->Column6, this->Column5});
			this->ReActionAniamtion_dataGridView->EditMode = System::Windows::Forms::DataGridViewEditMode::EditOnEnter;
			this->ReActionAniamtion_dataGridView->Location = System::Drawing::Point(12, 12);
			this->ReActionAniamtion_dataGridView->Name = L"ReActionAniamtion_dataGridView";
			this->ReActionAniamtion_dataGridView->RowTemplate->Height = 23;
			this->ReActionAniamtion_dataGridView->Size = System::Drawing::Size(726, 322);
			this->ReActionAniamtion_dataGridView->TabIndex = 0;
			this->ReActionAniamtion_dataGridView->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &ReActionAnimationTable::ReActionAniamtion_dataGridView_CellValueChanged);
			this->ReActionAniamtion_dataGridView->CellValidating += gcnew System::Windows::Forms::DataGridViewCellValidatingEventHandler(this, &ReActionAnimationTable::ReActionAniamtion_dataGridView_CellValidating);
			this->ReActionAniamtion_dataGridView->EditingControlShowing += gcnew System::Windows::Forms::DataGridViewEditingControlShowingEventHandler(this, &ReActionAnimationTable::ReActionAniamtion_dataGridView_EditingControlShowing);
			this->ReActionAniamtion_dataGridView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ReActionAnimationTable::ReActionAniamtion_dataGridView_KeyDown);
			this->ReActionAniamtion_dataGridView->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &ReActionAnimationTable::ReActionAniamtion_dataGridView_KeyUp);
			// 
			// Column1
			// 
			this->Column1->FillWeight = 200;
			this->Column1->HeaderText = L"Animation";
			this->Column1->Name = L"Column1";
			this->Column1->ReadOnly = true;
			this->Column1->Width = 200;
			// 
			// Column2
			// 
			this->Column2->FillWeight = 150;
			this->Column2->HeaderText = L"ReAction";
			this->Column2->Name = L"Column2";
			this->Column2->Resizable = System::Windows::Forms::DataGridViewTriState::True;
			this->Column2->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::Automatic;
			this->Column2->Width = 150;
			// 
			// Column3
			// 
			this->Column3->FillWeight = 80;
			this->Column3->HeaderText = L"Weight (0.3)";
			this->Column3->Name = L"Column3";
			this->Column3->Width = 80;
			// 
			// Column4
			// 
			this->Column4->FillWeight = 80;
			this->Column4->HeaderText = L"Speed (3)";
			this->Column4->Name = L"Column4";
			this->Column4->Width = 80;
			// 
			// Column6
			// 
			this->Column6->HeaderText = L"WeaponBoneName";
			this->Column6->Name = L"Column6";
			this->Column6->Resizable = System::Windows::Forms::DataGridViewTriState::True;
			// 
			// Column5
			// 
			this->Column5->FillWeight = 60;
			this->Column5->HeaderText = L"Disable";
			this->Column5->Name = L"Column5";
			this->Column5->Width = 60;
			// 
			// button1
			// 
			this->button1->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button1->Location = System::Drawing::Point(261, 352);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(89, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"완료";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &ReActionAnimationTable::Finish_button_Click);
			// 
			// Cancel_button
			// 
			this->Cancel_button->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->Cancel_button->Location = System::Drawing::Point(364, 352);
			this->Cancel_button->Name = L"Cancel_button";
			this->Cancel_button->Size = System::Drawing::Size(89, 23);
			this->Cancel_button->TabIndex = 2;
			this->Cancel_button->Text = L"취소";
			this->Cancel_button->UseVisualStyleBackColor = true;
			this->Cancel_button->Click += gcnew System::EventHandler(this, &ReActionAnimationTable::Cancel_button_Click);
			// 
			// modify_button
			// 
			this->modify_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->modify_button->Location = System::Drawing::Point(12, 340);
			this->modify_button->Name = L"modify_button";
			this->modify_button->Size = System::Drawing::Size(75, 23);
			this->modify_button->TabIndex = 3;
			this->modify_button->Text = L"수정";
			this->modify_button->UseVisualStyleBackColor = true;
			this->modify_button->Click += gcnew System::EventHandler(this, &ReActionAnimationTable::modify_button_Click);
			// 
			// Del_button
			// 
			this->Del_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Del_button->Location = System::Drawing::Point(93, 340);
			this->Del_button->Name = L"Del_button";
			this->Del_button->Size = System::Drawing::Size(75, 23);
			this->Del_button->TabIndex = 4;
			this->Del_button->Text = L"삭제";
			this->Del_button->UseVisualStyleBackColor = true;
			this->Del_button->Click += gcnew System::EventHandler(this, &ReActionAnimationTable::Del_button_Click);
			// 
			// ReActionAnimationTable
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(750, 387);
			this->Controls->Add(this->Del_button);
			this->Controls->Add(this->modify_button);
			this->Controls->Add(this->Cancel_button);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->ReActionAniamtion_dataGridView);
			this->Name = L"ReActionAnimationTable";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"ReActionAnimationTable";
			this->Load += gcnew System::EventHandler(this, &ReActionAnimationTable::ReActionAnimationTable_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ReActionAniamtion_dataGridView))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void ReActionAnimationTable_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void Finish_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ReActionAniamtion_dataGridView_CellValueChanged(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e);
private: System::Void Cancel_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void modify_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Del_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ReActionAniamtion_dataGridView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
private: System::Void ReActionAniamtion_dataGridView_EditingControlShowing(System::Object^  sender, System::Windows::Forms::DataGridViewEditingControlShowingEventArgs^  e);
private: System::Void ReActionAniamtion_dataGridView_CellValidating(System::Object^  sender, System::Windows::Forms::DataGridViewCellValidatingEventArgs^  e);
private: System::Void ReActionAniamtion_dataGridView_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
};
};
