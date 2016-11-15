#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "ProgressBarForm.h"

namespace Character_NET {

	/// <summary>
	/// NPCAnimationInfoSetting에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class NPCAnimationInfoSetting : public System::Windows::Forms::Form
	{
	private:
		String^	m_strSelectNPCName;

		ProgressBarForm	m_copyProgressBar;

		TCharacter*		m_phfPlayerModel;
		TCharacter*		m_phmPlayerModel;

		float	m_fCurrentProgressBarState;
		int		m_nCopyFullAniCount;

	private:
		void StartCopyAnimationInfo(string strNPCModelName, vector<SIMPLEANIMATION_INFO>& vecCopyAnimationList);
		int CollectionCopyNpcAnimationList(string strNpcName, vector<SIMPLEANIMATION_INFO>& vecCopyAniList, vector<string>& vecOutResult);
		void CalProgressBar();

		bool LoadNpcAnimationInfo(RMeshAnimationMgr& npcAnimationEventInfo, TAnimationInfoMgr& npcAnimationInfoMgr, string& strNPCAnimationFilePath, string& strNPCAnimationEventFileName, vector<SIMPLEANIMATION_INFO>& vecCopyAnimationList);
		void SaveNpcAnimationInfo(RMeshAnimationMgr& npcAnimationEventInfo, TAnimationInfoMgr& npcAnimationInfoMgr, string& strNPCAnimationEventFileName, vector<SIMPLEANIMATION_INFO>& vecCopyAnimationList);

		void CopyAnimationInfo(TCharacter* pCopyModel, vector<SIMPLEANIMATION_INFO>& vecCopySource, vector<SIMPLEANIMATION_INFO>& vecTargetSource, RMeshAnimationMgr& npcAnimationEventInfo, TAnimationInfoMgr& npcAnimationInfoMgr);
		void CopyAnimationEvent( TCharacter* pCopyModel, RMeshAnimationMgr &npcAnimationEventInfo, string& strSourceAnimationName, string& strTargetAnimationName );
		void CopyAnimationBlendInfo( TCharacter* pCopyModel, TAnimationInfoMgr &npcAnimationInfoMgr, string& strTargetAnimationName, vector<SIMPLEANIMATION_INFO>& vecTargetSource );
		void CopyAnimationOverlayInfo( TCharacter* pCopyModel, TAnimationInfoMgr &npcAnimationInfoMgr, string& strSourceAnimationName, string& strTargetAnimationName );
		void CopyAnimationReActionInfo( TCharacter* pCopyModel, TAnimationInfoMgr &npcAnimationInfoMgr, string& strSourceAnimationName, string& strTargetAnimationName );
		void CopyAnimationHitSound( TCharacter* pCopyModel, TAnimationInfoMgr &npcAnimationInfoMgr, string& strSourceAnimationName, string& strTargetAnimationName );
		
		void CopyAnimationSwordTrailEvent(RAnimationEvent& AniEvent, string& strTargetAnimationName);
		bool DoNotCopyAnimationEvent(RAnimationEvent* pAniEvent);


		RAnimation* GetNpcAnimation(RAnimationHashList& npcAnimationList, string strAniName);

	public:
		NPCAnimationInfoSetting(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//

			m_phfPlayerModel = NULL;
			m_phmPlayerModel = NULL;

			m_fCurrentProgressBarState = 0.0f;
			m_nCopyFullAniCount = 0;
		}

		void SetSelectNpcName(String^ strName);
		void Init(TCharacter* phf, TCharacter* phm);

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~NPCAnimationInfoSetting()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 


	private: System::Windows::Forms::Button^  cancle_button;
	private: System::Windows::Forms::ListBox^  npc_listBox;



	private: System::Windows::Forms::Button^  copy_button;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ListBox^  logOut_listBox;




	private: System::Windows::Forms::Label^  label3;

	private: System::Windows::Forms::GroupBox^  groupBox2;

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
			this->cancle_button = (gcnew System::Windows::Forms::Button());
			this->npc_listBox = (gcnew System::Windows::Forms::ListBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->copy_button = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->logOut_listBox = (gcnew System::Windows::Forms::ListBox());
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// cancle_button
			// 
			this->cancle_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->cancle_button->Location = System::Drawing::Point(260, 423);
			this->cancle_button->Name = L"cancle_button";
			this->cancle_button->Size = System::Drawing::Size(75, 23);
			this->cancle_button->TabIndex = 3;
			this->cancle_button->Text = L"취소";
			this->cancle_button->UseVisualStyleBackColor = true;
			this->cancle_button->Click += gcnew System::EventHandler(this, &NPCAnimationInfoSetting::cancle_button_Click);
			// 
			// npc_listBox
			// 
			this->npc_listBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->npc_listBox->FormattingEnabled = true;
			this->npc_listBox->HorizontalScrollbar = true;
			this->npc_listBox->ItemHeight = 12;
			this->npc_listBox->Location = System::Drawing::Point(6, 21);
			this->npc_listBox->Name = L"npc_listBox";
			this->npc_listBox->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
			this->npc_listBox->Size = System::Drawing::Size(452, 220);
			this->npc_listBox->Sorted = true;
			this->npc_listBox->TabIndex = 4;
			this->npc_listBox->DoubleClick += gcnew System::EventHandler(this, &NPCAnimationInfoSetting::npc_listBox_DoubleClick);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->ForeColor = System::Drawing::Color::Red;
			this->label3->Location = System::Drawing::Point(12, 282);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(295, 12);
			this->label3->TabIndex = 9;
			this->label3->Text = L"(주의) Player 모델 애니메이션 정보를 복사를 합니다.";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->npc_listBox);
			this->groupBox2->Location = System::Drawing::Point(6, 12);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(464, 260);
			this->groupBox2->TabIndex = 11;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"복사 할 NPC 리스트";
			// 
			// copy_button
			// 
			this->copy_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->copy_button->Location = System::Drawing::Point(147, 423);
			this->copy_button->Name = L"copy_button";
			this->copy_button->Size = System::Drawing::Size(75, 23);
			this->copy_button->TabIndex = 12;
			this->copy_button->Text = L"복사";
			this->copy_button->UseVisualStyleBackColor = true;
			this->copy_button->Click += gcnew System::EventHandler(this, &NPCAnimationInfoSetting::copy_button_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->ForeColor = System::Drawing::Color::Red;
			this->label1->Location = System::Drawing::Point(12, 303);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(317, 12);
			this->label1->TabIndex = 13;
			this->label1->Text = L"복사 대상자 이전 애니메이션 정보가 모두 삭제가 됩니다. ";
			// 
			// logOut_listBox
			// 
			this->logOut_listBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->logOut_listBox->BackColor = System::Drawing::Color::Gainsboro;
			this->logOut_listBox->FormattingEnabled = true;
			this->logOut_listBox->ItemHeight = 12;
			this->logOut_listBox->Location = System::Drawing::Point(6, 324);
			this->logOut_listBox->Name = L"logOut_listBox";
			this->logOut_listBox->Size = System::Drawing::Size(464, 88);
			this->logOut_listBox->TabIndex = 14;
			// 
			// NPCAnimationInfoSetting
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(482, 458);
			this->Controls->Add(this->logOut_listBox);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->copy_button);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->cancle_button);
			this->Name = L"NPCAnimationInfoSetting";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"NPCAnimationInfoSetting";
			this->Load += gcnew System::EventHandler(this, &NPCAnimationInfoSetting::NPCAnimationInfoSetting_Load);
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void NPCAnimationInfoSetting_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void copy_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void cancle_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void npc_listBox_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
			 copy_button_Click(sender, e);
		 }
};
}
