#pragma once

#include "XCharacterCaption.h"
#include "XDamageCaptionDraw.h"

class XCaptionMgr : public MSingletonRestricted< XCaptionMgr >
{
public:
	typedef map< MID, XCaption* > CAP_NODE_MAP;

	XCaptionMgr() : m_nIDGenerator(0), m_pCaptionFont(NULL) {}
	virtual ~XCaptionMgr();

	//////////////////////////////////////////////////////////////////////////
	// caption 을 렌더링할 전역정보
	//
	rs3::RFont*			m_pCaptionFont;

	//////////////////////////////////////////////////////////////////////////
	// caption 생성 / 제거
	//
	template< typename T >
	T*					InsertCaption(rs3::RSceneNode* pAssociatedSceneNode)
	{
		T* pCharCap = new T(NewID(), pAssociatedSceneNode);
		m_mapCaptionsWithNode.insert( CAP_NODE_MAP::value_type(pCharCap->GetID(), pCharCap) );
		return pCharCap;
	}
	bool				RemoveCaption( MID nID );
	bool				IsEmptyCaptionContainer(){ return m_mapCaptionsWithNode.empty(); }

	void				CreatDamageCaption();
	void				DestroyDamageCaption();

	CAP_NODE_MAP& GetNodeMap() { return m_mapCaptionsWithNode; } // for test
	//////////////////////////////////////////////////////////////////////////
	// Render
	//
	void				Render();

private:
	MID					m_nIDGenerator;
	MID					NewID() { return (++m_nIDGenerator); }


	CAP_NODE_MAP m_mapCaptionsWithNode;

	typedef map< float, XCaption* > CAP_DIST_MAP;
	CAP_DIST_MAP m_mapCaptionsWithDistance;

	struct BILLBOARD_RENDER_BUFFER
	{
		BILLBOARD_RENDER_BUFFER()
			: m_matView( RMatrix::IDENTITY )
			, m_matViewInverse( RMatrix::IDENTITY )
			, m_matProject( RMatrix::IDENTITY )
			, m_matZoomInTrans( RMatrix::IDENTITY )
			, m_matZoomOutTrans( RMatrix::IDENTITY )
			, m_matScaleInverse( RMatrix::IDENTITY )
		{
		}

		RMatrix			m_matView;
		RMatrix			m_matViewInverse;	// view matrix inverse

		RMatrix			m_matProject;

		RVector			m_vPosInViewCoord;

		RMatrix			m_matZoomInTrans;
		RMatrix			m_matZoomOutTrans;
		RMatrix			m_matScaleInverse;	// 줌인 * 스케일 역 * 줌아웃 -> 크기가 일정한 빌보드

	} m_matBuf;

	XDamageCaptionDraw	m_DamageCaptionDraw;

	void XCaptionMgr::DrawTextIn3DSpace( XCaption* pCharCap );
	void XCaptionMgr::DrawTextIn3DSpaceEqualScale( XCaption* pCharCap , float x, float y, float z );
};
