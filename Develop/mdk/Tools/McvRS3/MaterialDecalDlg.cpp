// MaterialDecalDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MaterialDecalDlg.h"


// MaterialDecalDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(MaterialDecalDlg, CDialog)

MaterialDecalDlg::MaterialDecalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MaterialDecalDlg::IDD, pParent)
{
}

MaterialDecalDlg::~MaterialDecalDlg()
{
}

void MaterialDecalDlg::InitTexDescControl(TextureDescControl& control, const RTextureDesc& desc)
{
	char strDefaultValue[10];

	sprintf_s(strDefaultValue, 10, "%d", desc.nMapChannel);
	control.ctrlMapchannel.SetWindowText(strDefaultValue);

	sprintf_s(strDefaultValue, 10, "%.3f", desc.UVTransform.GetOffset().x);
	control.ctrlUVOffsetX.SetWindowText(strDefaultValue);
	sprintf_s(strDefaultValue, 10, "%.3f", desc.UVTransform.GetOffset().y);
	control.ctrlUVOffsetY.SetWindowText(strDefaultValue);

	sprintf_s(strDefaultValue, 10, "%.3f", desc.UVTransform.GetScale().x);
	control.ctrlUVScaleX.SetWindowText(strDefaultValue);
	sprintf_s(strDefaultValue, 10, "%.3f", desc.UVTransform.GetScale().y);
	control.ctrlUVScaleY.SetWindowText(strDefaultValue);

	control.ctrlUVAddress.SetCurSel(2);
}

void MaterialDecalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MATERIALINDEX, m_DiffuseTexControl.ctrlMaterialName);

	DDX_Control(pDX, IDC_TEXTURENAME, m_DiffuseTexControl.ctrlTexture);
	DDX_Control(pDX, IDC_MAPCHANNEL, m_DiffuseTexControl.ctrlMapchannel);
	DDX_Control(pDX, IDC_UVOFFSETX, m_DiffuseTexControl.ctrlUVOffsetX);
	DDX_Control(pDX, IDC_UVOFFSETY, m_DiffuseTexControl.ctrlUVOffsetY);
	DDX_Control(pDX, IDC_UVSCALEX, m_DiffuseTexControl.ctrlUVScaleX);
	DDX_Control(pDX, IDC_UVSCALEY, m_DiffuseTexControl.ctrlUVScaleY);
	DDX_Control(pDX, IDC_UVADDRESS, m_DiffuseTexControl.ctrlUVAddress);

	m_DiffuseTexControl.ctrlUVAddress.Clear();
	m_DiffuseTexControl.ctrlUVAddress.InsertString(0, "Wrap");
	m_DiffuseTexControl.ctrlUVAddress.InsertString(1, "Mirror");
	m_DiffuseTexControl.ctrlUVAddress.InsertString(2, "Clamp");

	DDX_Control(pDX, IDC_TEXTURENAME2, m_MaskTexControl.ctrlTexture);
	DDX_Control(pDX, IDC_MAPCHANNEL2, m_MaskTexControl.ctrlMapchannel);
	DDX_Control(pDX, IDC_UVOFFSETX2, m_MaskTexControl.ctrlUVOffsetX);
	DDX_Control(pDX, IDC_UVOFFSETY2, m_MaskTexControl.ctrlUVOffsetY);
	DDX_Control(pDX, IDC_UVSCALEX2, m_MaskTexControl.ctrlUVScaleX);
	DDX_Control(pDX, IDC_UVSCALEY2, m_MaskTexControl.ctrlUVScaleY);
	DDX_Control(pDX, IDC_UVADDRESS2, m_MaskTexControl.ctrlUVAddress);

	m_MaskTexControl.ctrlUVAddress.Clear();
	m_MaskTexControl.ctrlUVAddress.InsertString(0, "Wrap");
	m_MaskTexControl.ctrlUVAddress.InsertString(1, "Mirror");
	m_MaskTexControl.ctrlUVAddress.InsertString(2, "Clamp");


	InitTexDescControl(m_DiffuseTexControl, m_DiffuseTexDesc);
	InitTexDescControl(m_MaskTexControl, m_MaskTexDesc);
}


BEGIN_MESSAGE_MAP(MaterialDecalDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &MaterialDecalDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_MATERIALINDEX, &MaterialDecalDlg::OnEnChangeMaterialName)
	ON_EN_CHANGE(IDC_TEXTURENAME, &MaterialDecalDlg::OnEnChangeTexturename)
	ON_EN_CHANGE(IDC_UVOFFSETX, &MaterialDecalDlg::OnEnChangeUvoffsetx)
	ON_EN_CHANGE(IDC_UVOFFSETY, &MaterialDecalDlg::OnEnChangeUvoffsety)
	ON_EN_CHANGE(IDC_UVSCALEX, &MaterialDecalDlg::OnEnChangeUvscalex)
	ON_EN_CHANGE(IDC_UVSCALEY, &MaterialDecalDlg::OnEnChangeUvscaley)
	ON_BN_CLICKED(IDC_BUTTON2, &MaterialDecalDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_MAPCHANNEL, &MaterialDecalDlg::OnEnChangeMapchannel)
	ON_CBN_SELCHANGE(IDC_UVADDRESS, &MaterialDecalDlg::OnCbnSelchangeUvaddress)
	ON_EN_CHANGE(IDC_TEXTURENAME2, &MaterialDecalDlg::OnEnChangeTexturename2)
	ON_EN_CHANGE(IDC_MAPCHANNEL2, &MaterialDecalDlg::OnEnChangeMapchannel2)
	ON_EN_CHANGE(IDC_UVOFFSETX2, &MaterialDecalDlg::OnEnChangeUvoffsetx2)
	ON_EN_CHANGE(IDC_UVOFFSETY2, &MaterialDecalDlg::OnEnChangeUvoffsety2)
	ON_EN_CHANGE(IDC_UVSCALEX2, &MaterialDecalDlg::OnEnChangeUvscalex2)
	ON_EN_CHANGE(IDC_UVSCALEY2, &MaterialDecalDlg::OnEnChangeUvscaley2)
	ON_CBN_SELCHANGE(IDC_UVADDRESS2, &MaterialDecalDlg::OnCbnSelchangeUvaddress2)
END_MESSAGE_MAP()

void MaterialDecalDlg::SetUVOffsetX(RTextureDesc& desc, float fValue)
{
	RVector2 vOffset = desc.UVTransform.GetOffset(); 
	vOffset.x = fValue;
	desc.UVTransform.SetOffset(vOffset);
}

void MaterialDecalDlg::SetUVOffsetY(RTextureDesc& desc, float fValue)
{
	RVector2 vOffset = desc.UVTransform.GetOffset(); 
	vOffset.y = fValue;
	desc.UVTransform.SetOffset(vOffset);
}

void MaterialDecalDlg::SetUVScaleX(RTextureDesc& desc, float fValue)
{
	RVector2 vScale = desc.UVTransform.GetScale(); 
	vScale.x = fValue;
	desc.UVTransform.SetScale(vScale); 
}

void MaterialDecalDlg::SetUVScaleY(RTextureDesc& desc, float fValue)
{
	RVector2 vScale = desc.UVTransform.GetScale(); 
	vScale.y = fValue;
	desc.UVTransform.SetScale(vScale);
}

// MaterialDecalDlg 메시지 처리기입니다.

void MaterialDecalDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if(GetMain()->m_pActor)
	{
		RTextureDesc* pDiffuseTexDesc = m_DiffuseTexDesc.TextureName.empty() ? NULL : &m_DiffuseTexDesc;
		RTextureDesc* pMaskTexDesc = m_MaskTexDesc.TextureName.empty() ? NULL : &m_MaskTexDesc;

		GetMain()->m_pActor->GetMaterialDecal()->AddDecalLayer(m_strMaterialName, pDiffuseTexDesc, pMaskTexDesc);
	}
}

void MaterialDecalDlg::OnEnChangeMaterialName()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strMaterial;
	m_DiffuseTexControl.ctrlMaterialName.GetWindowText(strMaterial);
	m_strMaterialName = strMaterial;
}

void MaterialDecalDlg::OnEnChangeTexturename()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strTexture;
	m_DiffuseTexControl.ctrlTexture.GetWindowText(strTexture);
	m_DiffuseTexDesc.TextureName = strTexture;
}

void MaterialDecalDlg::OnEnChangeUvoffsetx()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strOffsetX;
	m_DiffuseTexControl.ctrlUVOffsetX.GetWindowText(strOffsetX);

	SetUVOffsetX(m_DiffuseTexDesc, atof(strOffsetX));
}

void MaterialDecalDlg::OnEnChangeUvoffsety()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strOffsetY;
	m_DiffuseTexControl.ctrlUVOffsetY.GetWindowText(strOffsetY);

	SetUVOffsetY(m_DiffuseTexDesc, atof(strOffsetY));
}

void MaterialDecalDlg::OnEnChangeUvscalex()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strScaleX;
	m_DiffuseTexControl.ctrlUVScaleX.GetWindowText(strScaleX);

	SetUVScaleX(m_DiffuseTexDesc, atof(strScaleX));
}

void MaterialDecalDlg::OnEnChangeUvscaley()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strScaleY;
	m_DiffuseTexControl.ctrlUVScaleY.GetWindowText(strScaleY);

	SetUVScaleY(m_DiffuseTexDesc, atof(strScaleY));
}

void MaterialDecalDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(GetMain()->m_pActor)
		GetMain()->m_pActor->GetMaterialDecal()->RemoveDecalLayer(m_strMaterialName);
}

void MaterialDecalDlg::OnEnChangeMapchannel()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strMapchannel;
	m_DiffuseTexControl.ctrlMapchannel.GetWindowText(strMapchannel);
	m_DiffuseTexDesc.nMapChannel = _ttoi(strMapchannel);
}

void MaterialDecalDlg::OnCbnSelchangeUvaddress()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nAddress = m_DiffuseTexControl.ctrlUVAddress.GetCurSel();

	RTEXTUREADDRESS UVAddress;

	switch(nAddress)
	{
	case 0:
		UVAddress = RTADDRESS_WRAP;
		break;
	case 1:
		UVAddress = RTADDRESS_MIRROR;
		break;
	case 2:
		UVAddress = RTADDRESS_CLAMP;
		break;
	}

	m_DiffuseTexDesc.textureAddressU = UVAddress;
	m_DiffuseTexDesc.textureAddressV = UVAddress;
}

void MaterialDecalDlg::OnEnChangeTexturename2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTexture;
	m_MaskTexControl.ctrlTexture.GetWindowText(strTexture);
	m_MaskTexDesc.TextureName = strTexture;
}

void MaterialDecalDlg::OnEnChangeMapchannel2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strMapchannel;
	m_MaskTexControl.ctrlMapchannel.GetWindowText(strMapchannel);
	m_MaskTexDesc.nMapChannel = _ttoi(strMapchannel);
}

void MaterialDecalDlg::OnEnChangeUvoffsetx2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strOffsetX;
	m_MaskTexControl.ctrlUVOffsetX.GetWindowText(strOffsetX);

	SetUVOffsetX(m_MaskTexDesc, atof(strOffsetX));
}

void MaterialDecalDlg::OnEnChangeUvoffsety2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strOffsetY;
	m_MaskTexControl.ctrlUVOffsetY.GetWindowText(strOffsetY);

	SetUVOffsetY(m_MaskTexDesc, atof(strOffsetY));
}

void MaterialDecalDlg::OnEnChangeUvscalex2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strScaleX;
	m_MaskTexControl.ctrlUVScaleX.GetWindowText(strScaleX);

	SetUVScaleX(m_MaskTexDesc, atof(strScaleX));
}

void MaterialDecalDlg::OnEnChangeUvscaley2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strScaleY;
	m_MaskTexControl.ctrlUVScaleY.GetWindowText(strScaleY);

	SetUVScaleY(m_MaskTexDesc, atof(strScaleY));
}

void MaterialDecalDlg::OnCbnSelchangeUvaddress2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nAddress = m_MaskTexControl.ctrlUVAddress.GetCurSel();

	RTEXTUREADDRESS UVAddress;

	switch(nAddress)
	{
	case 0:
		UVAddress = RTADDRESS_WRAP;
		break;
	case 1:
		UVAddress = RTADDRESS_MIRROR;
		break;
	case 2:
		UVAddress = RTADDRESS_CLAMP;
		break;
	}

	m_MaskTexDesc.textureAddressU = UVAddress;
	m_MaskTexDesc.textureAddressV = UVAddress;
}
