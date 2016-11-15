#include "StdAfx.h"
#include "MCConfig.h"
#include "windowsx.h"
#include "shlobj.h"

// 경로명을 ITEMIDLIST로 변환한다.
LPITEMIDLIST ConvertPathToLpItemIdList(const char *pszPath)
{
	LPITEMIDLIST  pidl;
	LPSHELLFOLDER pDesktopFolder;
	OLECHAR       olePath[MAX_PATH];
	ULONG         chEaten;
	ULONG         dwAttributes;
	HRESULT       hr;

	if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
	{
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszPath, -1, olePath, MAX_PATH);
		hr = pDesktopFolder->ParseDisplayName(NULL,NULL,olePath,&chEaten, &pidl, &dwAttributes);
		pDesktopFolder->Release();
	}
	return pidl;
}

void MCConfig::UpdateEluDlgEnableItems(HWND hWnd)
{
//	EnableWindow(GetDlgItem(hWnd,IDC_EDIT_POLYGON_PER_NODES), m_bStaticMesh ? TRUE : FALSE );
	EnableWindow(GetDlgItem(hWnd,IDC_EDIT_POLYGON_PER_NODES), FALSE );

	// texture options
	EnableWindow(GetDlgItem(hWnd,IDC_EXPORT_TEXTURES), m_bUseDDS ? TRUE : FALSE);

	BOOL bTextureExport = (m_bUseDDS && m_bExportTextures) ? TRUE : FALSE;

	EnableWindow(GetDlgItem(hWnd,IDC_EDIT_TEXTURE_EXPORT_FOLDER),  bTextureExport );
	EnableWindow(GetDlgItem(hWnd,IDC_BUTTON_BROWSE), bTextureExport );

	EnableWindow(GetDlgItem(hWnd,IDC_RADIO_PLAIN_DXT1),	bTextureExport );
	EnableWindow(GetDlgItem(hWnd,IDC_RADIO_PLAIN_DXT3), bTextureExport );
	EnableWindow(GetDlgItem(hWnd,IDC_RADIO_PLAIN_DXT5),	bTextureExport );
	EnableWindow(GetDlgItem(hWnd,IDC_RADIO_PLAIN_NOCOMPRESS),	bTextureExport );
	EnableWindow(GetDlgItem(hWnd,IDC_RADIO_ALPHA_DXT1), bTextureExport );
	EnableWindow(GetDlgItem(hWnd,IDC_RADIO_ALPHA_DXT3),	bTextureExport );
	EnableWindow(GetDlgItem(hWnd,IDC_RADIO_ALPHA_DXT5), bTextureExport );
	EnableWindow(GetDlgItem(hWnd,IDC_RADIO_ALPHA_NOCOMPRESS),	bTextureExport );

	EnableWindow(GetDlgItem(hWnd,IDC_ATTACH_MESH), m_bMeshOut ? TRUE : FALSE );
}

static HWND		hWndEluDlgh;	// Export Elu Dialog의 핸들
int CALLBACK MCConfig::BrowseCallbackProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case BFFM_INITIALIZED:
			{
				char strDir[MAX_PATH];

				HWND hEdit = GetDlgItem(hWndEluDlgh,IDC_EDIT_TEXTURE_EXPORT_FOLDER);
				Edit_GetText(hEdit, strDir, MAX_PATH);

				SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)strDir);
			}
			break;
		case BFFM_SELCHANGED :
			break;
		case BFFM_VALIDATEFAILED :
			break;
	}
	return 0;
}

BOOL CALLBACK MCConfig::EluDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Interval animRange;
	hWndEluDlgh = hWnd;

	MCplug2 *exp = (MCplug2*)GetWindowLong(hWnd,GWL_USERDATA); 

	switch (msg) {

		case WM_INITDIALOG:
			exp = (MCplug2*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam); 
			CenterWindow(hWnd, GetParent(hWnd));

			CheckDlgButton(hWnd, IDC_MESH_OUT			, exp->m_options.m_bMeshOut); 
			CheckDlgButton(hWnd, IDC_ATTACH_MESH		, exp->m_options.m_bAttachMesh); 
			CheckDlgButton(hWnd, IDC_STATIC_MESH		, exp->m_options.m_bStaticMesh); 
			{
				char szBuffer[256];
				sprintf_s( szBuffer, 256, "%d", exp->m_options.m_nPolygonPerNodes );
				HWND hEdit = GetDlgItem(hWnd,IDC_EDIT_POLYGON_PER_NODES);
				Edit_SetText(hEdit,szBuffer);
			}
			CheckDlgButton(hWnd, IDC_ANI_OUT			, exp->m_options.m_bKeyAnimationOut);
			CheckDlgButton(hWnd, IDC_VERTEX_ANI_OUT		, exp->m_options.m_bVertexAnimationOut);
			CheckDlgButton(hWnd, IDC_OPTIMIZE_KEYS		, exp->m_options.m_bOptimize_keys);
			CheckDlgButton(hWnd, IDC_USE_DDS			, exp->m_options.m_bUseDDS);
			CheckDlgButton(hWnd, IDC_EXPORT_TEXTURES	, exp->m_options.m_bExportTextures);

			switch(exp->m_options.m_fmtPlain)
			{
			case DDS_FORMAT_DXT1 : CheckDlgButton(hWnd, IDC_RADIO_PLAIN_DXT1, BST_CHECKED);break;
			case DDS_FORMAT_DXT3 : CheckDlgButton(hWnd, IDC_RADIO_PLAIN_DXT3, BST_CHECKED);break;
			case DDS_FORMAT_DXT5 : CheckDlgButton(hWnd, IDC_RADIO_PLAIN_DXT5, BST_CHECKED);break;
			case DDS_FORMAT_NOCOMPRESS : CheckDlgButton(hWnd, IDC_RADIO_PLAIN_NOCOMPRESS, BST_CHECKED);break;
			default : CheckDlgButton(hWnd, IDC_RADIO_PLAIN_DXT1, BST_CHECKED);
			}

			switch(exp->m_options.m_fmtAlpha)
			{
			case DDS_FORMAT_DXT1 : CheckDlgButton(hWnd, IDC_RADIO_ALPHA_DXT1, BST_CHECKED);break;
			case DDS_FORMAT_DXT3 : CheckDlgButton(hWnd, IDC_RADIO_ALPHA_DXT3, BST_CHECKED);break;
			case DDS_FORMAT_DXT5 : CheckDlgButton(hWnd, IDC_RADIO_ALPHA_DXT5, BST_CHECKED);break;
			case DDS_FORMAT_NOCOMPRESS : CheckDlgButton(hWnd, IDC_RADIO_ALPHA_NOCOMPRESS, BST_CHECKED);break;
			default : CheckDlgButton(hWnd, IDC_RADIO_ALPHA_DXT1, BST_CHECKED);
			}

			// texture folder
			{
				HWND hEdit = GetDlgItem(hWnd,IDC_EDIT_TEXTURE_EXPORT_FOLDER);
				Edit_SetText(hEdit,exp->m_options.m_strTextureExportFolder.c_str());
			}
			exp->m_options.UpdateEluDlgEnableItems( hWnd);
			break;

			case WM_COMMAND:
				switch (LOWORD(wParam)) 
				{
				case IDC_USE_DDS:
				case IDC_EXPORT_TEXTURES:
				case IDC_STATIC_MESH:
				case IDC_MESH_OUT:
					{
						exp->m_options.m_bStaticMesh		= (IsDlgButtonChecked(hWnd, IDC_STATIC_MESH)); 
						exp->m_options.m_bMeshOut			= (IsDlgButtonChecked(hWnd, IDC_MESH_OUT)); 
						exp->m_options.m_bUseDDS			= IsDlgButtonChecked(hWnd, IDC_USE_DDS);
						exp->m_options.m_bExportTextures	= IsDlgButtonChecked(hWnd, IDC_EXPORT_TEXTURES);
						exp->m_options.UpdateEluDlgEnableItems(hWnd);
						return FALSE;
					}break;
				case IDC_BUTTON_BROWSE:
					{
						LPITEMIDLIST pidlBrowse;
						char tmp[MAX_PATH];

						BROWSEINFO BRinfo;
						BRinfo.hwndOwner = NULL;
						BRinfo.pidlRoot = NULL;      // 브라우징 할 루트 디렉토리
						BRinfo.pszDisplayName = tmp;    // 선택할 디렉토리가 저장될 버퍼
						BRinfo.lpszTitle = "텍스쳐 출력 디렉토리를 선택하세요"; // 출력 문자열
						BRinfo.ulFlags = BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE ;  // 디렉토리 선택 옵션
						BRinfo.lpfn = (BFFCALLBACK)BrowseCallbackProc;//NULL;       // 이벤트에 대한 사용자 정의 처리 함수
						BRinfo.lParam = 0;       // 처리 함수에 넘겨질 인수

						pidlBrowse = SHBrowseForFolder(&BRinfo); // 다이얼로그를 띄운다.

						if(pidlBrowse != NULL) {
							SHGetPathFromIDList(pidlBrowse, tmp); // 패스를 얻어온다.
							HWND hEdit = GetDlgItem(hWnd,IDC_EDIT_TEXTURE_EXPORT_FOLDER);
							Edit_SetText(hEdit,tmp);
						}

					}break;
				case IDOK:

					exp->m_options.m_bMeshOut			= (IsDlgButtonChecked(hWnd, IDC_MESH_OUT)); 
					exp->m_options.m_bAttachMesh		= (IsDlgButtonChecked(hWnd, IDC_ATTACH_MESH));
					exp->m_options.m_bStaticMesh		= (IsDlgButtonChecked(hWnd, IDC_STATIC_MESH)); 
					//{
					//	HWND hEdit = GetDlgItem(hWnd,IDC_EDIT_POLYGON_PER_NODES);
					//	char szBuffer[256];
					//	Edit_GetText(hEdit,szBuffer,sizeof(szBuffer));
					//	exp->m_options.m_nPolygonPerNodes = atoi( szBuffer );
					//}
					exp->m_options.m_bKeyAnimationOut	= (IsDlgButtonChecked(hWnd, IDC_ANI_OUT)); 
					exp->m_options.m_bVertexAnimationOut = (IsDlgButtonChecked(hWnd, IDC_VERTEX_ANI_OUT)); 
					exp->m_options.m_bOptimize_keys	= IsDlgButtonChecked(hWnd, IDC_OPTIMIZE_KEYS);
					exp->m_options.m_bUseDDS			= IsDlgButtonChecked(hWnd, IDC_USE_DDS);
					exp->m_options.m_bExportTextures	= IsDlgButtonChecked(hWnd, IDC_EXPORT_TEXTURES);

					if(IsDlgButtonChecked(hWnd, IDC_RADIO_PLAIN_DXT1))
						exp->m_options.m_fmtPlain = DDS_FORMAT_DXT1;
					if(IsDlgButtonChecked(hWnd, IDC_RADIO_PLAIN_DXT3))
						exp->m_options.m_fmtPlain = DDS_FORMAT_DXT3;
					if(IsDlgButtonChecked(hWnd, IDC_RADIO_PLAIN_DXT5))
						exp->m_options.m_fmtPlain = DDS_FORMAT_DXT5;
					if(IsDlgButtonChecked(hWnd, IDC_RADIO_PLAIN_NOCOMPRESS))
						exp->m_options.m_fmtPlain = DDS_FORMAT_NOCOMPRESS;

					if(IsDlgButtonChecked(hWnd, IDC_RADIO_ALPHA_DXT1))
						exp->m_options.m_fmtAlpha = DDS_FORMAT_DXT1;
					if(IsDlgButtonChecked(hWnd, IDC_RADIO_ALPHA_DXT3))
						exp->m_options.m_fmtAlpha = DDS_FORMAT_DXT3;
					if(IsDlgButtonChecked(hWnd, IDC_RADIO_ALPHA_DXT5))
						exp->m_options.m_fmtAlpha = DDS_FORMAT_DXT5;
					if(IsDlgButtonChecked(hWnd, IDC_RADIO_ALPHA_NOCOMPRESS))
						exp->m_options.m_fmtAlpha = DDS_FORMAT_NOCOMPRESS;

					{
						HWND hEdit = GetDlgItem(hWnd,IDC_EDIT_TEXTURE_EXPORT_FOLDER);
						char szBuffer[256];
						Edit_GetText(hEdit,szBuffer,sizeof(szBuffer));
						exp->m_options.m_strTextureExportFolder = szBuffer;

						if(exp->m_options.m_strTextureExportFolder.size())
						{
							// 마지막에 / 를 붙임
							if(*exp->m_options.m_strTextureExportFolder.rbegin()!='\\' &&
								*exp->m_options.m_strTextureExportFolder.rbegin()!='/')
								exp->m_options.m_strTextureExportFolder+="\\";
						}
					}

					EndDialog(hWnd, 1);
					break;

				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
				}
				break;
			default:
				return FALSE;
	}
	return TRUE;
} 


/**
config 파일 형식

파일 이름 : CFGFILENAME 으로 #define 되어 있음
#version
*/

const int CFG_VERSION = 1;

const char* const CONFIG_MESH_OUT			= "MESH_OUT";
const char* const CONFIG_STATIC_MESH		= "STATIC_MESH";
const char* const CONFIG_COLLISION			= "COLLISION";
const char* const CONFIG_FBX				= "FBX";
const char* const CONFIG_POLYGON_PER_NODES	= "POLYGON_PER_NODES";
const char* const CONFIG_ANI_OUT			= "ANI_OUT";
const char* const CONFIG_PLAINTEXTUREFORMAT = "PLAINTEXTUREFORMAT";
const char* const CONFIG_ALPHAEXTUREFORMAT	= "ALPHAEXTUREFORMAT";
const char* const CONFIG_VERTEX_ANI_OUT		= "VERTEX_ANI_OUT";
const char* const CONFIG_ANI_OPTIMIZE		= "ANI_OPTIMIZE";
const char* const CONFIG_ATTACH_MESH		= "ATTACH_MESH";
const char* const CONFIG_AS_SCENE_EXPORT	= "AS_SCENE_EXPORT";
//const char* const CONFIG_USE_PARTS_COLOR	= "USE_PARTS_COLOR";
const char* const CONFIG_USE_DDS			= "USE_DDS";
const char* const CONFIG_EXPORT_TEXTURE		= "EXPORT_TEXTURE";
const char* const CONFIG_EXPORT_TEXTURE_FOLDER	= "EXPORT_TEXTURE_FOLDER";

bool MCConfig::ReadConfig(TSTR szFileName)
{
	// default-config
	m_bMeshOut			= true;
	m_bStaticMesh		= false;
	m_nPolygonPerNodes	= 500;
	m_bKeyAnimationOut	= true;
	m_bVertexAnimationOut	= false;
	m_bOptimize_keys		= true;
	m_bAttachMesh			= false;
	m_bUseDDS				= true;
	m_bExportTextures		= true;
	m_fmtPlain = DDS_FORMAT_DXT1;
	m_fmtAlpha = DDS_FORMAT_DXT3;
	m_fmtAlphaTestPartsColor = DDS_FORMAT_DXT5;
	m_strTextureExportFolder = "";

	MXml kXml;
	if (!kXml.LoadFile(szFileName))
		return false;

	MXmlElement *pRoot = kXml.Doc()->RootElement();
	int version = 0;
	if (!pRoot->Attribute("version",&version))
		return false;

	if (version != CFG_VERSION)
		return false;

	MXmlElement *pChildElement = NULL;

	pChildElement = pRoot->FirstChildElement(CONFIG_MESH_OUT);
	_Contents(&m_bMeshOut, pChildElement);

	pChildElement = pRoot->FirstChildElement(CONFIG_STATIC_MESH);
	_Contents(&m_bStaticMesh, pChildElement);

//	pChildElement = pRoot->FirstChildElement(CONFIG_POLYGON_PER_NODES);
//	_Contents(&m_nPolygonPerNodes, pChildElement);

	pChildElement = pRoot->FirstChildElement(CONFIG_ANI_OUT);
	_Contents(&m_bKeyAnimationOut, pChildElement);

	pChildElement = pRoot->FirstChildElement(CONFIG_VERTEX_ANI_OUT);
	_Contents(&m_bVertexAnimationOut, pChildElement);

	pChildElement = pRoot->FirstChildElement(CONFIG_ANI_OPTIMIZE);
	_Contents(&m_bOptimize_keys, pChildElement);

	pChildElement = pRoot->FirstChildElement(CONFIG_ATTACH_MESH);
	_Contents(&m_bAttachMesh, pChildElement);

	pChildElement = pRoot->FirstChildElement(CONFIG_USE_DDS);
	_Contents(&m_bUseDDS, pChildElement);

	pChildElement = pRoot->FirstChildElement(CONFIG_EXPORT_TEXTURE);
	_Contents(&m_bExportTextures, pChildElement);

	pChildElement = pRoot->FirstChildElement(CONFIG_PLAINTEXTUREFORMAT);
	if ( pChildElement )
	{
		int nFormat;
		_Contents(&nFormat, pChildElement);
		m_fmtPlain = (DDS_FORMAT)nFormat;
	}

	pChildElement = pRoot->FirstChildElement(CONFIG_ALPHAEXTUREFORMAT);
	if ( pChildElement )
	{
		int nFormat;
		_Contents(&nFormat, pChildElement);
		m_fmtAlpha = (DDS_FORMAT)nFormat;
	}

	pChildElement = pRoot->FirstChildElement(CONFIG_EXPORT_TEXTURE_FOLDER);
	_Contents(m_strTextureExportFolder,pChildElement);

	return true;
}

void MCConfig::WriteConfig(TSTR szFileName)
{
	MXml kXml;
	MXmlElement *pRootElement = new MXmlElement("ROOT");
	pRootElement->SetAttribute("version",CFG_VERSION);
	kXml.Doc()->LinkEndChild(pRootElement);

	MXmlElement *pChildElement = NULL;

	pChildElement = new MXmlElement(CONFIG_MESH_OUT);
	_SetContents(pChildElement, m_bMeshOut);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_STATIC_MESH);
	_SetContents(pChildElement, m_bStaticMesh);
	pRootElement->LinkEndChild(pChildElement);

	//pChildElement = new MXmlElement(CONFIG_POLYGON_PER_NODES);
	//_SetContents(pChildElement, m_nPolygonPerNodes);
	//pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_ANI_OUT);
	_SetContents(pChildElement, m_bKeyAnimationOut);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_VERTEX_ANI_OUT);
	_SetContents(pChildElement, m_bVertexAnimationOut);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_ANI_OPTIMIZE);
	_SetContents(pChildElement, m_bOptimize_keys);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_ATTACH_MESH);
	_SetContents(pChildElement, m_bAttachMesh);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_USE_DDS);
	_SetContents(pChildElement, m_bUseDDS);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_EXPORT_TEXTURE);
	_SetContents(pChildElement, m_bExportTextures);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_PLAINTEXTUREFORMAT);
	_SetContents(pChildElement, m_fmtPlain);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_ALPHAEXTUREFORMAT);
	_SetContents(pChildElement, m_fmtAlpha);
	pRootElement->LinkEndChild(pChildElement);

	pChildElement = new MXmlElement(CONFIG_EXPORT_TEXTURE_FOLDER);
	_SetContents(pChildElement, m_strTextureExportFolder);
	pRootElement->LinkEndChild(pChildElement);

	kXml.SaveFile(szFileName);
	return;
}
