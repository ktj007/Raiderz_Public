#pragma once

#include "VL_ListenableView.h"
#include "afxwin.h"
#include <string>

// CRollupEtcCamera 대화 상자입니다.
/** @brief Camera Editing Rollup */
class CORETOOL_API CRollupEtcCamera : public CDialog, public CListenableViewImpl_Rollup<CRollupEtcCamera>
{
	DECLARE_DYNAMIC(CRollupEtcCamera)

public:
						CRollupEtcCamera(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual				~CRollupEtcCamera();					///< 소멸자

	// Listenable View
	virtual void		Listen(CoreMessage& _message);		///< Listen
	virtual const char*	GetViewName(){ return "Camera Animation"; }	///< View Name

	void				UpdateAndRenderAddedCamera();			///< 현재 편집 가능한 카메라 목록을 워크밴치에 랜더링

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ETC_CAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual void OnCancel(){}
	virtual void OnOK(){}

	DECLARE_MESSAGE_MAP()

private:
	CListBox	m_ctlCamListBox;
	int			m_nCameraCount;

	CButton		m_ctlViewAddedCamera;
	CButton		m_ctlCheckAniApplyToCam;
	CButton		m_ctlCheckToOrg;

	std::string	m_strCameraAnimationFile;

public:
	afx_msg void OnBnClickedButtonAddCamera();
	afx_msg void OnBnClickedButtonApplyCamAni();
	afx_msg void OnBnClickedButtonPlayCamAni();
	afx_msg void OnBnClickedButtonStopCamAni();
	afx_msg void OnBnClickedButtonAddedCamDelete();
};
