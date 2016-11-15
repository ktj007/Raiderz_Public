#include "stdafx.h"

#include "ModelInfoDlg.h"

using namespace rs3;

IMPLEMENT_DYNAMIC(CModelInfoDlg, CDialog)
CModelInfoDlg::CModelInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelInfoDlg::IDD, pParent)
{
	m_nSelCount = 0;

//	memset( m_pMtrlTable, 0 , sizeof(RMtrl*)*MAX_MTRL_TABLE );
}

CModelInfoDlg::~CModelInfoDlg()
{
}

void CModelInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MESHNODE, m_ListMeshNode);
	DDX_Control(pDX, IDC_LIST_MTRLNODE, m_ListMtrlNode);
	DDX_Control(pDX, IDC_COMBO_TEX_FILTER,  m_TexFilterCombo);
	DDX_Control(pDX, IDC_COMBO_TEX_BLEND,   m_TexBlendCombo);
	DDX_Control(pDX, IDC_COMBO_TOON_FILTER, m_ToonFilterCombo);
	DDX_Control(pDX, IDC_COMBO_TOON_BLEND,  m_ToonBlendCombo);
}


BEGIN_MESSAGE_MAP(CModelInfoDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MESHNODE_COLOR, OnBnClickedButtonMeshnodeColor)
	ON_BN_CLICKED(IDC_BUTTON_MTRLNODE_COLOR, OnBnClickedButtonMtrlnodeColor)
	ON_BN_CLICKED(IDC_TOON_FILE, OnBnClickedToonFile)
	ON_CBN_SELCHANGE(IDC_COMBO_TEX_FILTER, OnCbnSelchangeComboTexFilter)
	ON_CBN_SELCHANGE(IDC_COMBO_TEX_BLEND, OnCbnSelchangeComboTexBlend)
	ON_CBN_SELCHANGE(IDC_COMBO_TOON_FILTER, OnCbnSelchangeComboToonFilter)
	ON_CBN_SELCHANGE(IDC_COMBO_TOON_BLEND, OnCbnSelchangeComboToonBlend)
	ON_LBN_SELCHANGE(IDC_LIST_MTRLNODE, OnLbnSelchangeListMtrlnode)
END_MESSAGE_MAP()


// CModelInfoDlg 메시지 처리기입니다.

void  CModelInfoDlg::ClearListBox()
{
	int cnt,i;

	cnt = m_ListMeshNode.GetCount();

	for( i=0;i<cnt;i++ ) {
		m_ListMeshNode.DeleteString(0);
	}

	cnt = m_ListMtrlNode.GetCount();

	for( i=0;i<cnt;i++ ) {
		m_ListMtrlNode.DeleteString(0);
	}

	// combo

	cnt = m_TexFilterCombo.GetCount();

	for( i=0;i<cnt;i++ ) {
		m_TexFilterCombo.DeleteString(0);
	}

	cnt = m_TexBlendCombo.GetCount();

	for( i=0;i<cnt;i++ ) {
		m_TexBlendCombo.DeleteString(0);
	}

	cnt = m_ToonFilterCombo.GetCount();

	for( i=0;i<cnt;i++ ) {
		m_ToonFilterCombo.DeleteString(0);
	}

	cnt = m_ToonBlendCombo.GetCount();

	for( i=0;i<cnt;i++ ) {
		m_ToonBlendCombo.DeleteString(0);
	}

}

void  CModelInfoDlg::DataUpdate()
{
	// texture

	m_TexFilterCombo.AddString("Point");
	m_TexFilterCombo.AddString("Linear");

	m_TexFilterCombo.SetCurSel(0);

	m_TexBlendCombo.AddString("Modulate");
	m_TexBlendCombo.AddString("Modulatex2");
	m_TexBlendCombo.AddString("Modulatex4");
	m_TexBlendCombo.AddString("BlendTextureAlpha");

	m_TexBlendCombo.SetCurSel(3);

	// toon texture

	m_ToonFilterCombo.AddString("Point");
	m_ToonFilterCombo.AddString("Linear");

	m_ToonFilterCombo.SetCurSel(0);

	m_ToonBlendCombo.AddString("Modulate");
	m_ToonBlendCombo.AddString("Modulatex2");
	m_ToonBlendCombo.AddString("Modulatex4");
	m_ToonBlendCombo.AddString("Addsigned");
	m_ToonBlendCombo.AddString("Addsignedx2");
	m_ToonBlendCombo.AddString("Add");
	m_ToonBlendCombo.AddString("Subtract");
	m_ToonBlendCombo.AddString("AddSmooth");

	m_ToonBlendCombo.SetCurSel(1);
}

void  CModelInfoDlg::Begin()
{
	ClearListBox();

	DataUpdate();

	m_ListMeshNode.SetSel(0);
	m_ListMtrlNode.SetSel(0);

	RActor* pActor = GetMainView()->GetActor();

	if(pActor) {
/*
		RMeshNode* pMNode	= NULL;
		RMtrl* pMtrl		= NULL;

		static char temp[256];

		for(int i=0;i<pActor->m_pMesh->m_nDataNum;i++)
		{
			pMNode = pActor->m_pMesh->m_NodeTable[i];

			if(pMNode) {
				m_ListMeshNode.AddString( pMNode->GetName() );
			}
		}

		RMtrlMgr* pMtrlMgr = &pActor->m_pMesh->m_MtrlMgr;

		int mtrl_size = pMtrlMgr->size();

		for( i=0;i<mtrl_size;i++) {

			pMtrl = pMtrlMgr->m_NodeTable[i];

			if(pMtrl) {
				if( !pMtrl->m_TextureName.empty() ){
					sprintf(temp,"%s",pMtrl->GetName());
					m_ListMtrlNode.AddString( temp );
				}
			}
		}
*/
	}
}

void  CModelInfoDlg::End()
{

}

void CModelInfoDlg::OnBnClickedButtonMeshnodeColor()
{
	CColorDialog dlg;
	COLORREF col;

	BYTE a,r,g,b;

	CString str;

	if( dlg.DoModal()==IDOK ) {

		col = dlg.GetColor();

		a = (col>>24)&0xff;
		b = (col>>16)&0xff;
		g = (col>> 8)&0xff;
		r = (col    )&0xff;

		col = D3DCOLOR_ARGB(255,r,g,b);

		RActor* pActor = GetMainView()->GetActor();

		int sel = m_ListMeshNode.GetCurSel();
/*
		m_ListMeshNode.GetText(sel,str);

		RMeshNode* pMeshNode = pActor->m_pMesh->GetMeshData((char*)str.operator const char*());

		if( pMeshNode ) {
			pMeshNode->SetTexColor((DWORD)col);
		}
*/
	}
}

void CModelInfoDlg::OnBnClickedButtonMtrlnodeColor()
{
	CColorDialog dlg;
	COLORREF col;

	BYTE a,r,g,b;

	if( dlg.DoModal()==IDOK ) {

		col = dlg.GetColor();

		a = (col>>24)&0xff;
		b = (col>>16)&0xff;
		g = (col>> 8)&0xff;
		r = (col    )&0xff;

		col = D3DCOLOR_ARGB(255,r,g,b);
/*
		RActor* pActor = GetActor();

		if(!pActor) return;

		int sel = m_ListMtrlNode.GetCurSel();

		m_ListMtrlNode.GetText(sel,str);

		RMtrlMgr* pMtrlMgr = &pActor->m_pMesh->m_MtrlMgr;

		RMtrl* pMtrl = pMtrlMgr->GetToolMtrl((char*)str.operator const char*());
*/
//		RMtrl* pMtrl = GetCurrentSelectMtrl();

//		if(pMtrl) {
//			pMtrl->SetTColor((DWORD)col);
//		}
/*
		GetSelectMtrl();

		for(int i=0;i<m_nSelCount;i++ ) {

			m_pMtrlTable[i]->SetTexColor((DWORD)col);
		}
*/
	}
}

DWORD String2DWORD(char* str)
{
	DWORD hr = 0;
/*
		 if(!strcmp(str,"Point"))				hr = (DWORD)D3DTEXF_POINT;
	else if(!strcmp(str,"Linear"))				hr = (DWORD)D3DTEXF_LINEAR;
	else if(!strcmp(str,"Modulate"))			hr = (DWORD)D3DTOP_MODULATE;
	else if(!strcmp(str,"Modulatex2"))			hr = D3DTOP_MODULATE2X;
	else if(!strcmp(str,"Modulatex4"))			hr = D3DTOP_MODULATE4X;
	else if(!strcmp(str,"Addsigned"))			hr = D3DTOP_ADDSIGNED;
	else if(!strcmp(str,"Addsignedx2"))			hr = D3DTOP_ADDSIGNED2X;
	else if(!strcmp(str,"Add"))					hr = D3DTOP_ADD;
	else if(!strcmp(str,"Subtract"))			hr = D3DTOP_SUBTRACT;
	else if(!strcmp(str,"AddSmooth"))			hr = D3DTOP_ADDSMOOTH;
	else if(!strcmp(str,"BlendTextureAlpha"))	hr = D3DTOP_BLENDTEXTUREALPHA;
*/
	return hr;
}

void CModelInfoDlg::GetSelectMtrl()
{
/*
	CString str;

	RActor* pActor = GetMainView()->GetActor();

	if(!pActor) return;

	m_nSelCount = m_ListMtrlNode.GetSelCount();
	int nItemCount = m_ListMtrlNode.GetCount();

	if(nItemCount > MAX_MTRL_TABLE - 1)
		nItemCount = MAX_MTRL_TABLE - 1;
*/
/*
	RMtrlMgr* pMtrlMgr = &pActor->m_pMesh->m_MtrlMgr;

	int nPos = 0;

	for(int i=0;i<nItemCount;i++) {
		
		if(m_ListMtrlNode.GetSel(i)) {
		
			m_ListMtrlNode.GetText(i,str);

			m_pMtrlTable[nPos++] = pMtrlMgr->GetToolMtrl((char*)str.operator const char*());
		}
	}
*/
}
/*
RMtrl* CModelInfoDlg::GetCurrentSelectMtrl()
{
	CString str;

	RActor* pActor = GetMainView()->GetActor();

	if(!pActor) return NULL;

	int sel = m_ListMtrlNode.GetCurSel();

	if(sel==-1) return NULL;

	m_ListMtrlNode.GetText(sel,str);

	RMtrlMgr* pMtrlMgr = &pActor->m_pMesh->m_MtrlMgr;

	RMtrl* pMtrl = pMtrlMgr->GetToolMtrl((char*)str.operator const char*());
	
	return pMtrl;
}
*/
void CModelInfoDlg::OnBnClickedToonFile()
{
	CString t_str;

	CFileDialog dlg( TRUE,_T("bmp"),_T("*.bmp"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("BMP File (test.bmp) |*.bmp| JPG File (test.jpg)|*.jpg| PNG File (test.png)|*.png|"));

	if( dlg.DoModal()==IDOK ) {

		t_str = dlg.GetPathName();

//		RBaseTexture* pTex = RCreateBaseTexture(t_str.operator const char*(),RTextureType_Etc,false,false);

//		RMtrl* pMtrl = GetCurrentSelectMtrl();

//		if(pMtrl) {
//			pMtrl->m_pToonTexture = pTex;
//		}

/*		GetSelectMtrl();

		for(int i=0;i<m_nSelCount;i++ ) {
			m_pMtrlTable[i]->m_pToonTexture = pTex;
		}
*/
	}
}

void CModelInfoDlg::OnCbnSelchangeComboTexFilter()
{
//	RMtrl* pMtrl = GetCurrentSelectMtrl();

//	if(pMtrl) {

//		D3DTEXTUREFILTERTYPE type = D3DTEXF_LINEAR;

//		int sel = m_TexFilterCombo.GetCurSel();

//		if(sel==0) type = D3DTEXF_POINT;

//		pMtrl->m_FilterType = type;
//	}
/*
	int sel = 0;

	GetSelectMtrl();

	for(int i=0;i<m_nSelCount;i++ ) {

		D3DTEXTUREFILTERTYPE type = D3DTEXF_LINEAR;

		sel = m_TexFilterCombo.GetCurSel();

		if(sel==0) type = D3DTEXF_POINT;

		m_pMtrlTable[i]->m_FilterType = type;
	}
*/
}

void CModelInfoDlg::OnCbnSelchangeComboTexBlend()
{
//	RMtrl* pMtrl = GetCurrentSelectMtrl();

//	if(pMtrl) {
		
//		D3DTEXTUREOP op;

//		int sel = m_TexBlendCombo.GetCurSel();

//		     if(sel==0) op = D3DTOP_MODULATE;
//		else if(sel==1) op = D3DTOP_MODULATE2X;
//		else if(sel==2) op = D3DTOP_MODULATE4X;
//		else if(sel==3) op = D3DTOP_BLENDTEXTUREALPHA;

//		pMtrl->m_TextureBlendMode = op;
//	}
/*
	int sel;

	GetSelectMtrl();

	for(int i=0;i<m_nSelCount;i++ ) {

		D3DTEXTUREOP op;

		sel = m_TexBlendCombo.GetCurSel();

			 if(sel==0) op = D3DTOP_MODULATE;
		else if(sel==1) op = D3DTOP_MODULATE2X;
		else if(sel==2) op = D3DTOP_MODULATE4X;
		else if(sel==3) op = D3DTOP_BLENDTEXTUREALPHA;

		m_pMtrlTable[i]->m_TextureBlendMode = op;

	}
*/
}

void CModelInfoDlg::OnCbnSelchangeComboToonFilter()
{
//	RMtrl* pMtrl = GetCurrentSelectMtrl();

//	if(pMtrl) {

//		D3DTEXTUREFILTERTYPE type = D3DTEXF_LINEAR;

//		int sel = m_ToonFilterCombo.GetCurSel();

//		if(sel==0) type = D3DTEXF_POINT;

//		pMtrl->m_ToonFilterType = type;
//	}
/*
	int sel;

	GetSelectMtrl();

	for(int i=0;i<m_nSelCount;i++ ) {

		D3DTEXTUREFILTERTYPE type = D3DTEXF_LINEAR;

		sel = m_ToonFilterCombo.GetCurSel();

		if(sel==0) type = D3DTEXF_POINT;

		m_pMtrlTable[i]->m_ToonFilterType = type;
	}
*/
}

void CModelInfoDlg::OnCbnSelchangeComboToonBlend()
{
//	RMtrl* pMtrl = GetCurrentSelectMtrl();

//	if(pMtrl) {

//		D3DTEXTUREOP op;

//		int sel = m_ToonBlendCombo.GetCurSel();

//		     if(sel==0) op = D3DTOP_MODULATE;
//		else if(sel==1) op = D3DTOP_MODULATE2X;
//		else if(sel==2) op = D3DTOP_MODULATE4X;
//		else if(sel==3) op = D3DTOP_ADDSIGNED;
//		else if(sel==4) op = D3DTOP_ADDSIGNED2X;
//		else if(sel==5) op = D3DTOP_ADD;
//		else if(sel==6) op = D3DTOP_SUBTRACT;
//		else if(sel==7) op = D3DTOP_ADDSMOOTH;

//		pMtrl->m_ToonTextureBlendMode = op;
//	}
/*
	GetSelectMtrl();

	for(int i=0;i<m_nSelCount;i++ ) {

		D3DTEXTUREOP op;

		int sel = m_ToonBlendCombo.GetCurSel();

			 if(sel==0) op = D3DTOP_MODULATE;
		else if(sel==1) op = D3DTOP_MODULATE2X;
		else if(sel==2) op = D3DTOP_MODULATE4X;
		else if(sel==3) op = D3DTOP_ADDSIGNED;
		else if(sel==4) op = D3DTOP_ADDSIGNED2X;
		else if(sel==5) op = D3DTOP_ADD;
		else if(sel==6) op = D3DTOP_SUBTRACT;
		else if(sel==7) op = D3DTOP_ADDSMOOTH;

		m_pMtrlTable[i]->m_ToonTextureBlendMode = op;
	}
*/
}

void CModelInfoDlg::OnLbnSelchangeListMtrlnode()
{
/*
	RMtrl* pMtrl = GetCurrentSelectMtrl();

	D3DTEXTUREFILTERTYPE type = pMtrl->m_FilterType;

	D3DTEXTUREOP op = pMtrl->m_TextureBlendMode;

	int sel = 0;

	     if(type == D3DTEXF_POINT)  sel = 0;
	else if(type == D3DTEXF_LINEAR) sel = 1;

	m_TexFilterCombo.SetCurSel(sel);

		 if(op == D3DTOP_MODULATE )			sel = 0;
	else if(op == D3DTOP_MODULATE2X)		sel = 1;
	else if(op == D3DTOP_MODULATE4X)		sel = 2;
	else if(op == D3DTOP_BLENDTEXTUREALPHA) sel = 3;

	m_TexBlendCombo.SetCurSel(sel);

	// toon texture

	type = pMtrl->m_ToonFilterType;

	     if(type == D3DTEXF_POINT)  sel = 0;
	else if(type == D3DTEXF_LINEAR) sel = 1;

	m_ToonFilterCombo.SetCurSel(sel);

	op = pMtrl->m_ToonTextureBlendMode;

	     if(op == D3DTOP_MODULATE )			sel = 0;
	else if(op == D3DTOP_MODULATE2X)		sel = 1;
	else if(op == D3DTOP_MODULATE4X)		sel = 2;
	else if(op == D3DTOP_ADDSIGNED)			sel = 3;
	else if(op == D3DTOP_ADDSIGNED2X)		sel = 4;
	else if(op == D3DTOP_ADD)				sel = 5;
	else if(op == D3DTOP_SUBTRACT)			sel = 6;
	else if(op == D3DTOP_ADDSMOOTH)			sel = 7;

	m_ToonBlendCombo.SetCurSel(sel);
*/
/*
	RMtrl* pMtrl;

	GetSelectMtrl();

	for(int i=0;i<m_nSelCount;i++ ) {

		pMtrl = m_pMtrlTable[i];

		D3DTEXTUREFILTERTYPE type = pMtrl->m_FilterType;

		D3DTEXTUREOP op = pMtrl->m_TextureBlendMode;

		int sel = 0;

		if(type == D3DTEXF_POINT)  sel = 0;
		else if(type == D3DTEXF_LINEAR) sel = 1;

		m_TexFilterCombo.SetCurSel(sel);

			 if(op == D3DTOP_MODULATE )			sel = 0;
		else if(op == D3DTOP_MODULATE2X)		sel = 1;
		else if(op == D3DTOP_MODULATE4X)		sel = 2;
		else if(op == D3DTOP_BLENDTEXTUREALPHA) sel = 3;

		m_TexBlendCombo.SetCurSel(sel);

		// toon texture

		type = pMtrl->m_ToonFilterType;

		if(type == D3DTEXF_POINT)  sel = 0;
		else if(type == D3DTEXF_LINEAR) sel = 1;

		m_ToonFilterCombo.SetCurSel(sel);

		op = pMtrl->m_ToonTextureBlendMode;

			 if(op == D3DTOP_MODULATE )			sel = 0;
		else if(op == D3DTOP_MODULATE2X)		sel = 1;
		else if(op == D3DTOP_MODULATE4X)		sel = 2;
		else if(op == D3DTOP_ADDSIGNED)			sel = 3;
		else if(op == D3DTOP_ADDSIGNED2X)		sel = 4;
		else if(op == D3DTOP_ADD)				sel = 5;
		else if(op == D3DTOP_SUBTRACT)			sel = 6;
		else if(op == D3DTOP_ADDSMOOTH)			sel = 7;

		m_ToonBlendCombo.SetCurSel(sel);

	}
*/
}
