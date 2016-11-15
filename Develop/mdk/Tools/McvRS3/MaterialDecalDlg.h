#pragma once
#include "afxcmn.h"

// MaterialDecalDlg 대화 상자입니다.

struct TextureDescControl
{
	CEdit ctrlMaterialName;
	CEdit ctrlTexture;
	CEdit ctrlMapchannel;
	CEdit ctrlUVOffsetX;
	CEdit ctrlUVOffsetY;
	CEdit ctrlUVScaleX;
	CEdit ctrlUVScaleY;
	CComboBox ctrlUVAddress;
};

class MaterialDecalDlg : public CDialog
{
	DECLARE_DYNAMIC(MaterialDecalDlg)

public:
	MaterialDecalDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~MaterialDecalDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MATERIALDECAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	void InitTexDescControl(TextureDescControl& control, const RTextureDesc& desc);
	void SetUVOffsetX(RTextureDesc& desc, float fValue);
	void SetUVOffsetY(RTextureDesc& desc, float fValue);
	void SetUVScaleX(RTextureDesc& desc, float fValue);
	void SetUVScaleY(RTextureDesc& desc, float fValue);

	std::string m_strMaterialName;

	TextureDescControl m_DiffuseTexControl;
	TextureDescControl m_MaskTexControl;

	RTextureDesc m_DiffuseTexDesc;
	RTextureDesc m_MaskTexDesc;


public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeMaterialName();
	afx_msg void OnEnChangeTexturename();
	afx_msg void OnEnChangeUvoffsetx();
	afx_msg void OnEnChangeUvoffsety();
	afx_msg void OnEnChangeUvscalex();
	afx_msg void OnEnChangeUvscaley();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeMapchannel();
	afx_msg void OnCbnSelchangeUvaddress();
	afx_msg void OnEnChangeTexturename2();
	afx_msg void OnEnChangeMapchannel2();
	afx_msg void OnEnChangeUvoffsetx2();
	afx_msg void OnEnChangeUvoffsety2();
	afx_msg void OnEnChangeUvscalex2();
	afx_msg void OnEnChangeUvscaley2();
	afx_msg void OnCbnSelchangeUvaddress2();
};
