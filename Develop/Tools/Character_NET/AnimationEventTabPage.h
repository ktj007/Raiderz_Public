#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Character_NET {

	/// <summary>
	/// AnimationEventTabPage에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class AnimationEventTabPage : public System::Windows::Forms::Form
	{
	public:
		AnimationEventTabPage(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//

			m_bEnableCheck = false;

			m_bViewSwordEvent	= false;
			m_bViewOtherEvent	= false;
			m_bViewSoundEvent	= false;
			m_bViewFaceEvent	= false;
			m_bViewAllEvent		= true;

			UpdateViewEventButton();

		}

		void	AllChecked(bool bChecked);
		void	CheckedEvent(int nIndex, bool bChecked);
		void	GetCheckedEventList(vector<bool>& vecbChecked);
		bool	GetCheckedEvent(int nIndex);
	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~AnimationEventTabPage()
		{
			if (components)
			{
				delete components;
			}
		}
		bool m_bEnableCheck;

	private: System::Windows::Forms::Button^  EventAddbutton;
	public: System::Windows::Forms::CheckedListBox^  Event_checkedListBox;
	private: System::Windows::Forms::Button^  AllShowbutton;
	public: 
	private: System::Windows::Forms::Button^  Facebutton;
	private: System::Windows::Forms::Button^  Soundbutton;
	private: System::Windows::Forms::Button^  SwordTrailbutton;
	private: System::Windows::Forms::Button^  Otherbutton;

	private: 

	private: 
	protected: 

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

	public:
		bool							m_bViewSoundEvent;
		bool							m_bViewFaceEvent;
		bool							m_bViewSwordEvent;
		bool							m_bViewOtherEvent;
		bool							m_bViewAllEvent;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->EventAddbutton = (gcnew System::Windows::Forms::Button());
			this->Event_checkedListBox = (gcnew System::Windows::Forms::CheckedListBox());
			this->AllShowbutton = (gcnew System::Windows::Forms::Button());
			this->Facebutton = (gcnew System::Windows::Forms::Button());
			this->Soundbutton = (gcnew System::Windows::Forms::Button());
			this->SwordTrailbutton = (gcnew System::Windows::Forms::Button());
			this->Otherbutton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// EventAddbutton
			// 
			this->EventAddbutton->Location = System::Drawing::Point(1, 10);
			this->EventAddbutton->Name = L"EventAddbutton";
			this->EventAddbutton->Size = System::Drawing::Size(51, 23);
			this->EventAddbutton->TabIndex = 2;
			this->EventAddbutton->Text = L"Add";
			this->EventAddbutton->UseVisualStyleBackColor = true;
			this->EventAddbutton->Click += gcnew System::EventHandler(this, &AnimationEventTabPage::EventAddbutton_Click);
			// 
			// Event_checkedListBox
			// 
			this->Event_checkedListBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->Event_checkedListBox->FormattingEnabled = true;
			this->Event_checkedListBox->Location = System::Drawing::Point(58, 3);
			this->Event_checkedListBox->Name = L"Event_checkedListBox";
			this->Event_checkedListBox->Size = System::Drawing::Size(192, 164);
			this->Event_checkedListBox->TabIndex = 3;
			this->Event_checkedListBox->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &AnimationEventTabPage::Event_checkedListBox_MouseDoubleClick);
			this->Event_checkedListBox->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &AnimationEventTabPage::Event_checkedListBox_MouseeClick);
			this->Event_checkedListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &AnimationEventTabPage::Event_checkedListBox_SelectedIndexChanged);
			this->Event_checkedListBox->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, &AnimationEventTabPage::Event_checkedListBox_ItemCheck);
			this->Event_checkedListBox->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &AnimationEventTabPage::Event_checkedListBox_KeyUp);
			// 
			// AllShowbutton
			// 
			this->AllShowbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AllShowbutton->Location = System::Drawing::Point(1, 145);
			this->AllShowbutton->Name = L"AllShowbutton";
			this->AllShowbutton->Size = System::Drawing::Size(51, 23);
			this->AllShowbutton->TabIndex = 4;
			this->AllShowbutton->Text = L"ALL";
			this->AllShowbutton->UseVisualStyleBackColor = true;
			this->AllShowbutton->Click += gcnew System::EventHandler(this, &AnimationEventTabPage::AllShowbutton_Click);
			// 
			// Facebutton
			// 
			this->Facebutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Facebutton->Location = System::Drawing::Point(1, 118);
			this->Facebutton->Name = L"Facebutton";
			this->Facebutton->Size = System::Drawing::Size(51, 23);
			this->Facebutton->TabIndex = 5;
			this->Facebutton->Text = L"Face";
			this->Facebutton->UseVisualStyleBackColor = true;
			this->Facebutton->Click += gcnew System::EventHandler(this, &AnimationEventTabPage::Facebutton_Click);
			// 
			// Soundbutton
			// 
			this->Soundbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Soundbutton->Location = System::Drawing::Point(1, 91);
			this->Soundbutton->Name = L"Soundbutton";
			this->Soundbutton->Size = System::Drawing::Size(51, 23);
			this->Soundbutton->TabIndex = 6;
			this->Soundbutton->Text = L"Sound";
			this->Soundbutton->UseVisualStyleBackColor = true;
			this->Soundbutton->Click += gcnew System::EventHandler(this, &AnimationEventTabPage::Soundbutton_Click);
			// 
			// SwordTrailbutton
			// 
			this->SwordTrailbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->SwordTrailbutton->Location = System::Drawing::Point(1, 64);
			this->SwordTrailbutton->Name = L"SwordTrailbutton";
			this->SwordTrailbutton->Size = System::Drawing::Size(51, 23);
			this->SwordTrailbutton->TabIndex = 7;
			this->SwordTrailbutton->Text = L"Sword";
			this->SwordTrailbutton->UseVisualStyleBackColor = true;
			this->SwordTrailbutton->Click += gcnew System::EventHandler(this, &AnimationEventTabPage::SwordTrailbutton_Click);
			// 
			// Otherbutton
			// 
			this->Otherbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Otherbutton->Location = System::Drawing::Point(1, 38);
			this->Otherbutton->Name = L"Otherbutton";
			this->Otherbutton->Size = System::Drawing::Size(51, 23);
			this->Otherbutton->TabIndex = 8;
			this->Otherbutton->Text = L"Other...";
			this->Otherbutton->UseVisualStyleBackColor = true;
			this->Otherbutton->Click += gcnew System::EventHandler(this, &AnimationEventTabPage::Otherbutton_Click);
			// 
			// AnimationEventTabPage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(253, 172);
			this->ControlBox = false;
			this->Controls->Add(this->Otherbutton);
			this->Controls->Add(this->SwordTrailbutton);
			this->Controls->Add(this->Soundbutton);
			this->Controls->Add(this->Facebutton);
			this->Controls->Add(this->AllShowbutton);
			this->Controls->Add(this->Event_checkedListBox);
			this->Controls->Add(this->EventAddbutton);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"AnimationEventTabPage";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"AnimationEventTabPage";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void EventAddbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->m_TabAnimation.AddEvent();
			 }
	private: System::Void Event_checkedListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 g_pMainApp->m_TabAnimation.SelectedEventList(Event_checkedListBox->SelectedIndex);
			 }
	private: System::Void Event_checkedListBox_ItemCheck(System::Object^  sender, System::Windows::Forms::ItemCheckEventArgs^  e) {
				 int nCheck = ((System::Windows::Forms::CheckedListBox^)sender)->SelectedIndex;
				 CheckState state = e->NewValue;
				 if(m_bEnableCheck == false)
				 {
					 e->NewValue = e->CurrentValue;
				 }

				 m_bEnableCheck = false;
				 }
	private: System::Void Event_checkedListBox_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 // 이벤트 delete키로 삭제하기
				 if ( e->KeyCode == Keys::Delete )
				 {
					 int sel = Event_checkedListBox->SelectedIndex;

					 g_pMainApp->m_TabAnimation.DeleteEvent(sel);
				 }
			 }
	private: System::Void Event_checkedListBox_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 int count = ((System::Windows::Forms::CheckedListBox^)sender)->SelectedIndex;
				 if( count > -1 && e->X > 12)
				 {
					 g_pMainApp->m_TabAnimation.ModifyEvent(count);
				 }
			 }
	private: System::Void Event_checkedListBox_MouseeClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				m_bEnableCheck = false;
				if(e->X < 12)
				{
					m_bEnableCheck = true;
				}
			 }
	private: System::Void Soundbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 m_bViewSoundEvent = true;
				 m_bViewFaceEvent = false;
				 m_bViewAllEvent = false;
				 m_bViewSwordEvent = false;
				 m_bViewOtherEvent	= false;

				 UpdateViewEventButton();
			 }
	private: System::Void Facebutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 m_bViewSoundEvent = false;
				 m_bViewFaceEvent = true;
				 m_bViewAllEvent = false;
				 m_bViewSwordEvent = false;
				 m_bViewOtherEvent	= false;

				 UpdateViewEventButton();
			 }
	private: System::Void AllShowbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 m_bViewSoundEvent = false;
				 m_bViewFaceEvent = false;
				 m_bViewAllEvent = true;
				 m_bViewSwordEvent = false;
				 m_bViewOtherEvent	= false;

				 UpdateViewEventButton();
			 }
	 public: System::Void UpdateViewEventButton()
			 {
				AllShowbutton->Enabled = true;
				Facebutton->Enabled = true;
				Soundbutton->Enabled = true;
				SwordTrailbutton->Enabled = true;
				Otherbutton->Enabled = true;

				if(m_bViewAllEvent)
					AllShowbutton->Enabled = false;
				else if(m_bViewFaceEvent)
					Facebutton->Enabled = false;
				else if(m_bViewSoundEvent)
					Soundbutton->Enabled = false;	
				else if(m_bViewSwordEvent)
					SwordTrailbutton->Enabled = false;
				else if(m_bViewOtherEvent)
					Otherbutton->Enabled = false;

				g_pMainApp->m_TabAnimation.ResetAnimationEventList();
			 }
private: System::Void SwordTrailbutton_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_bViewSoundEvent = false;
			 m_bViewFaceEvent = false;
			 m_bViewAllEvent = false;
			 m_bViewSwordEvent = true;
			 m_bViewOtherEvent	= false;

			 UpdateViewEventButton();
		 }
private: System::Void Otherbutton_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_bViewSoundEvent = false;
			 m_bViewFaceEvent = false;
			 m_bViewAllEvent = false;
			 m_bViewSwordEvent = false;
			 m_bViewOtherEvent	= true;

			 UpdateViewEventButton();
		 }
};
}
