#pragma once
#include "VL_ListenableView.h"
#include "afxwin.h"

namespace rs3{ class RToolTerrain; class RDeviceD3D; struct RTextureInfo; };
//class rs3::RToolTerrain;
//class rs3::RDeviceD3D;
//struct rs3::RTextureInfo;

// CRollupTerrainSplat 대화 상자입니다.
/** @brief Terrain - Splatting Rollup */
class CORETOOL_API CRollupTerrainSplat : public CDialog, public CListenableViewImpl_Rollup<CRollupTerrainSplat>
{
	DECLARE_DYNAMIC(CRollupTerrainSplat)

public:
							CRollupTerrainSplat(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual					~CRollupTerrainSplat();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "Texture Brush"; }			///< View Name

	// Dialog
	virtual BOOL			OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_TERRAIN_SPLAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	int SelectLayer( int nLayer, bool bTileSelected);
	int	GetSelectedLayer();
	bool IsTileSelected();
	void UpdateLayerList(int _nSelectIndexAfterUpdate = -1, bool _bTilemapSelectedAfterUpdate = true);
	void InsertLayerListNormalmap( int nIdx, int nLayer);
	void InsertLayerListTilemap( int nIdx, int nLayer);
	void SelchangeListNormalmap( int nLayer, rs3::RToolTerrain* pTerrain, CDC *pDC, HDC hDC, RECT rt, rs3::RDeviceD3D *pd3ddevice);
	void SelchangeListTilemap( int nLayer, rs3::RToolTerrain* pTerrain, CDC *pDC, HDC hDC, RECT rt, rs3::RDeviceD3D *pd3ddevice);
	void SetTileInfoText( const rs3::RTextureInfo* pTexInfo);
	bool IsAlphaTexture( const rs3::RTextureInfo* pTexInfo);
	void CheckAlphaMaskForSpecular();
	void CheckUseNormalmap();
	void DeleteNormalMap( int n);
	void DeleteLayer( int n); 

	virtual void OnCancel();
	virtual void OnOK();
	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//afx_msg void OnPaint();
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	CString m_cstrViewSize;
	float m_fEditSize;
	CSliderCtrl m_sliderCtrlSize;
	float m_fStr;
	CSliderCtrl m_sliderCtrlStr;
	float m_fTileU, m_fTileV;
	int m_nCurrentLayer;

	bool m_bMarkingLayer;

	CCheckListBox m_ctlLayer;

	CEdit m_ctlTileInfo;
	CButton m_ctlAlphaChannelView;

	afx_msg void OnBnClickedButtonNewLayer();
	afx_msg void OnBnClickedButtonChangeLayer();
	afx_msg void OnBnClickedButtonDeleteLayer();
	afx_msg void OnLbnSelchangeListTile();
	afx_msg void OnCheckChangeListTile();

	afx_msg void OnEnChangeEditUV();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();

private:
	rs3::RToolTerrain* m_pTerrain_;
	CSpinButtonCtrl m_ctrlSpinU_;
	CSpinButtonCtrl m_ctrlSpinV_;

	DWORD m_dwLayerWidth;
	DWORD m_dwLayerHeight;
	afx_msg void OnBnClickedButtonResizeUp();
	afx_msg void OnBnClickedButtonResizeDown();

public:
	CButton m_ctlLayerMarking;

	afx_msg void OnBnClickedCheckUseMarking();
	afx_msg void OnBnClickedLayerRebuild();
	afx_msg void OnBnClickButtonAlphaView();
	afx_msg void OnStnClickedStaticSpec();
};
