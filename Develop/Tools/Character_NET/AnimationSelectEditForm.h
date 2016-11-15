#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// AnimationSelectEditForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class AnimationSelectEditForm : public System::Windows::Forms::Form
	{
	private:
		System::Object^	m_selectAnimation;

	private:
		void			FindAnimation(System::String^ strAnimation);
		bool			FindEditAnimation(System::String^ strAnimation);

		bool			AddEditAnimation(System::String^ strAnimation);
		void			DelEditAnimation();

	public:
		AnimationSelectEditForm(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

		void			SetEditAnimationList(vector<string>& vecEditAnimation);
		void			GetEditAnimationList(vector<string>& vecOutEditAnimation);

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~AnimationSelectEditForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  Edit_listBox;
	protected: 

	private: System::Windows::Forms::TextBox^  AnimationFind_textBox;
	private: System::Windows::Forms::ListBox^  Animation_listBox;
	private: System::Windows::Forms::Button^  OK_button;
	private: System::Windows::Forms::Button^  Cancel_button;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;



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
			this->Edit_listBox = (gcnew System::Windows::Forms::ListBox());
			this->AnimationFind_textBox = (gcnew System::Windows::Forms::TextBox());
			this->Animation_listBox = (gcnew System::Windows::Forms::ListBox());
			this->OK_button = (gcnew System::Windows::Forms::Button());
			this->Cancel_button = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// Edit_listBox
			// 
			this->Edit_listBox->AllowDrop = true;
			this->Edit_listBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->Edit_listBox->FormattingEnabled = true;
			this->Edit_listBox->ItemHeight = 12;
			this->Edit_listBox->Location = System::Drawing::Point(9, 16);
			this->Edit_listBox->Name = L"Edit_listBox";
			this->Edit_listBox->Size = System::Drawing::Size(249, 148);
			this->Edit_listBox->Sorted = true;
			this->Edit_listBox->TabIndex = 0;
			this->Edit_listBox->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &AnimationSelectEditForm::Edit_listBox_KeyUp);
			// 
			// AnimationFind_textBox
			// 
			this->AnimationFind_textBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->AnimationFind_textBox->Location = System::Drawing::Point(7, 19);
			this->AnimationFind_textBox->Name = L"AnimationFind_textBox";
			this->AnimationFind_textBox->Size = System::Drawing::Size(252, 21);
			this->AnimationFind_textBox->TabIndex = 1;
			this->AnimationFind_textBox->TextChanged += gcnew System::EventHandler(this, &AnimationSelectEditForm::AnimationFind_textBox_TextChanged);
			// 
			// Animation_listBox
			// 
			this->Animation_listBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->Animation_listBox->FormattingEnabled = true;
			this->Animation_listBox->ItemHeight = 12;
			this->Animation_listBox->Location = System::Drawing::Point(7, 50);
			this->Animation_listBox->Name = L"Animation_listBox";
			this->Animation_listBox->Size = System::Drawing::Size(252, 220);
			this->Animation_listBox->Sorted = true;
			this->Animation_listBox->TabIndex = 2;
			this->Animation_listBox->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &AnimationSelectEditForm::Animation_listBox_MouseDoubleClick);
			// 
			// OK_button
			// 
			this->OK_button->Location = System::Drawing::Point(42, 466);
			this->OK_button->Name = L"OK_button";
			this->OK_button->Size = System::Drawing::Size(75, 23);
			this->OK_button->TabIndex = 3;
			this->OK_button->Text = L"OK";
			this->OK_button->UseVisualStyleBackColor = true;
			this->OK_button->Click += gcnew System::EventHandler(this, &AnimationSelectEditForm::OK_button_Click);
			// 
			// Cancel_button
			// 
			this->Cancel_button->Location = System::Drawing::Point(151, 466);
			this->Cancel_button->Name = L"Cancel_button";
			this->Cancel_button->Size = System::Drawing::Size(75, 23);
			this->Cancel_button->TabIndex = 4;
			this->Cancel_button->Text = L"Cancel";
			this->Cancel_button->UseVisualStyleBackColor = true;
			this->Cancel_button->Click += gcnew System::EventHandler(this, &AnimationSelectEditForm::Cancel_button_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->Animation_listBox);
			this->groupBox1->Controls->Add(this->AnimationFind_textBox);
			this->groupBox1->Location = System::Drawing::Point(2, 4);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(268, 279);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"애니메이션 리스트";
			// 
			// groupBox2
			// 
			this->groupBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox2->Controls->Add(this->Edit_listBox);
			this->groupBox2->Location = System::Drawing::Point(3, 291);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(266, 169);
			this->groupBox2->TabIndex = 6;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"선택된 애니메이션";
			// 
			// AnimationSelectEditForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(273, 501);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->Cancel_button);
			this->Controls->Add(this->OK_button);
			this->Name = L"AnimationSelectEditForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"AnimationSelectEditForm";
			this->Load += gcnew System::EventHandler(this, &AnimationSelectEditForm::AnimationSelectEditForm_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void OK_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->DialogResult = System::Windows::Forms::DialogResult::OK;
			 this->Close();
		 }
private: System::Void Cancel_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void AnimationFind_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			 FindAnimation(AnimationFind_textBox->Text);
		 }
private: System::Void Animation_listBox_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void Edit_listBox_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if ( e->KeyCode == Keys::Delete )
				DelEditAnimation();
		 }
private: System::Void AnimationSelectEditForm_Load(System::Object^  sender, System::EventArgs^  e) {
			 FindAnimation(nullptr);
		 }
private: System::Void Edit_listBox_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
		 }
};
}
