#pragma once
#include "VL_ListenableView.h"
#include <vector>
#include <string>
#include <map>
#include "RTypes.h"
#include "afxcmn.h"
#include "RDeviceD3D.h"

struct SPieceGrassInfo
{
	SPieceGrassInfo(void) : m_nNumber(2), m_nSize(100){}
	~SPieceGrassInfo(void){}
	std::string m_strPieceGrassName;
	int m_nNumber;
	int m_nSize;
};

namespace rs3
{
	class RToolTerrain;
	class RGrassSystem;
};

// CRollupTerrainGrass 대화 상자입니다.
/** @brief Terrain - Grass Rollup */
class CORETOOL_API CRollupTerrainGrass : public CDialog, public CListenableViewImpl_Rollup<CRollupTerrainGrass>
{
	DECLARE_DYNAMIC(CRollupTerrainGrass)

public:
							CRollupTerrainGrass(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual					~CRollupTerrainGrass();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "Grass Brush"; }		///< View Name

	// Dialog
	virtual BOOL			OnInitDialog();
	virtual void			OnCancel();
	virtual void			OnOK();

	afx_msg void OnBnClickedButtonGrassOnTerrain();
	void ViewToggleGrass();
	BOOL GetViewStateGrass(){return !m_nCheck_; }

	enum ADD_GRASS_TYPE
	{
		LB_DOWN = 0,
		LB_DRAG
	};

	//public member
	//Slider - Working Range
	CString m_cstrViewSize;
	float m_fEditSize;
	CSliderCtrl m_sliderCtrlSize;

	//strength (= density)
	float m_fStr;
	CSliderCtrl m_sliderCtrlStr;

	//grass space
	int m_nSpace;
	CSliderCtrl m_sliderCtrlSpace;

	//radio - type
	//int m_nAddOrDelete;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_TERRAIN_GRASS };
public:
	void GetCurrentPieceGrassVector(std::vector<SPieceGrassInfo*>& _refPieceGrassVec);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonGrassListEdit();
	afx_msg void OnTvnSelchangedTreeGrassBrush(NMHDR *pNMHDR, LRESULT *pResult);
	/*afx_msg void OnBnClickedRadioGrassAdd();
	afx_msg void OnBnClickedRadioGrassDelete();*/

	afx_msg void OnBnClickedCheckGrassLod();
	afx_msg void OnBnClickedButtonGrassAdd();
	afx_msg void OnBnClickedButtonPGrassAdd();
	afx_msg void OnBnClickedButtonGrassDelete();
	afx_msg void OnBnClickedButtonGrassSync();
	
	afx_msg void OnEnChangeEditNumberSize();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
	void DisableControl_();
	void EnableControlAfterResourceSetted_();
	void ResetTreeWithResource_();
	typedef std::vector<std::string> STRING_CONTAINER;
	void VerifyTreeInfoAndGrassListInfo_(CTreeCtrl& _refTreeCtrl, STRING_CONTAINER& _refVecStr);

	CTreeCtrl m_ctrlGrassBrushTree_;
	HTREEITEM m_currentTreeitemHandle_;
	HTREEITEM m_beforeTreeitemHandle_;
	int m_nEditPieceSize_;
	int m_nEditPieceNum_;
	CSpinButtonCtrl m_ctrlSpinSize_;
	CSpinButtonCtrl m_ctrlSpinNum_;

	rs3::RToolTerrain* m_pTerrain_;
	rs3::RGrassSystem* m_pGrassSystem_;

	int m_nCheck_;
};
