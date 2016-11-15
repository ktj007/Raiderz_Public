#include "StdAfx.h"

#include "C_ControlMediatorHelperRender.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlPreviewSceneManager.h"
#include "C_ControlCursor.h"
#include "C_MintManager.h"

#include "C_XMLEditInfoFile.h"
#include "C_XMLUserConfig.h"

#include "WS_WorkSpaceManager.h"

#include "M_ToolCmdMgr.h"
#include "M_CommandBuffer.h"
#include "M_ToolLogicObjectManager.h"

#include "RFont.h"
#include "RSUtil.h"
#include "RRenderHelper.h"
#include "RRenderHelper2.h"
#include "RMeshUtil.h"
#include "RUtil.h"
#include "RActor.h"
#include "RSpotLightSceneNode.h"
#include "REffectSceneNode.h"
#include "M_ToolStaticMeshSceneNode.h"

#include "V_MFCHelper.h"
#include "VD_DlgProfile.h"
#include "VD_DlgClipPlaneHeight.h"
#include "VL_SceneNodeList.h"
#include "VR_TerrainGrass.h"
#include "VR_EtcCollision.h"

// 편집하기 위해 더해진 카메라 랜더링
#include "VR_EtcCamera.h"

#include <algorithm>

using namespace rs3;
CControlMediatorHelperRender::CControlMediatorHelperRender(void)
: m_bPause_(false)
, m_bShowPanel_(false)
, m_bScreenLog_(false)
, m_bTerrainInfo_(false)
, m_bViewAlphaSorting(false)
, m_bViewLightingOnly(false)
, m_bViewStaticMeshRenderCell(false)
, m_bViewStaticMeshRenderAll(false)
{
	m_pControlMediator_ = NULL;

	m_pDeviceD3D_ = NULL;
	m_pFont_ = NULL;
	m_matTranslation_.MakeIdentity();
	
	m_pDlgProfile_ = NULL;
	m_pDlgClipPlaneHeight_ = NULL;
	m_bShowWorldCenterAxis_ = true;
}

CControlMediatorHelperRender::~CControlMediatorHelperRender(void)
{
	if ( m_pDlgProfile_ )
	{
		m_pDlgProfile_->DestroyWindow();
		delete m_pDlgProfile_;
		m_pDlgProfile_ = NULL;
	}

	if (m_pDlgClipPlaneHeight_)
	{
		m_pDlgClipPlaneHeight_->DestroyWindow();
		delete m_pDlgClipPlaneHeight_;
		m_pDlgClipPlaneHeight_ = NULL;
	}
}

void CControlMediatorHelperRender::InitHelperRender(CControlMediator* _pMediator, int _width, int _height)
{
	m_pControlMediator_ = _pMediator;
	m_pDeviceD3D_ = m_pControlMediator_->GetCurrentDevice();

	//화면에 뿌려줄 font 생성, device 종속적
	m_pFont_ = new RFont();
	// m_pFont_->Initialize(m_pDeviceD3D_); // RSingletonSceneManager에서 한번만 하도록 변경
	if ( m_pFont_->Create(m_pDeviceD3D_, L"굴림체", 12) == false )
		SAFE_DELETE(m_pFont_);

	//init helper
	rs3::RToolSceneManager* pToolSceneManager = m_pControlMediator_->GetToolSceneManager();
	L3DCursor::GetInstance()->Init(m_pControlMediator_, m_pDeviceD3D_, pToolSceneManager);
	CControlPreviewSceneManager::GetInstance()->Init(m_pControlMediator_, m_pDeviceD3D_, pToolSceneManager);

	m_width_ = _width;
	m_height_ = _height;

	//rect init
	const int nGap = 3;
	const int nSize = 300;
	m_infoRenderRect_[0].x = nGap;
	m_infoRenderRect_[0].y = nGap;
	m_infoRenderRect_[0].z = 0;
	m_infoRenderRect_[0].rhw = 1;
	m_infoRenderRect_[0].dwColor = D3DCOLOR_RGBA(0, 0, 0, 128);

	m_infoRenderRect_[1].x = nSize;
	m_infoRenderRect_[1].y = nGap;
	m_infoRenderRect_[1].z = 0;
	m_infoRenderRect_[1].rhw = 1;
	m_infoRenderRect_[1].dwColor = D3DCOLOR_RGBA(0, 0, 0, 128);

	m_infoRenderRect_[2].x = nGap;
	m_infoRenderRect_[2].y = float(_height - nGap);
	m_infoRenderRect_[2].z = 0;
	m_infoRenderRect_[2].rhw = 1;
	m_infoRenderRect_[2].dwColor = D3DCOLOR_RGBA(0, 0, 0, 128);

	m_infoRenderRect_[3].x = nSize;
	m_infoRenderRect_[3].y = float(_height - nGap);
	m_infoRenderRect_[3].z = 0;
	m_infoRenderRect_[3].rhw = 1;
	m_infoRenderRect_[3].dwColor = D3DCOLOR_RGBA(0, 0, 0, 128);

	// 프로파일 뷰 생성
	_ASSERT(m_pDlgProfile_==NULL);
	m_pDlgProfile_ = new CDlgProfile();
	if ( m_pDlgProfile_->Create(IDD_DIALOG_PROFILE, NULL))
	{
		m_pDlgProfile_->ShowWindow(SW_HIDE);
	}

	m_pDlgClipPlaneHeight_ = new CDlgClipPlaneHeight();
	if (m_pDlgClipPlaneHeight_->Create(IDD_CLIP_HEIGHT, NULL))
	{
		m_pDlgClipPlaneHeight_->SetControl(m_pControlMediator_);
		m_pDlgClipPlaneHeight_->Hide();
	}

	int nRenderHelperSize = 0;
	CControlMediatorHelperObjectRenderer** pRenderHelpers = m_pControlMediator_->GetWorkSpaceMgr()->GetHelperObjectRenderers(nRenderHelperSize);
	for (int i = 0; i < nRenderHelperSize; ++i)
	{
		pRenderHelpers[i]->InitDevice(m_pControlMediator_, m_pDeviceD3D_, m_pFont_);
	}
}

void CControlMediatorHelperRender::ResetHelperRender(rs3::RToolSceneManager* _pSceneManager)
{
	L3DCursor::GetInstance()->Release();
	//프리뷰
	CControlPreviewSceneManager::GetInstance()->Release();

	L3DCursor::GetInstance()->Init(m_pControlMediator_, m_pDeviceD3D_, _pSceneManager);
	CControlPreviewSceneManager::GetInstance()->Init(m_pControlMediator_, m_pDeviceD3D_, _pSceneManager);

	int nRenderHelperSize = 0;
	CControlMediatorHelperObjectRenderer** pRenderHelpers = m_pControlMediator_->GetWorkSpaceMgr()->GetHelperObjectRenderers(nRenderHelperSize);
	for (int i = 0; i < nRenderHelperSize; ++i)
	{
		pRenderHelpers[i]->OnResetDevice();
	}
}

void CControlMediatorHelperRender::DestroyHelperRender()
{
	CControlPreviewSceneManager::GetInstance()->Release();
	L3DCursor::GetInstance()->Release();

	// destroy render helpers
	if (m_pControlMediator_)
	{
		int nRenderHelperSize = 0;
		CControlMediatorHelperObjectRenderer** pRenderHelpers = m_pControlMediator_->GetWorkSpaceMgr()->GetHelperObjectRenderers(nRenderHelperSize);
		for (int i = 0; i < nRenderHelperSize; ++i)
		{
			pRenderHelpers[i]->OnDestroyDevice();
		}
	}

	//글꼴 지우기
	if(m_pFont_ != NULL)
	{
		m_pFont_->Destroy();
		delete m_pFont_;
		m_pFont_ = NULL;
	}
}

void CControlMediatorHelperRender::RenderPause()
{
	if(m_pDeviceD3D_ == NULL || m_pFont_ == NULL)
		return;

	m_pDeviceD3D_->Clear(true, true, true, D3DCOLOR_XRGB(35,135,195), 1.0f, 0L);
	m_pDeviceD3D_->BeginScene();

	m_pFont_->BeginFont();
	m_pFont_->DrawText_UNICODE( m_width_/2, m_height_/2, L"PAUSE ........");
	m_pFont_->EndFont();

	m_pDeviceD3D_->EndScene();
	m_pDeviceD3D_->Flip();
}

void CControlMediatorHelperRender::SetVisibleProfile()
{
	if ( m_pDlgProfile_ )
	{
		m_pDlgProfile_->ShowWindow(SW_SHOW);
		m_pDlgProfile_->UpdateWindow();
	}
}

void CControlMediatorHelperRender::ToggleViewSideInfoPanel()
{
	static bool bPanel = false;

	//pause 이면 아무것도 안한다.
	if(m_bPause_)
		return;

	bPanel = !bPanel;
	m_bShowPanel_ = bPanel;

	if(m_bShowPanel_)
	{
		FILE *stream;
		char line[256];
		m_vecStrShortcutText_.clear();

		if( fopen_s( &stream, "hot_key.txt", "r" ) == 0 )
		{
		  while ( fgets( line, 256, stream ) != NULL)
		  {
			 m_vecStrShortcutText_.push_back(line);
		  }
		  fclose( stream );
		}
		else
			m_vecStrShortcutText_.push_back( "hot_key.txt 파일읽기 실패!!" );
	}
}

void CControlMediatorHelperRender::OnUpdateHelperRender()
{
	if ( m_pDeviceD3D_ == NULL )
		return;
	if ( m_pControlMediator_ == NULL )
		return;

	if ( m_pDlgProfile_ )
	{
		m_pDlgProfile_->UpdateProfile(m_pDeviceD3D_);
	}
}


void CControlMediatorHelperRender::OnDrawFreeText( char* _szText, int _iX, int _iY )
{
	if(m_pDeviceD3D_ == NULL || m_pFont_ == NULL)
		return;

	m_pFont_->BeginFont();
	m_pFont_->DrawText_UNICODE( _iX, _iY, MLocale::ConvAnsiToUTF16(_szText).c_str(), -1, 0xffff0000);
	m_pFont_->EndFont();
}


void CControlMediatorHelperRender::OnDrawHelperRender()
{
	if(m_pDeviceD3D_ == NULL || m_pFont_ == NULL)
		return;

	// 렌더링 순서
	// 컬리전/네비메쉬 - 포인트라이트 월드센터 - 선택오브젝트 - 선택 Gizmo
	// - 카메라더미박스 - 디버깅박스 - 커서 - WorkSpace - 사이드패널 - 쿼드팝업

	// collision 은 기본 기능으로
	CRollupEtcCollision* pCollision = m_pControlMediator_->GetViewPtr<CRollupEtcCollision>();
	if(pCollision)
		pCollision->GetCollisionTree().Render(m_pDeviceD3D_);

	// collision and navigation mesh
	CControlMediatorHelperObjectRenderer* pRenderHelper = m_pControlMediator_->GetWorkSpaceMgr()->GetHelperObjectRenderer();
	if (pRenderHelper)
	{
		pRenderHelper->OnRenderNavigationMesh();
	}

	// 포인트라이트
	if (m_vecPointLightRenderlist.empty() == false)
	{
		// render state
		RRenderHelper::SetRenderState();

		for(std::vector< rs3::RSceneNode* >::iterator itr = m_vecPointLightRenderlist.begin();
			itr != m_vecPointLightRenderlist.end(); ++itr )
		{
			RSceneNode* pSceneNode = *itr;
			rs3::RRenderHelper::RenderBox( RMatrix::IDENTITY, pSceneNode->GetAABB(), 0xffffff00 );
		}
		ClearPointLightRenderList();

		RRenderHelper::EndRenderState();
	}

	// render state
	RRenderHelper::SetRenderState();

	// 월드 센터축
	if (m_bShowWorldCenterAxis_)
		RRenderHelper::RenderAxis(RMatrix::IDENTITY, 35.f, 0.45, 1.6, 2.5);

	// 선택 물체
	SCENEOBJECTMAP* pMapSelectedSceneObject =  m_pControlMediator_->GetHelperAction()->GetSelectedSceneObjectPtr();
	ENTITY_OBJECTLIST* pListSelectedEntityObject = m_pControlMediator_->GetHelperAction()->GetSelectedEntityObject();

	size_t tNodeSize = pMapSelectedSceneObject->size();
	bool bEntityEmpty = pListSelectedEntityObject->empty();
	if( tNodeSize != 0 || !bEntityEmpty )
	{
		rs3::RMatrix viewMat(m_pDeviceD3D_->GetTransform(RST_VIEW));
		rs3::RVector vViewPos;

		// SceneNode Object --
		if(tNodeSize != 0)
		{
			bool bCheckRotateAxis = false;

			rs3::RVector* pCenter = m_pControlMediator_->GetHelperAction()->GetSelectSceneNodeCenter();
			_ASSERT(pCenter);
			viewMat.TransformVect(*pCenter, vViewPos);
			m_matTranslation_.SetTranslation(*pCenter);
			float fFactor = fabs(vViewPos.z) * 0.38;
			const WORKBENCH_STATE::STATE eEditState = m_pControlMediator_->GetCurrentWBState();

			for ( SCENEOBJECTMAP::iterator it = pMapSelectedSceneObject->begin(); it != pMapSelectedSceneObject->end(); ++it )
			{
				rs3::RSceneNode *pSceneNode = (RSceneNode*)(it->first);
				if(pSceneNode->GetVisible() == false) 
					continue; //그냥 스킵

				rs3::RRenderHelper::RenderBox( RMatrix::IDENTITY, pSceneNode->GetAABB());

				if ( stricmp(pSceneNode->GetNodeIDString(), RSID_SPOTLIGHTSCENENODE) == 0 )
				{
					RSpotLightSceneNode* pSpotLight = MDynamicCast(RSpotLightSceneNode, pSceneNode);

					RViewFrustum LightFrumstum = pSpotLight->GetLightFrustum();
					rs3::RRenderHelper::RenderPyramid(pSpotLight->GetWorldPosition(), LightFrumstum, 0xFFFFFFFF, true);
				}				

				if( tNodeSize == 1 && eEditState == WORKBENCH_STATE::OBJECT_ROTATE)
				{
					bCheckRotateAxis = true;

					rs3::RMatrix matTransform = pSceneNode->GetLocalTransform();
					matTransform.SetTranslation(*pCenter);
					//선택 축은 모든것 위에
					m_pDeviceD3D_->SetDepthEnable(false);
					RRenderHelper::RenderAxis(matTransform, fFactor, fFactor * 0.003, fFactor * 0.01, fFactor * 0.015);
					m_pDeviceD3D_->SetDepthEnable(true);
				}
			}

// 			// 이동
// 			if(!bCheckRotateAxis)
// 			{
// 				//선택 축은 모든것 위에
// 				m_pDeviceD3D_->SetDepthEnable(false);
// 				RRenderHelper::RenderAxis(m_matTranslation_, fFactor, fFactor * 0.003, fFactor * 0.01, fFactor * 0.015);
// 				m_pDeviceD3D_->SetDepthEnable(true);
// 			}
		}
	}

	// Logic 편집 모드일 경우
	// render selected logic Objects --
	if (pRenderHelper)
	{
		pRenderHelper->OnRenderSelection();
	}

	// 기본 logic object 선택 렌더링
	{
		LOGIC_OBJECTLIST* pListSelectedLogicObject = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();
		for(LOGIC_OBJECTLIST::iterator it_logic = pListSelectedLogicObject->begin(); it_logic != pListSelectedLogicObject->end(); ++it_logic)
		{
			CLogicObject* pLogicObject = *it_logic;
			rs3::RRenderHelper::RenderBox( RMatrix::IDENTITY, pLogicObject->GetAABB() );
		}
	}

	RRenderHelper::SetRenderState();
	m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::LOGIC_RENDER );

	m_pFont_->BeginFont();
	m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::LOGIC_TEXTINFO_RENDER );
	m_pFont_->EndFont();

	m_pDeviceD3D_->SetDepthEnable(true);

	RRenderHelper::EndRenderState();

	// Logic Object Rendering
	if(m_pControlMediator_->m_mapInfo.m_bLoaded)
	{
		if (pRenderHelper)
		{
			pRenderHelper->OnRenderObject();
		}
	}

	// 카메라 편집용 더미박스
	CRollupEtcCamera* pRollupCamera = m_pControlMediator_->GetViewPtr<CRollupEtcCamera>();
	if(pRollupCamera)
		pRollupCamera->UpdateAndRenderAddedCamera();

	// 박스 헬퍼랜더링
	if (m_vecBoxToRender.empty() == false)
	{
		RRenderHelper::SetRenderState();

		for(std::vector< std::pair<rs3::RBoundingBox, DWORD> >::iterator itr = m_vecBoxToRender.begin();
			itr != m_vecBoxToRender.end(); ++itr )
		{
			rs3::RRenderHelper::RenderBox( RMatrix::IDENTITY, itr->first, itr->second );
		}

		ClearRenderBoxes();
		RRenderHelper::EndRenderState();
	}

	// WorkSpace
	m_pControlMediator_->GetWorkSpaceMgr()->RenderingWBHelper();

	// 커서
	L3DCursor::GetInstance()->Render(m_pDeviceD3D_);

	//사이드 패널
	if(m_bShowPanel_)
	{
		RRenderHelper::SetRenderState();

		//set alpha blending
		m_pDeviceD3D_->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		m_pDeviceD3D_->SetTextureStageState( 0, RTSS_ALPHAOP, RTOP_SELECTARG1);
		m_pDeviceD3D_->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_DIFFUSE);

		m_pDeviceD3D_->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG1);
		m_pDeviceD3D_->SetTextureStageState( 0, RTSS_COLORARG1, RTA_DIFFUSE);
		
		//draw blacked transparent board
		m_pDeviceD3D_->SetFvF( CUSTOM_RENDER_INFO_BOX::FVF );
		m_pDeviceD3D_->DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2,(void*)m_infoRenderRect_, sizeof(CUSTOM_RENDER_INFO_BOX));

		//draw font
		m_pFont_->BeginFont();
		m_pFont_->DrawText_UNICODE( 10, 10, L"Side Information Window");

		if(m_pControlMediator_->m_mapInfo.m_bLoaded)
		{
			CToolCmdMgr* pCmdMgr = m_pControlMediator_->GetToolCmdMgr();
			CToolLogicObjectManager* pLogicObjectMgr = m_pControlMediator_->GetToolLogicObjectMgr();

			// for view state
			CDlgSceneNodeList* pDlgSceneNodeList = m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>();
			CRollupTerrainGrass* pRollupTerrainGrass = m_pControlMediator_->GetViewPtr<CRollupTerrainGrass>();

			// 다음 취소
			m_pFont_->DrawText_UNICODE( 10, 25, MLocale::ConvAnsiToUTF16(pCmdMgr->GetNextUndoMsg()).c_str());

			// Ref Mesh Name
			const char* szRefMesh = CXMLUserConfig::GetReference().GetRefMeshName();
			if(szRefMesh != NULL)
				m_pFont_->DrawText_UNICODE( 10, 40, MLocale::ConvAnsiToUTF16(szRefMesh).c_str() + 11 );
			else
				m_pFont_->DrawText_UNICODE( 10, 40, L"설정된 Reference Mesh 가 없습니다." );

			//View Toggle information
			DWORD color ; //0xffff0000 : 빨강 0xff0000ff : 노랑
			if( pDlgSceneNodeList->GetViewState( rs3::RSID_TERRAINSCENENODE ) ) color = 0xffffffff; else color = 0xffff0000;
			m_pFont_->DrawText_UNICODE(10, 55, L"1.Terrain", -1, color);

			if( pDlgSceneNodeList->GetViewState( rs3::RSID_LIGHTSCENENODE) ) color = 0xffffffff; else color = 0xffff0000;
			m_pFont_->DrawText_UNICODE(110, 55, L"2.Light Object", -1, color);

			if( pDlgSceneNodeList->GetViewState( rs3::RSID_ACTOR ) ) color = 0xffffffff; else color = 0xffff0000;
			m_pFont_->DrawText_UNICODE(210, 55, L"3.Elu Object", -1, color);

			if( pDlgSceneNodeList->GetViewState( rs3::RSID_TREE ) ) color = 0xffffffff; else color = 0xffff0000;
			m_pFont_->DrawText_UNICODE(10, 70, L"4.Spt Object", -1, color);

			if( pRollupTerrainGrass->GetViewStateGrass() ) color = 0xffffffff; else color = 0xffff0000;
			m_pFont_->DrawText_UNICODE(110, 70, L"5.Grass", -1, color);

			if( pDlgSceneNodeList->GetViewState( rs3::RSID_WATERPLANE ) ) color = 0xffffffff; else color = 0xffff0000;
			m_pFont_->DrawText_UNICODE(210, 70, L"6.Water", -1, color);

			if(!m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
			{
				if (pRenderHelper)
				{
					pRenderHelper->OnRenderObjectTextInfo();
				}
			}
		}
		else
		{
			m_pFont_->DrawText_UNICODE( 10, 25, L"로딩된 맵이 없습니다.");
		}

		for( unsigned int i=0; i<m_vecStrShortcutText_.size(); i++)
		{
			int nLineStride = m_pFont_->GetHeight() + 2;
			m_pFont_->DrawText_UNICODE( 10, 110 + i*nLineStride, MLocale::ConvAnsiToUTF16(m_vecStrShortcutText_[i].c_str()).c_str() );
		}
		m_pFont_->EndFont();

		RRenderHelper::EndRenderState();
	}
	else
	{
		// 프로파일 정보 출력

		const int drawCallLimit = 3000;

		const RDeviceProfileInfo& info = REngine::GetDevice().GetDeviceProfileInfo();
		int nTotalDrawCall = info.GetTotalDrawCall();
		int nTotalReducedDrawCall = info.GetTotalReducedDrawCall();
		int nTotalDrawPolygon = info.GetTotalDrawPolygon();
		float fFps = REngine::GetDevice().GetFrameRate();

		unsigned int nExcludedByFogCulling = REngine::GetResourceProfiler().GetExcludedByFogCullingCount();
		unsigned int nExcludedByDistanceCulling = REngine::GetResourceProfiler().GetExcludedByDistanceCullingCount();

		//---

		m_pFont_->BeginFont();

		const float textPosX = 5.f;
		const float textPosY = 15.f;

		char str[MAX_PATH];
		sprintf_s( str, MAX_PATH, "DrawCall %d/%d | Reduced DrawCall %d | Polygon %d | FogCull %d | DistanceCull %d |FPS %3.3f",
			nTotalDrawCall, drawCallLimit, nTotalReducedDrawCall, nTotalDrawPolygon, nExcludedByFogCulling, nExcludedByDistanceCulling, fFps );

		m_pFont_->DrawText_UNICODE( textPosX+1, textPosY+1, MLocale::ConvAnsiToUTF16(str).c_str(), -1, 0xA0000000 );

		if( nTotalDrawCall > drawCallLimit )
			m_pFont_->DrawText_UNICODE( textPosX, textPosY, MLocale::ConvAnsiToUTF16(str).c_str(), -1, 0xFFFF0000 );
		else
			m_pFont_->DrawText_UNICODE( textPosX, textPosY, MLocale::ConvAnsiToUTF16(str).c_str() );

		m_pFont_->EndFont();

		//--

		RRenderHelper::SetRenderState();
		bool bFogEnable = REngine::GetDevice().GetFogEnable();
		REngine::GetDevice().SetFogEnable( false );
		m_pFont_->BeginFont();

		const float graphPosX = 5.f;
		const float graphPosY = 5.f;
		const float graphWidth = 300.f;
		const float graphHeight = 3.f;
		
		const DWORD colorBlack	= 0xFF000000;
		const DWORD colorRed	= 0xFFFF0000;
		const DWORD colorGreen	= 0xFF00FF00;
		const DWORD colorBlue	= 0xFF0000FF;

		float graphRate = drawCallLimit / graphWidth;

		float graphEndColorPos = nTotalDrawCall / (float)drawCallLimit;
		if( graphEndColorPos > 1.f )
			graphEndColorPos = 1.f;
		DWORD graphEndColor = (int)(graphEndColorPos * 255.f);
		graphEndColor = D3DCOLOR_ARGB( 255, graphEndColor, 255-graphEndColor, 0 );

		float fDrawCallLength = nTotalDrawCall / graphRate;
		if( fDrawCallLength > graphWidth )
		{
			RRenderHelper::RenderQuadColor( graphPosX, graphPosY, graphWidth, graphHeight, colorGreen, graphEndColor, colorGreen, graphEndColor );
			RRenderHelper::RenderQuadColor( graphPosX+graphWidth, graphPosY, fDrawCallLength-graphWidth, graphHeight, colorRed, colorRed, colorRed, colorRed );
		}
		else
		{
			RRenderHelper::RenderQuadColor( graphPosX, graphPosY, fDrawCallLength, graphHeight, colorGreen, graphEndColor, colorGreen, graphEndColor );
		}

		float fDrawReducedCallLength = nTotalReducedDrawCall / graphRate;
		RRenderHelper::RenderQuadColor( graphPosX, graphPosY+graphHeight, fDrawReducedCallLength, graphHeight, colorBlue, colorBlue, colorBlue, colorBlue );

		m_pFont_->EndFont();
		REngine::GetDevice().SetFogEnable( bFogEnable );
		RRenderHelper::EndRenderState();
	}

	// 텍스트 정보 출력
	if (m_mapDragTextToRender.empty() == false)
	{
		RRenderHelper::SetRenderState();
		bool bFogEnable = REngine::GetDevice().GetFogEnable();
		REngine::GetDevice().SetFogEnable( false );

		m_pFont_->BeginFont();
		for (std::map<std::string, CUSTOM_RENDER_TEXT>::iterator itr = m_mapDragTextToRender.begin(); itr != m_mapDragTextToRender.end(); ++itr)
		{
			CUSTOM_RENDER_TEXT& textInfo = itr->second;
			m_pFont_->DrawText_UNICODE( textInfo.m_x, textInfo.m_y, MLocale::ConvAnsiToUTF16(textInfo.m_strText.c_str()).c_str(), -1, D3DCOLOR_XRGB(255, 255, 255) , 1.2);
		}
		m_pFont_->EndFont();

		REngine::GetDevice().SetFogEnable( bFogEnable );
		RRenderHelper::EndRenderState();

	}


	if ( m_bTerrainInfo_ )
	{
		RToolTerrain* pToolTerrain = (RToolTerrain*) m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
		if (pToolTerrain)
			pToolTerrain->RenderPatchInfo(m_pFont_);
	}

	if( m_bViewAlphaSorting )
	{
		RCameraSceneNode* pCamera = m_pControlMediator_->GetToolCamera();
		SCENE_SET* pCulledList = pCamera->GetViewFrustumCullingMgr()->GetCulledList(RRP_ALPHA);

		SCENE_SET::iterator	iter;
		int k=0;
		int nOrder = 0;
		for( iter = pCulledList->begin() ; iter != pCulledList->end() ; iter++)
		{
			RSceneNode *pNode = (*iter);

			RVector vecPosition = pNode->GetWorldPosition();

			if(pCamera->GetViewFrustum().Test(vecPosition))
			{
				DWORD dwColor = ( nOrder%2==0 ) ? 0xffff00ff : 0xffffff00;
				int textOffset = ( nOrder%2==0 ) ? -5 : 5;
				RMatrix matWorld;
				matWorld.SetTranslationMatrix( vecPosition);

				RRenderHelper::SetRenderState();
				RRenderHelper::RenderSphere(matWorld, 10.f, dwColor);

				vec2 screen_pos;
				GetScreenFromWorld(&screen_pos,&vecPosition,
					&REngine::GetDevice().GetViewport(), 
					&RMatrix::IDENTITY, 
					&pCamera->GetViewMatrix(), 
					&pCamera->GetProjectionMatrix());

				char szBuffer[256];
				sprintf(szBuffer,"%d , %s", nOrder, pNode->GetNodeName().c_str());
				m_pFont_->BeginFont();
				m_pFont_->DrawText_UNICODE((int)screen_pos.x, (int)screen_pos.y + textOffset, MLocale::ConvAnsiToUTF16(szBuffer).c_str(), -1, dwColor);
				m_pFont_->EndFont();
			}
		}

	}

	//민트 매니저 렌더링
	CMintManager::GetReference().RenderMint();

	// Instant Render Scenenodes 초기화
	//ClearInstantRenderSceneNodes();
}

void CControlMediatorHelperRender::ToggleMapDetailInfo()
{
	m_bTerrainInfo_ = (m_bTerrainInfo_ ? false : true);
}

bool CControlMediatorHelperRender::GetMapDetailInfoState()
{
	return m_bTerrainInfo_;
}

void CControlMediatorHelperRender::ShowToggleClipPlane()
{
	if ( m_pDlgClipPlaneHeight_ )
	{
		m_pDlgClipPlaneHeight_->ShowToggle();
	}
}

bool CControlMediatorHelperRender::IsClipPlane()
{
	if (m_pDlgClipPlaneHeight_)
	{
		return m_pDlgClipPlaneHeight_->IsShow();
	}

	return false;
}

void CControlMediatorHelperRender::RegisterToInstantRenderSceneNode( rs3::RSceneNode* _pSceneNode )
{
	_pSceneNode->RemoveFromParent();
	rs3::REngine::GetSceneManager().AddSceneNode(_pSceneNode);

	m_vecInstantRenderSceneNodes.push_back(_pSceneNode);
}

void CControlMediatorHelperRender::ClearInstantRenderSceneNodes()
{
	for(std::vector< rs3::RSceneNode* >::iterator itr = m_vecInstantRenderSceneNodes.begin();
		itr != m_vecInstantRenderSceneNodes.end(); ++itr)
	{
		(*itr)->RemoveFromParent();
	}

	m_vecInstantRenderSceneNodes.clear();
}

void CControlMediatorHelperRender::ToggleViewStaticMeshRenderAll()
{
	m_bViewStaticMeshRenderAll = !m_bViewStaticMeshRenderAll;
	rs3::RToolStaticMeshSceneNode* pStaticMesh = static_cast<rs3::RToolStaticMeshSceneNode*>(rs3::REngine::GetSceneManager().GetCurrentStaticMeshSceneNode());
	if( pStaticMesh )
	{
		pStaticMesh->SetRenderAll( m_bViewStaticMeshRenderAll );
	}
}
