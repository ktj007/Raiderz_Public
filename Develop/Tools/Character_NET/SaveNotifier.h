#pragma once

public ref class SaveNotifier
{
public: SaveNotifier(void) { Init();}

private: System::Windows::Forms::ToolStripStatusLabel^  TalentPosInfoStatusLabel;
private: System::Windows::Forms::ToolStripStatusLabel^  TalentExtStatusLabel;
private: System::Windows::Forms::ToolStripStatusLabel^  TalentEffectStatusLabel;
private: System::Windows::Forms::ToolStripStatusLabel^  TalentHitInfoStatusLabel;
private: System::Windows::Forms::ToolStripStatusLabel^  MeshInfoStatusLabel;
private: System::Windows::Forms::ToolStripStatusLabel^  ChangeMtrlItemStatusLabel;
private: System::Windows::Forms::ToolStripStatusLabel^  ChangeMtrlNPCStatusLabel;
private: System::Windows::Forms::ToolStripStatusLabel^  AnimaionLabel;
private: System::Windows::Forms::ToolStripStatusLabel^  MeshColLabel;

protected:
		bool	m_bSaveTalentTime;
		bool	m_bSaveTalentEvent;
		bool	m_bSaveTalentEffect;
		bool	m_bSaveTalentHitInfo;
		bool	m_bSaveTalentPosInfo;
		bool	m_bSaveMeshInfo;
		bool	m_bSaveChangeMtrlItem;
		bool	m_bSaveChangeMtrlNPC;
		bool	m_bSaveAnimation;

		void	SaveTalentPosInfo();
		void	SaveMeshInfo();
		void	SaveChangeMtrlItem();
		void	SaveChangeMtrlNPC();
		void	SaveAnimation();

public: System::Windows::Forms::ToolStripStatusLabel^ RefreshSaveNoticeLabel(System::Windows::Forms::ToolStripStatusLabel^ label, bool bSave, System::String^ Text);
		System::Void SaveChangeMtrlNPCNotice( bool bsave );
		System::Void SaveChangeMtrlItemNotice( bool bsave );
		System::Void SaveMeshInfoNotice( bool bsave );
		System::Void SaveTalentHitInfoNotice( bool bsave );
		System::Void SaveTalentEffectNotice( bool bsave );
		System::Void SaveTalentExtNotice(bool bsave);
		System::Void SaveTalentPosInfoNotice(bool bsave);
		System::Void SaveAnimationNotice( bool bsave );

public:
		void	Init();

		void	SaveAll();

		bool	SaveTalentEventInfo();
		bool	SaveTalentHitInfo();
		bool	SaveTalentEffectInfo();

		void	SetSaveTalentPosInfo( bool val );
		void	SetSaveMeshInfo( bool val );
		void	SetSaveChangeMtrlItem( bool val );
		void	SetSaveChangeMtrlNPC( bool val );
		void	SetSaveTalentEvent( bool val );
		void	SetSaveTalentTime( bool val );
		void	SetSaveTalentEffect( bool val );
		void	SetSaveTalentHitInfo( bool val );
		void	SetSaveAnimation(bool val);

		bool	IsSaveDone();

		void	ResetSaveAll();
		void	ResetModelSave();

		void	SaveCeckSum();
};
