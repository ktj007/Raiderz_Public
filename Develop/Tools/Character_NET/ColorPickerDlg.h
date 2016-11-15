#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace ColorPickerControls {

	/// <summary>
	/// ColorPicker에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>

	public ref class ColorPickerDlg : public System::Windows::Forms::Form
	{
	public:
		ColorPickerDlg(void)
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
		~ColorPickerDlg()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  Close_button;
	private: Sano::PersonalProjects::ColorPicker::Controls::ColorPanel^  colorPanel;
	public:	System::Drawing::Color pColor;

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
			this->Close_button = (gcnew System::Windows::Forms::Button());
			this->colorPanel = (gcnew Sano::PersonalProjects::ColorPicker::Controls::ColorPanel());
			this->SuspendLayout();
			// 
			// Close_button
			// 
			this->Close_button->Location = System::Drawing::Point(211, 289);
			this->Close_button->Name = L"Close_button";
			this->Close_button->Size = System::Drawing::Size(181, 29);
			this->Close_button->TabIndex = 1;
			this->Close_button->Text = L"닫기";
			this->Close_button->UseVisualStyleBackColor = true;
			this->Close_button->Click += gcnew System::EventHandler(this, &ColorPickerDlg::Close_button_Click);
			// 
			// colorPanel
			// 
			this->colorPanel->AllowDrop = true;
			this->colorPanel->Location = System::Drawing::Point(10, 12);
			this->colorPanel->Name = L"colorPanel";
			this->colorPanel->Size = System::Drawing::Size(600, 271);
			this->colorPanel->TabIndex = 4;
			// 
			// ColorPickerDlg
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->ClientSize = System::Drawing::Size(621, 351);
			this->ControlBox = false;
			this->Controls->Add(this->colorPanel);
			this->Controls->Add(this->Close_button);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"ColorPickerDlg";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"ColorPicker";
			this->TopMost = true;
			this->ResumeLayout(false);

		}

#pragma endregion
	private: System::Void Close_button_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->DialogResult = System::Windows::Forms::DialogResult::OK;
				 this->Close();
			 }
	public: DWORD GetColor()
			{
				return colorPanel->picCurrentColor->BackColor.ToArgb();
			}
	public: void SetInitColor(System::Drawing::Color color)
			{
				colorPanel->picCurrentColor->BackColor = color;
			}
	public: void SetInitColor(DWORD dwcolor)
			{
				System::Drawing::Color color;
				color.FromArgb(dwcolor);
				SetInitColor(color);
			}
	};
}
