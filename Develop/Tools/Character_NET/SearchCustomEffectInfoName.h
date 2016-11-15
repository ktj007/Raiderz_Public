#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// SearchCustomEffectInfoName에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class SearchCustomEffectInfoName : public System::Windows::Forms::Form
	{
	public:
		String^			m_strSelectCustoEffectInfoName;

	private:
		void SearchEffectList();
		bool CompareEffectName(string& strSource, string& strTarget);

	public:
		SearchCustomEffectInfoName(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

		void Init();
		void SearchEffectList(string& strSearchText, vector<string>& vecResult);

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~SearchCustomEffectInfoName()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  search_listBox;
	private: System::Windows::Forms::Button^  search_button;
	protected: 

	private: System::Windows::Forms::TextBox^  search_textBox;


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
			this->search_listBox = (gcnew System::Windows::Forms::ListBox());
			this->search_button = (gcnew System::Windows::Forms::Button());
			this->search_textBox = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// search_listBox
			// 
			this->search_listBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->search_listBox->FormattingEnabled = true;
			this->search_listBox->ItemHeight = 12;
			this->search_listBox->Location = System::Drawing::Point(13, 41);
			this->search_listBox->Name = L"search_listBox";
			this->search_listBox->Size = System::Drawing::Size(245, 244);
			this->search_listBox->TabIndex = 0;
			this->search_listBox->DoubleClick += gcnew System::EventHandler(this, &SearchCustomEffectInfoName::search_listBox_DoubleClick);
			// 
			// search_button
			// 
			this->search_button->Location = System::Drawing::Point(206, 8);
			this->search_button->Name = L"search_button";
			this->search_button->Size = System::Drawing::Size(52, 23);
			this->search_button->TabIndex = 1;
			this->search_button->Text = L"검색";
			this->search_button->UseVisualStyleBackColor = true;
			this->search_button->Click += gcnew System::EventHandler(this, &SearchCustomEffectInfoName::search_button_Click);
			// 
			// search_textBox
			// 
			this->search_textBox->Location = System::Drawing::Point(12, 10);
			this->search_textBox->Name = L"search_textBox";
			this->search_textBox->Size = System::Drawing::Size(188, 21);
			this->search_textBox->TabIndex = 2;
			this->search_textBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &SearchCustomEffectInfoName::search_textBox_KeyDown);
			// 
			// SearchCustomEffectInfoName
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(267, 306);
			this->Controls->Add(this->search_textBox);
			this->Controls->Add(this->search_button);
			this->Controls->Add(this->search_listBox);
			this->Name = L"SearchCustomEffectInfoName";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"이펙트(effect.xml) 검색창";
			this->Load += gcnew System::EventHandler(this, &SearchCustomEffectInfoName::SearchCustomEffectInfoName_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void search_textBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 if(e->KeyCode == Keys::Enter)
				 {
					 SearchEffectList();
				 }
			 }
private: System::Void search_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 SearchEffectList();
		 }
private: System::Void SearchCustomEffectInfoName_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void search_listBox_DoubleClick(System::Object^  sender, System::EventArgs^  e);
};
}
