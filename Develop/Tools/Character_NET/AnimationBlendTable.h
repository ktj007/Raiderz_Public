#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// AnimationBlendTable에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class AnimationBlendTable : public System::Windows::Forms::Form
	{
	private:
		String^	m_strSelectAniName;			// 오직 한개만 선택한 애니 이름
	private: System::Windows::Forms::ColumnHeader^  BlendValue_columnHeader;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  animationBlendData_Modify_button;
	private: System::Windows::Forms::Button^  animationBlendData_Reverse_button;


			 bool	m_bSave;

	private:
		void	FindAnimation(System::String^ strAnimation);
		int		FindAnimationItemIndexAnimation(String^ strAnimation);

		ANIMATION_BLEND_LIST * GetAniBlendList(string strAni);
		ANIMATION_BLEND_DATA * GetAniBlendInfo(string strAni, string strBlendAni);
		void	GetSelectSourceAniList(vector<string>& vecSelectAni);
		void	GetSelectBlendAniList(vector<string>& vecBlendAni);

		bool	Show_AnimationBlendInputForm(vector<string>& vecSorceAni, vector<string>& vecOutBlendAni, float& fBlendTime, bool bDisableAniText);
		bool	AddAnimationBlendInfo(string strAniName, string strBlendAni, float fBlendTime);
		bool	DelAnimationBlendInfo(string strAniName, string strBlendAni);

		void	Load_AnimationList();
		void	Load_AnimationBlendList();

		void	ReLoad_AnimationList();
		void	ReSetCurrentSelectListView();

	public:
		void	InitSetAnimationInfo(String^ strAni);

		AnimationBlendTable(void)
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
		~AnimationBlendTable()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 
	private: System::Windows::Forms::Label^  ModelName_label;




	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ListView^  BlendAnimation_listView;
	private: System::Windows::Forms::ColumnHeader^  BlendAnimation_columnHeader;




	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::ListView^  Animation_listView;
	private: System::Windows::Forms::ColumnHeader^  AniName_columnHeader;
	private: System::Windows::Forms::ColumnHeader^  BlendCount_columnHeader;

	private: System::Windows::Forms::TextBox^  SearchAnimation_textBox;
	private: System::Windows::Forms::Button^  Finish_button;
	private: System::Windows::Forms::Button^  AddAnimationBlend_button;



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
			this->ModelName_label = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->animationBlendData_Reverse_button = (gcnew System::Windows::Forms::Button());
			this->animationBlendData_Modify_button = (gcnew System::Windows::Forms::Button());
			this->BlendAnimation_listView = (gcnew System::Windows::Forms::ListView());
			this->BlendAnimation_columnHeader = (gcnew System::Windows::Forms::ColumnHeader());
			this->BlendValue_columnHeader = (gcnew System::Windows::Forms::ColumnHeader());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->Animation_listView = (gcnew System::Windows::Forms::ListView());
			this->AniName_columnHeader = (gcnew System::Windows::Forms::ColumnHeader());
			this->BlendCount_columnHeader = (gcnew System::Windows::Forms::ColumnHeader());
			this->SearchAnimation_textBox = (gcnew System::Windows::Forms::TextBox());
			this->Finish_button = (gcnew System::Windows::Forms::Button());
			this->AddAnimationBlend_button = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox2->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// ModelName_label
			// 
			this->ModelName_label->AutoSize = true;
			this->ModelName_label->Font = (gcnew System::Drawing::Font(L"굴림", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->ModelName_label->Location = System::Drawing::Point(41, 19);
			this->ModelName_label->Name = L"ModelName_label";
			this->ModelName_label->Size = System::Drawing::Size(0, 16);
			this->ModelName_label->TabIndex = 1;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->animationBlendData_Reverse_button);
			this->groupBox2->Controls->Add(this->animationBlendData_Modify_button);
			this->groupBox2->Controls->Add(this->BlendAnimation_listView);
			this->groupBox2->Location = System::Drawing::Point(239, 6);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(216, 367);
			this->groupBox2->TabIndex = 10;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"블랜딩 애니메이션";
			// 
			// animationBlendData_Reverse_button
			// 
			this->animationBlendData_Reverse_button->Location = System::Drawing::Point(138, 19);
			this->animationBlendData_Reverse_button->Name = L"animationBlendData_Reverse_button";
			this->animationBlendData_Reverse_button->Size = System::Drawing::Size(30, 23);
			this->animationBlendData_Reverse_button->TabIndex = 9;
			this->animationBlendData_Reverse_button->Text = L"←";
			this->animationBlendData_Reverse_button->UseVisualStyleBackColor = true;
			this->animationBlendData_Reverse_button->Click += gcnew System::EventHandler(this, &AnimationBlendTable::animationBlendData_Reverse_button_Click);
			// 
			// animationBlendData_Modify_button
			// 
			this->animationBlendData_Modify_button->Location = System::Drawing::Point(173, 20);
			this->animationBlendData_Modify_button->Name = L"animationBlendData_Modify_button";
			this->animationBlendData_Modify_button->Size = System::Drawing::Size(37, 23);
			this->animationBlendData_Modify_button->TabIndex = 8;
			this->animationBlendData_Modify_button->Text = L"수정";
			this->animationBlendData_Modify_button->UseVisualStyleBackColor = true;
			this->animationBlendData_Modify_button->Click += gcnew System::EventHandler(this, &AnimationBlendTable::animationBlendData_Modify_button_Click);
			// 
			// BlendAnimation_listView
			// 
			this->BlendAnimation_listView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->BlendAnimation_columnHeader, 
				this->BlendValue_columnHeader});
			this->BlendAnimation_listView->FullRowSelect = true;
			this->BlendAnimation_listView->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->BlendAnimation_listView->Location = System::Drawing::Point(7, 44);
			this->BlendAnimation_listView->Name = L"BlendAnimation_listView";
			this->BlendAnimation_listView->ShowItemToolTips = true;
			this->BlendAnimation_listView->Size = System::Drawing::Size(203, 315);
			this->BlendAnimation_listView->Sorting = System::Windows::Forms::SortOrder::Ascending;
			this->BlendAnimation_listView->TabIndex = 7;
			this->BlendAnimation_listView->UseCompatibleStateImageBehavior = false;
			this->BlendAnimation_listView->View = System::Windows::Forms::View::Details;
			this->BlendAnimation_listView->SelectedIndexChanged += gcnew System::EventHandler(this, &AnimationBlendTable::BlendAnimation_listView_SelectedIndexChanged);
			this->BlendAnimation_listView->DoubleClick += gcnew System::EventHandler(this, &AnimationBlendTable::BlendAnimation_listView_DoubleClick);
			this->BlendAnimation_listView->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &AnimationBlendTable::BlendAnimation_listView_KeyUp);
			// 
			// BlendAnimation_columnHeader
			// 
			this->BlendAnimation_columnHeader->Width = 100;
			// 
			// BlendValue_columnHeader
			// 
			this->BlendValue_columnHeader->Width = 80;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->Animation_listView);
			this->groupBox1->Controls->Add(this->SearchAnimation_textBox);
			this->groupBox1->Location = System::Drawing::Point(3, 6);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(219, 368);
			this->groupBox1->TabIndex = 9;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"애니메이션";
			// 
			// Animation_listView
			// 
			this->Animation_listView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->AniName_columnHeader, 
				this->BlendCount_columnHeader});
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
			this->Animation_listView->SelectedIndexChanged += gcnew System::EventHandler(this, &AnimationBlendTable::Animation_listView_SelectedIndexChanged);
			this->Animation_listView->DoubleClick += gcnew System::EventHandler(this, &AnimationBlendTable::Animation_listView_DoubleClick);
			// 
			// AniName_columnHeader
			// 
			this->AniName_columnHeader->Text = L"";
			this->AniName_columnHeader->Width = 150;
			// 
			// BlendCount_columnHeader
			// 
			this->BlendCount_columnHeader->Text = L"";
			this->BlendCount_columnHeader->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->BlendCount_columnHeader->Width = 34;
			// 
			// SearchAnimation_textBox
			// 
			this->SearchAnimation_textBox->Location = System::Drawing::Point(6, 19);
			this->SearchAnimation_textBox->Name = L"SearchAnimation_textBox";
			this->SearchAnimation_textBox->Size = System::Drawing::Size(204, 21);
			this->SearchAnimation_textBox->TabIndex = 2;
			this->SearchAnimation_textBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &AnimationBlendTable::SearchAnimation_textBox_KeyDown);
			// 
			// Finish_button
			// 
			this->Finish_button->Location = System::Drawing::Point(193, 420);
			this->Finish_button->Name = L"Finish_button";
			this->Finish_button->Size = System::Drawing::Size(75, 23);
			this->Finish_button->TabIndex = 8;
			this->Finish_button->Text = L"완료";
			this->Finish_button->UseVisualStyleBackColor = true;
			this->Finish_button->Click += gcnew System::EventHandler(this, &AnimationBlendTable::Finish_button_Click);
			// 
			// AddAnimationBlend_button
			// 
			this->AddAnimationBlend_button->Font = (gcnew System::Drawing::Font(L"굴림", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->AddAnimationBlend_button->Location = System::Drawing::Point(217, 191);
			this->AddAnimationBlend_button->Name = L"AddAnimationBlend_button";
			this->AddAnimationBlend_button->Size = System::Drawing::Size(26, 23);
			this->AddAnimationBlend_button->TabIndex = 11;
			this->AddAnimationBlend_button->Text = L"╋";
			this->AddAnimationBlend_button->UseVisualStyleBackColor = true;
			this->AddAnimationBlend_button->Click += gcnew System::EventHandler(this, &AnimationBlendTable::AddAnimationBlend_button_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->ForeColor = System::Drawing::Color::Red;
			this->label1->Location = System::Drawing::Point(3, 383);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(373, 12);
			this->label1->TabIndex = 12;
			this->label1->Text = L"주의 : 모든 애니메이션에는 블렌딩을 하지 않습니다.(2020.7.10부터)";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->ForeColor = System::Drawing::Color::Red;
			this->label2->Location = System::Drawing::Point(38, 401);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(297, 12);
			this->label2->TabIndex = 13;
			this->label2->Text = L"블렌딩을 하기 위해서는 설정을 꼭 해주시기 바랍니다.";
			// 
			// AnimationBlendTable
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(458, 452);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->AddAnimationBlend_button);
			this->Controls->Add(this->Finish_button);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->ModelName_label);
			this->Name = L"AnimationBlendTable";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"AnimationBlendTable";
			this->Load += gcnew System::EventHandler(this, &AnimationBlendTable::AnimationBlendTable_Load);
			this->groupBox2->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void AnimationBlendTable_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void Finish_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void AddAnimationBlend_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Animation_listView_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 Load_AnimationBlendList();
		 }
private: System::Void SearchAnimation_textBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		 	 if(e->KeyCode == Keys::Enter)
			 {
				 FindAnimation(SearchAnimation_textBox->Text);
			 }
		 }
private: System::Void BlendAnimation_listView_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
private: System::Void BlendAnimation_listView_DoubleClick(System::Object^  sender, System::EventArgs^  e);
private: System::Void Animation_listView_DoubleClick(System::Object^  sender, System::EventArgs^  e);
private: System::Void animationBlendData_Modify_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void animationBlendData_Reverse_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void BlendAnimation_listView_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
};
}
