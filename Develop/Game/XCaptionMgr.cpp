#include "stdafx.h"
#include "XCaptionMgr.h"
#include "XCharacter.h"

#include "RCameraSceneNode.h"
#include "XSystem.h"


XCaptionMgr::~XCaptionMgr()
{
	for (CAP_NODE_MAP::iterator itr = m_mapCaptionsWithNode.begin(); itr != m_mapCaptionsWithNode.end(); )
	{
		XCaption* pCaption = itr->second;

		if (pCaption->m_bSelfDelete)
		{
			delete pCaption;
			itr = m_mapCaptionsWithNode.erase(itr);
			continue;
		}

		++itr;
	}
}


bool XCaptionMgr::RemoveCaption( MID nID )
{
	CAP_NODE_MAP::iterator itr = m_mapCaptionsWithNode.find(nID);
	if (itr != m_mapCaptionsWithNode.end() )
	{
		XCaption* pCaption = itr->second;
		if (pCaption->m_bSelfDelete == false)
		{
			delete pCaption;
		}

		m_mapCaptionsWithNode.erase(itr);
		return true;
	}

	return false;
}

void XCaptionMgr::Render()
{
	TIMEPROFILE_THIS_FUNCTION;
	PFC_THISFUNC;

	// device
	rs3::RDevice* device = REngine::GetDevicePtr();
	if (device == NULL)
		return;

	// camera
	RCameraSceneNode* pCam = REngine::GetSceneManagerPtr()->GetPrimaryCamera();
	if( pCam == NULL )
		return;

	// matrix for billboard
	m_matBuf.m_matView = device->GetTransform( RST_VIEW);
	m_matBuf.m_matProject = device->GetTransform( RST_PROJECTION);

	bool bValid = m_matBuf.m_matView.GetInverse(&m_matBuf.m_matViewInverse);
	if (bValid == false)
		return;

	RVector vCamPos(pCam->GetPosition());

	m_DamageCaptionDraw.Init();

	// sorting
	m_mapCaptionsWithDistance.clear();
	for (CAP_NODE_MAP::iterator itr = m_mapCaptionsWithNode.begin(); itr != m_mapCaptionsWithNode.end(); )
	{
		XCaption* pCaption = itr->second;
		rs3::RSceneNode* pSceneNode = pCaption->GetSceneNode();

		bool bNotDelete = pCaption->Update(vCamPos, m_matBuf.m_matViewInverse);
		if (bNotDelete == false && pCaption->m_bSelfDelete)
		{
			delete pCaption;
			itr = m_mapCaptionsWithNode.erase(itr);
			continue;
		}

		if (pCaption->GetVisible() == true  &&  pCaption->IsPassRender() == false && pCaption->IsFontRender() == true  &&  pSceneNode->IsPrimaryCamRenerable() )
		{
			float fDist = vCamPos.DistanceToSq( pCaption->GetPosition() );

			m_mapCaptionsWithDistance.insert( CAP_DIST_MAP::value_type( fDist, pCaption ) );
		}
		else if(pCaption->IsPassRender() == false && pCaption->IsFontRender() == false && pCaption->CheckRenderByCamera(pCam))
		{
			// 데미지이다.
			m_DamageCaptionDraw.SetDamageCaption(pCaption->RenderInfo.m_matForRender, (wstring)pCaption->GetText(), pCaption->RenderInfo.m_dwColor);
		}

		++itr;
	}

	// Character Caption Rendering
	if (rs3::RFont::BeginFont( ) )
	{
		// 거리로 소팅된 순서로 렌더링
		for ( CAP_DIST_MAP::reverse_iterator r_itr = m_mapCaptionsWithDistance.rbegin(); r_itr != m_mapCaptionsWithDistance.rend(); ++r_itr )
		{
			XCaption* pCharCap = r_itr->second;

			pCharCap->preRender();

			// 거리에 따라 크기가 변하는 경우
			REngine::GetDevice().SetTransform(RST_VIEW, m_matBuf.m_matView);
			DrawTextIn3DSpace( r_itr->second );

			device->SetTransform( RST_PROJECTION, m_matBuf.m_matProject);
			pCharCap->postRender();

			//// 거리에 따라 크기가 변하지 않는 경우
			//CAP_RENDER_INFO& renderInfo = r_itr->second;
			//RMatrix& matT = renderInfo.m_matForRender;
			//DrawTextIn3DSpaceEqualScale( renderInfo , matT._41, matT._42, matT._43 );
		}
		REngine::GetDevice().SetTransform(RST_VIEW, m_matBuf.m_matView);
		device->SetTransform( RST_PROJECTION, m_matBuf.m_matProject);
		m_DamageCaptionDraw.Render();

		rs3::RFont::EndFont();
	}

	// 뷰 행렬 돌림
	device->SetTransform( RST_VIEW, m_matBuf.m_matView );
}

void XCaptionMgr::DrawTextIn3DSpaceEqualScale( XCaption* pCharCap , float x, float y, float z )
{
	// set for inverse view matrix
	vec3 vPos(x, y, z);

	m_matBuf.m_matView.TransformVect(vPos, m_matBuf.m_vPosInViewCoord);
	m_matBuf.m_matZoomInTrans.SetTranslation( - m_matBuf.m_vPosInViewCoord );
	m_matBuf.m_matZoomOutTrans.SetTranslation( m_matBuf.m_vPosInViewCoord );

	m_matBuf.m_matScaleInverse._11 = - m_matBuf.m_vPosInViewCoord.z / 500.f;
	m_matBuf.m_matScaleInverse._22 = - m_matBuf.m_vPosInViewCoord.z / 500.f;

	REngine::GetDevice().SetTransform(RST_VIEW, m_matBuf.m_matView * m_matBuf.m_matZoomInTrans * m_matBuf.m_matScaleInverse * m_matBuf.m_matZoomOutTrans );
	DrawTextIn3DSpace(pCharCap);
}

void XCaptionMgr::DrawTextIn3DSpace( XCaption* pCharCap )
{
	_ASSERT( m_pCaptionFont != NULL );

	if(pCharCap->GetText().empty()) return;

	m_pCaptionFont->DrawTextIn3DSpace( pCharCap->RenderInfo.m_matForRender, pCharCap->GetText().c_str(), -1, pCharCap->RenderInfo.m_dwColor );
}

void XCaptionMgr::CreatDamageCaption()
{
	m_DamageCaptionDraw.Create(wstring(L"number.png"), XCONST::CAPTION_DAMAGE_FONT_SIZE);
}

void XCaptionMgr::DestroyDamageCaption()
{
	m_DamageCaptionDraw.Destroy();
}