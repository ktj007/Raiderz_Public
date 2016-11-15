// RView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <afxdlgs.h>
#include <Mmsystem.h>
#include "EffectTool.h"
#include "RView.h"
#include "RConfiguration.h"
#include "RDeviceD3D.h"

#include "RToolUtil.h"
#include "RToolHelper.h"
#include "RMeshUtil.h"
#include "RMeshMgr.h"

#include "RBaseEmitter.h"
#include "REmitterExtend.h"
#include "REmitterXML.h"
//#include "d3exutil.h"
#include "REffectInstance.h"
#include "RApplication.h"
#include "RRenderHelper.h"

#include "MFileSystem.h"

using namespace rs3;

extern CRView*			g_rView;
extern CControlView*	g_ControlView;

// CRView

IMPLEMENT_DYNCREATE(CRView, CView)

CRView::CRView()
: m_pEffectMesh(NULL)
{
	g_rView = this;

	m_pDevice = NULL;

	m_fRotStep	= 0.1f;
	m_fZoomStep = 0.1f;

	m_pSky		= NULL;

	m_pEffect	= NULL;
	m_pEmitter	= NULL;

	m_bAxis		= true;
	m_bWired	= false;
	m_bGrid		= true;
	m_bEmitter	= false;

	m_dwBGColor = D3DCOLOR_XRGB( 192, 192, 192);

	// 캐릭터 텍스쳐 path [9/5/2006 madduck]
	config.SetTexturePath("../../Data/Texture/SpeedTree/;../../Data/Texture/IndoorMap/;../../Data/Texture/MapObject/;../../Data/Texture/Character/;../../Data/Texture/SFX");

	if ( !ConfigFromXML())
	{
		config.SetShaderPath(m_strPathname);
	}
	config.SetShaderPath(m_strPathname);

	//config.SetEffectPath("../../Data/SFX/");
}

bool CRView::ConfigFromXML()
{
	MXml kXml;
	if ( !kXml.LoadFile(FILENAME_CONFIG))
		return false;

	TCHAR tStr[256];
	MXmlElement *pElement = kXml.Doc()->FirstChildElement("shaderpath");
	if ( !pElement )
		return false;

	_Contents(tStr,pElement,"",&kXml);

	if ( (tStr[0] >= _T('A') && tStr[0] <= _T('Z'))
		|| (tStr[0] >= _T('a') && tStr[0] <= _T('z')) )	// is alphabet ?
	{
		m_strPathname = tStr;

	}
	else
	{
		m_strPathname += tStr;
	}

	return true;
}

CRView::~CRView()
{
	Destroy();

	m_pSceneManager->Destroy();

//	RMeshMgr::GetInstance()->DelAll();
	RMeshMgr::GetInstance()->Destroy();
	RToolApplication::Destroy();
}

void CRView::Destroy()
{
	L3DICONObject::GetInstance()->ReleaseObject();

	m_pSceneManager->RemoveChild(m_pSky);
	SAFE_DELETE(m_pSky);

	m_pSceneManager->RemoveChild(m_pTextNode);
	SAFE_DELETE(m_pTextNode);

	m_pFont->Finalize();
	SAFE_DELETE(m_pFont);

	m_pSceneManager->RemoveChild(&m_kCamera);

	//  [10/17/2006 madduck]
	m_pActor.Stop();
	//RMeshMgr::GetInstance()->DelAll();
	m_pActor.Destroy();
	m_pSceneManager->RemoveChild(&m_pActor);

	//m_pSceneManager->RemoveChild(m_pEffect);
	SAFE_DELETE(m_pEffect);
}

BEGIN_MESSAGE_MAP(CRView, CView)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
//	ON_WM_DROPFILES()
ON_WM_KEYUP()
END_MESSAGE_MAP()


// CRView 그리기입니다.

void CRView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}


// CRView 진단입니다.

#ifdef _DEBUG
void CRView::AssertValid() const
{
	CView::AssertValid();
}

void CRView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CRView 메시지 처리기입니다.

void CRView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	// [10/12/2006 madduck]
	// TODO : RToolApplication::Create() 이걸 사용했음....
	//if(RToolApplication::Create())
	{
		m_bActive = true;

		m_pDevice = new RDeviceD3D();
		RSetDevice(m_pDevice);
		if ( m_pDevice )
		{
			RECT rt;
			GetClientRect(&rt);
			// Create Device 
			config.depthFormat = RFMT_D16;
			config.m_bFullScreen = false;
			config.m_nWidth = rt.right - rt.left;
			config.m_nHeight = rt.bottom - rt.top;
			config.pixelFormat = RFMT_ARGB32;

			if ( false == m_pDevice->Create(GetSafeHwnd()) )
			{
				DebugBreak();
				OutputDebugString("Device 생성 실패\n");
				PostQuitMessage(-1);
			}   
		}

		m_pSceneManager = new RSceneManager(m_pDevice);
		if(!m_pSceneManager->Create(false))
		{
			DebugBreak();
			OutputDebugString("manager 생성 실패\n");
			PostQuitMessage(-1);
		}   
	}

	InitState();
}

void CRView::InitEffect()
{
	if( m_pEffect == NULL)
	{
		m_pEffect = new REffect;
		m_pSceneManager->AddChild(m_pEffect);
	}
}

void CRView::ClearEffect()
{
	if( m_pEffect)
	{
		//m_pEffect->Destroy();
		SAFE_DELETE(m_pEffect);

		CPage1 *pPage = &g_ControlView->m_Page1;
		if ( pPage->GetSafeHwnd() != NULL )
		{
			pPage->InitValue();
		}

		m_pEmitter		= NULL;
		m_pEffectMesh	= NULL;
	}
}

void CRView::InitState(void)
{
	InitEffect();

	m_kCamera.Reset();
	m_kCamera.SetFov( MMath::PI/4.0f, (float)config.m_nWidth/(float)config.m_nHeight);
	m_kCamera.SetNearFarZ(3.0f, 9999.9f);
	m_kCamera.UpdateViewFrustrum();
	m_kCamera.SetActive(true);
	m_pSceneManager->AddChild(&m_kCamera);
	m_pSceneManager->SetPrimaryCamera(&m_kCamera);

	//  [8/29/2006 madduck]
	m_kCamera.Zoom(400);

	L3DICONObject::GetInstance()->CreateObject(((RDeviceD3D*)m_pDevice)->GetD3DDevice());

	m_pSky = new RSky;
	m_pSky->Init();
	m_pSky->Restore("sky.bmp");
	m_pSceneManager->AddChild(m_pSky);

	m_pFont = new RFont;
	m_pTextNode = new RTextSceneNode;

	m_pFont->Create(GetDevice(),"굴림",12);
	m_pFont->Initialize(GetDevice());
	m_pTextNode->SetFont(m_pFont);
	m_pTextNode->SetText("test");
	m_pTextNode->SetPosition(0,0);
	m_pSceneManager->AddChild(m_pTextNode);
}

void CRView::ResetCamera()
{
	m_kCamera.Reset();
	//m_kCamera.SetFov( MMath::PI/4.0f, (float)config.m_nWidth/(float)config.m_nHeight);
	//m_kCamera.SetNearFarZ(3.0f, 9999.9f);
	//m_kCamera.UpdateViewFrustrum();
	//m_kCamera.SetActive(true);
	//m_pSceneManager->AddChild(&m_kCamera);
	//m_pSceneManager->SetPrimaryCamera(&m_kCamera);

	//  [8/29/2006 madduck]
	m_kCamera.Zoom(400);
}

void CRView::OnWireFrame()
{
	m_bWired = !m_bWired;
}

void CRView::OnWireFrameUpdateUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( (m_bWired ? 1 : 0 ));
}
void CRView::OnViewAxis()
{
	m_bAxis = !m_bAxis;
}

void CRView::OnViewAxisUpdateUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( (m_bAxis ? 1 : 0 ));
}

void CRView::OnViewGrid()
{
	m_bGrid = !m_bGrid;
}

void CRView::OnViewGridUpdateUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( (m_bGrid ? 1 : 0 ));
}

void CRView::OnViewEmitter()
{
	m_bEmitter = !m_bEmitter;
}

void CRView::OnViewEmitterUpdateUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( (m_bEmitter ? 1 : 0 ));
}

void CRView::OnViewPath()
{
	if(m_pEmitter)
		m_pEmitter->m_bPath = !m_pEmitter->m_bPath;
}

void CRView::OnNew()
{
	ClearEffect();

	return;

	REffectMgr* pEffectMgr = REffectMgr::GetInstance();
	pEffectMgr->LoadEffectList("EffectList.xml");

	m_pSceneManager->RemoveChild(m_pEffect);
	SAFE_DELETE(m_pEffect);

	m_pEffect = (REffect*)pEffectMgr->CreateEffect("dmg_melee.Effect.xml");

	m_pSceneManager->AddChild(m_pEffect);

	m_pEmitter = (REmitter*)m_pEffect->GetEmitter(0);

	m_pEffect->Start();

	return;

	//////////////////////////////////////////////////////////////////////////

	//파일 저장 test
	list<REffectInstance*>::iterator itor;

	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	//SaveXML(this,xml.Doc());
	for (itor = REffectMgr::GetInstance()->m_InstCopyList.begin();
		itor != REffectMgr::GetInstance()->m_InstCopyList.end(); ++itor )
	{
		REffectInstance* pObject = (*itor);

		MXmlNode *pXMLNode = xml.Doc();

		MXmlElement *pElement = new MXmlElement("EFFECT");
		pObject->SaveToXML(*pElement);
		pXMLNode->LinkEndChild(pElement);

	}

	xml.SaveFile("madduck.xml");	

}

void CRView::RenderState(void)
{
	// RenderState
//	LPDIRECT3DDEVICE9 lpd3dDevice = m_pdevice3d->GetD3DDevice();

	if ( m_bWired )
		m_pDevice->SetFillMode(RFILL_WIRE);
	else
		m_pDevice->SetFillMode(RFILL_SOLID);

// 	if ( m_bKeyState[VK_UP] )
// 		m_kCamera.MoveAtPoint(D3DXVECTOR3(0,0,0.1f));
// 
// 	if ( m_bKeyState[VK_DOWN] )
// 		m_kCamera.MoveAtPoint(D3DXVECTOR3(0,0,-0.1f));
// 
// 	if ( m_bKeyState[VK_LEFT] )
// 		m_kCamera.RotationH(-m_fRotStep*5);
// 	if ( m_bKeyState[VK_RIGHT] )
// 		m_kCamera.RotationH(+m_fRotStep*5);
}

void CRView::RenderScene()
{
	if ( NULL == m_pDevice )
		return;
	
	static DWORD dwTime = GetTickCount();
	DWORD dwElapseTime = GetTickCount() - dwTime;
	dwTime += dwElapseTime;

	RenderState();

	// 스플릿윈도우크기조절시 화면 Fov 조정 [8/11/2006 madduck]
	{
		RECT rt;
		GetClientRect(&rt);
		m_kCamera.SetFov( MMath::PI/4.0f, (float)rt.right/(float)rt.bottom);
	}

	// 배경색 clear에서 ㅡ,ㅡ
	//m_kCamera.SetClearColor(m_dwBGColor);
	m_kCamera.UpdateViewFrustrum();

	RMatrix viewMatrix = m_kCamera.GetViewMatrix();

	m_pDevice->SetTransform( RST_VIEW, viewMatrix);
	m_pDevice->SetTransform( RST_PROJECTION, m_kCamera.GetProjectionMatrix());
	m_pDevice->SetTransform( RST_WORLD, RMatrix::IDENTITY);

	m_pDevice->Clear(true, true, false, m_dwBGColor);//D3DCOLOR_XRGB(0, 0, 0));
	m_pDevice->BeginScene();
	{
		if ( m_bGrid )
		{
			m_pDevice->SetLighting( false );

			RRenderHelper::SetRenderState();
			RToolHelper::DrawGrid(RVector(-200, 200 , 0),10.f,10.f,40,40, 0x228f8f8f );
			RToolHelper::DrawCenter(400.f,400.f,0xff88ff88);
			RRenderHelper::EndRenderState();
		}

		if(m_bAxis)
		{
			RMatrix rmat;
			rmat.MakeIdentity();
			float matscale = m_kCamera.m_fDist / 5.0f;
			if( matscale < 10.0f)
				matscale = 10.0f;
			_draw_matrix(m_pDevice, rmat, matscale);
		}

		if( m_bEmitter && (m_pEmitter != NULL) )
		{
			EMITTER_TYPE eType = m_pEmitter->GetEmitterType();
			const RVector *vRange = m_pEmitter->GetRange();
			const RVector vPos = m_pEmitter->GetPosition();

			RMatrix mat;
			mat.MakeIdentity();
			//범위는 (0,0,0)을 중심으로 위치한다.
			mat.SetScale(RVector(vRange->x*0.5f, vRange->y*0.5f, vRange->z*0.5f));
			//mat.SetScale(RVector(vRange->x, vRange->y, vRange->z));
			mat.MultiplyTo(m_pEmitter->GetLocalTransform(), mat);
			mat.SetTranslation(vPos);
			m_pDevice->SetTransform( RST_WORLD,(RMatrix)mat);

			switch ( eType )
			{
			case EMITTER_SPHERE :
				//L3DICONObject::GetInstance()->RenderSphere();
				RRenderHelper::RenderSphere(mat, 1.0f, 0xffffffff);
				//RRenderHelper::RenderBox(RMatrix::IDENTITY, m_pEmitter->GetAABB(), 0xffff0000);
				break;
			case EMITTER_BOX :
				//L3DICONObject::GetInstance()->RenderBox();
				RRenderHelper::RenderBox(mat, RBoundingBox(RVector(-1,-1,-1),RVector(1,1,1)));//, 0xffff0000);
				//RRenderHelper::RenderBox(RMatrix::IDENTITY, m_pEmitter->GetAABB(), 0xffff0000);
				break;
			case EMITTER_CLIPBOX:
				//L3DICONObject::GetInstance()->RenderBox();
				//RRenderHelper::RenderBox(RMatrix::IDENTITY, m_pEmitter->GetAABB(), 0xffff0000);
				break;
			}
		}

		m_pSceneManager->Update();
		m_pSceneManager->Render();

		if( m_pEmitter )
		{
			CString str;
			str.Format(" Time : %f \n Emitter Name : %s \n Loop : %s\n Render Num : %d \n Total Render Num : %d \n ElapseTime : %f",
				m_pEmitter->GetTime(), m_pEmitter->GetName().c_str(), m_pEmitter->IsLooping() ? "true" : "false",m_pEmitter->GetRenderParticleNum(), m_pEffect->GetRenderParticleNums(),dwElapseTime/1000.0f);
			m_pTextNode->SetText(str);

			//str.Format("Render Num : %d", m_pEmitter->GetRenderParticleNum());
			//m_pTextNode->SetPosition(0, 20);
			//m_pTextNode->SetText(str);

			//str.Format("total Render Num : %d", m_pEffect->GetRenderParticleNums());
			//m_pTextNode->SetPosition(0, 40);
			//m_pTextNode->SetText(str);

			//str.Format("ElapseTime : %f", dwElapseTime/1000.0f);
			//m_pTextNode->SetPosition(0, 60);
			//m_pTextNode->SetText(str);
		}
		else if( m_pEffectMesh)
		{
			CString str;
			str.Format(" Mesh Name : %s \n ElapseTime : %f",m_pEffectMesh->m_pMesh->GetFileName(),dwElapseTime/1000.0f);
			m_pTextNode->SetText(str);

			RRenderHelper::RenderBox(RMatrix::IDENTITY, m_pEffectMesh->GetAABB() );
		}
	}
	m_pDevice->EndScene();
	m_pDevice->Flip();
}

void CRView::OnMouseMove(UINT nFlags, CPoint point)
{
	static CPoint old_pos;
	CPoint pos = point - old_pos;

	if( nFlags & MK_LBUTTON   ) 
	{
		float n;
		n = -pos.x * m_fRotStep;
		m_kCamera.RotationH(n);
		n = -pos.y * m_fRotStep;
		m_kCamera.RotationV(n);	
	}
	else if( nFlags & MK_RBUTTON ) 
	{
	}
	else if ( nFlags & MK_MBUTTON )
	{ // move
		// X, Y Vector를 구한다.
		RMatrix viewMatrix = m_kCamera.GetViewMatrix();
		D3DXVECTOR3 vBaseXAxis( viewMatrix._11, viewMatrix._21, viewMatrix._31);
		D3DXVECTOR3 vBaseYAxis( viewMatrix._12, viewMatrix._22, viewMatrix._32);
		D3DXVECTOR3 vBaseZAxis( viewMatrix._13, viewMatrix._23, viewMatrix._33);
		D3DXVECTOR3 vCrs;

		D3DXVec3Cross(&vCrs,&D3DXVECTOR3(0, 0, 1),&vBaseXAxis);
		D3DXVECTOR3 vMoveVec = ( vBaseXAxis*(pos.x*-1.1f) + vCrs*(pos.y*1.1f));
		m_kCamera.MoveAtPoint(vMoveVec);
	}

	old_pos = point;

	__super::OnMouseMove(nFlags, point);
}

BOOL CRView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_kCamera.Zoom(-zDelta*m_fZoomStep);
	RenderScene();

	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CRView::OnViewStart()
{
	if ( m_pEffect )	m_pEffect->Start();
}

void CRView::OnViewStop()
{
	if ( m_pEffect )	m_pEffect->Stop();
}

void CRView::OnViewPause()
{
	if ( m_pEffect )	m_pEffect->Pause();
}

void CRView::OnViewBgcolor()
{
	CColorDialog Dlg( 0, CC_FULLOPEN);
	if ( Dlg.DoModal() == IDOK )
	{
		COLORREF color = Dlg.GetColor();
		DWORD r = color & 0xff;
		DWORD g = (color >> 8) & 0xff;
		DWORD b = (color >> 16) & 0xff;
		m_dwBGColor = D3DCOLOR_XRGB( r, g, b);
	}
}

void CRView::EffectFileOpen(const char* szName)
{
	ClearEffect();
	InitEffect();

	m_pEmitter = REmitterFromXML::LoadEffectFile(szName, m_pEffect);

	CPage1 *pPage = &g_ControlView->m_Page1;
	if( pPage->GetSafeHwnd() != NULL)
	{
		for( REffect::REMITTERMAP::iterator itor = m_pEffect->m_EmitterMap.begin();
			itor != m_pEffect->m_EmitterMap.end(); itor++)
		{
			pPage->AddListBox((*itor).first.c_str());
		}
		for( REffect::RACTORMAP::iterator itor = m_pEffect->m_MeshMap.begin();
			itor != m_pEffect->m_MeshMap.end(); itor++)
		{
			pPage->AddListBox((*itor).first.c_str());
		}
		//시작하면서 메쉬를 지정해줄려구 했는데 메쉬가 없으니 에러가 나네.
		//리스트에서 선택하면 지정되니 빼자.
		//REffect::RACTORMAP::iterator itor = m_pEffect->m_MeshMap.begin();
		//m_pEffectMesh = (*itor).second;
	}

	//ModifyParameter();	
}

void CRView::OnEffectFileOpen()
{
	CFileDialog Dlg (TRUE, "Effect xml", "*.Effect.xml",
	OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, "Effect XML Files (*.Effect.xml)|*.Effect.xml|All Files (*.*)|*.*||", this);

	if ( Dlg.DoModal() == IDOK )
	{
		EffectFileOpen(Dlg.GetFileName());

		CPage1 *pPage1 = &g_ControlView->m_Page1;
		if ( pPage1->GetSafeHwnd())
		{
			//pPage1->SetEffectName(Dlg.GetFileName());
			pPage1->SetEffectName(m_pEffect->GetName().c_str());
			pPage1->UpdateData(FALSE);
		}
	}
}

void CRView::SaveEffectXML(const char* szName)
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	m_pSceneManager->SaveXML(m_pEffect,xml.Doc());

	xml.SaveFile(szName);
}

bool CRView::SaveEffectFile(const char* szFileName)
{
	MXml kXml;

	MXmlDeclaration *pDecl = kXml.CreateDeclaration("1.0","UTF-8","");
	kXml.Doc()->LinkEndChild(pDecl);

	MXmlElement *ppElement = new MXmlElement("REFFECT_SOURCE");
	_SetAttribute(ppElement,"name", m_pEffect->GetName());// szFileName);//pEffect->m_szName );
	_SetContents(ppElement, "");
	kXml.Doc()->LinkEndChild(ppElement);

	for( REffect::REMITTERMAP::iterator itor = m_pEffect->m_EmitterMap.begin();
		itor != m_pEffect->m_EmitterMap.end(); itor++)
	{
		REmitter *pEmitter = (*itor).second;

		MXmlElement *pElement = new MXmlElement("REMITTER");

		string str = string(pEmitter->GetName()) + ".Emitter.xml";
		_SetContents(pElement, str.c_str());

		ppElement->LinkEndChild(pElement);
		_SetContents(ppElement,"");

		//  [8/29/2006 madduck]
		REmitterFromXML::SaveFile(pEmitter, str.c_str());
	}
	for( REffect::RACTORMAP::iterator itor = m_pEffect->m_MeshMap.begin();
		itor != m_pEffect->m_MeshMap.end(); itor++)
	{
		RActor* pMeshActor = (*itor).second;

		MXmlElement *pElement = new MXmlElement("RMESHEFFECT");
		_SetAttribute(pElement, "Loop", 
			pMeshActor->m_pMesh->m_AniMgr.GetAnimation(0)->m_AniLoopType ? false : true);

		_SetAttribute(pElement, "scale", pMeshActor->GetScale().x);

		string str = (*itor).first;
		_SetContents(pElement, str.c_str());

		ppElement->LinkEndChild(pElement);
		_SetContents(ppElement,"");
	}

	//RSceneNodeList NodeList = m_pEffect->GetChildren();
	//for (RSceneNodeList::iterator it = NodeList.begin(); it != NodeList.end(); ++it )
	//{
	//	REmitter *pEmitter = (REmitter*)(*it);

	//	//if(strlen(pEmitter->GetName()) == 0) continue;
	//	if(!MIsExactlyClass(REmitter,*it)) continue;

	//	MXmlElement *pElement = new MXmlElement("REMITTER");

	//	string str = string(pEmitter->GetName()) + ".Emitter.xml";
	//	_SetContents(pElement, str.c_str());

	//	ppElement->LinkEndChild(pElement);
	//	_SetContents(ppElement,"");

	//	//  [8/29/2006 madduck]
	//	REmitterFromXML::SaveFile(pEmitter, str.c_str());

	//}

	//int i = 0;
	//for(std::list<RActor*>::iterator it = m_pEffect->m_MeshList.begin();
	//	it != m_pEffect->m_MeshList.end(); it++, i++)
	//{
	//	RActor* pMeshActor = (*it);

	//	MXmlElement *pElement = new MXmlElement("RMESHEFFECT");
	//	_SetAttribute(pElement, "Loop", 
	//		pMeshActor->m_pMesh->m_AniMgr.GetAnimation(0)->m_AniLoopType ? false : true);

	//	_SetAttribute(pElement, "scale", pMeshActor->GetScale().x);

	//	string str = m_pEffect->m_MeshNameList[i];
	//	_SetContents(pElement, str.c_str());
	//
	//	ppElement->LinkEndChild(pElement);
	//	_SetContents(ppElement,"");
	//}

	kXml.SaveFile(szFileName);

	return true;
}

void CRView::OnEffectFileSaveAs()
{
	CFileDialog Dlg (FALSE, "Effect xml", "*.Effect.xml",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, "Effect XML Files (*.Effect.xml)|*.Effect.xml|All Files (*.*)|*.*||", this);

	if ( Dlg.DoModal() == IDOK && (m_pEmitter || m_pEffectMesh ) )
	{
		//m_pSceneManager->SaveToXML(Dlg.GetFileName());

		//SaveEffectXML(Dlg.GetFileName());

		SaveEffectFile(Dlg.GetFileName());
	}
}

void CRView::OnEffectFileSave()
{
	if( m_pEmitter || m_pEffectMesh )
		SaveEffectFile(m_pEffect->GetFileName().c_str());
}

void CRView::OnEmitterFileOpen()
{
	CFileDialog Dlg (TRUE, "Emitter xml", "*.Emitter.xml",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
		"Emitter XML Files (*.Emitter.xml)|*.Emitter.xml|MeshEffect File (test.elu) |*.elu|All Files (*.*)|*.*||", this);

	if ( Dlg.DoModal() == IDOK )
	{
		//const char* filename = Dlg.GetFileName();
		//int len = strlen(filename);

		CString strfilename = Dlg.GetFileName();
		int len = strfilename.GetLength();
		const char* filename = strfilename;

		if(strncmp(&filename[len-4],".xml",4)==0) {

			EmitterFileOpen(filename);
		}
		else if(strncmp(&filename[len-4],".elu",4)==0) {

			MeshEffectFileOpen(Dlg.GetPathName(), Dlg.GetFileName());
		}
		else {
			::MessageBox(NULL,"알수없는 확장자를 가진 파일입니다~","info",MB_OK);
			return;
		}
	}
}

void CRView::EmitterFileOpen(const char* filename)
{
	m_pEmitter = REmitterFromXML::LoadFile(filename);

	m_pEffect->AddChild(m_pEmitter);

	//m_pEffect->m_EmitterList.push_back(m_pEmitter);
	//m_pEffect->m_EmitterNameList.push_back(m_pEmitter->GetName());
	//m_pEffect->m_EmitterMap.insert(REffect::REMITTERMAP::value_type(m_pEmitter->GetName(), m_pEmitter));
	m_pEffect->InsertEmitter(m_pEmitter->GetName(), m_pEmitter);

	m_pEmitter->Start();

	CPage1* pPage = &g_ControlView->m_Page1;
	if( pPage->GetSafeHwnd() != NULL)
	{
		pPage->AddListBox(m_pEmitter->GetName().c_str());

		int size = pPage->m_EmitterList.GetCount();

		//pPage->m_EmitterList.SetCurSel(size-1);	
	}

	ModifyParameter();
}

void CRView::OnEmitterFileSave()
{
	CFileDialog Dlg (FALSE, "Emitter xml", "*.Emitter.xml",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, "Emitter XML Files (*.Emitter.xml)|*.Emitter.xml|All Files (*.*)|*.*||", this);

	if ( Dlg.DoModal() == IDOK && m_pEmitter )
	{
		CString str = Dlg.GetFileTitle();

		char name[128] = {0,};
		strncpy(name , str.GetString(), strlen(str.GetString()) - 8);
		m_pEmitter->SetName(name);

		if(REmitterFromXML::SaveFile(m_pEmitter,Dlg.GetFileName()))
		{
			CPage1 *pPage = &g_ControlView->m_Page1;
			if( pPage->GetSafeHwnd() != NULL)
			{
				int sel = pPage->m_EmitterList.GetCurSel();
				pPage->m_EmitterList.DeleteString(sel);
				pPage->m_EmitterList.InsertString(sel, m_pEmitter->GetName().c_str());
			}
		}
	}
}

void CRView::LoadEffectFile(const char* szFileName)
{
	if( m_pEffect)
	{
		m_pEffect->Destroy();

		CPage1 *pPage = &g_ControlView->m_Page1;
		if ( pPage->GetSafeHwnd() != NULL )
		{
			pPage->InitValue();
		}

		m_pEmitter = NULL;
	}


	MXml kXml;
	string name = config.GetEffectPath(szFileName);

	if ( !kXml.LoadFile(name.c_str(), NULL))
		return ;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("ROOT");
	if ( pRootElement == NULL )
		return ;

	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement ; pChildElement = pChildElement->FirstChildElement())
	{
		if (!stricmp(pChildElement->Value(), "REFFECT") )
		{
			m_pEffect->LoadXml(*pChildElement->FirstChildElement("SCENE"));
			break;
		}
	}

	m_pEmitter = m_pEffect->GetEmitter(0);

	assert(m_pEmitter);
	if( !m_pEmitter)	return;

	CPage1 *pPage = &g_ControlView->m_Page1;
	if( pPage->GetSafeHwnd() != NULL)
	{
		RSceneNodeList NodeList = m_pEffect->GetChildren();
		for (RSceneNodeList::iterator it = NodeList.begin(); it != NodeList.end(); ++it )
		{
			REmitter *pEmitter = (REmitter*)(*it);
			pPage->AddListBox(pEmitter->GetName().c_str());
		}
	}
}

//다이얼로그에 Emitter 값 세팅.
void CRView::ModifyParameter()
{
	if ( m_pEmitter == NULL )
		return;

	CPage1 *pPage1 = &g_ControlView->m_Page1;
	CPageEmitter *pPage5 = &g_ControlView->m_PageEmitter;
	CPage2 *pPage2 = &g_ControlView->m_Page2;
	CPage3 *pPage3 = &g_ControlView->m_Page3;

	if ( pPage1->GetSafeHwnd())
	{
		pPage1->SetEffectName(m_pEffect->GetName().c_str());
		if(m_pEffectMesh)
		{
			AnimationLoopType boolloop = m_pEffectMesh->m_pMesh->m_AniMgr.GetAnimation(0)->GetAnimationLoopType();
			pPage1->m_bMeshLooping	= !boolloop;// ? RAniLoopType_Loop : RAniLoopType_HoldLastFrame;
			pPage1->m_fScale		= m_pEffectMesh->GetScale().x;
		}
		pPage1->UpdateData(FALSE);
		pPage1->OnEnChangeEdites();
	}
	if ( pPage5->GetSafeHwnd())
	{
		pPage5->m_uMaxCount = m_pEmitter->GetMaxParticle();
		pPage5->m_uAmount	= m_pEmitter->GetAmount();
		pPage5->m_fLife		= m_pEmitter->GetEmitLife();
		pPage5->m_bLooping	= ( m_pEmitter->IsLooping() ? TRUE : FALSE );
		pPage5->m_bSorting	= ( m_pEmitter->IsSorting() ? TRUE : FALSE );
		pPage5->m_bRotation = ( m_pEmitter->IsRotate() ? TRUE : FALSE );
		pPage5->m_fMinRadian= m_pEmitter->GetRotationRadian().m_Min;
		pPage5->m_fMaxRadian= m_pEmitter->GetRotationRadian().m_Max;
		pPage5->m_bColorRand= ( m_pEmitter->IsRandomColor() ? TRUE : FALSE );
		pPage5->m_bVelRand	= ( m_pEmitter->IsRandomVel() ? TRUE : FALSE );
		pPage5->m_bGather	= ( m_pEmitter->IsGather() ? TRUE : FALSE );
		pPage5->m_bExplosion= ( m_pEmitter->IsExplosion() ? TRUE : FALSE );
		pPage5->m_fAngleD	= m_pEmitter->GetAngle();

		//
		pPage5->m_fDirX		= m_pEmitter->GetRange()->x;
		pPage5->m_fDirY		= m_pEmitter->GetRange()->y;
		pPage5->m_fDirZ		= m_pEmitter->GetRange()->z;

		// Billboard radio button 선택
		pPage5->m_ctlRadioXYZ.SetCheck(0);
		pPage5->m_ctlRadioXY.SetCheck(0);
		pPage5->m_ctlRadioPlaneXY.SetCheck(0);
		pPage5->m_ctlRadioSpread.SetCheck(0);

		switch ( m_pEmitter->GetBillboardType())
		{
		case BILLBOARD_XYZ:
			pPage5->m_ctlRadioXYZ.SetCheck(1);
		break;
		case BILLBOARD_XZ:
			pPage5->m_ctlRadioXY.SetCheck(1);
		break;
		case PLANE_XY:
			pPage5->m_ctlRadioPlaneXY.SetCheck(1);
		break;
		case LINE_SPREAD:
			pPage5->m_ctlRadioSpread.SetCheck(1);
		break;
		}

		pPage5->m_ctlType.SetCurSel((int)m_pEmitter->GetEmitterType());

		pPage5->UpdateData(FALSE);
		pPage5->GetVariables(m_pEmitter->m_vecEmitterVar);
		pPage5->OnEnChangeEdites();
	}
	if(pPage3->GetSafeHwnd())
	{
		// 코드가 꼬였넹...
		//pPage3->m_fMaxLife	= m_pEmitter->GetPMaxLife();
		//pPage3->m_fMinLife	= m_pEmitter->GetPMinLife();

		pPage3->m_fMaxLife	= m_pEmitter->GetParticleLife().m_Max;//GetPMaxLife();
		pPage3->m_fMinLife	= m_pEmitter->GetParticleLife().m_Min;//GetPMinLife();

		pPage3->UpdateData(FALSE);
		pPage3->GetVariables(m_pEmitter->m_vecParticleVar);
		pPage3->UpdateGradation();
		pPage3->OnEnChangeEdits();
	}
	if ( pPage2->GetSafeHwnd())
	{
		pPage2->SetSrcBlendCtrl( m_pEmitter->GetSrcBlend());
		pPage2->SetDstBlendCtrl( m_pEmitter->GetDstBlend());
		pPage2->m_fAlphaRef = m_pEmitter->GetAlphaRef();
	}
	else
	{
		pPage2->m_eSrcBlend = m_pEmitter->GetSrcBlend();
		pPage2->m_eDstBlend = m_pEmitter->GetDstBlend();
	}

	pPage2->CheckRadioButton();
	pPage2->m_strTextureName = m_pEmitter->GetTextureName();
	pPage2->m_fTexX		= m_pEmitter->GetTexUV()->x;
	pPage2->m_fTexY		= m_pEmitter->GetTexUV()->y;
	pPage2->m_fTexU		= m_pEmitter->GetTexUV()->z;
	pPage2->m_fTexV		= m_pEmitter->GetTexUV()->w;

	//m_pEmitter->SetTexturePath( pPage2->m_strPathname.GetString());
	config.SetTexturePath(pPage2->m_strPathname.GetString());
	m_pEmitter->RestoreTexture();

	return;
}

void CRView::DelMeshEffect(LPARAM lParam)
{
	CPage1 *pPage = &g_ControlView->m_Page1;
	if( pPage->GetSafeHwnd() != NULL)
	{
		CString str;
		pPage->m_EmitterList.GetText(lParam, str);
		pPage->m_EmitterList.DeleteString(lParam);

		if(m_pEffect->DeleteMesh(str))
			m_pEffectMesh = NULL;
	}
}

void CRView::CreateEmitter()
{
	CPageEmitter* pPageEmitter = &g_ControlView->m_PageEmitter;
	int sel = pPageEmitter->m_ctlType.GetCurSel();

	m_pEmitter = m_pEffect->CreateEmitter((EMITTER_TYPE)sel);
	ASSERT(m_pEmitter);

	//ModifyParameter();

	CPage1 *pPage = &g_ControlView->m_Page1;
	if( pPage->GetSafeHwnd() != NULL)
	{
		//int size = pPage->m_EmitterList.GetCount();

		CString cstr;
		cstr.Format("_%d", timeGetTime());

		string sz;
		sz = m_pEffect->GetName() + "_" + m_pEmitter->GetName() + cstr.GetString();
		m_pEmitter->SetName(sz);

		pPage->AddListBox(sz.c_str());

		pPage->m_EmitterList.SetCurSel(0);
	}

	m_pEffect->InsertEmitter(m_pEmitter->GetName(), m_pEmitter);
}

void CRView::ModifyParticle(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case DUCK_MSG_CREATEEMITTER:
		{
			CreateEmitter();
		}
		return;
	case DUCK_MSG_SETEFFECTNAME:
		{
			if( m_pEffect )
			{
				CPage1* pPage1 = &g_ControlView->m_Page1;
				if(pPage1->GetSafeHwnd() != NULL)
					m_pEffect->SetName(string(pPage1->m_strEffectName));
			}
		}
		return;
	}

	//if ( wParam == 1 )
	//{
	//	CPageEmitter*pPage5 = &g_ControlView->m_PageEmitter;
	//	int sel = pPage5->m_ctlType.GetCurSel();

	//	m_pEmitter = m_pEffect->CreateEmitter(sel);
	//	ASSERT(m_pEmitter);

	//	ModifyParameter();

	//	CPage1 *pPage = &g_ControlView->m_Page1;
	//	if( pPage->GetSafeHwnd() != NULL)
	//	{
	//		int size = pPage->m_EmitterList.GetCount();

	//		string sz = m_pEmitter->GetName();
	//		CString cstr;
	//		cstr.Format("%s_%s_%d",m_pEffect->GetEffectName().c_str(), sz.c_str(), timeGetTime());
	//		m_pEmitter->SetName(cstr);

	//		pPage->AddListBox(cstr);

	//		pPage->m_EmitterList.SetCurSel(0);
	//	}

	//	//m_pEffect->m_EmitterList.push_back(m_pEmitter);
	//	//m_pEffect->m_EmitterNameList.push_back(m_pEmitter->GetName());
	//	//m_pEffect->m_EmitterMap.insert(REffect::REMITTERMAP::value_type(m_pEmitter->GetName(), m_pEmitter));
	//	m_pEffect->InsertEmitter(m_pEmitter->GetName(), m_pEmitter);
	//}
	if ( (wParam == 2 /*|| wParam == 1*/)&& m_pEmitter )
	{
		CPage2 *pPage2 = &g_ControlView->m_Page2;
		if ( pPage2->GetSafeHwnd() != NULL )
		{
			m_pEmitter->SetSrcBlend( pPage2->m_eSrcBlend );
			m_pEmitter->SetDstBlend( pPage2->m_eDstBlend );
			m_pEmitter->SetAlphaRef( pPage2->m_fAlphaRef );

			pPage2->SetSrcBlendCtrl( m_pEmitter->GetSrcBlend());
			pPage2->SetDstBlendCtrl( m_pEmitter->GetDstBlend());

			//ModifyParameter();
			return;
		}
	}
	if ( /*wParam == 1 ||*/ wParam == 3 )	//SendMessageTexture()
	{
		CPage2 *pPage = &g_ControlView->m_Page2;
		if ( pPage->GetSafeHwnd() != NULL )
		{
			//CString str = pPage->m_strPathname + "\\" + pPage->m_strTextureName;
			if( m_pEmitter )
			{
				m_pEmitter->SetTexUV( RVector4( pPage->m_fTexX, pPage->m_fTexY, pPage->m_fTexU, pPage->m_fTexV) );

				//m_pEmitter->SetTexturePath(pPage->m_strPathname.GetString());
				config.SetTexturePath(pPage->m_strPathname.GetString());
				m_pEmitter->SetTextureName(pPage->m_strTextureName.GetString());
				m_pEmitter->RestoreTexture();
			}

			pPage->DrawTextureToPicbox();
		}
		return;
	}
	if( wParam == 4)	//SendMessageSelectMesh()
	{
		CPage1 *pPage1 = &g_ControlView->m_Page1;
		if( pPage1->GetSafeHwnd() != NULL)
		{
			CString str;
			pPage1->m_EmitterList.GetText(lParam, str);
			string str1 = str;
			REffect::RACTORMAP::iterator it = m_pEffect->m_MeshMap.find(str1);
			if( it != m_pEffect->m_MeshMap.end())
			{
				m_pEffectMesh = (*it).second;
				m_pEmitter = NULL;
			}
		}

		//ModifyParameter();

		return;
	}
	if( wParam == 5)	//SendMessageSelectEmitter()
	{
		CPage1* pPage1 = &g_ControlView->m_Page1;
		CString str;
		pPage1->m_EmitterList.GetText(lParam, str);
		string str2 = str;

		string str1 = MGetPureFileName(str2);

		m_pEmitter = m_pEffect->GetEmitter(str);

		string Emittername = (*m_pEmitter).GetName();// + ".Emitter";
		assert( str1 == Emittername.c_str() );

		ModifyParameter();

		return;
	}
	if( wParam == 6)	//SendMessageDelEmitter()
	{
		assert(m_pEmitter);

		string str = m_pEmitter->GetName();// + ".Emitter.xml";
		bool btrue = m_pEffect->DelEmitter(str); // RemoveChild
		assert(btrue);

		m_pEmitter = NULL;
		SAFE_DELETE(m_pEmitter);

		CPage1 *pPage = &g_ControlView->m_Page1;
		if( pPage->GetSafeHwnd() != NULL)
		{
			pPage->m_EmitterList.DeleteString(lParam);

			if( lParam == 0) return;

			pPage->m_EmitterList.SetCurSel(lParam-1);

			if(!pPage->IsMeshEffect(lParam-1))
				ModifyParticle(5, lParam-1);
		}
	}
	if( wParam == 7)	//SendMessageLoadEmitter()
	{
		InitEffect();

		//이미터와 메쉬 둘다추가 할수 있다
		OnEmitterFileOpen();
		return;
	}
	if( wParam == 8)	//SendMessageSaveEmitter()
	{
		OnEmitterFileSave();
	}
	if( wParam == 9)
	{
		CPage2 *pPage = &g_ControlView->m_Page2;
		if( pPage->GetSafeHwnd() != NULL)
		{
			RActorNode* pTargetNode = m_pActor.GetActorNode(pPage->m_strDumy);
			assert(pTargetNode);
			m_pEffect->SetParent(pTargetNode);
		}
		return;
	}
	if( wParam == 10)	//SendMessageMeshModify()
	{
		CPage1 *pPage1 = &g_ControlView->m_Page1;
		if( pPage1->GetSafeHwnd() != NULL && m_pEffectMesh != NULL)
		{
			m_pEffectMesh->m_pMesh->m_AniMgr.GetAnimation(0)->m_AniLoopType
				= pPage1->m_bMeshLooping ? RAniLoopType_Loop : RAniLoopType_HoldLastFrame;
			m_pEffectMesh->SetScale( RVector(1.0f, 1.0f, 1.0f) * pPage1->m_fScale);
		}
		return;
	}
	if( wParam == 26)	//SendMessageDelMesh()
	{
		DelMeshEffect(lParam);
		return;
	}
	if( wParam == 27)	//SendMessageEmitterModify()
	{
		CPageEmitter*pPage5 = &g_ControlView->m_PageEmitter;
		if(pPage5->GetSafeHwnd() != NULL && m_pEmitter)
		{
			m_pEmitter->Stop();

			m_pEmitter->SetMaxParticle( pPage5->m_uMaxCount );
			m_pEmitter->SetEmitLife( pPage5->m_fLife );
			m_pEmitter->SetLooping( (pPage5->m_bLooping == TRUE));
			m_pEmitter->SetSorting( (pPage5->m_bSorting == TRUE));
			m_pEmitter->SetRotate((pPage5->m_bRotation == TRUE));
			m_pEmitter->SetRotationRadian(CMinMax<float>(pPage5->m_fMinRadian, pPage5->m_fMaxRadian));
			m_pEmitter->SetRandomColor((pPage5->m_bColorRand == TRUE));
			m_pEmitter->SetRandomVel((pPage5->m_bVelRand == TRUE));
			m_pEmitter->SetGather(pPage5->m_bGather == TRUE);
			m_pEmitter->SetExplosion( pPage5->m_bExplosion == TRUE);

			if ( pPage5->m_ctlRadioXYZ.GetCheck())
			{
				m_pEmitter->SetBillboardType( BILLBOARD_XYZ );
			}
			else if ( pPage5->m_ctlRadioXY.GetCheck())
			{
				m_pEmitter->SetBillboardType( BILLBOARD_XZ );
			}
			else if ( pPage5->m_ctlRadioPlaneXY.GetCheck())
			{
				m_pEmitter->SetBillboardType( PLANE_XY );
			}
			else if ( pPage5->m_ctlRadioSpread.GetCheck())
			{
				m_pEmitter->SetBillboardType( LINE_SPREAD );
			}
			
			//m_pEmitter->SetRange( RVector(pPage5->m_fDirX, pPage5->m_fDirY, pPage5->m_fDirZ)); => m_vecEmitterVar에 포함. 다음에 보면 지우기.

			bool bRefresh = m_pEmitter->RefreshEmitterVar();
			ASSERT(bRefresh);

			m_pEmitter->Start();
			return;
		}
	}
	if( wParam == 28)	//SendMessageParticleModyfy()
	{
		CPage3 *pPage3 = &g_ControlView->m_Page3;
		if(pPage3->GetSafeHwnd() != NULL && m_pEmitter)
		{
			//m_pEmitter->SetPMaxLife( pPage3->m_fMaxLife );
			//m_pEmitter->SetPMinLife( pPage3->m_fMinLife );
			m_pEmitter->SetParticleLife(CMinMax<float>(pPage3->m_fMinLife, pPage3->m_fMaxLife));

			bool bRefresh = m_pEmitter->RefreshParticleVar();	//particlevariable sort
			ASSERT(bRefresh);
		}
		return;
	}

	////SetFocus();

	//if ( m_pEmitter == NULL )
	//	return;

	////-----------------------------------------------------------------------
	////다이얼로그의 값들을 저장(m_pEmitter에 저장)

	//CPage1 *pPage1 = &g_ControlView->m_Page1;
	//if(pPage1->GetSafeHwnd() != NULL)
	//{
	//	m_pEffect->SetEffectName(pPage1->m_strEffectName.GetString());
	//}
}

void CRView::OnMeshEffectFileOpen()
{
	CFileDialog dlg( TRUE,_T("elu"),_T("*.elu")
		,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR
		,_T("Model File (test.elu) |*.elu|"));

	if( dlg.DoModal()==IDOK ) {
		
		MeshEffectFileOpen(dlg.GetPathName(), dlg.GetFileName());
	}
}

RActor* CRView::MeshEffectFileOpen(const char* pathname, const char* filename)
{
	RActor* pActor = MeshFileOpen(pathname);

	if( pActor == NULL) return NULL;

	m_pEffectMesh = pActor;
	m_pEffect->AddChild(pActor);
	m_pEffect->InsertMesh(string(filename), pActor);
	//m_pEffect->m_MeshMap.insert(REffect::RACTORMAP::value_type(str, pActor));
	//m_pEffect->m_MeshList.push_back(pMeshActor);
	//m_pEffect->m_MeshNameList.push_back(str);

	CPage1 *pPage = &g_ControlView->m_Page1;
	if( pPage->GetSafeHwnd() != NULL)
		pPage->AddListBox(filename);

	return pActor;
}

RActor* CRView::MeshFileOpen(const char* FileName)
{
	//RMesh* pMesh = RMeshMgr::GetInstance()->Add(FileName);
	//if(!pMesh) return NULL;

	RActor* pMeshActor = new RActor;
	bool bCreated = pMeshActor->Create(FileName);
	if(!bCreated) return NULL;

	pMeshActor->SetPosition(RVector(0,0,0));

	string anifilename(FileName);
	anifilename += ".ani";

	//Animation
	//char filetitle[256] = {0,};
	//strncpy(filetitle, FileName, strlen(FileName));
	//strcat(filetitle, ".ani");

	RMesh* pMesh = pMeshActor->m_pMesh;
	RAnimation* pAnimation = pMesh->m_AniMgr.Add("idle", (char*)anifilename.c_str(), 0);
	//pMeshActor->SetAnimation( pMesh->m_AniMgr.GetAnimation(0) ,RAT_NONE );
	bool ctrue = pMeshActor->SetAnimation(pAnimation);

	return pMeshActor;
}

void CRView::OnCharFileOpen()
{
	CFileDialog dlg( TRUE,_T("elu"),_T("*.elu")
		,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR
		,_T("Model File (test.elu) |*.elu|Animation File (test.ani)|*.ani|"));

	if( dlg.DoModal()==IDOK ) {

		ActorFileOpen(dlg.GetPathName());
	}
}

void CRView::ActorFileOpen(const char* filename)
{
	int len = strlen(filename);

	if(strncmp(&filename[len-4],".ani",4)==0) {

		LoadAniFile(filename);
	}
	else if(strncmp(&filename[len-4],".elu",4)==0) {

		LoadFile(filename);
	}
	/*
	else if(strncmp(&filename[len-4],".xml",4)==0) {

		LoadXmlFile(filename);

	}
	*/
	else if( (strncmp(&filename[len-4],".bmp",4)==0 ) || 
		(strncmp(&filename[len-4],".png",4)==0 ) || 
		(strncmp(&filename[len-4],".tga",4)==0 ) ||
		(strncmp(&filename[len-4],".jpg",4)==0 ) ||
		(strncmp(&filename[len-4],".dds",4)==0 ) ) {
			{
				return;
			}

		}
	else {
		::MessageBox(NULL,"알수없는 확장자를 가진 파일입니다~","info",MB_OK);
		return;
	}
}

void CRView::LoadFile(const char* FileName)
{
	m_pActor.Stop();
	//m_pSceneManager->GetMeshManager()->Del(m_pActor.m_pMesh);
	m_pActor.Destroy();
	RMeshMgr::GetInstance()->Destroy();
	m_pSceneManager->RemoveChild(&m_pActor);

	//RMesh* pMesh = RMeshMgr::GetInstance()->Add(FileName);
	//if(!pMesh) return;

	//m_pActor = new RActor;
	if(!m_pActor.Create(FileName)) return;

	m_pActor.SetAnimation("idle");
	m_pActor.Play();
	m_pActor.SetPosition(RVector(0,0,0));
	m_pSceneManager->AddChild(&m_pActor);

	CPage2 *pPage = &g_ControlView->m_Page2;
	if( pPage->GetSafeHwnd() != NULL)
	{
		pPage->m_ctlDumy.Clear();

		int nSize = m_pActor.GetActorNodeCount();
		for( int i = 0; i < nSize; ++i)
		{
			string str = m_pActor.GetActorNode(i)->m_pMeshNode->GetName();
			pPage->m_ctlDumy.InsertString(i, str.c_str());
		}
	}
}

/*
void CRView::LoadXmlFile(const char* FileName)
{
	m_pActor.Stop();
	m_pActor.Destroy();
	RMeshMgr::GetInstance()->Destroy();
	m_pSceneManager->RemoveChild(&m_pActor);

	int nID = RMeshMgr::GetInstance()->AddXml((char*)FileName);
	if( nID == -1 ) return;

	RMesh* pMesh = RMeshMgr::GetInstance()->GetFast(nID);

	m_pActor.Create(pMesh);
	m_pActor.SetAnimation("idle");
	m_pActor.Play();
	m_pActor.SetPosition(RVector(0,0,0));
	m_pSceneManager->AddChild(&m_pActor);

	//	RAnimation* rani = m_pActor.m_pMesh->m_AniMgr
	// 	int dd = rani->m_NodeHashList->GetAniNodeCount();
	// 	int ddd = rani->m_AniTypeHashList->GetAniNodeCount();
	// 
	// 	m_pSceneManager-


	CPage2 *pPage = &g_ControlView->m_Page2;
//	if( pPage->GetSafeHwnd() != NULL)
	{
		int nSize = m_pActor.GetActorNodeCount();
		for( int i = 0; i < nSize; ++i)
		{
			string str = m_pActor.GetActorNode(i)->m_pMeshNode->GetName();
			pPage->m_ctlDumy.InsertString(i, str.c_str());
		}
	}

	// 	RActorNode* pTargetNode = m_pActor.GetActorNode("dummy_1hsword");
	// 	assert(pTargetNode);
	// 	m_pEffect->SetPatent(pTargetNode);
	// 	m_pActor.AddChild(m_pEffect);

}
*/

void CRView::LoadAniFile(const char* FileName)
{
	m_pActor.Stop();
	m_AniMgr.DelAll();
	m_AniMgr.Add("idle", (char*)FileName, m_AniMgr.m_nIDCount);
	m_pActor.Stop();
	m_pActor.SetAnimation( m_AniMgr.GetAnimation(0) ,RAT_NONE );
	m_pActor.Play();
}

void CRView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if( nChar == VK_SPACE)
	{
		ResetCamera();
	}

	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}
