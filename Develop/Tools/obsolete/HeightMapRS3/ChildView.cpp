// ChildView.cpp : CChildView 클래스의 구현
//

#include "stdafx.h"
#include "HeightMapRS3.h"
#include "ChildView.h"
#include "RTypes.h"
#include "RApplication.h"

#include <string>
#include "RMeshUtil.h"

using namespace std;
using namespace rs3;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//LPDIRECT3D9 CChildView::m_lpD3D9 = NULL;
//LPDIRECT3DDEVICE9 CChildView::m_lpd3dDevice = NULL;

// CChildView
CPoint old_pos;
CPoint now_pos;

RDeviceD3D* CChildView::m_pdeviced3d = NULL;

CChildView::CChildView() : 
m_pTerrain(NULL)
, m_pMesh(NULL)
, m_pFont(NULL)
, m_vBoxPosition(100.0f, 100.0f, 0.0f)
{
    m_pSceneManager = NULL;
	m_pTreeSystem = NULL;
}

CChildView::~CChildView()
{
	if ( m_pTerrain )
	{
		m_pSceneManager->RemoveChild(m_pTerrain);
		delete m_pTerrain;
		m_pTerrain = NULL;
	}
	if ( m_pTreeSystem )
	{
		m_pTreeSystem->RemoveFromParent();
		SAFE_DELETE(m_pTreeSystem);
	}

	if ( m_pFont )
		m_pFont->Release();
	if ( m_pMesh )
		m_pMesh->Release();

	SAFE_DELETE(m_pSceneManager);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pdeviced3d);
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void CChildView::InitState()
{
	//Timer.StartTimer();

	RECT rt;
	GetClientRect(&rt);

	UINT uWidth = rt.right - rt.left;
	UINT uHeight = rt.bottom - rt.top;

	RMatrix matProj;
	matProj.SetProjectionMatrixFovRH(PI/4.0f,(float)uWidth/(float)uHeight, 4.0f, 80000.0f);
	m_pdeviced3d->SetTransform( RST_PROJECTION, matProj);

	LPDIRECT3DDEVICE9 lpd3dDevice = m_pdeviced3d->GetD3DDevice();
	lpd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);

	m_pdeviced3d->SetTextureFilter( 0, RTF_BILINEAR_ANISO );
	m_pdeviced3d->SetTextureFilter( 1, RTF_BILINEAR_ANISO );

	m_pSceneManager = new RSceneManager(m_pdeviced3d);

	// 파일 로드
	m_pTerrain = new RTerrain(m_pdeviced3d);
	m_pTerrain->ImportDisplacementmapFromBMPFile("disp.bmp", 32, 2, 10000, 10000, 5000, 128);
	m_pTerrain->CreateLayer(2);
	m_pTerrain->LoadTileMapFromFile( 0, 400.0f, 400.0f, "tile1.bmp");
	m_pTerrain->LoadAlphaMapFromFile( 0, "alpha1.dds");
	m_pTerrain->LoadTileMapFromFile( 1, 600.0f, 600.0f,"tile2.bmp");
	m_pTerrain->LoadAlphaMapFromFile( 1, "alpha2.dds");
	
	RSetDevice(m_pdeviced3d);

	m_pCamera = new L3DCamera();
	m_pCamera->Reset();
	m_pCamera->SetNearFarZ( 4.0f, 80000.0f);

	string strPath("./data/");

	m_pTreeSystem = new RTreeSystem();
	m_pTreeSystem->Create(strPath.c_str());

	string strTreeNames[] = 
	{
		"RedOak.spt"
		,"GiantSequoia.spt"
		,"GreenAsh.spt"
	};

	for ( int i = 0; i<3; ++i )
	{
		string strTemp = strPath + strTreeNames[i];
		m_pTreeSystem->AddTrees(strTemp.c_str(), ::rand(), 150.0f, 50.0f);
	}

	for ( int i = 0; i<250; ++i )
	{
		float x = ::rand()/(float)RAND_MAX * m_pTerrain->GetWidthSize();
		float y = ::rand()/(float)RAND_MAX * m_pTerrain->GetHeightSize();
		RVector vPos;
		m_pTerrain->GetPickVertical(vPos, x, y);

		RVector vFront(0, 1, 0), vDir;
		RMatrix mat;
		mat.MakeIdentity();
		mat.SetRotationX(D3DXToRadian((float)(::rand()%180)) );
		mat.TransformNormal(vFront, vDir);

		RTreeInstance *pInstance = m_pTreeSystem->MakeInstance(strTreeNames[::rand()%3].c_str());
		if ( pInstance )
		{
			pInstance->SetPosition(vPos);
			pInstance->SetDirection(vDir, RVector(0, 0, 1));
			pInstance->UpdateTransform();

			m_kNodeList.push_back(pInstance);
			m_pTreeSystem->AddChild(pInstance);
		}
	}

	m_pTreeSystem->SetWindStrength(0.1f);

	m_pSceneManager->AddChild(m_pTerrain);
	m_pSceneManager->AddChild(m_pCamera);
	m_pSceneManager->AddChild(m_pTreeSystem);
	m_pSceneManager->SetActiveCamera(m_pCamera);
}

void CChildView::RenderState()
{

}

void CChildView::RenderScene()
{
	ASSERT(m_pdeviced3d);

	RMatrix mat;
	mat.MakeIdentity();

	if ( !GetAsyncKeyState('2'))
		m_pdeviced3d->SetFillMode( RFILL_SOLID );
	else
		m_pdeviced3d->SetFillMode( RFILL_WIRE );

	if ( GetAsyncKeyState(VK_LEFT))
		m_vBoxPosition.x -= .33f;
	if ( GetAsyncKeyState(VK_RIGHT))
		m_vBoxPosition.x += .33f;
	if ( GetAsyncKeyState(VK_UP))
		m_vBoxPosition.y += .33f;
	if ( GetAsyncKeyState(VK_DOWN))
		m_vBoxPosition.y -= .33f;

	RECT rt;
	GetClientRect(&rt);

	RVector pos, dir;
	RVector v;

	RMatrix projMatrix = m_pCamera->GetProjectionMatrix();

	v.x =  ( ( ( 2.0f * now_pos.x ) / rt.right ) - 1 ) / projMatrix._11;
	v.y = -( ( ( 2.0f * now_pos.y ) / rt.bottom ) - 1 ) / projMatrix._22;
	v.z = -1.0f;

	RMatrix viewMatrix = m_pCamera->GetViewMatrix();
	RMatrix m;
	viewMatrix.GetInverse(&m, NULL);
	dir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	dir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	dir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;

	pos.x = m._41;
	pos.y = m._42;
	pos.z = m._43;

	static RVector p;
	static RVector q;
	if ( GetAsyncKeyState(VK_SPACE))
	{
		p = RVector( pos.x, pos.y, pos.z );
		q = RVector(p.x+dir.x*100000.0f, p.y+dir.y*100000.0f, p.z+dir.z*100000.0f);
	}

	m_pdeviced3d->Clear();

	LPDIRECT3DDEVICE9 pDevice = m_pdeviced3d->GetD3DDevice();

	m_pdeviced3d->BeginScene();
	{
		m_pSceneManager->Update();
		m_pSceneManager->Render();
		//for ( RSceneNodeList::iterator it = m_kNodeList.begin(); it != m_kNodeList.end(); ++it )	
		//	RSceneNode *pNode = *it;
			
		RMatrix mat;
		mat.MakeIdentity();

		_draw_matrix(mat, 100.0f);

		RECT rt = { 0, 20, 0, 0 };
		//TCHAR aStr[32];
		//float fFPS = Timer.GetCurrentFPS();
		//_stprintf(aStr,_T("FPS: %f"), fFPS);
		//m_pFont->DrawText(NULL, aStr, -1,&rt, DT_NOCLIP | DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 0, 0));

		m_pdeviced3d->EndScene();
	}
	m_pdeviced3d->Flip();
	//Timer.Flip();
}

// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트
	// 암것도 하지 않음 .
}


void CChildView::OnTimer(UINT nIDEvent)
{
	RenderScene();
	CWnd::OnTimer(nIDEvent);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	float m_fRotStep = 0.5f;
	now_pos = point;
	CPoint pos = point - old_pos;

	if( nFlags & MK_LBUTTON   ) 
	{// pan
	}
	else if( nFlags & MK_RBUTTON ) 
	{// rot
		float n;
		n = -pos.x * m_fRotStep;
		m_pCamera->RotationH(n);
		n = -pos.y * m_fRotStep;
		m_pCamera->RotationV(n);	
	}
	else if ( nFlags & MK_MBUTTON )
	{ // move
		// X, Y Vector를 구한다.
		RMatrix kViewMatrix = m_pCamera->GetViewMatrix();
		D3DXVECTOR3 vBaseXAxis( kViewMatrix._11, kViewMatrix._21, kViewMatrix._31);
		D3DXVECTOR3 vBaseYAxis( kViewMatrix._12, kViewMatrix._22, kViewMatrix._32);
		D3DXVECTOR3 vBaseZAxis( kViewMatrix._13, kViewMatrix._23, kViewMatrix._33);
		D3DXVECTOR3 vCrs;

		D3DXVec3Cross(&vCrs,&D3DXVECTOR3(0, 0, 1),&vBaseXAxis);
		D3DXVECTOR3 vMoveVec = ( vBaseXAxis*(pos.x*-0.5f) + vCrs*(pos.y*0.5f));
		vMoveVec *= 3.0f;
		RVector vec(vMoveVec.x, vMoveVec.y, vMoveVec.z);
		m_pCamera->MoveAtPoint(vec);
	}

	old_pos = point;
	RenderScene();

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	float m_fZoomStep = 0.01f;

	if ( GetAsyncKeyState(VK_LSHIFT) )
		m_fZoomStep = 10.0f;
	m_pCamera->Zoom(-zDelta*m_fZoomStep);
	RenderScene();


	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
