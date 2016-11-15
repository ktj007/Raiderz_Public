#pragma once

#include "MainApp.h"
#include "ManagedUtil.h"
#include "RTrail.h"
#include "REmitter.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace EffectTool_NET {

	/// <summary>
	/// PicForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class PicForm : public System::Windows::Forms::Form
	{
	public:
		PicForm(rs3::RTrail* pTrail)
		{
			trail = pTrail;
			emitter = NULL;
			Init();

			pictureBox1->Image = MHelp::BitmapFromTexture(g_pMainApp->GetDevice(), trail->GetTexture());
			pictureBox2->Image = MHelp::BitmapFromTextureAlpha(g_pMainApp->GetDevice(), trail->GetTexture());

			const RVector4 *texUV = trail->GetTexUV();
			RVector2 vTexPivot( (texUV->x + texUV->z)/2, (texUV->y + texUV->w)/2 );

			SetTextureRect(texUV->x, texUV->y, texUV->z, texUV->w);
			SetTexturePivot(vTexPivot.x, vTexPivot.y);
		}

		PicForm(rs3::REmitter* pEmitter)
		{
			trail = NULL;
			emitter = pEmitter;
			Init();

			pictureBox1->Image = MHelp::BitmapFromTexture(g_pMainApp->GetDevice(), emitter->GetTexture());
			pictureBox2->Image = MHelp::BitmapFromTextureAlpha(g_pMainApp->GetDevice(), emitter->GetTexture());

			const RVector4 *pTexUV = emitter->GetTexUV();
			const RVector2 *pTexPivot = emitter->GetTexUVPivot();
			SetTextureRect(pTexUV->x, pTexUV->y, pTexUV->z, pTexUV->w);
			SetTexturePivot(pTexPivot->x, pTexPivot->y);

			UpdateEmitterUVAnimation();
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~PicForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: rs3::RTrail* trail;
	private: rs3::REmitter* emitter;
	private: System::Drawing::RectangleF^ texUV;
	private: System::Drawing::PointF^ texPivot;
	public: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Drawing::Drawing2D::GraphicsPath^ mousePath;
	private: System::Drawing::Drawing2D::GraphicsPath^ mousePivotPath;
	private: System::Drawing::Point^ mouseEnd;
	private: System::Boolean^	bDown;
	private: System::Boolean^	bDownInPivot;
	private: System::Drawing::RectangleF^ rectangle;
	private: System::Drawing::Rectangle^ pivotRectangle;
	private: System::Windows::Forms::NumericUpDown^  UnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  HnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  WnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  VnumericUpDown;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
	public: System::Windows::Forms::PictureBox^  pictureBox2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::RadioButton^  TrailUVLeftRadioButton;
	private: System::Windows::Forms::RadioButton^  TrailUVTopRadioButton;
	private: System::Windows::Forms::RadioButton^  TrailUVRightRadioButton;
	private: System::Windows::Forms::RadioButton^  TrailUVBottomRadioButton;
	private: System::Windows::Forms::Label^  TrailUVDirectionLable;
	private: System::Windows::Forms::CheckBox^  UVAniCheckBox;
	private: System::Windows::Forms::NumericUpDown^  UVAniXDivideNumericUpDown;
	private: System::Windows::Forms::Label^  UVAniXDivideLable;
	private: System::Windows::Forms::Label^  UVAniYDivideLable;
	private: System::Windows::Forms::NumericUpDown^  UVAniYDivideNumericUpDown;
	private: System::Windows::Forms::Label^  UVAniMaxFrameLabel;
	private: System::Windows::Forms::NumericUpDown^  UVAniMaxFrameNumericUpDown;
	private: System::Windows::Forms::Label^  UVAniDelayLabel;
	private: System::Windows::Forms::NumericUpDown^  UVAniDelayNumericUpDown;
	private: System::Windows::Forms::CheckBox^  UVAniLoopCheckBox;
	private: System::Windows::Forms::TabControl^  TabControl;
	private: System::Windows::Forms::TabPage^  UVEditTabPage;
	private: System::Windows::Forms::TabPage^  UVAniTabPage;
	private: System::Windows::Forms::TabPage^  TrailOptionTabPage;
	private: System::Windows::Forms::Button^  ResetPivotButton;
	private: System::Windows::Forms::NumericUpDown^  PivotYNumericUpDown;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::NumericUpDown^  PivotXNumericUpDown;
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
			System::Windows::Forms::Label^  label5;
			System::Windows::Forms::Label^  label6;
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->UnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->HnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->WnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->VnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->TrailUVLeftRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->TrailUVTopRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->TrailUVRightRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->TrailUVBottomRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->TrailUVDirectionLable = (gcnew System::Windows::Forms::Label());
			this->UVAniCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->UVAniXDivideNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->UVAniXDivideLable = (gcnew System::Windows::Forms::Label());
			this->UVAniYDivideLable = (gcnew System::Windows::Forms::Label());
			this->UVAniYDivideNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->UVAniMaxFrameLabel = (gcnew System::Windows::Forms::Label());
			this->UVAniMaxFrameNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->UVAniDelayLabel = (gcnew System::Windows::Forms::Label());
			this->UVAniDelayNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->UVAniLoopCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->TabControl = (gcnew System::Windows::Forms::TabControl());
			this->UVEditTabPage = (gcnew System::Windows::Forms::TabPage());
			this->UVAniTabPage = (gcnew System::Windows::Forms::TabPage());
			this->TrailOptionTabPage = (gcnew System::Windows::Forms::TabPage());
			this->ResetPivotButton = (gcnew System::Windows::Forms::Button());
			this->PivotYNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->PivotXNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			label5 = (gcnew System::Windows::Forms::Label());
			label6 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->HnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->WnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VnumericUpDown))->BeginInit();
			this->statusStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UVAniXDivideNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UVAniYDivideNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UVAniMaxFrameNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UVAniDelayNumericUpDown))->BeginInit();
			this->TabControl->SuspendLayout();
			this->UVEditTabPage->SuspendLayout();
			this->UVAniTabPage->SuspendLayout();
			this->TrailOptionTabPage->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PivotYNumericUpDown))->BeginInit();
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PivotXNumericUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// label5
			// 
			label5->AutoSize = true;
			label5->Location = System::Drawing::Point(10, 8);
			label5->Name = L"label5";
			label5->Size = System::Drawing::Size(52, 12);
			label5->TabIndex = 11;
			label5->Text = L"Pivot X :";
			// 
			// label6
			// 
			label6->AutoSize = true;
			label6->Location = System::Drawing::Point(142, 8);
			label6->Name = L"label6";
			label6->Size = System::Drawing::Size(52, 12);
			label6->TabIndex = 29;
			label6->Text = L"Pivot Y :";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->pictureBox1->Location = System::Drawing::Point(237, 121);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(128, 128);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &PicForm::pictureBox1_MouseMove);
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &PicForm::pictureBox1_MouseDown);
			this->pictureBox1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &PicForm::pictureBox1_Paint);
			this->pictureBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &PicForm::pictureBox1_MouseUp);
			// 
			// UnumericUpDown
			// 
			this->UnumericUpDown->Location = System::Drawing::Point(33, 4);
			this->UnumericUpDown->Name = L"UnumericUpDown";
			this->UnumericUpDown->Size = System::Drawing::Size(62, 21);
			this->UnumericUpDown->TabIndex = 0;
			this->UnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::UnumericUpDown_ValueChanged);
			// 
			// HnumericUpDown
			// 
			this->HnumericUpDown->Location = System::Drawing::Point(318, 4);
			this->HnumericUpDown->Name = L"HnumericUpDown";
			this->HnumericUpDown->Size = System::Drawing::Size(62, 21);
			this->HnumericUpDown->TabIndex = 3;
			this->HnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::HnumericUpDown_ValueChanged);
			// 
			// WnumericUpDown
			// 
			this->WnumericUpDown->Location = System::Drawing::Point(223, 4);
			this->WnumericUpDown->Name = L"WnumericUpDown";
			this->WnumericUpDown->Size = System::Drawing::Size(62, 21);
			this->WnumericUpDown->TabIndex = 2;
			this->WnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::WnumericUpDown_ValueChanged);
			// 
			// VnumericUpDown
			// 
			this->VnumericUpDown->Location = System::Drawing::Point(128, 4);
			this->VnumericUpDown->Name = L"VnumericUpDown";
			this->VnumericUpDown->Size = System::Drawing::Size(62, 21);
			this->VnumericUpDown->TabIndex = 1;
			this->VnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::VnumericUpDown_ValueChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 6);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(21, 12);
			this->label1->TabIndex = 5;
			this->label1->Text = L"U :";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(101, 6);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(21, 12);
			this->label2->TabIndex = 6;
			this->label2->Text = L"V :";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(196, 6);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(23, 12);
			this->label3->TabIndex = 7;
			this->label3->Text = L"W :";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(291, 6);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(21, 12);
			this->label4->TabIndex = 8;
			this->label4->Text = L"H :";
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripStatusLabel1});
			this->statusStrip1->Location = System::Drawing::Point(0, 354);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(594, 22);
			this->statusStrip1->TabIndex = 9;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(121, 17);
			this->toolStripStatusLabel1->Text = L"toolStripStatusLabel1";
			// 
			// pictureBox2
			// 
			this->pictureBox2->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->pictureBox2->BackColor = System::Drawing::Color::Black;
			this->pictureBox2->Location = System::Drawing::Point(237, 217);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(128, 128);
			this->pictureBox2->TabIndex = 10;
			this->pictureBox2->TabStop = false;
			this->pictureBox2->Visible = false;
			this->pictureBox2->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &PicForm::pictureBox2_MouseMove);
			this->pictureBox2->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &PicForm::pictureBox2_MouseDown);
			this->pictureBox2->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &PicForm::pictureBox2_Paint);
			this->pictureBox2->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &PicForm::pictureBox2_MouseUp);
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button1->Location = System::Drawing::Point(510, 30);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(62, 23);
			this->button1->TabIndex = 11;
			this->button1->Text = L"Alpha";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &PicForm::button1_Click);
			// 
			// TrailUVLeftRadioButton
			// 
			this->TrailUVLeftRadioButton->AutoSize = true;
			this->TrailUVLeftRadioButton->Location = System::Drawing::Point(114, 7);
			this->TrailUVLeftRadioButton->Name = L"TrailUVLeftRadioButton";
			this->TrailUVLeftRadioButton->Size = System::Drawing::Size(43, 16);
			this->TrailUVLeftRadioButton->TabIndex = 10;
			this->TrailUVLeftRadioButton->TabStop = true;
			this->TrailUVLeftRadioButton->Text = L"Left";
			this->TrailUVLeftRadioButton->UseVisualStyleBackColor = true;
			this->TrailUVLeftRadioButton->CheckedChanged += gcnew System::EventHandler(this, &PicForm::TrailUVLeftRadioButton_CheckedChanged);
			// 
			// TrailUVTopRadioButton
			// 
			this->TrailUVTopRadioButton->AutoSize = true;
			this->TrailUVTopRadioButton->Location = System::Drawing::Point(163, 7);
			this->TrailUVTopRadioButton->Name = L"TrailUVTopRadioButton";
			this->TrailUVTopRadioButton->Size = System::Drawing::Size(45, 16);
			this->TrailUVTopRadioButton->TabIndex = 11;
			this->TrailUVTopRadioButton->TabStop = true;
			this->TrailUVTopRadioButton->Text = L"Top";
			this->TrailUVTopRadioButton->UseVisualStyleBackColor = true;
			this->TrailUVTopRadioButton->CheckedChanged += gcnew System::EventHandler(this, &PicForm::TrailUVTopRadioButton_CheckedChanged);
			// 
			// TrailUVRightRadioButton
			// 
			this->TrailUVRightRadioButton->AutoSize = true;
			this->TrailUVRightRadioButton->Location = System::Drawing::Point(214, 7);
			this->TrailUVRightRadioButton->Name = L"TrailUVRightRadioButton";
			this->TrailUVRightRadioButton->Size = System::Drawing::Size(51, 16);
			this->TrailUVRightRadioButton->TabIndex = 12;
			this->TrailUVRightRadioButton->TabStop = true;
			this->TrailUVRightRadioButton->Text = L"Right";
			this->TrailUVRightRadioButton->UseVisualStyleBackColor = true;
			this->TrailUVRightRadioButton->CheckedChanged += gcnew System::EventHandler(this, &PicForm::TrailUVRightRadioButton_CheckedChanged);
			// 
			// TrailUVBottomRadioButton
			// 
			this->TrailUVBottomRadioButton->AutoSize = true;
			this->TrailUVBottomRadioButton->Location = System::Drawing::Point(271, 7);
			this->TrailUVBottomRadioButton->Name = L"TrailUVBottomRadioButton";
			this->TrailUVBottomRadioButton->Size = System::Drawing::Size(62, 16);
			this->TrailUVBottomRadioButton->TabIndex = 13;
			this->TrailUVBottomRadioButton->TabStop = true;
			this->TrailUVBottomRadioButton->Text = L"Bottom";
			this->TrailUVBottomRadioButton->UseVisualStyleBackColor = true;
			this->TrailUVBottomRadioButton->CheckedChanged += gcnew System::EventHandler(this, &PicForm::TrailUVBottomRadioButton_CheckedChanged);
			// 
			// TrailUVDirectionLable
			// 
			this->TrailUVDirectionLable->AutoSize = true;
			this->TrailUVDirectionLable->Location = System::Drawing::Point(6, 8);
			this->TrailUVDirectionLable->Name = L"TrailUVDirectionLable";
			this->TrailUVDirectionLable->Size = System::Drawing::Size(107, 12);
			this->TrailUVDirectionLable->TabIndex = 14;
			this->TrailUVDirectionLable->Text = L"Trail UV Direction:";
			// 
			// UVAniCheckBox
			// 
			this->UVAniCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->UVAniCheckBox->AutoSize = true;
			this->UVAniCheckBox->Location = System::Drawing::Point(510, 8);
			this->UVAniCheckBox->Name = L"UVAniCheckBox";
			this->UVAniCheckBox->Size = System::Drawing::Size(62, 16);
			this->UVAniCheckBox->TabIndex = 15;
			this->UVAniCheckBox->Text = L"UV Ani";
			this->UVAniCheckBox->UseVisualStyleBackColor = true;
			this->UVAniCheckBox->CheckedChanged += gcnew System::EventHandler(this, &PicForm::UVAniCheckBox_CheckedChanged);
			// 
			// UVAniXDivideNumericUpDown
			// 
			this->UVAniXDivideNumericUpDown->Location = System::Drawing::Point(33, 4);
			this->UVAniXDivideNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->UVAniXDivideNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->UVAniXDivideNumericUpDown->Name = L"UVAniXDivideNumericUpDown";
			this->UVAniXDivideNumericUpDown->Size = System::Drawing::Size(43, 21);
			this->UVAniXDivideNumericUpDown->TabIndex = 16;
			this->UVAniXDivideNumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->UVAniXDivideNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::UVAniXDivideNumericUpDown_ValueChanged);
			// 
			// UVAniXDivideLable
			// 
			this->UVAniXDivideLable->AutoSize = true;
			this->UVAniXDivideLable->Location = System::Drawing::Point(6, 7);
			this->UVAniXDivideLable->Name = L"UVAniXDivideLable";
			this->UVAniXDivideLable->Size = System::Drawing::Size(21, 12);
			this->UVAniXDivideLable->TabIndex = 17;
			this->UVAniXDivideLable->Text = L"X :";
			// 
			// UVAniYDivideLable
			// 
			this->UVAniYDivideLable->AutoSize = true;
			this->UVAniYDivideLable->Location = System::Drawing::Point(82, 7);
			this->UVAniYDivideLable->Name = L"UVAniYDivideLable";
			this->UVAniYDivideLable->Size = System::Drawing::Size(21, 12);
			this->UVAniYDivideLable->TabIndex = 19;
			this->UVAniYDivideLable->Text = L"Y :";
			// 
			// UVAniYDivideNumericUpDown
			// 
			this->UVAniYDivideNumericUpDown->Location = System::Drawing::Point(109, 4);
			this->UVAniYDivideNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->UVAniYDivideNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->UVAniYDivideNumericUpDown->Name = L"UVAniYDivideNumericUpDown";
			this->UVAniYDivideNumericUpDown->Size = System::Drawing::Size(39, 21);
			this->UVAniYDivideNumericUpDown->TabIndex = 18;
			this->UVAniYDivideNumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->UVAniYDivideNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::UVAniYDivideNumericUpDown_ValueChanged);
			// 
			// UVAniMaxFrameLabel
			// 
			this->UVAniMaxFrameLabel->AutoSize = true;
			this->UVAniMaxFrameLabel->Location = System::Drawing::Point(154, 7);
			this->UVAniMaxFrameLabel->Name = L"UVAniMaxFrameLabel";
			this->UVAniMaxFrameLabel->Size = System::Drawing::Size(38, 12);
			this->UVAniMaxFrameLabel->TabIndex = 21;
			this->UVAniMaxFrameLabel->Text = L"Max :";
			// 
			// UVAniMaxFrameNumericUpDown
			// 
			this->UVAniMaxFrameNumericUpDown->Location = System::Drawing::Point(198, 4);
			this->UVAniMaxFrameNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, 0});
			this->UVAniMaxFrameNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->UVAniMaxFrameNumericUpDown->Name = L"UVAniMaxFrameNumericUpDown";
			this->UVAniMaxFrameNumericUpDown->Size = System::Drawing::Size(41, 21);
			this->UVAniMaxFrameNumericUpDown->TabIndex = 20;
			this->UVAniMaxFrameNumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->UVAniMaxFrameNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::UVAniMaxFrameNumericUpDown_ValueChanged);
			// 
			// UVAniDelayLabel
			// 
			this->UVAniDelayLabel->AutoSize = true;
			this->UVAniDelayLabel->Location = System::Drawing::Point(245, 7);
			this->UVAniDelayLabel->Name = L"UVAniDelayLabel";
			this->UVAniDelayLabel->Size = System::Drawing::Size(45, 12);
			this->UVAniDelayLabel->TabIndex = 23;
			this->UVAniDelayLabel->Text = L"Delay :";
			// 
			// UVAniDelayNumericUpDown
			// 
			this->UVAniDelayNumericUpDown->DecimalPlaces = 2;
			this->UVAniDelayNumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->UVAniDelayNumericUpDown->Location = System::Drawing::Point(296, 4);
			this->UVAniDelayNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->UVAniDelayNumericUpDown->Name = L"UVAniDelayNumericUpDown";
			this->UVAniDelayNumericUpDown->Size = System::Drawing::Size(49, 21);
			this->UVAniDelayNumericUpDown->TabIndex = 22;
			this->UVAniDelayNumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->UVAniDelayNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::UVAniDelayNumericUpDown_ValueChanged);
			// 
			// UVAniLoopCheckBox
			// 
			this->UVAniLoopCheckBox->AutoSize = true;
			this->UVAniLoopCheckBox->Location = System::Drawing::Point(351, 5);
			this->UVAniLoopCheckBox->Name = L"UVAniLoopCheckBox";
			this->UVAniLoopCheckBox->Size = System::Drawing::Size(52, 16);
			this->UVAniLoopCheckBox->TabIndex = 24;
			this->UVAniLoopCheckBox->Text = L"Loop";
			this->UVAniLoopCheckBox->UseVisualStyleBackColor = true;
			this->UVAniLoopCheckBox->CheckedChanged += gcnew System::EventHandler(this, &PicForm::UVAniLoopCheckBox_CheckedChanged);
			// 
			// TabControl
			// 
			this->TabControl->Controls->Add(this->UVEditTabPage);
			this->TabControl->Controls->Add(this->UVAniTabPage);
			this->TabControl->Controls->Add(this->TrailOptionTabPage);
			this->TabControl->Location = System::Drawing::Point(5, 2);
			this->TabControl->Name = L"TabControl";
			this->TabControl->SelectedIndex = 0;
			this->TabControl->Size = System::Drawing::Size(499, 54);
			this->TabControl->TabIndex = 27;
			// 
			// UVEditTabPage
			// 
			this->UVEditTabPage->Controls->Add(this->VnumericUpDown);
			this->UVEditTabPage->Controls->Add(this->label1);
			this->UVEditTabPage->Controls->Add(this->UnumericUpDown);
			this->UVEditTabPage->Controls->Add(this->label4);
			this->UVEditTabPage->Controls->Add(this->HnumericUpDown);
			this->UVEditTabPage->Controls->Add(this->label3);
			this->UVEditTabPage->Controls->Add(this->WnumericUpDown);
			this->UVEditTabPage->Controls->Add(this->label2);
			this->UVEditTabPage->Location = System::Drawing::Point(4, 22);
			this->UVEditTabPage->Name = L"UVEditTabPage";
			this->UVEditTabPage->Padding = System::Windows::Forms::Padding(3);
			this->UVEditTabPage->Size = System::Drawing::Size(491, 28);
			this->UVEditTabPage->TabIndex = 0;
			this->UVEditTabPage->Text = L"UVEdit";
			this->UVEditTabPage->UseVisualStyleBackColor = true;
			// 
			// UVAniTabPage
			// 
			this->UVAniTabPage->Controls->Add(this->UVAniDelayLabel);
			this->UVAniTabPage->Controls->Add(this->UVAniXDivideLable);
			this->UVAniTabPage->Controls->Add(this->UVAniLoopCheckBox);
			this->UVAniTabPage->Controls->Add(this->UVAniMaxFrameNumericUpDown);
			this->UVAniTabPage->Controls->Add(this->UVAniXDivideNumericUpDown);
			this->UVAniTabPage->Controls->Add(this->UVAniYDivideLable);
			this->UVAniTabPage->Controls->Add(this->UVAniMaxFrameLabel);
			this->UVAniTabPage->Controls->Add(this->UVAniDelayNumericUpDown);
			this->UVAniTabPage->Controls->Add(this->UVAniYDivideNumericUpDown);
			this->UVAniTabPage->Location = System::Drawing::Point(4, 22);
			this->UVAniTabPage->Name = L"UVAniTabPage";
			this->UVAniTabPage->Padding = System::Windows::Forms::Padding(3);
			this->UVAniTabPage->Size = System::Drawing::Size(491, 28);
			this->UVAniTabPage->TabIndex = 1;
			this->UVAniTabPage->Text = L"UVAnimation";
			this->UVAniTabPage->UseVisualStyleBackColor = true;
			// 
			// TrailOptionTabPage
			// 
			this->TrailOptionTabPage->Controls->Add(this->TrailUVLeftRadioButton);
			this->TrailOptionTabPage->Controls->Add(this->TrailUVDirectionLable);
			this->TrailOptionTabPage->Controls->Add(this->TrailUVTopRadioButton);
			this->TrailOptionTabPage->Controls->Add(this->TrailUVBottomRadioButton);
			this->TrailOptionTabPage->Controls->Add(this->TrailUVRightRadioButton);
			this->TrailOptionTabPage->Location = System::Drawing::Point(4, 22);
			this->TrailOptionTabPage->Name = L"TrailOptionTabPage";
			this->TrailOptionTabPage->Size = System::Drawing::Size(491, 28);
			this->TrailOptionTabPage->TabIndex = 2;
			this->TrailOptionTabPage->Text = L"TrailOption";
			this->TrailOptionTabPage->UseVisualStyleBackColor = true;
			// 
			// ResetPivotButton
			// 
			this->ResetPivotButton->Location = System::Drawing::Point(268, 3);
			this->ResetPivotButton->Name = L"ResetPivotButton";
			this->ResetPivotButton->Size = System::Drawing::Size(82, 22);
			this->ResetPivotButton->TabIndex = 9;
			this->ResetPivotButton->Text = L"Reset Pivot";
			this->ResetPivotButton->UseVisualStyleBackColor = true;
			this->ResetPivotButton->Click += gcnew System::EventHandler(this, &PicForm::ResetPivotButton_Click);
			// 
			// PivotYNumericUpDown
			// 
			this->PivotYNumericUpDown->Location = System::Drawing::Point(200, 4);
			this->PivotYNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->PivotYNumericUpDown->Name = L"PivotYNumericUpDown";
			this->PivotYNumericUpDown->Size = System::Drawing::Size(62, 21);
			this->PivotYNumericUpDown->TabIndex = 28;
			this->PivotYNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::PivotYNumericUpDown_ValueChanged);
			// 
			// panel1
			// 
			this->panel1->Controls->Add(label6);
			this->panel1->Controls->Add(label5);
			this->panel1->Controls->Add(this->ResetPivotButton);
			this->panel1->Controls->Add(this->PivotYNumericUpDown);
			this->panel1->Controls->Add(this->PivotXNumericUpDown);
			this->panel1->Location = System::Drawing::Point(5, 55);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(499, 28);
			this->panel1->TabIndex = 29;
			// 
			// PivotXNumericUpDown
			// 
			this->PivotXNumericUpDown->Location = System::Drawing::Point(64, 4);
			this->PivotXNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->PivotXNumericUpDown->Name = L"PivotXNumericUpDown";
			this->PivotXNumericUpDown->Size = System::Drawing::Size(62, 21);
			this->PivotXNumericUpDown->TabIndex = 10;
			this->PivotXNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &PicForm::PivotXNumericUpDown_ValueChanged);
			// 
			// PicForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(594, 376);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->TabControl);
			this->Controls->Add(this->UVAniCheckBox);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->pictureBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->MinimumSize = System::Drawing::Size(600, 200);
			this->Name = L"PicForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"PicForm";
			this->Load += gcnew System::EventHandler(this, &PicForm::PicForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->HnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->WnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VnumericUpDown))->EndInit();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UVAniXDivideNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UVAniYDivideNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UVAniMaxFrameNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->UVAniDelayNumericUpDown))->EndInit();
			this->TabControl->ResumeLayout(false);
			this->UVEditTabPage->ResumeLayout(false);
			this->UVEditTabPage->PerformLayout();
			this->UVAniTabPage->ResumeLayout(false);
			this->UVAniTabPage->PerformLayout();
			this->TrailOptionTabPage->ResumeLayout(false);
			this->TrailOptionTabPage->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PivotYNumericUpDown))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PivotXNumericUpDown))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Init() {
				InitializeComponent();

				mousePath = gcnew System::Drawing::Drawing2D::GraphicsPath;
				mousePivotPath = gcnew System::Drawing::Drawing2D::GraphicsPath;
				mouseEnd  = gcnew System::Drawing::Point;
				bDown	  = gcnew System::Boolean;
				bDownInPivot = gcnew System::Boolean;
				rectangle = gcnew System::Drawing::RectangleF;
				pivotRectangle = gcnew System::Drawing::Rectangle;

				texUV = gcnew System::Drawing::RectangleF(0.f, 0.f, 0.f, 0.f);
				texPivot = gcnew System::Drawing::PointF(0.f, 0.f);

				RefreshTabPage();
				EnableTrailOption();
			 }
	private: System::Void PicForm_Load(System::Object^  sender, System::EventArgs^  e) {
				 this->ClientSize = System::Drawing::Size(pictureBox1->Image->Width+20, pictureBox1->Image->Height+150);

				 // 텍스쳐 피봇 설정이 초기화 되어서 미리 보관
				 float texPivotX = texPivot->X;
				 float texPivotY = texPivot->Y;

				 pictureBox1->Width = pictureBox1->Image->Width;
				 pictureBox1->Height= pictureBox1->Image->Height;

				 int x = (this->ClientSize.Width - pictureBox1->Width)/2;
				 pictureBox1->Location = System::Drawing::Point(x, 85);

				 pictureBox2->Width = pictureBox1->Image->Width;
				 pictureBox2->Height= pictureBox1->Image->Height;

				 pictureBox2->Location = System::Drawing::Point(x, 85);

				 UnumericUpDown->Maximum = int(pictureBox1->Image->Width	- 1);
				 VnumericUpDown->Maximum = int(pictureBox1->Image->Height	- 1);
				 WnumericUpDown->Maximum = int(pictureBox1->Image->Width	- 1);
				 HnumericUpDown->Maximum = int(pictureBox1->Image->Height	- 1);
				 PivotXNumericUpDown->Maximum = int(pictureBox1->Image->Width	- 1);
				 PivotYNumericUpDown->Maximum = int(pictureBox1->Image->Height	- 1);

				 int U = static_cast<int>(texUV->X * (int)UnumericUpDown->Maximum);
				 int V = static_cast<int>(texUV->Y * (int)VnumericUpDown->Maximum);
				 int W = static_cast<int>(texUV->Width  * (int)WnumericUpDown->Maximum);
				 int H = static_cast<int>(texUV->Height * (int)HnumericUpDown->Maximum);

				 UnumericUpDown->Value = U;
				 VnumericUpDown->Value = V;
				 WnumericUpDown->Value = W;
				 HnumericUpDown->Value = H;

				 // 미리 보관 된 피봇 셋팅 적용
				 int pivotX = static_cast<int>(texPivotX * (int)UnumericUpDown->Maximum);
				 int pivotY = static_cast<int>(texPivotY * (int)VnumericUpDown->Maximum);
				 PivotXNumericUpDown->Value = pivotX;
				 PivotYNumericUpDown->Value = pivotY;
			 }
	private: System::Void pictureBox1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
				 e->Graphics->DrawPath( System::Drawing::Pens::Red, mousePath );
				 e->Graphics->DrawPath( System::Drawing::Pens::Red, mousePivotPath );

				 toolStripStatusLabel1->Text = String::Format("U : {0}, V : {1}, W : {2}, H :{3}",
						 texUV->X, texUV->Y, texUV->Width, texUV->Height);
			 }
	private: System::Void pictureBox1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 pictureBox_MouseDown(sender, e, pictureBox1);
			 }
	private: System::Void pictureBox1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 pictureBox_MouseMove(sender, e, pictureBox1);
			 }
	private: System::Void pictureBox1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 bDown = safe_cast<System::Boolean^>(false);
				 bDownInPivot = safe_cast<System::Boolean^>(false);
			 }
	 
	// UV 를 나타내는 사각형을 새로 그린다.
	private: System::Void UpdateUVRectangle() {
				 delete mousePath;
				 mousePath = gcnew System::Drawing::Drawing2D::GraphicsPath;

				 if( UVAniCheckBox->Checked )
				 {
					 rs3::REmitter::REmitterUVAniDesc desc;
					 if( emitter && emitter->GetUVModifierAnimationDesc( desc ) )
					 {
						 int width = pictureBox1->Width / desc.nCountX;
						 int height = pictureBox1->Height / desc.nCountY;

						 for( int i = 0; i < desc.nMaxFrame; ++i )
						 {
							 int posX = ( i % desc.nCountX ) * width;
							 int posY = ( i / desc.nCountX ) * height;

							 System::Drawing::Rectangle^ rect = gcnew System::Drawing::Rectangle( posX, posY, width, height );
							 if( posX + width == pictureBox1->Width )
								rect->Width -= 1;

							 if( posY + height == pictureBox1->Height )
								rect->Height -= 1;

							 mousePath->AddRectangle( *rect );
						 }
					 }
				 }
				 else
				 {
					 mousePath->AddRectangle(*rectangle);
				 }

				 pictureBox1->Invalidate();
				 pictureBox2->Invalidate();
			 }
	 // Emitter에 UV를 새로 만들고, 피벗을 중심에 둔다.
	private: System::Void UpdateEmitterTexUV() {
				 texUV->X = (float)UnumericUpDown->Value / (float)UnumericUpDown->Maximum;
				 texUV->Y = (float)VnumericUpDown->Value / (float)VnumericUpDown->Maximum;
				 texUV->Width = (float)(WnumericUpDown->Value) / (float)WnumericUpDown->Maximum;
				 texUV->Height = (float)(HnumericUpDown->Value) / (float)HnumericUpDown->Maximum;

				 SetPivotPathFromUVWH();
		  }

	private: System::Void UnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if( WnumericUpDown->Value >= (UnumericUpDown->Maximum-UnumericUpDown->Value)) WnumericUpDown->Value = (UnumericUpDown->Maximum-UnumericUpDown->Value);
				 rectangle->X = (float)UnumericUpDown->Value;

				 UpdateUVRectangle();
				 UpdateEmitterTexUV();
			 }
	private: System::Void VnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if( HnumericUpDown->Value >= (VnumericUpDown->Maximum-VnumericUpDown->Value)) HnumericUpDown->Value = (VnumericUpDown->Maximum-VnumericUpDown->Value);
				 rectangle->Y = (float)VnumericUpDown->Value;

				 UpdateUVRectangle();
				 UpdateEmitterTexUV();
			 }
	private: System::Void WnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 rectangle->Width = (float)WnumericUpDown->Value;

				 UpdateUVRectangle();
				 UpdateEmitterTexUV();
			 }
	private: System::Void HnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 rectangle->Height = (float)HnumericUpDown->Value;

				 UpdateUVRectangle();
				 UpdateEmitterTexUV();
			 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			 pictureBox1->Visible = !pictureBox1->Visible;
			 pictureBox2->Visible = !pictureBox1->Visible;
		 }
private: System::Void pictureBox2_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
			 pictureBox1_Paint(sender, e);
		 }
private: System::Void pictureBox2_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 pictureBox_MouseDown(sender, e, pictureBox2);
		 }
private: System::Void pictureBox2_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 pictureBox_MouseMove(sender, e, pictureBox2);
		 }
private: System::Void pictureBox2_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 pictureBox1_MouseUp(sender, e);
		 }

private: System::Void pictureBox_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e, System::Windows::Forms::PictureBox^ pb) {
			 // 피벗안에 클릭한 경우
			 if ( bDownInPivot->Equals(true) )
			 {
				 // pivot path

				 int x = e->X;
				 int y = e->Y;

				 if( x < 0 )
					 x = 0;

				 if( x > (int)UnumericUpDown->Maximum )
					 x = (int)UnumericUpDown->Maximum;

				 if( y < 0 )
					 y = 0;

				 if( y > (int)VnumericUpDown->Maximum )
					 y = (int)VnumericUpDown->Maximum;

				 PivotXNumericUpDown->Value = x;
				 PivotYNumericUpDown->Value = y;
			 }
			 else if( bDown->Equals(true) )
			 {
				 mouseEnd->X = e->X;
				 mouseEnd->Y = e->Y;

				 if( mouseEnd->X < 0) mouseEnd->X = 0;
				 if( mouseEnd->Y < 0) mouseEnd->Y = 0;
				 if( mouseEnd->X > UnumericUpDown->Maximum) mouseEnd->X = (int)UnumericUpDown->Maximum;
				 if( mouseEnd->Y > VnumericUpDown->Maximum) mouseEnd->Y = (int)VnumericUpDown->Maximum;
				 if( mouseEnd->X < UnumericUpDown->Value)	UnumericUpDown->Value = mouseEnd->X;
				 if( mouseEnd->Y < VnumericUpDown->Value)	VnumericUpDown->Value = mouseEnd->Y;

				 WnumericUpDown->Value = mouseEnd->X - UnumericUpDown->Value;
				 HnumericUpDown->Value = mouseEnd->Y - VnumericUpDown->Value;

				 if( WnumericUpDown->Value >= (UnumericUpDown->Maximum-UnumericUpDown->Value)) WnumericUpDown->Value = (UnumericUpDown->Maximum-UnumericUpDown->Value);
				 if( HnumericUpDown->Value >= (VnumericUpDown->Maximum-VnumericUpDown->Value)) HnumericUpDown->Value = (VnumericUpDown->Maximum-VnumericUpDown->Value);

				 // mouse path
				 delete mousePath;
				 mousePath = gcnew System::Drawing::Drawing2D::GraphicsPath;
				 mousePath->AddRectangle(System::Drawing::Rectangle((int)UnumericUpDown->Value, (int)VnumericUpDown->Value, (int)WnumericUpDown->Value, (int)HnumericUpDown->Value));

				 // pivot path
				 SetPivotPathFromUVWH();
			 }
			 pb->Invalidate();
		 }

private: System::Void pictureBox_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e, System::Windows::Forms::PictureBox^ pb) {
			 if ( IsInPivotCircle( e->X, e->Y ) )
			 {
				 bDownInPivot = true;
			 }
			 else if( IsUVEditMode() )
			 {
				 bDown = true;

				 delete mousePath;
				 mousePath = gcnew System::Drawing::Drawing2D::GraphicsPath;

				 UnumericUpDown->Value = e->X;
				 VnumericUpDown->Value = e->Y;
			 }
			 pb->Invalidate();
		 }

ref class _RECT
{
public:
	static const int SIZE = 10;
	static const int HALF = SIZE/2;
};

private: System::Void SetPivotPathFromUVWH() {
			 // Pivot을 UV의 중심에 놓는다.
			 int x = (int)(UnumericUpDown->Value + WnumericUpDown->Value / 2);
			 int y = (int)(VnumericUpDown->Value + HnumericUpDown->Value / 2);

			 PivotXNumericUpDown->Value = x;
			 PivotYNumericUpDown->Value = y;
		 }

private: System::Void SetPivotPathFromMousePos(int x, int y) {
			 pivotRectangle->X = x - _RECT::HALF;
			 pivotRectangle->Y = y - _RECT::HALF;
			 pivotRectangle->Width = _RECT::SIZE;
			 pivotRectangle->Height = _RECT::SIZE;

			 int x1 = x - _RECT::HALF;
			 int y1 = y - _RECT::HALF;
			 int x2 = x1 + _RECT::SIZE;
			 int y2 = y1 + _RECT::SIZE;

			 delete mousePivotPath;
			 mousePivotPath = gcnew System::Drawing::Drawing2D::GraphicsPath;
			 mousePivotPath->AddEllipse( *pivotRectangle);
			 mousePivotPath->CloseFigure();
			 mousePivotPath->AddLine( x1, y1, x2, y2 );
			 mousePivotPath->CloseFigure();
			 mousePivotPath->AddLine( x1, y2, x2, y1 );

			 // uv 좌표와 똑같은 방법으로 넣는다.
			 float pu = (float)(x)/(float)(UnumericUpDown->Maximum);
			 float pv = (float)(y)/(float)(VnumericUpDown->Maximum);

			 texPivot->X = pu;
			 texPivot->Y = pv;
		 }

public: System::Void SetTextureRect(float l, float t, float r, float b)
		{
			texUV->X = l;
			texUV->Y = t;
			texUV->Width = r - l;
			texUV->Height = b - t;
		}

public: System::Void SetTexturePivot(float x, float y)
	   {
		   texPivot->X = x;
		   texPivot->Y = y;
	   }
public: RectangleF^ GetTextureRect()
		{
			return texUV;
		}
public: PointF^ GetTexturePivot()
		{
			return texPivot;
		}
private: System::Void EnableTrailOption()
		{
			if( !trail )
				return;

			if( trail->IsUVGenLeft() )
				TrailUVLeftRadioButton->Checked = true;
			else
			if( trail->IsUVGenTop() )
				TrailUVTopRadioButton->Checked = true;
			else
			if( trail->IsUVGenRight() )
				TrailUVRightRadioButton->Checked = true;
			else
			if( trail->IsUVGenBottom() )
				TrailUVBottomRadioButton->Checked = true;
		}
private: System::Void TrailUVLeftRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			if( trail )
				trail->SetUVGenLeft();
		}
private: System::Void TrailUVTopRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			if( trail )
				trail->SetUVGenTop();
		}
private: System::Void TrailUVRightRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			if( trail )
				trail->SetUVGenRight();
		}
private: System::Void TrailUVBottomRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			if( trail )
				trail->SetUVGenBottom();
		}
private: System::Void UVAniCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			RefreshTabPage();

			if( UVAniCheckBox->Checked )
				SetEmitterUVAnimation();
			else
				SetEmitterUVDefault();

			UpdateUVRectangle();
		}
 private: System::Void RefreshTabPage() {
			TabControl->TabPages->Remove(UVEditTabPage);
			TabControl->TabPages->Remove(UVAniTabPage);
			TabControl->TabPages->Remove(TrailOptionTabPage);

			if( UVAniCheckBox->Checked )
			{
				TabControl->TabPages->Add(UVAniTabPage);
			}
			else
			{
				TabControl->TabPages->Add(UVEditTabPage);
			}

			if( trail )
				TabControl->TabPages->Add(TrailOptionTabPage);
		}
private: System::Void UVAniXDivideNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 SetEmitterUVAnimation();
			 UpdateUVRectangle();
		}
private: System::Void UVAniYDivideNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 SetEmitterUVAnimation();
			 UpdateUVRectangle();
		}
private: System::Void UVAniMaxFrameNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 SetEmitterUVAnimation();
			 UpdateUVRectangle();
		}
private: System::Void UVAniDelayNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 SetEmitterUVAnimation();
		}
private: System::Void UVAniLoopCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 SetEmitterUVAnimation();
			 UVAniDelayNumericUpDown->Enabled = UVAniLoopCheckBox->Checked;
		}
private: System::Void SetEmitterUVDefault() {
			 if( !emitter )
				 return;

			 emitter->SetUVModifierDefault();
		}
private: System::Void SetEmitterUVAnimation() {
			 if( !emitter )
				 return;

			 rs3::REmitter::REmitterUVAniDesc desc;
			 desc.nCountX = System::Decimal::ToInt32( UVAniXDivideNumericUpDown->Value );
			 desc.nCountY = System::Decimal::ToInt32( UVAniYDivideNumericUpDown->Value );
			 desc.nMaxFrame = System::Decimal::ToInt32( UVAniMaxFrameNumericUpDown->Value );
			 desc.fFrameInterval = System::Decimal::ToSingle( UVAniDelayNumericUpDown->Value ) / 30.f;
			 desc.bLoop = UVAniLoopCheckBox->Checked;

			 emitter->SetUVModifierAnimation( &desc );
		}
private: System::Void UpdateEmitterUVAnimation() {
			 if( !emitter )
				 return;

			 rs3::REmitter::REmitterUVAniDesc desc;
	 		 if( emitter->GetUVModifierAnimationDesc( desc ) )
			 {
				 UVAniXDivideNumericUpDown->Value = desc.nCountX;
				 UVAniYDivideNumericUpDown->Value = desc.nCountY;
				 UVAniMaxFrameNumericUpDown->Value = desc.nMaxFrame;
				 UVAniDelayNumericUpDown->Value = Decimal(desc.fFrameInterval * 30.f);
				 UVAniLoopCheckBox->Checked = desc.bLoop;
				 UVAniDelayNumericUpDown->Enabled = UVAniLoopCheckBox->Checked;

				 UVAniCheckBox->Checked = true;
			 }
		}
private: System::Void ResetPivotButton_Click(System::Object^  sender, System::EventArgs^  e) {
			int pivotX = static_cast<int>(0.5f * (int)UnumericUpDown->Maximum);
			int pivotY = static_cast<int>(0.5f * (int)VnumericUpDown->Maximum);
			PivotXNumericUpDown->Value = pivotX;
			PivotYNumericUpDown->Value = pivotY;
		}
private: System::Void PivotXNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdatePivot();
		 }
private: System::Void PivotYNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdatePivot();
		 }
private: System::Void UpdatePivot() {
			 int x = (int)PivotXNumericUpDown->Value;
			 int y = (int)PivotYNumericUpDown->Value;
			 SetPivotPathFromMousePos(x, y);
			 pictureBox1->Invalidate();
			 pictureBox2->Invalidate();
		 }
private: System::Boolean IsUVEditMode() {
			 if( emitter && emitter->GetUVModifierType() == EMITTER_UVMOD_DEFAULT )
				 return true;

			 if( trail )
				 return true;

			 return false;
		 }
private: System::Boolean IsInPivotCircle( int x, int y ) {
			 if( x >= pivotRectangle->Left &&
				 x <= pivotRectangle->Right &&
				 y >= pivotRectangle->Top &&
				 y <= pivotRectangle->Bottom )
			 {
				 return true;
			 }

			 return false;
		 }
};
}
