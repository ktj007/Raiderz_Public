#include "StdAfx.h"
#include "RCameraScreenMotionBlurController.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RShaderCompositeManager.h"



namespace rs3 {



class ScreenMotionBlurControllerListener : public RListener
{
	RCameraScreenMotionBlurController* m_pController;
public:
	ScreenMotionBlurControllerListener(RCameraScreenMotionBlurController* pController) : m_pController(pController) {}

	virtual void Update() override
	{
		m_pController->Destroy();
		m_pController->Create();
	}
};



RCameraScreenMotionBlurController::RCameraScreenMotionBlurController(  RCameraSceneNode* pCameraSceneNode )
	: RCameraSceneNodeController( RCCT_POSTRENDER, "2_SCREEN_MOTION_BLUR", true )
	, m_pCameraSceneNode(pCameraSceneNode)
	, m_pColorBufferOwn(NULL)
	, m_pColorBufferOwnOrg(NULL)
	, m_pShader(NULL)
	, m_nType(SMB_BY_CAM_MOVE)
	, m_dwScatStartTime(0)
{
	// 초기값은 false로 하고 그때 그때 필요시 active시키는 걸로 한다.
	m_bActive	= false;
	m_bLockFlag = false;

	m_dwWidth = 0;
	m_dwHeight = 0;

	m_nSamplerCnt = 10;
	m_fSpeed = 5;

	m_matPrevVP = m_pCameraSceneNode->GetViewProjectionMatrix();
	m_matPrevVP.GetInverse( &m_matInvVP);

	m_invalidateRotate = true;

	m_vEpicenter = RVector4( 0.5f, 0.5f, 0, 0);

	// 뷰프로젝션 매트릭스의 이전 값을 셋팅
	m_matPrevVP = m_pCameraSceneNode->GetViewProjectionMatrix();
	m_matPrevV = m_pCameraSceneNode->GetViewMatrix();
	m_vPrePos = m_pCameraSceneNode->GetPosition();
	m_vPreUp = m_pCameraSceneNode->GetUp();
	m_vPreDir = m_pCameraSceneNode->GetDirection();

	// event 구독
	m_pEventListener = new ScreenMotionBlurControllerListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pEventListener );

	if( !Create())
		mlog3("ScreenMotionBlur 카메라 생성 실패!\n");
}

RCameraScreenMotionBlurController::~RCameraScreenMotionBlurController(void)
{
	// event 해제
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pEventListener );

	SAFE_DELETE(m_pEventListener);

	Destroy();
}






bool RCameraScreenMotionBlurController::Create()
{
	
	RFORMAT eBufferFormat = RFMT_A8R8G8B8;

	m_dwWidth	= (DWORD)REngine::GetDevice().GetScreenWidth();
	m_dwHeight	= (DWORD)REngine::GetDevice().GetScreenHeight();

	if( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;
	if( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	// 사용 할지 안할지 모르지만 런타임 시 다른 부분(HDR등)에서 MRT 사용 유무가 바뀔 수 있으므로 우선 무조건 생성
	m_pColorBufferOwn = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eBufferFormat);
	if(R_NONE==m_pColorBufferOwn) return false;

	m_pShader = REngine::GetDevice().CreateShaderFX("ScreenMotionBlur.fx");

	// OnPreRender()에서 기존 백업 하고 내꺼 설정
	// 대신 조건이 OnPreRender()시와 OnPostRender()시의 컨트롤러 수행 순서가 바뀌어야 한다.

	return true;
}

void RCameraScreenMotionBlurController::Destroy()
{
	REngine::GetDevicePtr()->DeleteTexture( m_pColorBufferOwn);

	if (m_pShader)
		REngine::GetDevice().DeleteShaderFX(m_pShader);

	m_bLockFlag = false;
}


void RCameraScreenMotionBlurController::Restore()
{
	REngine::GetDevice().ColorFill( m_pColorBufferOwn );

	m_bLockFlag = false;
}


//-----------------------------------------------------------------------------
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void RCameraScreenMotionBlurController::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, int bSmallSize)
{
	REngine::GetDevice().SetFillMode(RFILL_SOLID);

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	// For more info see the doc page titled "Directly Mapping Texels to Pixels"
	float fWidth5, fHeight5;

	if( bSmallSize == 1)
	{
		fWidth5		= 256;
		fHeight5	= 256;
	}
	else
	{
		fWidth5		= (float)m_dwWidth;
		fHeight5	= (float)m_dwHeight;
	}

	// Draw the quad
	RVt_pos_tex_rhw vQuad[4];
	vQuad[0].vPos	= RVector4( -0.5f, -0.5f, 0.0f, 1.0f );
	vQuad[0].u		= fLeftU;
	vQuad[0].v		= fTopV;

	vQuad[1].vPos	= RVector4( -0.5f, fHeight5, 0.0f, 1.0f );
	vQuad[1].u		= fLeftU;
	vQuad[1].v		= fBottomV;

	vQuad[2].vPos	= RVector4( fWidth5, -0.5f, 0.0f, 1.0f );
	vQuad[2].u		= fRightU;
	vQuad[2].v		= fTopV;

	vQuad[3].vPos	= RVector4( fWidth5, fHeight5, 0.0f, 1.0f );
	vQuad[3].u		= fRightU;
	vQuad[3].v		= fBottomV;

	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetFvF(RVt_pos_tex_rhw::FVF);
	REngine::GetDevice().DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2, vQuad, sizeof( RVt_pos_tex_rhw ) );
	REngine::GetDevice().SetDepthEnable(true, true);
}


//-----------------------------------------------------------------------------
// Name: RenderDOF()
// Desc: Renders Blur effect
//-----------------------------------------------------------------------------
void RCameraScreenMotionBlurController::Render()
{
	// 활성화 시만 처리
	if( !m_bActive || !GetInferredRenderingStrategy())
		return;

	FrameMove();

	RTexture* pRT1 = NULL;
	// 카메라 이동 모드일 경우만 뎁스 정보를 사용한다. 진앙지모드에서는 RT1을 NULL로 해두었다가 돌려놓는다.
	if( m_nType == SMB_BY_EPICENTER)
	{
		pRT1 = REngine::GetDevice().GetRenderTarget( 1);
		REngine::GetDevice().SetRenderTarget( 1, NULL);
	}

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
	REngine::GetDevice().SetTexture(2, NULL);
	REngine::GetDevice().SetTexture(3, NULL);
	REngine::GetDevice().SetTexture(4, NULL);
	REngine::GetDevice().SetTexture(5, NULL);


	REngine::GetDevice().SetAlphaTestEnable(false);

	RTechnique Technique;
	switch( m_nType)
	{
	case SMB_BY_CAM_MOVE: Technique = m_pShader->GetTechnique("ScreenMotionBlurBasisCamMove");	break;
	case SMB_BY_EPICENTER: Technique = m_pShader->GetTechnique("ScreenMotionBlurBasisEpicenter");	break;
	}
	
	REngine::GetDevice().SetTexture(0, m_pColorBufferOwn);
	// 카메라 이동 모드일 경우만 뎁스 정보를 사용한다.
	if( m_nType == SMB_BY_CAM_MOVE)
		REngine::GetDevice().SetTexture(1, GetInferredRenderingStrategy()->GetDepthBuffer());
	
	if( m_nType == SMB_BY_CAM_MOVE)
	{
		m_pCameraSceneNode->GetViewProjectionMatrix().GetInverse( &m_matInvVP);
		// 뷰프로젝션매트릭스를 그대로 쓰지 않고 위치를 (0,0,0)으로 만든다. 
		RMatrix matDownScaleV;
		RVector3 vDownScalePos(0,0,0);
		RVector3 vDownScaleDir = m_pCameraSceneNode->GetDirection();
		RVector3 vDownScaleUp = m_pCameraSceneNode->GetUp();
		matDownScaleV.SetLookAtMatrixRH( vDownScalePos, vDownScalePos+vDownScaleDir, vDownScaleUp);
		RMatrix matDownScaleVP = matDownScaleV * m_pCameraSceneNode->GetProjectionMatrix();
		matDownScaleVP.GetInverse( &m_matInvVP);
		

		// 이전 매트릭스를 그대로 적용 하면 프레임마다의 간격 및 방향이 일정하지 않아서
		// 이번 매트릭스를 그대로 사용 하지 않고 방향 및 회전 정보를 뽑아내도록 하자
		// 프로젝션 매트릭스는 프레임 단위로 변하지 않으므로( 변한다면 왜!! 설마.. 말도안되.. 후버댐!)
		// 뷰매트릭스를 만들자.
		float fSlerp = REngine::GetDevice().GetFrameRate() / 30.0f;
		
		// 한줄로 써 넣는 것보다 가독성 측면에서 이게 낫네..
		// pos
		RVector3 vPos = m_vPrePos - m_pCameraSceneNode->GetPosition();
		vPos *= fSlerp;
		//vPos += m_pCameraSceneNode->GetPosition();
		vPos += vDownScalePos;
		// dir
		RVector3 vDir =  m_vPreDir - m_pCameraSceneNode->GetDirection();
		vDir *= fSlerp;
		vDir += m_pCameraSceneNode->GetDirection();
		// up
		RVector3 vUp =  m_vPreUp - m_pCameraSceneNode->GetUp();
		vUp *= fSlerp;
		vUp += m_pCameraSceneNode->GetUp();


		// 만들어진 위치 가지고 뷰매트릭스 생성
		RMatrix matMaedenPervView;
		if( m_invalidateRotate)
			// 회전 정보를 무효화 하는 경우는 현재의 정보로 위치와 상향 벡터 설정
			matMaedenPervView.SetLookAtMatrixRH( vPos, vPos + m_pCameraSceneNode->GetDirection(), m_pCameraSceneNode->GetUp() );
		else
			matMaedenPervView.SetLookAtMatrixRH( vPos, vPos+vDir, vUp );
		// 최종 만들어진 이전 프레임 뷰 프로젝션 매트릭스
		m_matPrevVP = matMaedenPervView * m_pCameraSceneNode->GetProjectionMatrix();

		m_pShader->GetConstant("g_previousViewProjectionMatrix").SetMatrix( m_matPrevVP);
		m_pShader->GetConstant("g_ViewProjectionInverseMatrix").SetMatrix( m_matInvVP);
	}
		
	m_pShader->GetConstant("g_numSamples").SetInt( m_nSamplerCnt);
	m_pShader->GetConstant("g_fFarZ").SetFloat( m_pCameraSceneNode->GetFarZ());
	m_pShader->GetConstant("g_fSpeed").SetFloat( m_nType == SMB_BY_CAM_MOVE ? m_fSpeed : m_fSpeed * 0.01f);
	m_pShader->GetConstant("g_vEpicenter").SetVector4( m_vEpicenter);

	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 1, RTF_POINT );

	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetAlphaTestEnable(false);

	REngine::GetDevice().SetDepthEnable( false, false);

	REngine::GetDevice().SetTextureAddressClamp( 0);
	REngine::GetDevice().SetTextureAddressClamp( 1);

	UINT uiPass, uiPassCount;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f, false );

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR );

	REngine::GetDevice().SetTextureAddressWrap( 0);
	REngine::GetDevice().SetTextureAddressWrap( 1);

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
	REngine::GetDevice().SetTexture(2, NULL);
	REngine::GetDevice().SetTexture(3, NULL);
	REngine::GetDevice().SetTexture(4, NULL);
	REngine::GetDevice().SetTexture(5, NULL);

	// 카메라 이동 모드일 경우만 뎁스 정보를 사용한다. 진앙지모드에서는 RT1을 NULL로 해두었다가 돌려놓는다.
	if( m_nType == SMB_BY_EPICENTER)
	{
		REngine::GetDevice().SetRenderTarget( 1, pRT1);
	}


}


//-----------------------------------------------------------------------------
// Name: RenderOverlay()
// Desc: Visializes one of the surfaces in the overlay
//-----------------------------------------------------------------------------
void RCameraScreenMotionBlurController::RenderOverlay()
{
	// 활성화 시만 처리
	if( !m_bActive || !GetInferredRenderingStrategy())
		return;

	// 정보 오버레이는 RT1에 관여 하지 않는다.
//	RTexture* pRT1 = REngine::GetDevice().GetRenderTarget( 1);
//	REngine::GetDevice().SetRenderTarget( 1, NULL);

	RTechnique TechniqueOverlay = m_pShader->GetTechnique( "DepthOverlay");

	m_pShader->GetConstant("depthScale").SetFloat( 0.8f);

	REngine::GetDevice().SetTexture(0, m_pColorBufferOwn);
	//REngine::GetDevice().SetTexture(0, m_pDepthBuffer);
	REngine::GetDevice().SetTexture(1, NULL);

	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetAlphaTestEnable(false);

	REngine::GetDevice().SetDepthEnable( false, false);	// 090625, OZ

	UINT uiPass, uiPassCount;
	TechniqueOverlay.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		TechniqueOverlay.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f, 1);

		TechniqueOverlay.EndPass();
	}
	TechniqueOverlay.End();

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);

	// 정보 오버레이는 RT1에 관여 하지 않는다.
//	REngine::GetDevice().SetRenderTarget( 1, pRT1);

}


// RT1을 미리 특정 색으로 채운다 - 090626, OZ
void RCameraScreenMotionBlurController::RenderColorFill()
{
	// 활성화 시만 처리
	if(!m_bActive || !GetInferredRenderingStrategy())
		return;
}


void RCameraScreenMotionBlurController::OnRender()
{
	// 활성화 시만 처리
	if(!m_bActive || !GetInferredRenderingStrategy())
		return;

	REngine::GetDevice().SetFogEnable(false);

	Render();

	//RenderOverlay();

}


void RCameraScreenMotionBlurController::OnPostRender()
{
	// 활성화 시만 처리
	if(!m_bActive || !GetInferredRenderingStrategy())
		return;

	// 기존 백업용으로 돌려놓는다. - 090617, OZ
	REngine::GetDevice().SetRenderTarget( 0, m_pColorBufferOwnOrg );

	OnRender();

	// 뷰프로젝션 매트릭스의 이전 값을 셋팅
	m_matPrevVP = m_pCameraSceneNode->GetViewProjectionMatrix();
	m_matPrevV = m_pCameraSceneNode->GetViewMatrix();
	m_vPrePos = m_pCameraSceneNode->GetPosition();
	m_vPreUp = m_pCameraSceneNode->GetUp();
	m_vPreDir = m_pCameraSceneNode->GetDirection();

	m_bLockFlag = false;
	
}


// 랜더 타겟을 관리 - 090617, OZ
void RCameraScreenMotionBlurController::OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy)
{
	RCameraSceneNodeController::OnPreRender( pInferredRenderingStrategy);

	// 활성화 시만 처리
	if(!m_bActive || !GetInferredRenderingStrategy())
		return;

	// OnPreRender()에서 기존 백업 하고 내꺼 설정
	// 대신 조건이 OnPreRender()시와 OnPostRender()시의 컨트롤러 수행 순서가 바뀌어야 한다.

	// 기존 RT 백업
	m_pColorBufferOwnOrg = REngine::GetDevice().GetRenderTarget(0);

	// 새 RT 지정
	REngine::GetDevice().SetRenderTarget( 0, m_pColorBufferOwn );

	RShaderCompositeManager::m_cFarZ.SetFloat( m_pCameraSceneNode->GetFarZ());

	// 이전 위치 값을 가지고 현재의 이동 방향 구한다.
	RVector3 m_vMoveDir = m_pCameraSceneNode->GetPosition() - m_vPrePos;
	m_vMoveDir.Normalize();

	// 랜더 타겟을 셋팅했다. 액티브 플래그가 변경 되면 안된다.
	m_bLockFlag = true;
}


void RCameraScreenMotionBlurController::OnRemoveFromCamera()
{
	//카메라 씬 노드의 정보를 건드리지 않게 되었다 - 090617, OZ
	//m_pCameraSceneNode->SetRenderTarget( 0, RT_FRAMEBUFFER );
	//m_pCameraSceneNode->SetRenderTarget( 1, NULL);
}

void RCameraScreenMotionBlurController::OnClear()
{
	REngine::GetDevice().Clear( true, true, 
		m_pCameraSceneNode->isHaveStencil(),
		m_pCameraSceneNode->GetClearColor(),
		m_pCameraSceneNode->GetClearDepth(),
		m_pCameraSceneNode->GetClearStencil() );

}




void RCameraScreenMotionBlurController::PlayScat( float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType)
{
	SetBasisByEpicenter();

	m_dwScatStartTime = REngine::GetTimer().GetTime();

	m_dwPlayTime = (DWORD)(float)( fPlayTime * 1000.0f);
	m_dwScatTime = (DWORD)(float)( fScatTime * 1000.0f);
	m_fScatSpeed = fSpeed;
	m_nScarFactorType = nScarFactorType;

}


void RCameraScreenMotionBlurController::FrameMove()
{
	if( m_nType == SMB_BY_EPICENTER && m_dwScatStartTime > 0)
	{
		DWORD dwElap = REngine::GetTimer().GetTime() - m_dwScatStartTime;
		if( dwElap > m_dwPlayTime)
		{
			m_dwScatStartTime = 0;
			m_fSpeed = 0;
			m_nScarFactorType = 0;
		}
		else
		{
			float fFactor;
			// 퍼지는 과정
			if( dwElap < m_dwScatTime)
			{
				fFactor = (float)dwElap / (float)m_dwScatTime;
			}
			// 모아지는 과정
			else
			{
				fFactor = (float)( m_dwPlayTime-dwElap) / (float)(m_dwPlayTime-m_dwScatTime);
			}

			switch( m_nScarFactorType)
			{
			case SAT_FAST_SCAT: fFactor = 1.0f - ((1.0f-fFactor) * (1.0f-fFactor));	// 1
				break;
			case SAT_SLOW_SCAT: fFactor *= fFactor;									// 2
				break;
			case SAT_GATTER: fFactor = ((1.0f-fFactor) * (1.0f-fFactor));			// 3
				break;
			}

			m_fSpeed = m_fScatSpeed * fFactor;
		}
	}
	else
	{
		m_dwScatStartTime = 0;
	}
}



void RCameraScreenMotionBlurController::SetEpicenterProjectionCoord( float x, float y)
{ 
	float centerX = (x+1)/2.0f;
	if( centerX < 0)
		centerX = 0;
	else if( centerX > 1)
		centerX = 1;
	float centerY = (-y+1)/2.0f;
	if( centerY < 0)
		centerY = 0;
	else if( centerY > 1)
		centerY = 1;

	m_vEpicenter = RVector4( centerX, centerY, 0, 0);	
}



void RCameraScreenMotionBlurController::SetEpicenterWorld( RCameraSceneNode* pCurrentCamera, RVector3& v)
{
	// 월드상의 위치를 투영 좌표계로 투영시키고 이를 텍스쳐 좌표로 변환.. 
	RVector4 vProjPos = v * pCurrentCamera->GetViewProjectionMatrix();
	SetEpicenterProjectionCoord( vProjPos.x, vProjPos.y);	

}



}