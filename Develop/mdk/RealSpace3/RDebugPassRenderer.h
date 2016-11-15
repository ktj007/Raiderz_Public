#pragma once

#include "RPassRenderer.h"

namespace rs3 {

class RActor;
class RActorNode;

class RS_API RDebugPassRenderer : public RPassRenderer
{
public:
	RDebugPassRenderer();
	virtual ~RDebugPassRenderer();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID ¼³Á¤
	//
	DECLARE_PASS_RENDERER_ID( RRP_DEBUG );


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// »ý¼º / ¼Ò¸ê
	//
	virtual void Init();
	virtual void Destroy();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// update / rendering
	//
	virtual bool IsCurrentShaderRejective(){ return true; }
	virtual void RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD eMothod);
	virtual void RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void Update(float fTimeDelta);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ·»´õ¸µ ÇÛÆÛ °´Ã¼ Ãß°¡ÇÏ±â
	//
	void		AddDebugBox(const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		AddDebugBox(const RMatrix& _refMatWorld, RBoundingBox& bbox, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		AddDebugAxis(const RMatrix& _refMatWorld, float _fAxisSize, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		AddDebugBoxAndAxis(const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor = 0xffffffff);

	void		SetCurrentRenderingTextFont(RFont* pFont){ m_pFont = pFont; }
	void		AddDebugText(const RVector& vPos, const char* pSzText, float _fLifeTime, DWORD _dwColor = 0xffffffff);

	void		AddDebugLine(const RVector& _refVecStart, const RVector& _refVecEnd, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		AddDebugSphere(const RMatrix& _refMatWorld, float _fRadius, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		AddDebugCapsule(MCapsule &_capsule, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		AddDebugOBox(MOBox &_obox, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		AddDebugPolycon(const RMatrix& _refMatWorld, RVector* pVector, int nVertexCount, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		AddDebugPlane(const RMatrix& _refMatWorld, MPlane& _refPlane, float fSize, float _fLifeTime, DWORD _dwColor = 0xffffffff);
	void		ShowDebugBox(const string& strName, const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor);
	void		ShowDebugBox(const string& strName, const RMatrix& _refMatWorld, RBoundingBox& bbox, float _fLifeTime, DWORD _dwColor);
	void		ShowDebugBoxAndAxis(const string& strName, const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor);
	void		ShowDebugAxis(const string& strName, const RMatrix& _refMatWorld, float _fAxisSize, float _fLifeTime, DWORD _dwColor);
	void		ShowDebugSphere(const string& strName, const RMatrix& _refMatWorld, float _fRadius, float _fLifeTime, DWORD _dwColor);
	void		ShowDebugLine(const string& strName, const RVector& _refVecStart, const RVector& _refVecEnd, float _fLifeTime, DWORD _dwColor );
	void		ShowDebugCapsule(const string& strName, MCapsule &_capsule, float _fLifeTime, DWORD _dwColor);
	void		ShowDebugOBox(const string& strName, MOBox &_obox, float _fLifeTime, DWORD _dwColor);

private:

	struct SDebugBox
	{
		SDebugBox(const RMatrix& _refMatWorld, RBoundingBox& bbox, float _fLifeTime, DWORD _dwColor, bool _bRenderAxis)
		: m_matWorld(_refMatWorld)
		, m_box(bbox)
		, m_fLifeTime(_fLifeTime)
		, m_dwColor(_dwColor)
		, m_bRenderAxis(_bRenderAxis)
		, m_bRenderBox( true )
		{
		}
		SDebugBox(const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor, bool _bRenderAxis, bool _bRenderBox = true)
		: m_matWorld(_refMatWorld)
		, m_fLifeTime(_fLifeTime)
		, m_dwColor(_dwColor)
		, m_bRenderAxis(_bRenderAxis)
		, m_bRenderBox(_bRenderBox)
		{
			m_box.vmax.Set(_fBoxSize, _fBoxSize, _fBoxSize);
			m_box.vmin.Set( -_fBoxSize, -_fBoxSize, -_fBoxSize);
		}

		RBoundingBox	m_box;
		RMatrix			m_matWorld;

		float			m_fLifeTime;
		DWORD			m_dwColor;
		bool			m_bRenderAxis;
		bool			m_bRenderBox;
	};

	struct SDebugSphere
	{
		SDebugSphere(const RMatrix& _refMatWorld, float _fRadius, float _fLifeTime, DWORD _dwColor)
		: m_matWorld(_refMatWorld)
		, m_fRadius(_fRadius)
		, m_fLifeTime(_fLifeTime)
		, m_dwColor(_dwColor)
		{}
		float			m_fRadius;
		RMatrix			m_matWorld;

		float			m_fLifeTime;
		DWORD			m_dwColor;
	};

	struct SDebugLine
	{
		SDebugLine(const RVector& _refVecStart, const RVector& _refVecEnd, float _fLifeTime, DWORD _dwColor)
			: m_fLifeTime(_fLifeTime)
			, m_dwColor(_dwColor)
			, m_vStartPos(_refVecStart)
			, m_vEndPos( _refVecEnd)
		{}

		float			m_fLifeTime;
		DWORD			m_dwColor;

		RVector			m_vStartPos;
		RVector			m_vEndPos;
	};

	struct SDebugCapsule
	{
		SDebugCapsule(MCapsule &_capsule, float _fLifeTime, DWORD _dwColor)
			: m_Capsule(_capsule)
			, m_fLifeTime(_fLifeTime)
			, m_dwColor(_dwColor)
		{}

		MCapsule m_Capsule;

		float			m_fLifeTime;
		DWORD			m_dwColor;
	};

	struct SDebugOBox
	{
		SDebugOBox(MOBox &_obox, float _fLifeTime, DWORD _dwColor)
			: m_OBox(_obox)
			, m_fLifeTime(_fLifeTime)
			, m_dwColor(_dwColor)
		{}

		MOBox			m_OBox;

		float			m_fLifeTime;
		DWORD			m_dwColor;
	};


	struct SDebugPolycon
	{
		SDebugPolycon(const RMatrix& _refMatWorld, RVector* pVector, int nVertexCount, float _fLifeTime, DWORD _dwColor)
			: m_matWorld(_refMatWorld)
			, m_nVertexCount(nVertexCount)
			, m_fLifeTime(_fLifeTime)
			, m_dwColor(_dwColor)
			, m_nTriangleCount(0)
		{
			if(nVertexCount >= 3)
			{
				m_pVertex = new RVector[nVertexCount];
				for(int i = 0; i < nVertexCount; i++)
				{
					m_pVertex[i] = pVector[i];
				}

				m_nTriangleCount = nVertexCount / 3;
			}
		}

		RVector*		m_pVertex;
		RMatrix			m_matWorld;

		int				m_nVertexCount;
		int				m_nTriangleCount;
		float			m_fLifeTime;
		DWORD			m_dwColor;
	};

	struct SDebugPlane
	{
		SDebugPlane(const RMatrix& _refMatWorld, MPlane& _refPlane, float fSize, float _fLifeTime, DWORD _dwColor)
			: m_matWorld(_refMatWorld)
			, m_fSize(fSize)
			, m_fLifeTime(_fLifeTime)
			, m_dwColor(_dwColor)
		{
			m_vAxisY = _refPlane.Normal().CrossProduct(RVector::AXISZ);
			m_vAxisX = _refPlane.Normal().CrossProduct(m_vAxisY);
		}

		RVector			m_vAxisX;
		RVector			m_vAxisY;
		RMatrix			m_matWorld;

		float			m_fSize;
		float			m_fLifeTime;
		DWORD			m_dwColor;
	};

	struct SDebugText
	{
		SDebugText(const RVector& vPos, const char* pSzText, float _fLifeTime, DWORD _dwColor)
			: m_vPos(vPos)
			, m_strText(pSzText)
			, m_fLifeTime(_fLifeTime)
			, m_dwColor(_dwColor)
		{
		}

		RVector			m_vPos;
		std::string		m_strText;

		float			m_fLifeTime;
		DWORD			m_dwColor;
	};

	std::list< SDebugBox >		m_listDebugBoxes;
	std::list< SDebugSphere >	m_listDebugSpheres;
	std::list< SDebugLine >		m_listDebugLines;
	std::list< SDebugCapsule >	m_listDebugCapsule;
	std::list< SDebugOBox >		m_listDebugOBox;
	std::list< SDebugPolycon >	m_listDebugPolycon;
	std::list< SDebugPlane >	m_listDebugPlane;
	std::list< SDebugText >		m_listDebugText;

	std::map< string, SDebugBox* >		m_mapDebugBoxes;
	std::map< string, SDebugSphere* >	m_mapDebugSpheres;
	std::map< string, SDebugLine* >		m_mapDebugLines;
	std::map< string, SDebugCapsule* >	m_mapDebugCapsule;
	std::map< string, SDebugOBox* >		m_mapDebugOBox;

	float	m_fElapsedTime;
	RFont*	m_pFont;
	RMatrix	m_matTextReflectAndScale;
};

}
