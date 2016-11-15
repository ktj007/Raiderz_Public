#include "StdAfx.h"
#include "Gizmo.h"
#include "C_ControlMediatorHelperAction.h"

#include "RRenderHelper.h"
#include "RFont.h"
#include "RSUtil.h"
#include "RCameraSceneNode.h"
#include "MainFrm.h"
#include "C_ControlMediatorHelperRender.h"

using namespace rs3;

//////////////////////////////////////////////////////////////////////////
// Gizmo Components

const DWORD GIZMO_SELECTED_COLOR = 0xffffff00;
const float TOLER_DISTANCE = 0.03f;	// 대략 스크린픽셀로 5-10 쯤되려나..

class GizmoComponent
{
public:
	/// Gizmo 가 origin 위치, fSize 크기로 있을때 (마우스 피킹 선의 월드 선분) pq 와의 피킹되었는가, pfDistance : 거리
	virtual bool		PickTest( const RVector& origin, float fSizeFactor, const RVector& p, const RVector& q, float* pfDistance ) = 0;
	virtual void		OnRender( const RVector& origin, const RMatrix& localframe, float fSizeFactor, bool bSelected, rs3::RFont* pFont, RCameraSceneNode* pCamera ) = 0;

	AXIS_STATE			GetApplyState() { return m_applyState; }

protected:

	AXIS_STATE			m_applyState;
};


/// x, y, z 축에 해당하는 콤포넌트
const float DEFAULT_SIZE_AXIS = 0.2f;

class GizmoComponent_Axis : public GizmoComponent
{
public:
	GizmoComponent_Axis(RVector& axis, DWORD dwColor, AXIS_STATE applyState, const char* szName)
		: m_axis(axis)
		, m_dwColor(dwColor)
		, m_szName( szName )
	{
		m_applyState = applyState;
	}

	virtual bool		PickTest( const RVector& origin, float fSizeFactor, const RVector& p, const RVector& q, float* pfDistance ) override
	{
		float fSize = DEFAULT_SIZE_AXIS * fSizeFactor;

		float fAxisDistance = MMath::GetDistanceSegment( origin, origin + fSize * m_axis, p, q );
		float fTolerDist = fSize * TOLER_DISTANCE;
		*pfDistance = fAxisDistance - fTolerDist;

		return *pfDistance < 0 ;
	}
	
	virtual void		OnRender( const RVector& origin, const RMatrix& localframe, float fSizeFactor, bool bSelected, rs3::RFont* pFont, RCameraSceneNode* pCamera ) override
	{
		float fSize = DEFAULT_SIZE_AXIS * fSizeFactor;

		float fCylinderLength = fSize;
		float fCylinderRadius = fSize * 0.005;
		float fCornRadius = fSize* 0.03;
		float fCornLength = fSize * 0.10;

		DWORD dwColorAxis = bSelected ? GIZMO_SELECTED_COLOR : m_dwColor;

		RVector dir = m_axis * localframe;
		RRenderHelper::SetRenderState();
		RRenderHelper::RenderCylinder(origin,dir,fCylinderRadius,fCylinderLength, dwColorAxis);
		RRenderHelper::RenderCorn(origin+fCylinderLength*dir,dir,fCornRadius,fCornLength, m_dwColor );

		RVector vecPolar(origin+(fCylinderLength+1.2f*fCornLength)*dir );
		MVector2 vecScreen;
		GetScreenFromWorld( &vecScreen, &vecPolar, 
			&REngine::GetDevice().GetViewport(),
			&RMatrix::IDENTITY, &pCamera->GetViewMatrix(), &pCamera->GetProjectionMatrix());
		pFont->BeginFont();
		// 글자 중앙이 원하는곳에 오도록 offset -4 ( 대략 글자크기 반 )
		pFont->DrawText_UNICODE(  vecScreen.x -4 , vecScreen.y -4 , MLocale::ConvAnsiToUTF16(m_szName).c_str(), -1, dwColorAxis );
		pFont->EndFont();
	}

protected:
	RVector m_axis;
	DWORD	m_dwColor;
	const char* m_szName;
};

/// xy, yz, xz 평면에 해당하는 콤포넌트, 
const float DEFAULT_SIZE_PLANE = 0.08f;
class GizmoComponent_Plane : public GizmoComponent
{
public:
	GizmoComponent_Plane(RVector& axis1, DWORD dwColor1, RVector& axis2, DWORD dwColor2, AXIS_STATE applyState)
		: m_axis1(axis1)
		, m_axis2(axis2)
		, m_dwColor1(dwColor1)
		, m_dwColor2(dwColor2)
	{
		m_applyState = applyState;		
	}

	virtual bool		PickTest( const RVector& origin, float fSizeFactor, const RVector& p, const RVector& q, float* pfDistance ) override
	{
		// 2개의 보조 선분 과의 거리
		float fSize = DEFAULT_SIZE_PLANE * fSizeFactor;
		const float PLANE_TOLER_DISTANCE = 0.08f;
		float fTolerDist = fSize * PLANE_TOLER_DISTANCE;

		RVector localAxis1 = fSize * m_axis1;
		RVector localAxis2 = fSize * m_axis2;
		m_vertices[0] = origin;
		m_vertices[1] = origin + localAxis1;
		m_vertices[2] = origin + localAxis1 + localAxis2;
		m_vertices[3] = origin + localAxis2;

		float fAxisDistance = MMath::GetDistanceSegment( m_vertices[1], m_vertices[2], p, q );
		*pfDistance = fAxisDistance - fTolerDist;

		fAxisDistance = MMath::GetDistanceSegment( m_vertices[2], m_vertices[3], p, q );
		*pfDistance = min(*pfDistance , fAxisDistance - fTolerDist );

		if( *pfDistance < 0 )
			return true;

		// 보조 선분이 선택이 안되었다면 평면 picking 후 거리계산
 		MVector3 vecIntPos;

		// triangle fan 이라 가정
		for( int i=0; i<2; i++ )
		{
			if( MMath::IsIntersectNoCull( p, q-p, m_vertices[0], m_vertices[i+1], m_vertices[i+2], &vecIntPos ) )
			{
				*pfDistance = (vecIntPos - p).Length();
				return true;
			}
		}
		return false;
	}

	virtual void		OnRender( const RVector& origin, const RMatrix& localframe, float fSizeFactor, bool bSelected, rs3::RFont* pFont, RCameraSceneNode* pCamera ) override
	{
		float fSize = DEFAULT_SIZE_PLANE * fSizeFactor;

		RVector localAxis1 = fSize * m_axis1 * localframe;
		RVector localAxis2 = fSize * m_axis2 * localframe;
		m_vertices[0] = origin;
		m_vertices[1] = origin + localAxis1;
		m_vertices[2] = origin + localAxis1 + localAxis2;
		m_vertices[3] = origin + localAxis2;

		RRenderHelper::SetRenderState();

		RRenderHelper::RenderLine( MMatrix::IDENTITY, m_vertices[1], m_vertices[2], bSelected ? GIZMO_SELECTED_COLOR : m_dwColor1 );
		RRenderHelper::RenderLine( MMatrix::IDENTITY, m_vertices[2], m_vertices[3], bSelected ? GIZMO_SELECTED_COLOR : m_dwColor2 );

		if( bSelected )
		{
			DWORD dwSelectedPlaneColor = 0x80000000 | (0xffffff & GIZMO_SELECTED_COLOR);
			REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
			RRenderHelper::RenderTriangles( MMatrix::IDENTITY, m_vertices, 2, dwSelectedPlaneColor , RPT_TRIANGLEFAN );
		}
	}

protected:
	RVector m_axis1;
	DWORD	m_dwColor1;
	RVector m_axis2;
	DWORD	m_dwColor2;
	RVector	m_vertices[4];
};

/// x, y, z 축 회전에 쓰일 원 콤포넌트
const float DEFAULT_SIZE_CIRCLE = 0.1f;
class GizmoComponent_Circle : public GizmoComponent
{
public:
	GizmoComponent_Circle(RVector& axis, DWORD dwColor, AXIS_STATE applyState)
		: m_axis(axis)
		, m_dwColor(dwColor)
	{
		m_applyState = applyState;		
	}

	virtual bool		PickTest( const RVector& origin, float fSizeFactor, const RVector& p, const RVector& q, float* pfDistance ) override
	{
		float fSize = DEFAULT_SIZE_CIRCLE * fSizeFactor;

		// 회전 원이 그려진 평면 ( origin 을 지나면서 upvector가 m_axis인 평면)
		RPlane rotationPlane(m_axis, origin);

		RVector intPos;
		bool bIntersect = rotationPlane.Intersect(intPos, p, q);
		if( bIntersect )
		{
			float fDistanceFromCenter = ( origin - intPos ).Length();
			float fTolerDist = fSize * 3 * TOLER_DISTANCE;	// 원이 클릭해서 잡기가 쉽지 않아 더 두텁게 인정해줌
			*pfDistance = fabs( fDistanceFromCenter - fSize );
			return *pfDistance - fTolerDist < 0 ;
		}

		return false;
	}

	virtual void		OnRender( const RVector& origin, const RMatrix& localframe, float fSizeFactor, bool bSelected, rs3::RFont* pFont, RCameraSceneNode* pCamera ) override
	{
		RMatrix matTM;
		RVector dir( m_axis.y, m_axis.z, m_axis.x );
		matTM.SetLocalMatrix( origin, dir * localframe, m_axis * localframe);
		
		float fSize = DEFAULT_SIZE_CIRCLE * fSizeFactor;

		RMatrix matScaleTM;
		matScaleTM.SetScaleMatrix( fSize, fSize, fSize );
		matTM = matScaleTM * matTM ;


		// render circle ?
		const int DONUT_SEGMENT = 40;

		float d = 1.f;

		RVector vecToCamera = Normalize( origin - pCamera->GetWorldPosition() );

		RVt_pos_clr donutVertices[DONUT_SEGMENT];
		for( int i=0; i<DONUT_SEGMENT; i++ )
		{
			float theta = (2*MMath::PI*i)/(DONUT_SEGMENT-1);
			donutVertices[i].vPos = RVector( d * sin(theta), d * cos(theta), 0.f) * matTM;
			bool bBack = DotProduct( vecToCamera, Normalize(donutVertices[i].vPos - origin) ) > 0.1f ;
			donutVertices[i].dwColor = bSelected ? GIZMO_SELECTED_COLOR : 
				bBack ? 0xff808080 : m_dwColor;
		}

		RRenderHelper::SetRenderState();

		REngine::GetDevice().SetTransform( RST_WORLD, RMatrix::IDENTITY );
		REngine::GetDevice().SetFvF( RVt_pos_clr::FVF );
		REngine::GetDevice().SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
		REngine::GetDevice().SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);

		REngine::GetDevice().DrawPrimitiveUP(RPT_LINESTRIP, DONUT_SEGMENT-1, donutVertices,sizeof(RVt_pos_clr));
		RRenderHelper::EndRenderState();

	}

protected:
	RVector m_axis;
	DWORD	m_dwColor;
};

// 외곽 회색 원
class GizmoComponent_DummyCircle : public GizmoComponent
{
public:
	GizmoComponent_DummyCircle() {}

	virtual bool		PickTest( const RVector& origin, float fSizeFactor, const RVector& p, const RVector& q, float* pfDistance ) override
	{
		return false;
	}

	virtual void		OnRender( const RVector& origin, const RMatrix& localframe, float fSizeFactor, bool bSelected, rs3::RFont* pFont, RCameraSceneNode* pCamera ) override
	{
		RMatrix matTM;

		RVector vecToCamera = Normalize( origin - pCamera->GetWorldPosition() );

		matTM.SetLocalMatrix( origin, vecToCamera * localframe, RVector(0,0,1) * localframe );

		float fSize = DEFAULT_SIZE_CIRCLE * fSizeFactor;

		RRenderHelper::SetRenderState();
		RRenderHelper::RenderCircle( matTM, fSize, 0xff808080);
	}
};

//////////////////////////////////////////////////////////////////////////

Gizmo::Gizmo( CControlMediator* pControlMediator_ )
: m_pControlMediator_(pControlMediator_)
, m_pFont( pControlMediator_->GetHelperRender()->GetRFont() )
, m_pDeviceD3D_(pControlMediator_->GetCurrentDevice())
, m_pCamera(pControlMediator_->GetToolCamera())
, m_state(AXIS_NULL)
{
}

Gizmo::~Gizmo(void)
{
	for( unsigned int i=0; i<m_vecComponents.size(); i++)
	{
		delete m_vecComponents[i];
	}
	m_vecComponents.clear();
}

/// screen space 에서 같은 크기로 보여주기 위한 world size factor 를 구한다
float Gizmo::GetSizeFactor()
{
	rs3::RMatrix viewMat(m_pDeviceD3D_->GetTransform(RST_VIEW));
	rs3::RVector vViewPos;

	RVector pos = GetPosition();
	viewMat.TransformVect(pos, vViewPos);

	float fFactor = fabs(vViewPos.z);
	return fFactor;
}

void Gizmo::UpdateStates()
{
	// get p, q
	RVector p, q;
	LONG cx = m_pControlMediator_->m_mouseInfo.m_currentMouseX;
	LONG cy = m_pControlMediator_->m_mouseInfo.m_currentMouseY;
	m_pControlMediator_->GetHelperAction()->GetMouseLine(cx, cy, p, q);

	// get pos
	RVector pos = GetPosition();

	// get local transformed p, q
	RMatrix matLocalFrameInv;
	if (m_pControlMediator_->GetHelperAction()->GetSelectedObjectActionFrame().GetInverse(&matLocalFrameInv))
	{
		p -= pos;
		p = p * matLocalFrameInv;
		p += pos;

		q -= pos;
		q = q * matLocalFrameInv;
		q += pos;
	}

	AXIS_STATE newState = AXIS_NULL;
	float fMinDistance = FLT_MAX;
	float fSizeFactor = GetSizeFactor();

	for( unsigned int i=0; i<m_vecComponents.size(); i++)
	{
		GizmoComponent* pComponent = m_vecComponents[i];
		float fDistance;
		if( pComponent->PickTest( pos, fSizeFactor, p, q, &fDistance ) )
		{
			if( fDistance < fMinDistance )
			{
				newState = pComponent->GetApplyState();
				fMinDistance = fDistance;
			}
		}
	}

	if( newState != m_state )
		m_state = newState;
}

void Gizmo::Render()
{
	//선택 축은 모든것 위에
	m_pDeviceD3D_->SetDepthEnable(false);

	RVector pos = GetPosition();
	float fSizeFactor = GetSizeFactor();

	const RMatrix& matActionFrame = m_pControlMediator_->GetHelperAction()->GetSelectedObjectActionFrame();
	for( unsigned int i=0; i<m_vecComponents.size(); i++)
	{
		GizmoComponent* pComponent = m_vecComponents[i];
		bool bSelected = (pComponent->GetApplyState() == m_state);
		pComponent->OnRender( pos, matActionFrame, fSizeFactor, bSelected, m_pFont, m_pCamera );
	}
	
	m_pDeviceD3D_->SetDepthEnable(true);

}

//////////////////////////////////////////////////////////////////////////
// Position Gizmo

CControlMediatorHelperGizmo_Position::CControlMediatorHelperGizmo_Position( CControlMediator* pControlMediator_ )
: Gizmo( pControlMediator_ )
{
	m_vecComponents.push_back( new GizmoComponent_Axis(RVector(1,0,0), 0xffff0000, AXIS_X, "x" ) );
	m_vecComponents.push_back( new GizmoComponent_Axis(RVector(0,1,0), 0xff00ff00, AXIS_Y, "y" ) );
	m_vecComponents.push_back( new GizmoComponent_Axis(RVector(0,0,1), 0xff0000ff, AXIS_Z, "z" ) );

	m_vecComponents.push_back( new GizmoComponent_Plane(RVector(1,0,0), 0xffff0000, RVector(0,1,0), 0xff00ff00, AXIS_XY) );
	m_vecComponents.push_back( new GizmoComponent_Plane(RVector(0,1,0), 0xff00ff00, RVector(0,0,1), 0xff0000ff, AXIS_YZ) );
	m_vecComponents.push_back( new GizmoComponent_Plane(RVector(1,0,0), 0xffff0000, RVector(0,0,1), 0xff0000ff, AXIS_XZ) );
	
}


//////////////////////////////////////////////////////////////////////////
// Rotation Gizmo

CControlMediatorHelperGizmo_Rotation::CControlMediatorHelperGizmo_Rotation( CControlMediator* pControlMediator_ )
: Gizmo( pControlMediator_ )
{
	m_vecComponents.push_back( new GizmoComponent_DummyCircle );
	m_vecComponents.push_back( new GizmoComponent_Circle(RVector(1,0,0), 0xffff0000, AXIS_X) );
	m_vecComponents.push_back( new GizmoComponent_Circle(RVector(0,1,0), 0xff00ff00, AXIS_Y) );
	m_vecComponents.push_back( new GizmoComponent_Circle(RVector(0,0,1), 0xff0000ff, AXIS_Z) );
}
