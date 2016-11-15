#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// AnimationHitSoundTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class AnimationHitSoundTabPage : public System::Windows::Forms::Form
	{
	private:
		bool	m_bSoundLoad;

	public:
		AnimationHitSoundTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
			m_bSoundLoad = false;
		}

		void InitEffectList();

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~AnimationHitSoundTabPage()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::ComboBox^  HitSound_comboBox;
	private: System::Windows::Forms::Button^  OK_button;
	public: System::Windows::Forms::Label^  AnimationName_label;
	private: 
	public: 
	protected: 

	protected: 

	protected: 


	public: 
	protected: 

	protected: 

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
			this->HitSound_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->OK_button = (gcnew System::Windows::Forms::Button());
			this->AnimationName_label = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// HitSound_comboBox
			// 
			this->HitSound_comboBox->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->HitSound_comboBox->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->HitSound_comboBox->FormattingEnabled = true;
			this->HitSound_comboBox->Location = System::Drawing::Point(16, 32);
			this->HitSound_comboBox->Name = L"HitSound_comboBox";
			this->HitSound_comboBox->Size = System::Drawing::Size(190, 20);
			this->HitSound_comboBox->Sorted = true;
			this->HitSound_comboBox->TabIndex = 0;
			this->HitSound_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &AnimationHitSoundTabPage::HitSound_comboBox_SelectedIndexChanged);
			this->HitSound_comboBox->TextChanged += gcnew System::EventHandler(this, &AnimationHitSoundTabPage::HitSound_comboBox_TextChanged);
			// 
			// OK_button
			// 
			this->OK_button->Location = System::Drawing::Point(74, 68);
			this->OK_button->Name = L"OK_button";
			this->OK_button->Size = System::Drawing::Size(75, 23);
			this->OK_button->TabIndex = 1;
			this->OK_button->Text = L"적용";
			this->OK_button->UseVisualStyleBackColor = true;
			this->OK_button->Click += gcnew System::EventHandler(this, &AnimationHitSoundTabPage::OK_button_Click);
			// 
			// AnimationName_label
			// 
			this->AnimationName_label->AutoSize = true;
			this->AnimationName_label->Location = System::Drawing::Point(14, 9);
			this->AnimationName_label->Name = L"AnimationName_label";
			this->AnimationName_label->Size = System::Drawing::Size(0, 12);
			this->AnimationName_label->TabIndex = 2;
			// 
			// AnimationHitSoundTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(231, 112);
			this->ControlBox = false;
			this->Controls->Add(this->AnimationName_label);
			this->Controls->Add(this->OK_button);
			this->Controls->Add(this->HitSound_comboBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"AnimationHitSoundTabPage";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"AnimationHitSoundTabPage";
			this->Load += gcnew System::EventHandler(this, &AnimationHitSoundTabPage::AnimationHitSoundTabPage_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void HitSoundAddbutton_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void AnimationHitSoundTabPage_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void HitSound_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void HitSound_comboBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void OK_button_Click(System::Object^  sender, System::EventArgs^  e);
};
}
