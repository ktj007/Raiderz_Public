#pragma once

#include "RTypes.h"
#include "d3d9types.h"

class CDlgProfile;
class CDlgClipPlaneHeight;
class CControlMediator;

namespace rs3
{
	class RDeviceD3D;
	class RFont;
	class RToolSceneManager;
	class RSceneNode;
}

class CORETOOL_API CControlMediatorHelperObjectRenderer
{
public:
	CControlMediatorHelperObjectRenderer()
	{
		m_pControlMediator_ = NULL;
		m_pDeviceD3D_ = NULL;
		m_pFont_ = NULL;
	}

	virtual ~CControlMediatorHelperObjectRenderer(){}

	void InitDevice(CControlMediator* _pControlMediator, rs3::RDeviceD3D* _pDevice, rs3::RFont* _pFont)
	{
		m_pControlMediator_ = _pControlMediator;
		m_pDeviceD3D_ = _pDevice;
		m_pFont_ = _pFont;

		OnInitDevice();
	}

	virtual void OnInitDevice(){}
	virtual void OnResetDevice(){}
	virtual void OnDestroyDevice(){}

	virtual void OnRenderNavigationMesh() = 0;
	virtual void OnRenderSelection() = 0;
	virtual void OnRenderObject() = 0;
	virtual void OnRenderObjectTextInfo() = 0;

protected:
	CControlMediator*	m_pControlMediator_;
	rs3::RDeviceD3D*	m_pDeviceD3D_;
	rs3::RFont*			m_pFont_;
};

class CORETOOL_API CControlMediatorHelperRender
{
	friend class CControlMediator;

	//Render And Update HelperRender
	void OnDrawHelperRender();
	void OnUpdateHelperRender();
	void RenderPause();

	void InitHelperRender(CControlMediator* _pMediator, int _width, int _height);
	void ResetHelperRender(rs3::RToolSceneManager* _pSceneManager);
	void DestroyHelperRender();

public:

	void OnDrawFreeText( char* _szText, int _iX, int _iY );

	//////////////////////////////////////////////////////////////////////////
	//
	// Information Toogle
	//
	//////////////////////////////////////////////////////////////////////////

	// side panel info
	void ToggleViewSideInfoPanel();

	//Profile View
	void SetVisibleProfile();

	//Field NPC
	void SetVisibleFieldNPC();

	//map info
	void ToggleMapDetailInfo();
	bool GetMapDetailInfoState();

	// alpha sorting info
	void ToggleViewAlphaSorting();
	bool GetViewAlphaSorting();

	// view lighting only
	void ToggleViewLightingOnly();
	bool GetViewLightingOnly();

	// static mesh render cell
	void ToggleViewStaticMeshRenderCell();
	bool GetViewStaticMeshRenderCell();

	// static mesh render all
	void ToggleViewStaticMeshRenderAll();
	bool GetViewStaticMeshRenderAll();

	// clip plane
	void ShowToggleClipPlane();
	bool IsClipPlane();

	// view world center axis
	void SetVisibleWorldCenterAxis(bool _bShow){ m_bShowWorldCenterAxis_ = _bShow; }


	//////////////////////////////////////////////////////////////////////////
	//
	// Register To Render. Just for current render tick
	//
	//////////////////////////////////////////////////////////////////////////

	void RegisterToRenderBox(const rs3::RBoundingBox& _refBox, DWORD _dwColor = -1 );
	void ClearRenderBoxes();

	void RegisterToRenderPointLight( rs3::RSceneNode* _pPointLight );
	void ClearPointLightRenderList();

	void RegisterToInstantRenderSceneNode( rs3::RSceneNode* _pSceneNode);
	void ClearInstantRenderSceneNodes();

	//////////////////////////////////////////////////////////////////////////
	//
	// Text Information and the font related
	//
	//////////////////////////////////////////////////////////////////////////

	rs3::RFont* GetRFont()
	{
		return m_pFont_;
	}

	std::string& GetDragInfoText( const char* _pSzTextName, LONG _nNewPosX, LONG _nNewPosY )
	{
		CUSTOM_RENDER_TEXT& sTextInfo = m_mapDragTextToRender[ _pSzTextName ];
		sTextInfo.m_x = _nNewPosX;
		sTextInfo.m_y = _nNewPosY;
		return sTextInfo.m_strText;
	}

	void RemoveDragInfoText( const char* _pSzTextName )
	{
		m_mapDragTextToRender.erase(_pSzTextName);
	}

private:
	CDlgProfile*			m_pDlgProfile_;
	CDlgClipPlaneHeight*	m_pDlgClipPlaneHeight_;

	CControlMediatorHelperRender(void);
	~CControlMediatorHelperRender(void);

	struct CUSTOM_RENDER_INFO_BOX
	{
		float x, y, z, rhw;
		DWORD dwColor;
		static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	} m_infoRenderRect_[4];

	CControlMediator* m_pControlMediator_;
	rs3::RDeviceD3D* m_pDeviceD3D_;
	rs3::RFont* m_pFont_;

	bool m_bPause_;
	bool m_bShowPanel_;
	bool m_bScreenLog_;
	bool m_bTerrainInfo_;			///< 지형정보
	bool m_bViewAlphaSorting;		///< 알파소팅 정보 보여주기
	bool m_bViewLightingOnly;		///< 라이팅만 보여주기
	bool m_bViewStaticMeshRenderCell;///< Static Mesh Cell, Portal 그리기
	bool m_bViewStaticMeshRenderAll;///< Static Mesh 무조건 그리기
	bool m_bShowWorldCenterAxis_;	///< World Center Axis

	UINT m_width_;
	UINT m_height_;

	vector<string> m_vecStrShortcutText_;
	//랜더링시에 헬퍼 객체를 이동시키는 matrix
	rs3::RMatrix			m_matTranslation_;

	// 포인트 라이트의 랜더링 리스트
	std::vector< rs3::RSceneNode* > m_vecPointLightRenderlist;

	// 랜더링할 바운딩 박스
	std::vector< std::pair<rs3::RBoundingBox, DWORD> > m_vecBoxToRender;

	// 한 틱만 랜더링할 씬노드
	std::vector< rs3::RSceneNode* > m_vecInstantRenderSceneNodes;

	// 랜더링할 텍스트 정보
	struct CUSTOM_RENDER_TEXT
	{
		std::string m_strText;
		LONG m_x, m_y;
	};
	std::map< std::string, CUSTOM_RENDER_TEXT > m_mapDragTextToRender;
};

//////////////////////////////////////////////////////////////////////////
// inline
inline void CControlMediatorHelperRender::ToggleViewAlphaSorting() { m_bViewAlphaSorting = !m_bViewAlphaSorting; }
inline bool CControlMediatorHelperRender::GetViewAlphaSorting()	{ return m_bViewAlphaSorting; }

inline void CControlMediatorHelperRender::ToggleViewLightingOnly() { m_bViewLightingOnly = !m_bViewLightingOnly; }
inline bool CControlMediatorHelperRender::GetViewLightingOnly()	{ return m_bViewLightingOnly; }

inline void CControlMediatorHelperRender::ToggleViewStaticMeshRenderCell() { m_bViewStaticMeshRenderCell = !m_bViewStaticMeshRenderCell; }
inline bool CControlMediatorHelperRender::GetViewStaticMeshRenderCell()	{ return m_bViewStaticMeshRenderCell; }

inline bool CControlMediatorHelperRender::GetViewStaticMeshRenderAll()	{ return m_bViewStaticMeshRenderAll; }

//////////////////////////////////////////////////////////////////////////

inline void CControlMediatorHelperRender::RegisterToRenderBox( const rs3::RBoundingBox& _refBox, DWORD _dwColor /*= -1 */ )
{
	m_vecBoxToRender.push_back( std::pair<rs3::RBoundingBox, DWORD >(_refBox, _dwColor) );
}
inline void CControlMediatorHelperRender::ClearRenderBoxes()
{
	m_vecBoxToRender.clear();
}
inline void CControlMediatorHelperRender::RegisterToRenderPointLight( rs3::RSceneNode* _pPointLight )
{
	m_vecPointLightRenderlist.push_back(_pPointLight);
}
inline void CControlMediatorHelperRender::ClearPointLightRenderList()
{
	m_vecPointLightRenderlist.clear();
}