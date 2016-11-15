// VD_DlgNewMiniMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgNewMiniMap.h"
#include "C_ControlMediator.h"

#include "RTreePassRenderer.h"
#include "RWaterPassRenderer.h"
#include "VL_SceneNodeList.h"

#include "M_ToolStaticMeshSceneNode.h"

#include "WS_WorkSpaceManager.h"
#include "WS_Field.h"

// GDI Plus
#include <GdiPlus.h> 
#include <atlconv.h>

// CDlgNewMiniMap 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgNewMiniMap, CDialog)

CDlgNewMiniMap::CDlgNewMiniMap(CWnd* pParent /*=NULL*/)
: CDialog(CDlgNewMiniMap::IDD, pParent)
{
}

CDlgNewMiniMap::~CDlgNewMiniMap()
{
}

void CDlgNewMiniMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNewMiniMap, CDialog)
END_MESSAGE_MAP()

bool CDlgNewMiniMap::CreateMiniMap(CControlMediator* _pControlMediator)
{
	using namespace rs3;
	CControlMediator::MAP_INFO& mapInfo = _pControlMediator->m_mapInfo;

	RDevice* pDevice = REngine::GetDevicePtr();
	_ASSERT( NULL != pDevice );

	RToolSceneManager* pSceneManager = _pControlMediator->GetToolSceneManager();
	ASSERT(pSceneManager);

	// camera
	RToolCamera* pCamera = _pControlMediator->GetToolCamera();
	ASSERT(pCamera);

	// 스크린샷 싸이즈
	const int nWidth = 2048;
	const int nHeight = 2048;

	// 오리지날 정보
	RVector vOrgCamPos = pCamera->GetPosition();
	RVector vOrgCamDir = pCamera->GetDirection();

	float fOldNearZ = pCamera->GetNearZ();
	float fOldFarZ = pCamera->GetFarZ();
	float fOldFovX = pCamera->GetFovX();

	REngine::GetSceneManager().GetPassRenderer<RTreePassRenderer>()->SetTreeLOD( 1000000, 1000000 );

	RTerrain* pTerrain = _pControlMediator->GetToolSceneManager()->GetCurrentTerrain();
	RToolStaticMeshSceneNode* pStaticMesh = static_cast<RToolStaticMeshSceneNode*>(_pControlMediator->GetToolSceneManager()->GetCurrentStaticMeshSceneNode());


	RBoundingBox aabb;
	aabb.Initialize();
	if( pTerrain )
	{
		aabb = pTerrain->GetAABB();
	}

	pSceneManager->GatherAABB( &aabb );

	if( pStaticMesh )
	{
		if( pTerrain )
			aabb.Add( pStaticMesh->GetAABB() );
		else
			aabb = pStaticMesh->GetAABB();
	}

	// StaticMesh는 모든 셀을 무조건 렌더링한다
	bool bStaticMeshRenderAll = false;
	if( pStaticMesh )
	{
		bStaticMeshRenderAll = pStaticMesh->IsRenderAll();
		pStaticMesh->SetRenderAll( true );
	}

	// 카메라 위치를 가운데 위쪽에 배치
	RVector aabbCenter = aabb.GetCenter();
	const RVector camPos = RVector( aabbCenter.x, aabbCenter.y, aabb.vmax.z );
	const RVector camDir = RVector( 0.f, 0.f, -1.f );
	const RVector camUp = RVector( 0.f, 1.f, 0.f );
	pCamera->Set( camPos, camDir, camUp );

	// view 가 identity 가 되도록 설정
	RVector vLength = aabb.vmax - aabb.vmin;
	if( vLength.z == 0.0f )
	{
		vLength.z = 1.0f;
	}
	vLength.z *= 1.1f;
	RMatrix orthoProjection;
	orthoProjection.MakeIdentity();
	orthoProjection.SetOrthoRH( vLength.x, vLength.y, 0.0f, vLength.z );
	pCamera->SetUserProjection( orthoProjection );
	pCamera->UpdateViewFrustrum();

	// 포그 끄기
	bool bFoged = pCamera->GetFogEnable();
	if( bFoged )
		pCamera->SetFogEnable(false);

	// 디스턴스 컬링 끄기
	bool bOldDistanceCullEnable = pCamera->GetDistanceCullEnable();
	pCamera->SetDistanceCullEnable( false );

	CWorkSpaceField* pField = _pControlMediator->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);
	int fieldId = pField->m_fieldInfo.m_nReferZone;
	string szField = pField->m_fieldInfo.m_strFieldFile;

	char szFieldID[512] = {0, };
	sprintf_s(szFieldID, sizeof(szFieldID), "..\\..\\Data\\Interface\\minimap\\%05d.bmp", fieldId);

	// 씬 업데이트
	pSceneManager->UpdateTimer();
	pSceneManager->Update();



	//버퍼 크기 재설정
	RRenderingStrategy* pRenderingStrategy = pSceneManager->GetRenderingStrategy();
	_ASSERT( pRenderingStrategy );
	pRenderingStrategy->Invalidate();
	pRenderingStrategy->Init( nWidth, nHeight );
	pRenderingStrategy->SetBackgroundBufferDivideSize(1);

	RWaterPassRenderer* pWaterPassRenderer = pSceneManager->GetPassRenderer<RWaterPassRenderer>(false);
	if( pWaterPassRenderer )
		pWaterPassRenderer->RecreateBuffer();

	// 스크린샷
	pCamera->RenderScreenShot(szFieldID, NULL, nWidth, nHeight);
	REngine::GetDevice().Flip();

	//버퍼 복구
	pRenderingStrategy->Invalidate();
	pRenderingStrategy->Init( pDevice->GetScreenWidth(), pDevice->GetScreenHeight() );
	pRenderingStrategy->SetBackgroundBufferDivideSize(2);

	if( pWaterPassRenderer )
		pWaterPassRenderer->RecreateBuffer();

	// 포그 복구
	if( bFoged )
		pCamera->SetFogEnable(true);

	// 디스턴스 컬링 복구
	pCamera->SetDistanceCullEnable( bOldDistanceCullEnable );

	// 카메라 재위치
	pCamera->SetFov(fOldFovX);
	pCamera->SetNearFarZ(fOldNearZ, fOldFarZ);
	pCamera->SetPosAndDir(vOrgCamPos, vOrgCamDir);
	pCamera->UpdateViewFrustrum();

	// StaticMesh 렌더링 모드 복구
	if( pStaticMesh )
		pStaticMesh->SetRenderAll( bStaticMeshRenderAll );

	return true;
}
