#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>

#include "TTalentInfo.h"

class TPartsHitInfoViewer;

class TTalentHitTabController
{
private:
	bool				m_bFromSetSegmentParam;
	bool				m_bHitCapsuleAllView;

	int					m_nSegID;
	int					m_nCapsuleID;

	vector<int>			m_vecShowCapsuleID;

	float				m_fPrePlayTime;
	bool				m_bHitCapsuleAniSync;

	struct CapsuleInfo
	{
		int			skillid;
		int			segid;
		int			capsuleid;
		RCapsule	capsule;	//top, bottom, height, rad
		bool		bDohitCheck;

		float		fCheckTime;
		int			nAddtiveDamage;
		float		fHitMotionDelay;
	};

	typedef std::deque<CapsuleInfo*> CAPSULEINFOQUEUE;
	CAPSULEINFOQUEUE	m_queueCapsuleInfo;
private:
	void DeleteSelSegment();
	void DeleteSelCapsule();
	void AddSegment();
	void DeleteSegment(int segID);
	int AddEmptySegment(CSTalentInfo* pTalentInfo);

	void UpdateTreeView();
	void UpdateControlsEnabled();
	void UpdateReferenceControls();
	void UpdateDereferenceControls();

public:
	TTalentHitTabController();
	~TTalentHitTabController();
	void Init ();
	void Play(float fTime);
	void Render();

	void Clear();
	void Serialize();		// 실제 판정 정보로부터 View를 갱신

	void UpdateTalentHitEffectDir(CSTalentInfo* pTalentInfo, bool bCheck = false);
	void UpdateTalentHitEffectDir(CSTalentInfo* pTalentInfo, int nSegID = -1);
	//조절점1
	void ApplySegmentParams(int segID);
	//조절점2
	void OnChangeSegmentTop(int segID);
	void SetCapsuleProperty(int segID, int capsuleID);

	void AddCapsule();
	void DeleteCapsule(int segID, int capsuleID);

	void MoveSegmentBottom(int dx, int dy, int dz);

	void MoveSegmentTop(int dx, int dy, int dz);

	void AddDo();
	void AddReDo();

	void CopyCapsule();
	void PasteCapsule();


	int GetSegID() { return m_nSegID; }
	int GetCapsuleID() { return m_nCapsuleID; }

	void UndoCommand();
	void RedoCommand();
	void ClearQueue();
	void SetDo(CapsuleInfo* capsuleinfo);


	void ClearSel();
	void OnChangeSelSegmentTop();
	void SetSelCapsuleProperty();

	// Form1으로부터의 이벤트
	void OnAddSegmentClick();
	void OnDeleteSegmentClick();
	void OnAddCapsuleClick();
	void OnHitTreeViewAfterSelect(System::Windows::Forms::TreeViewEventArgs^  e);
	void OnHitReferenceTextChanged(System::Object^  sender);
	void OnHitReferenceTextChanged(int nRefTalentID, int nRefTalentMode);
	void OnAniSync_checkBox_CheckedChanged(bool bCheck);
	void OnHitTreeViewAllSelect(bool bCheck);
	void OnDeDereferenceTalentEffectIDViewer();
};



