#pragma once

#include "VL_ListenableView.h"
#include "afxwin.h"
//#include "RDecalSceneNode.h"

namespace rs3{ class RDecalSceneNode;; };

// CRollupEtcDecal 대화 상자입니다.
/** @brief Decal Pilot Rollup */
enum DECAL_TYPE
{
	DECAL_TYPE_PROJECTION = 0,
	DECAL_TYPE_POLYGON
};

class CORETOOL_API CRollupEtcDecal : public CDialog, public CListenableViewImpl_Rollup<CRollupEtcDecal>
{
	DECLARE_DYNAMIC(CRollupEtcDecal)

public:
						CRollupEtcDecal(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual				~CRollupEtcDecal();						///< 소멸자

	// Listenable View
	virtual void		Listen(CoreMessage& _message);		///< Listen
	virtual const char*	GetViewName(){ return "Decal Test"; }	///< View Name

	virtual BOOL		OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ETC_DECAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual void OnCancel(){}
	virtual void OnOK(){}

	DECLARE_MESSAGE_MAP()

public:
	bool			m_bCreate;	// 1 : 생성 상태인가, 0 : 삭제 상태인가
	DECAL_TYPE		m_eDecalType;

	float 			m_fDecalSize_X;
	float 			m_fDecalSize_Y;
	float 			m_fDecalSize_Z;

	float			m_fDecalRotate_Z;

	CString			m_strTextureFileName;

	afx_msg void OnBnClickedDecalCreate_Projection();
	afx_msg void OnBnClickedDecalCreate_Polygon();
	afx_msg void OnBnClickedDecalClear();	// 삭제

	afx_msg void OnChangeDecalSizeX();
	afx_msg void OnChangeDecalSizeY();
	afx_msg void OnChangeDecalSizeZ();
	afx_msg void OnChangeDecalRotateZ();

	afx_msg void OnBnClickedTextureOpen();
};
