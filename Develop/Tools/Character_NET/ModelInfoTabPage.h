#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// ModelInfoTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class ModelInfoTabPage : public System::Windows::Forms::Form
	{
	public:
		ModelInfoTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}
		void		InitModelInfoTab(const char* FileName);
		int			GetActorNodesPolygonCount();
		unsigned long GetTexMemSize();
		void		NoneSelectedNodeInfo();
		void		SetSelectedNodeInfo(String^ nodeName);
		void		InitMeshNodeInfo();

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~ModelInfoTabPage()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button^  AllNodesSelectbutton;
	protected: 
	private: System::Windows::Forms::Button^  ItemNodesSelectbutton;
	private: System::Windows::Forms::Button^  SkinNodesSelectbutton;
	private: System::Windows::Forms::Button^  NoneSelectBonebutton;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	public: System::Windows::Forms::TextBox^  Mat44textBox;
	private: 
	public: System::Windows::Forms::TextBox^  Mat43textBox;
	public: System::Windows::Forms::TextBox^  Mat42textBox;
	public: System::Windows::Forms::TextBox^  Mat41textBox;
	public: System::Windows::Forms::TextBox^  Mat34textBox;
	public: System::Windows::Forms::TextBox^  Mat33textBox;
	public: System::Windows::Forms::TextBox^  Mat32textBox;
	public: System::Windows::Forms::TextBox^  Mat31textBox;
	public: System::Windows::Forms::TextBox^  Mat24textBox;
	public: System::Windows::Forms::TextBox^  Mat23textBox;
	public: System::Windows::Forms::TextBox^  Mat22textBox;
	public: System::Windows::Forms::TextBox^  Mat21textBox;
	public: System::Windows::Forms::TextBox^  Mat14textBox;
	public: System::Windows::Forms::TextBox^  Mat13textBox;
	public: System::Windows::Forms::TextBox^  Mat12textBox;
	public: System::Windows::Forms::TextBox^  Mat11textBox;
	public: System::Windows::Forms::TextBox^  BipParenttextBox;
	public: System::Windows::Forms::TextBox^  BipNametextBox;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	public: 
	public: System::Windows::Forms::TextBox^  TexMemtextBox;
	private: 
	private: System::Windows::Forms::Label^  label6;
	public: 
	public: System::Windows::Forms::TextBox^  PolygoncounttextBox;
	private: 
	private: System::Windows::Forms::Label^  label5;
	public: 
	public: System::Windows::Forms::TextBox^  BipCounttextBox;
	private: 
	public: System::Windows::Forms::TextBox^  FileNametextBox;
	public: System::Windows::Forms::TreeView^  BiptreeView;
	public: System::Windows::Forms::ComboBox^  MeshNode_comboBox;
	public: 

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
			System::Windows::Forms::Label^  label7;
			System::Windows::Forms::Label^  label4;
			System::Windows::Forms::Label^  label3;
			System::Windows::Forms::Label^  label2;
			System::Windows::Forms::Label^  label1;
			System::Windows::Forms::Label^  label8;
			this->AllNodesSelectbutton = (gcnew System::Windows::Forms::Button());
			this->ItemNodesSelectbutton = (gcnew System::Windows::Forms::Button());
			this->SkinNodesSelectbutton = (gcnew System::Windows::Forms::Button());
			this->NoneSelectBonebutton = (gcnew System::Windows::Forms::Button());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->Mat44textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat43textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat42textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat41textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat34textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat33textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat32textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat31textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat24textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat23textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat22textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat21textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat14textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat13textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat12textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mat11textBox = (gcnew System::Windows::Forms::TextBox());
			this->BipParenttextBox = (gcnew System::Windows::Forms::TextBox());
			this->BipNametextBox = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->TexMemtextBox = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->PolygoncounttextBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->BipCounttextBox = (gcnew System::Windows::Forms::TextBox());
			this->FileNametextBox = (gcnew System::Windows::Forms::TextBox());
			this->BiptreeView = (gcnew System::Windows::Forms::TreeView());
			this->MeshNode_comboBox = (gcnew System::Windows::Forms::ComboBox());
			label7 = (gcnew System::Windows::Forms::Label());
			label4 = (gcnew System::Windows::Forms::Label());
			label3 = (gcnew System::Windows::Forms::Label());
			label2 = (gcnew System::Windows::Forms::Label());
			label1 = (gcnew System::Windows::Forms::Label());
			label8 = (gcnew System::Windows::Forms::Label());
			this->groupBox2->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// label7
			// 
			label7->AutoSize = true;
			label7->Location = System::Drawing::Point(6, 66);
			label7->Name = L"label7";
			label7->Size = System::Drawing::Size(105, 12);
			label7->TabIndex = 8;
			label7->Text = L"Matrix - OffsetTM";
			// 
			// label4
			// 
			label4->AutoSize = true;
			label4->Location = System::Drawing::Point(6, 39);
			label4->Name = L"label4";
			label4->Size = System::Drawing::Size(49, 12);
			label4->TabIndex = 1;
			label4->Text = L"본 부모:";
			// 
			// label3
			// 
			label3->AutoSize = true;
			label3->Location = System::Drawing::Point(6, 17);
			label3->Name = L"label3";
			label3->Size = System::Drawing::Size(49, 12);
			label3->TabIndex = 0;
			label3->Text = L"본 이름:";
			// 
			// label2
			// 
			label2->AutoSize = true;
			label2->Location = System::Drawing::Point(10, 49);
			label2->Name = L"label2";
			label2->Size = System::Drawing::Size(57, 12);
			label2->TabIndex = 2;
			label2->Text = L"뼈대총수:";
			// 
			// label1
			// 
			label1->AutoSize = true;
			label1->Location = System::Drawing::Point(10, 24);
			label1->Name = L"label1";
			label1->Size = System::Drawing::Size(57, 12);
			label1->TabIndex = 0;
			label1->Text = L"파일이름:";
			// 
			// label8
			// 
			label8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			label8->AutoSize = true;
			label8->Location = System::Drawing::Point(14, 227);
			label8->Name = L"label8";
			label8->Size = System::Drawing::Size(57, 12);
			label8->TabIndex = 8;
			label8->Text = L"메쉬노드:";
			// 
			// AllNodesSelectbutton
			// 
			this->AllNodesSelectbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->AllNodesSelectbutton->Location = System::Drawing::Point(6, 192);
			this->AllNodesSelectbutton->Name = L"AllNodesSelectbutton";
			this->AllNodesSelectbutton->Size = System::Drawing::Size(46, 23);
			this->AllNodesSelectbutton->TabIndex = 18;
			this->AllNodesSelectbutton->Text = L"All";
			this->AllNodesSelectbutton->UseVisualStyleBackColor = true;
			this->AllNodesSelectbutton->Click += gcnew System::EventHandler(this, &ModelInfoTabPage::AllNodesSelectbutton_Click);
			// 
			// ItemNodesSelectbutton
			// 
			this->ItemNodesSelectbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ItemNodesSelectbutton->Location = System::Drawing::Point(110, 192);
			this->ItemNodesSelectbutton->Name = L"ItemNodesSelectbutton";
			this->ItemNodesSelectbutton->Size = System::Drawing::Size(41, 23);
			this->ItemNodesSelectbutton->TabIndex = 17;
			this->ItemNodesSelectbutton->Text = L"Item";
			this->ItemNodesSelectbutton->UseVisualStyleBackColor = true;
			this->ItemNodesSelectbutton->Click += gcnew System::EventHandler(this, &ModelInfoTabPage::ItemNodesSelectbutton_Click);
			// 
			// SkinNodesSelectbutton
			// 
			this->SkinNodesSelectbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SkinNodesSelectbutton->Location = System::Drawing::Point(58, 192);
			this->SkinNodesSelectbutton->Name = L"SkinNodesSelectbutton";
			this->SkinNodesSelectbutton->Size = System::Drawing::Size(46, 23);
			this->SkinNodesSelectbutton->TabIndex = 16;
			this->SkinNodesSelectbutton->Text = L"Skin";
			this->SkinNodesSelectbutton->UseVisualStyleBackColor = true;
			this->SkinNodesSelectbutton->Click += gcnew System::EventHandler(this, &ModelInfoTabPage::SkinNodesSelectbutton_Click);
			// 
			// NoneSelectBonebutton
			// 
			this->NoneSelectBonebutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->NoneSelectBonebutton->Location = System::Drawing::Point(157, 192);
			this->NoneSelectBonebutton->Name = L"NoneSelectBonebutton";
			this->NoneSelectBonebutton->Size = System::Drawing::Size(81, 23);
			this->NoneSelectBonebutton->TabIndex = 15;
			this->NoneSelectBonebutton->Text = L"NoneSelect";
			this->NoneSelectBonebutton->UseVisualStyleBackColor = true;
			this->NoneSelectBonebutton->Click += gcnew System::EventHandler(this, &ModelInfoTabPage::NoneSelectBonebutton_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox2->Controls->Add(this->Mat44textBox);
			this->groupBox2->Controls->Add(this->Mat43textBox);
			this->groupBox2->Controls->Add(this->Mat42textBox);
			this->groupBox2->Controls->Add(this->Mat41textBox);
			this->groupBox2->Controls->Add(this->Mat34textBox);
			this->groupBox2->Controls->Add(this->Mat33textBox);
			this->groupBox2->Controls->Add(this->Mat32textBox);
			this->groupBox2->Controls->Add(this->Mat31textBox);
			this->groupBox2->Controls->Add(this->Mat24textBox);
			this->groupBox2->Controls->Add(this->Mat23textBox);
			this->groupBox2->Controls->Add(this->Mat22textBox);
			this->groupBox2->Controls->Add(this->Mat21textBox);
			this->groupBox2->Controls->Add(this->Mat14textBox);
			this->groupBox2->Controls->Add(this->Mat13textBox);
			this->groupBox2->Controls->Add(this->Mat12textBox);
			this->groupBox2->Controls->Add(this->Mat11textBox);
			this->groupBox2->Controls->Add(label7);
			this->groupBox2->Controls->Add(this->BipParenttextBox);
			this->groupBox2->Controls->Add(this->BipNametextBox);
			this->groupBox2->Controls->Add(label4);
			this->groupBox2->Controls->Add(label3);
			this->groupBox2->Location = System::Drawing::Point(4, 384);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(254, 192);
			this->groupBox2->TabIndex = 14;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"본 정보";
			// 
			// Mat44textBox
			// 
			this->Mat44textBox->Location = System::Drawing::Point(129, 162);
			this->Mat44textBox->Name = L"Mat44textBox";
			this->Mat44textBox->ReadOnly = true;
			this->Mat44textBox->Size = System::Drawing::Size(35, 21);
			this->Mat44textBox->TabIndex = 24;
			// 
			// Mat43textBox
			// 
			this->Mat43textBox->Location = System::Drawing::Point(88, 162);
			this->Mat43textBox->Name = L"Mat43textBox";
			this->Mat43textBox->ReadOnly = true;
			this->Mat43textBox->Size = System::Drawing::Size(35, 21);
			this->Mat43textBox->TabIndex = 23;
			// 
			// Mat42textBox
			// 
			this->Mat42textBox->Location = System::Drawing::Point(47, 162);
			this->Mat42textBox->Name = L"Mat42textBox";
			this->Mat42textBox->ReadOnly = true;
			this->Mat42textBox->Size = System::Drawing::Size(35, 21);
			this->Mat42textBox->TabIndex = 22;
			// 
			// Mat41textBox
			// 
			this->Mat41textBox->Location = System::Drawing::Point(6, 162);
			this->Mat41textBox->Name = L"Mat41textBox";
			this->Mat41textBox->ReadOnly = true;
			this->Mat41textBox->Size = System::Drawing::Size(35, 21);
			this->Mat41textBox->TabIndex = 21;
			// 
			// Mat34textBox
			// 
			this->Mat34textBox->Location = System::Drawing::Point(129, 135);
			this->Mat34textBox->Name = L"Mat34textBox";
			this->Mat34textBox->ReadOnly = true;
			this->Mat34textBox->Size = System::Drawing::Size(35, 21);
			this->Mat34textBox->TabIndex = 20;
			// 
			// Mat33textBox
			// 
			this->Mat33textBox->Location = System::Drawing::Point(88, 135);
			this->Mat33textBox->Name = L"Mat33textBox";
			this->Mat33textBox->ReadOnly = true;
			this->Mat33textBox->Size = System::Drawing::Size(35, 21);
			this->Mat33textBox->TabIndex = 19;
			// 
			// Mat32textBox
			// 
			this->Mat32textBox->Location = System::Drawing::Point(47, 135);
			this->Mat32textBox->Name = L"Mat32textBox";
			this->Mat32textBox->ReadOnly = true;
			this->Mat32textBox->Size = System::Drawing::Size(35, 21);
			this->Mat32textBox->TabIndex = 18;
			// 
			// Mat31textBox
			// 
			this->Mat31textBox->Location = System::Drawing::Point(6, 135);
			this->Mat31textBox->Name = L"Mat31textBox";
			this->Mat31textBox->ReadOnly = true;
			this->Mat31textBox->Size = System::Drawing::Size(35, 21);
			this->Mat31textBox->TabIndex = 17;
			// 
			// Mat24textBox
			// 
			this->Mat24textBox->Location = System::Drawing::Point(129, 108);
			this->Mat24textBox->Name = L"Mat24textBox";
			this->Mat24textBox->ReadOnly = true;
			this->Mat24textBox->Size = System::Drawing::Size(35, 21);
			this->Mat24textBox->TabIndex = 16;
			// 
			// Mat23textBox
			// 
			this->Mat23textBox->Location = System::Drawing::Point(88, 108);
			this->Mat23textBox->Name = L"Mat23textBox";
			this->Mat23textBox->ReadOnly = true;
			this->Mat23textBox->Size = System::Drawing::Size(35, 21);
			this->Mat23textBox->TabIndex = 15;
			// 
			// Mat22textBox
			// 
			this->Mat22textBox->Location = System::Drawing::Point(47, 108);
			this->Mat22textBox->Name = L"Mat22textBox";
			this->Mat22textBox->ReadOnly = true;
			this->Mat22textBox->Size = System::Drawing::Size(35, 21);
			this->Mat22textBox->TabIndex = 14;
			// 
			// Mat21textBox
			// 
			this->Mat21textBox->Location = System::Drawing::Point(6, 108);
			this->Mat21textBox->Name = L"Mat21textBox";
			this->Mat21textBox->ReadOnly = true;
			this->Mat21textBox->Size = System::Drawing::Size(35, 21);
			this->Mat21textBox->TabIndex = 13;
			// 
			// Mat14textBox
			// 
			this->Mat14textBox->Location = System::Drawing::Point(129, 81);
			this->Mat14textBox->Name = L"Mat14textBox";
			this->Mat14textBox->ReadOnly = true;
			this->Mat14textBox->Size = System::Drawing::Size(35, 21);
			this->Mat14textBox->TabIndex = 12;
			// 
			// Mat13textBox
			// 
			this->Mat13textBox->Location = System::Drawing::Point(88, 81);
			this->Mat13textBox->Name = L"Mat13textBox";
			this->Mat13textBox->ReadOnly = true;
			this->Mat13textBox->Size = System::Drawing::Size(35, 21);
			this->Mat13textBox->TabIndex = 11;
			// 
			// Mat12textBox
			// 
			this->Mat12textBox->Location = System::Drawing::Point(47, 81);
			this->Mat12textBox->Name = L"Mat12textBox";
			this->Mat12textBox->ReadOnly = true;
			this->Mat12textBox->Size = System::Drawing::Size(35, 21);
			this->Mat12textBox->TabIndex = 10;
			// 
			// Mat11textBox
			// 
			this->Mat11textBox->Location = System::Drawing::Point(6, 81);
			this->Mat11textBox->Name = L"Mat11textBox";
			this->Mat11textBox->ReadOnly = true;
			this->Mat11textBox->Size = System::Drawing::Size(35, 21);
			this->Mat11textBox->TabIndex = 9;
			// 
			// BipParenttextBox
			// 
			this->BipParenttextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->BipParenttextBox->Location = System::Drawing::Point(69, 36);
			this->BipParenttextBox->Name = L"BipParenttextBox";
			this->BipParenttextBox->ReadOnly = true;
			this->BipParenttextBox->Size = System::Drawing::Size(176, 21);
			this->BipParenttextBox->TabIndex = 3;
			// 
			// BipNametextBox
			// 
			this->BipNametextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->BipNametextBox->Location = System::Drawing::Point(69, 14);
			this->BipNametextBox->Name = L"BipNametextBox";
			this->BipNametextBox->ReadOnly = true;
			this->BipNametextBox->Size = System::Drawing::Size(176, 21);
			this->BipNametextBox->TabIndex = 2;
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->TexMemtextBox);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->PolygoncounttextBox);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->BipCounttextBox);
			this->groupBox1->Controls->Add(label2);
			this->groupBox1->Controls->Add(this->FileNametextBox);
			this->groupBox1->Controls->Add(label1);
			this->groupBox1->Location = System::Drawing::Point(4, 256);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(254, 122);
			this->groupBox1->TabIndex = 13;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"캐릭터기본정보";
			// 
			// TexMemtextBox
			// 
			this->TexMemtextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TexMemtextBox->Location = System::Drawing::Point(69, 92);
			this->TexMemtextBox->Name = L"TexMemtextBox";
			this->TexMemtextBox->ReadOnly = true;
			this->TexMemtextBox->Size = System::Drawing::Size(176, 21);
			this->TexMemtextBox->TabIndex = 7;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(7, 97);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(60, 12);
			this->label6->TabIndex = 6;
			this->label6->Text = L"TexMem:";
			// 
			// PolygoncounttextBox
			// 
			this->PolygoncounttextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->PolygoncounttextBox->Location = System::Drawing::Point(69, 68);
			this->PolygoncounttextBox->Name = L"PolygoncounttextBox";
			this->PolygoncounttextBox->ReadOnly = true;
			this->PolygoncounttextBox->Size = System::Drawing::Size(176, 21);
			this->PolygoncounttextBox->TabIndex = 5;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(10, 73);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(57, 12);
			this->label5->TabIndex = 4;
			this->label5->Text = L"폴리곤수:";
			// 
			// BipCounttextBox
			// 
			this->BipCounttextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->BipCounttextBox->Location = System::Drawing::Point(69, 44);
			this->BipCounttextBox->Name = L"BipCounttextBox";
			this->BipCounttextBox->ReadOnly = true;
			this->BipCounttextBox->Size = System::Drawing::Size(176, 21);
			this->BipCounttextBox->TabIndex = 3;
			// 
			// FileNametextBox
			// 
			this->FileNametextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->FileNametextBox->Location = System::Drawing::Point(69, 20);
			this->FileNametextBox->Name = L"FileNametextBox";
			this->FileNametextBox->ReadOnly = true;
			this->FileNametextBox->Size = System::Drawing::Size(176, 21);
			this->FileNametextBox->TabIndex = 1;
			// 
			// BiptreeView
			// 
			this->BiptreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->BiptreeView->HideSelection = false;
			this->BiptreeView->Location = System::Drawing::Point(4, 7);
			this->BiptreeView->Name = L"BiptreeView";
			this->BiptreeView->Size = System::Drawing::Size(254, 181);
			this->BiptreeView->TabIndex = 12;
			this->BiptreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &ModelInfoTabPage::BiptreeView_AfterSelect);
			// 
			// MeshNode_comboBox
			// 
			this->MeshNode_comboBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->MeshNode_comboBox->FormattingEnabled = true;
			this->MeshNode_comboBox->Location = System::Drawing::Point(73, 224);
			this->MeshNode_comboBox->Name = L"MeshNode_comboBox";
			this->MeshNode_comboBox->Size = System::Drawing::Size(177, 20);
			this->MeshNode_comboBox->TabIndex = 19;
			this->MeshNode_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &ModelInfoTabPage::MeshNode_comboBox_SelectedIndexChanged);
			// 
			// ModelInfoTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(262, 583);
			this->Controls->Add(label8);
			this->Controls->Add(this->MeshNode_comboBox);
			this->Controls->Add(this->AllNodesSelectbutton);
			this->Controls->Add(this->ItemNodesSelectbutton);
			this->Controls->Add(this->SkinNodesSelectbutton);
			this->Controls->Add(this->NoneSelectBonebutton);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->BiptreeView);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"ModelInfoTabPage";
			this->Text = L"ModelInfoTabPage";
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
private: System::Void BiptreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 g_pMainApp->m_TabModelInfo.SetSelectedNodeInfo(e->Node->Text);
			 SetSelectedNodeInfo(e->Node->Text);
		 }
private: System::Void SkinNodesSelectbutton_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_TabModelInfo.SkinNodesSelectButtonClick();
		 }
private: System::Void ItemNodesSelectbutton_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_TabModelInfo.ItemNodesSelectButtonClick();
		 }
private: System::Void NoneSelectBonebutton_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_TabModelInfo.NodeNoneSelectedButtonClick();

			 NoneSelectedNodeInfo();
		 }
private: System::Void AllNodesSelectbutton_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void MeshNode_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
};
}
