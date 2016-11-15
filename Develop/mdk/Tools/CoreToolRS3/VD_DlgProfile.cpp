// V_DlgProfile.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgProfile.h"

#include "RDeviceD3D.h"
#include "REngine.h"
#include "RListener.h"
#include "RTextureManager.h"

#include "MFileSystem.h"

// CDlgProfile 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgProfile, CDialog)

CDlgProfile::CDlgProfile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProfile::IDD, pParent)
{
}

CDlgProfile::~CDlgProfile()
{ 
}

void CDlgProfile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DRAWPOLYGON_PROFILE, m_ctlListDP);
	DDX_Control(pDX, IDC_LIST_USINGRESOURCE_PROFILE, m_ctlListUR);
	DDX_Control(pDX, IDC_EDIT_TIP, m_editURTip);
}


BEGIN_MESSAGE_MAP(CDlgProfile, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CDlgProfile::OnBnClickedButtonUpdate)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_USINGRESOURCE_PROFILE, &CDlgProfile::OnLvnItemchangedListUsingresourceProfile)
END_MESSAGE_MAP()

using namespace rs3;

void CDlgProfile::UpdateProfile(RDevice *pDevice)
{
	if ( !IsWindowVisible())
		return;

	char pcBuffer[128];
	pcBuffer[0] = '\0';

	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_TEXT;

	for ( unsigned int ui=0; ui< rs3::REngine::GetResourceProfiler().GetTypeCount(); ++ui )
	{
		sprintf(pcBuffer,"%d",REngine::GetResourceProfiler().GetPolygonCount((rs3::PROFILE_POLYGON_TYPE)ui));

		item.iItem = ui;
		item.iSubItem = 1;
		item.pszText = pcBuffer;

		m_ctlListDP.SetItem(&item);
	}
}

BOOL CDlgProfile::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlListDP.DeleteAllItems();
	m_ctlListDP.DeleteColumn(0);
	m_ctlListDP.DeleteColumn(1);
	m_ctlListDP.InsertColumn(0, "이름", LVCFMT_RIGHT, 112);
	m_ctlListDP.InsertColumn(1, "폴리곤", LVCFMT_RIGHT, 130);

	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_TEXT;

	char pcBuffer[128];
	pcBuffer[0] = '\0';

	for ( UINT ui = 0; ui< REngine::GetResourceProfiler().GetTypeCount(); ++ui )
	{
		strcpy_s(pcBuffer, sizeof(pcBuffer), REngine::GetResourceProfiler().GetTypeName((rs3::PROFILE_POLYGON_TYPE)ui));
				
		item.iItem = ui;
		item.iSubItem = 0;
		item.pszText = pcBuffer;

		m_ctlListDP.InsertItem(&item);
		
		strcpy(pcBuffer, "0");
		item.iSubItem = 1;
		item.pszText = pcBuffer;
		m_ctlListDP.InsertItem(&item);
	}

	m_ctlListUR.DeleteAllItems();
	m_ctlListUR.DeleteColumn(0);
	m_ctlListUR.DeleteColumn(1);

	m_ctlListUR.InsertColumn(0, "이름", LVCFMT_RIGHT, 240);
	m_ctlListUR.InsertColumn(1, "용량(MB)", LVCFMT_RIGHT, 80);

	return TRUE;  
	// return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgProfile::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

}

void CDlgProfile::OnBnClickedButtonUpdate()
{
	if ( !IsWindowVisible())
		return;

	RDevice *pDevice = REngine::GetDevicePtr();
	if ( pDevice == NULL ) 
		return;

	const int N = 1024 * 1024;
	const float F = 1024.f * 1024.f;

	class Impl
	{
	public:
		static int InsertStringToColumn1And2(CListCtrl &ctrl, int nRow, LPSTR szString1, LPSTR szString2, DWORD dwData = 0)
		{
			LVITEM item;
			memset(&item, 0, sizeof(item));
			item.iItem = nRow;
			item.mask = LVIF_TEXT;		
			item.iSubItem = 0;
			item.pszText = szString1;
			ctrl.InsertItem(&item);

			item.iSubItem = 1;
			item.pszText = szString2;
			ctrl.SetItem(&item);
			
			ctrl.SetItemData(nRow, dwData);
			return ++nRow;
		}
	};

	const RTextureManager *pTextureManager = pDevice->GetTextureManager();

	UINT uVertexBufferSize = pDevice->GetVertexBufferMemoryUsed();
	char pcBuffer[128];
	pcBuffer[0] = '\0';

	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_TEXT;

	m_ctlListUR.DeleteAllItems();

	int nRow = 0;
	nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, "Vertex Size", itoa(uVertexBufferSize/N, pcBuffer, 10), (DWORD)uVertexBufferSize);

	int nFileTextureSize = 0;	// 파일 텍스쳐 크기

	map<string,int> nCountTable;	// 각 텍스쳐 크기 (정해진 폴더에 들어있는 파일들)
	int nUnknownFileSize = 0;	// 알 수 없는 파일 크기

	typedef pair<string, string> FILENAME_AND_DESC;
	vector<FILENAME_AND_DESC> nEtcKinds;	// etc에 해당하는 텍스쳐 구분자
	vector<int> nEtcKindsCount;				// etc에 해당하는 텍스쳐 구분자에 따라 크기를 세는 벡터

//	nEtcKinds.push_back(FILENAME_AND_DESC(".lightmap", "오브젝트의 라이트맵"));
	nEtcKinds.push_back(FILENAME_AND_DESC("_lightmap", "static-mesh"));
	nEtcKinds.push_back(FILENAME_AND_DESC("_light", "static-light"));
	nEtcKinds.push_back(FILENAME_AND_DESC("", "None"));

//	const vector<string> &pathList = pTextureManager->GetPath().GetPathList();
// 	const RPathSplitter* pPath = REngine::GetConfig().GetPathSplitter( RP_TEXTURE );
// 	nCountTable.resize(pPath->GetPathCount(), 0);
	nEtcKindsCount.resize(nEtcKinds.size(), 0);

	const RRESOURCEHASHMAP &texMap = pTextureManager->GetResourceMap();
	for ( RRESOURCEHASHMAP::const_iterator itr = texMap.begin(); itr != texMap.end(); ++itr )
	{
		const RTexture* pTexture = (const RTexture*)itr->second;
		const string &texName = pTexture->GetName();
		unsigned int texSize = pTexture->GetInfo().GetSize();

		nFileTextureSize += texSize;

		int pos = 0;
		if ( ( pos = texName.rfind('/')) != string::npos
			|| ( pos = texName.rfind('\\')) != string::npos )
		{
			// etc 에 넣음.
			for ( size_t si = 0; si<nEtcKinds.size(); ++si )
			{
				if ( texName.find(nEtcKinds[si].first) != string::npos 
					|| nEtcKinds[si].first.length() == 0 )
				{
					nEtcKindsCount[si] += texSize;
					break;
				}
			}
		}
		else
		{
			string filename( texName );
			string filename_dds( filename + ".dds");

			MFile file;
			if ( file.Open(filename_dds.c_str()) ||
				file.Open(filename.c_str()) )
			{
				string strFileName = file.GetFileName();
				string strPath = MGetPurePath( strFileName );
				nCountTable[strPath] += texSize;
				file.Close();
			}
			else
				nUnknownFileSize += texSize;
		}
	}	// end RESOURCEHASHMAP

	nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, "File Texture", "");
#if 0
	sprintf(pcBuffer, "%6.2f", nFileTextureSize/F);
	nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, "File Texture size", pcBuffer);
#endif

	// 정해진 폴더에 들어 있는 파일 정보들
	size_t si = 0;
	for ( map<string,int>::iterator itr = nCountTable.begin(); itr!= nCountTable.end(); ++itr )
	{
		char pcPathName[MAX_PATH];
		pcPathName[0] = '\0';
		sprintf(pcPathName,"    %s", itr->first.c_str());
		pcBuffer[0] = '\0';
		sprintf(pcBuffer, "%6.2f", itr->second/F);

		nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, pcPathName, pcBuffer, itr->second);
		++si;
	}

	// nUnknown file
	if ( nUnknownFileSize )
	{
		sprintf(pcBuffer, "%6.2f", nUnknownFileSize/F);
		nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, "    Unknown", pcBuffer, nUnknownFileSize);
	}

	// ETC
	nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, "    Etc", "");
	 
	// TODO :Alpha Texture Map ( CreateTexture 할 때 임의의 이름을 지정할 수 있게 되기 전 까지는 수동으로 계산 )
	RTerrain *pTerrain = REngine::GetSceneManagerPtr()->GetCurrentTerrain();

	int nTerrainAlphaMaskSize = 0;
	if ( pTerrain )
	{
		int nPatchNum = pTerrain->GetWidthCount() * pTerrain->GetHeightCount();
		for ( int i = 0; i<pTerrain->GetLayerNum(); ++i )
		{
			const RTerrain::LayerInfo *pLayerInfo = pTerrain->GetLayerInfo(i);

			DWORD dwPatchSize = pLayerInfo->dwAlphaPatchWidth * pLayerInfo->dwAlphaPatchHeight * 4;		// ARGB32 format
			nTerrainAlphaMaskSize += ( dwPatchSize * nPatchNum );
		}
	}

	pcBuffer[0] = '\0';
	sprintf(pcBuffer, "%6.2f", nTerrainAlphaMaskSize/F);
	nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, "   >지형 마스크맵", pcBuffer, nTerrainAlphaMaskSize);

	for ( size_t si = 0; si<nEtcKinds.size(); ++si )
	{
		char pcName[128];
		pcName[0] = '\0';
		sprintf_s(pcName, sizeof(pcName), "   >%s",nEtcKinds[si].second.c_str());

		pcBuffer[0] = '\0';
		sprintf(pcBuffer, "%6.2f", nEtcKindsCount[si]/F);

		nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, pcName, pcBuffer, (DWORD)nEtcKindsCount[si]);
	}

	pcBuffer[0] = '\0';
	UINT uAllUsingTextureSize = pDevice->GetTextureMemoryUsed();	// texture manager 의 것을 호출해주어야 하는 것이 옳지만, const 문제로 간접적인 호출을 하도록 하였음.

	UINT uUnnamedTextureSize = uAllUsingTextureSize - nFileTextureSize;
	uUnnamedTextureSize -= nTerrainAlphaMaskSize;

	sprintf(pcBuffer, "%6.2f", uUnnamedTextureSize/F);
	nRow = Impl::InsertStringToColumn1And2(m_ctlListUR, nRow, "Unnamed-Texture", pcBuffer, uUnnamedTextureSize);

	DWORD dwTotalSize = uVertexBufferSize + nFileTextureSize + uUnnamedTextureSize + nTerrainAlphaMaskSize;
	_ASSERT((nFileTextureSize + uUnnamedTextureSize + nTerrainAlphaMaskSize)==uAllUsingTextureSize);

	pcBuffer[0] = '\0';
	sprintf(pcBuffer, "Total Size : %6.2f\n", dwTotalSize/(1024.f*1024.f));
	m_editURTip.SetWindowText(pcBuffer);
}

void CDlgProfile::OnLvnItemchangedListUsingresourceProfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	char pcBuffer[128];
	pcBuffer[0] = '\0';

	POSITION pos = m_ctlListUR.GetFirstSelectedItemPosition();
	if ( pos == NULL )
	{
		int nItemCount = m_ctlListUR.GetItemCount();
		DWORD dwTotalSize = 0;

		for ( int i = 0; i<nItemCount; ++i )
			dwTotalSize += (DWORD)m_ctlListUR.GetItemData(i);

		sprintf(pcBuffer, "Total Size : %6.2f\n", dwTotalSize/(1024.f*1024.f));
		m_editURTip.SetWindowText(pcBuffer);
		return;
	}

	DWORD dwSize = 0;
	while ( pos )
	{
		int nItem = m_ctlListUR.GetNextSelectedItem(pos);
		dwSize += (DWORD)m_ctlListUR.GetItemData(nItem);
	}

	sprintf(pcBuffer, "Selected Size : %6.2f\n", dwSize/(1024.f*1024.f));
	m_editURTip.SetWindowText(pcBuffer);
}
