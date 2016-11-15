#pragma once
#include "VL_ListenableView.h"
#include "RTypes.h"
#include "M_CollisionTree.h"

#include "afxwin.h"
#include "resource.h"

// CRollupEtcCollision 대화 상자입니다.
/** @brief Map View Rollup */
class CORETOOL_API CRollupEtcCollision : public CDialog, public CListenableViewImpl_Rollup<CRollupEtcCollision>
{
	DECLARE_DYNAMIC(CRollupEtcCollision)

public:
						CRollupEtcCollision(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual				~CRollupEtcCollision();						///< 소멸자

	// Listenable View
	virtual void		Listen(CoreMessage& _message);			///< Listen
	virtual const char*	GetViewName(){ return "Collision"; }		///< View Name

	void				SetCollisionFileName( const string& strFileName );
	string				GetCollisionFileName() { return m_strCollisionFileName; }
	CCollisionTree&		GetCollisionTree();


// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ETC_COLLISION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	// cl2 view toggle
	CButton m_ctlCl2ViewBtn;
	void Cl2ViewToggle();
	bool CreateCl2(const char* _pSzFileName);
	afx_msg void OnBnClickedCl2ViewBtn();
	afx_msg void OnBnClickedButtonCreateCl2As();

protected:
	string			m_strCollisionFileName;
	CCollisionTree	m_collisionTree;
};

inline CCollisionTree&	CRollupEtcCollision::GetCollisionTree() { return m_collisionTree; }
