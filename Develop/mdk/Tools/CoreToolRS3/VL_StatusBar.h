/// @file VL_StatusBar.h
#pragma once

#include "VL_ListenableView.h"
#include "ext_lib/InitDialogBar.h"
#include "resource.h"
#include "V_MFCAnchorSupport.h"
#include "V_EditCtrl.h"

/**
	@brief StatusBar
	상태표시줄
*/

class CORETOOL_API CCoreToolStatusBar : public CMFCStatusBar, public CListenableViewImpl_Fixed<CCoreToolStatusBar>
{
	DECLARE_DYNAMIC(CCoreToolStatusBar)

public:
							CCoreToolStatusBar(CWnd* pParent = NULL);		///< 표준 생성자입니다.
	virtual					~CCoreToolStatusBar();						///< 소멸자

	BOOL					Create(CWnd* pWnd);
	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "StatusBar"; }			///< View Name

	void					SetPaneText(LPCTSTR lpszNewText);

	enum { IDD = 0 };		// dummy for compile, TODO: 삭제

protected:
	virtual void	OnUpdateCmdUI(CCmdUI* pCmdUI); ///< 툴바의 컨트롤을 활성화 하기위한 함수
	virtual BOOL	OnInitStatusBar(); ///< Self-Drawing Button 기능을 이용하기 위해 생성시에 호출하는 메소드

	void			OnSelectionChanged();	///< 선택이 변경되었을때
	void			OnEnterPosEditCtrl();	///< 위치 콘트롤에 enter 입력
	void			OnEnterScaleEditCtrl();	///< 스케일 콘트롤에 enter 입력

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	CCoreToolEdit			m_editControls[4];	// pos x,y,z, scale
};