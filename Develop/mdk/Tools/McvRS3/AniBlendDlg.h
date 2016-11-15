#pragma once


// CAniBlendDlg 대화 상자입니다.

#include "GridCtrl_src/GridCtrl.h"

namespace rs3
{
	class RMeshAnimationMgr;
};

class CAniBlendDlg : public CDialog
{
	DECLARE_DYNAMIC(CAniBlendDlg)

public:
	CAniBlendDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAniBlendDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EDIT_ANIBLEND };

	void ClearAniBlendGrid();
	void ResetAniBlendGrid( RMeshAnimationMgr* animationMgr, const map< string, string>* pExternAni =NULL);
	void ChangeAnimationBlendType( RANIMATIONTRANSITIONTYPE blendType );
	void ChangeAnimationBlendTypeAllTarget( RANIMATIONTRANSITIONTYPE blendType );
	void ChangeAnimationBlendDuration();
	BLENDDATA GetAnimationTransData( const char* source, const char* target );

private:
	void RefreshGridData( BLENDDATA blendData, int row, int col );

	CGridCtrl				m_Grid;
	RMeshAnimationMgr*		m_AnimationMgr;

	static const UINT m_sCellTextFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
