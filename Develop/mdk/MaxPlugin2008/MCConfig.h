#pragma once

//////////////////////////////////////////////////////////////////////////
// 익스포트 옵션들

enum DDS_FORMAT {
	DDS_FORMAT_NOCOMPRESS = 0,
	DDS_FORMAT_DXT1,
	DDS_FORMAT_DXT3,
	DDS_FORMAT_DXT5,

	DDS_FORMAT_END
};

const D3DFORMAT D3D_DDS_FORMAT[DDS_FORMAT_END] = { 
	D3DFMT_UNKNOWN, 
	D3DFMT_DXT1, 
	D3DFMT_DXT3,
	D3DFMT_DXT5,
};


class MCConfig {
public:
	BOOL		m_bMeshOut;
	BOOL		m_bStaticMesh;
	int			m_nPolygonPerNodes;
	BOOL		m_bKeyAnimationOut;
	BOOL		m_bVertexAnimationOut;
	BOOL		m_bOptimize_keys;	// 애니메이션 키를 최적화 할것인가 여부
	// 이게 꺼져있으면 Actor가 생성될때 딸린 effect 및 light 가 항상 같이 생성된다

	BOOL		m_bAttachMesh;

	BOOL		m_bUseDDS;			// dds 사용
	BOOL		m_bExportTextures;	// 텍스쳐 처리
	DDS_FORMAT	m_fmtPlain;
	DDS_FORMAT	m_fmtAlpha;
	DDS_FORMAT	m_fmtAlphaTestPartsColor;
	string		m_strTextureExportFolder;	/// 텍스쳐 익스포트 폴더

	bool		ReadConfig(TSTR szFileName);
	void		WriteConfig(TSTR szFileName);

	void		UpdateEluDlgEnableItems(HWND hWnd);

	static int CALLBACK		BrowseCallbackProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK	EluDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
