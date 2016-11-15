#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// MeshInfoTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class MeshInfoTabPage : public System::Windows::Forms::Form
	{
	protected:
		void	SetMeshInfoData(System::String^ ModelName);

	public:
		void	SetMeshInfo(System::String^ ModelName);

	public:
		MeshInfoTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~MeshInfoTabPage()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::Label^  ModelName_label;
	protected: 
	private: System::Windows::Forms::TabControl^  tabControl1;
	public: 
	private: System::Windows::Forms::TabPage^  MeshBaseInfo_tabPage;
	private: System::Windows::Forms::PropertyGrid^  MeshInfo_propertyGrid;


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
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->MeshBaseInfo_tabPage = (gcnew System::Windows::Forms::TabPage());
			this->MeshInfo_propertyGrid = (gcnew System::Windows::Forms::PropertyGrid());
			this->tabControl1->SuspendLayout();
			this->MeshBaseInfo_tabPage->SuspendLayout();
			this->SuspendLayout();
			// 
			// ModelName_label
			// 
			this->ModelName_label->BackColor = System::Drawing::SystemColors::Window;
			this->ModelName_label->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->ModelName_label->Location = System::Drawing::Point(12, 9);
			this->ModelName_label->Name = L"ModelName_label";
			this->ModelName_label->Size = System::Drawing::Size(116, 23);
			this->ModelName_label->TabIndex = 0;
			this->ModelName_label->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->MeshBaseInfo_tabPage);
			this->tabControl1->Location = System::Drawing::Point(4, 41);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(267, 590);
			this->tabControl1->TabIndex = 1;
			// 
			// MeshBaseInfo_tabPage
			// 
			this->MeshBaseInfo_tabPage->Controls->Add(this->MeshInfo_propertyGrid);
			this->MeshBaseInfo_tabPage->Location = System::Drawing::Point(4, 21);
			this->MeshBaseInfo_tabPage->Name = L"MeshBaseInfo_tabPage";
			this->MeshBaseInfo_tabPage->Padding = System::Windows::Forms::Padding(3);
			this->MeshBaseInfo_tabPage->Size = System::Drawing::Size(259, 565);
			this->MeshBaseInfo_tabPage->TabIndex = 0;
			this->MeshBaseInfo_tabPage->Text = L"메쉬정보";
			this->MeshBaseInfo_tabPage->UseVisualStyleBackColor = true;
			// 
			// MeshInfo_propertyGrid
			// 
			this->MeshInfo_propertyGrid->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->MeshInfo_propertyGrid->Location = System::Drawing::Point(5, 10);
			this->MeshInfo_propertyGrid->Name = L"MeshInfo_propertyGrid";
			this->MeshInfo_propertyGrid->PropertySort = System::Windows::Forms::PropertySort::Categorized;
			this->MeshInfo_propertyGrid->Size = System::Drawing::Size(248, 549);
			this->MeshInfo_propertyGrid->TabIndex = 0;
			this->MeshInfo_propertyGrid->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &MeshInfoTabPage::MeshInfo_propertyGrid_PropertyValueChanged);
			// 
			// MeshInfoTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(276, 638);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->ModelName_label);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"MeshInfoTabPage";
			this->Text = L"MeshInfoTabPage";
			this->tabControl1->ResumeLayout(false);
			this->MeshBaseInfo_tabPage->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void MeshInfo_propertyGrid_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e);
};
}
