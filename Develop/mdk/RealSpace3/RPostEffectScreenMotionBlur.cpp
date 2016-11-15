#include "StdAfx.h"
#include "RPostEffectScreenMotionBlur.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RShaderCompositeManager.h"
#include "RTechnique.h"
#include "RShaderFX.h"


namespace rs3 {



RPostEffectScreenMotionBlur::RPostEffectScreenMotionBlur()
: RPostEffectInterface( PE_SCREEN_MOTION_BLUR, RRP_NONE )
, m_pShader(NULL)
, m_nType(SMB_BY_CAM_MOVE)
, m_dwScatStartTime(0)
{

	m_dwWidth = 0;
	m_dwHeight = 0;

	m_nSamplerCnt = 10;
	m_fSpeed = 5;

	//m_matPrevVP = m_pCameraSceneNode->GetViewProjectionMatrix();
	//m_matPrevVP.GetInverse( &m_matInvVP);

	m_invalidateRotate = true;

	m_vEpicenter = RVector4( 0.5f, 0.5f, 0, 0);

	// 뷰프로젝션 매트릭스의 이전 값을 셋팅
	//m_matPrevVP = m_pCameraSceneNode->GetViewProjectionMatrix();
	//m_matPrevV = m_pCameraSceneNode->GetViewMatrix();
	//m_vPrePos = m_pCameraSceneNode->GetPosition();
	//m_vPreUp = m_pCameraSceneNode->GetUp();
	//m_vPreDir = m_pCameraSceneNode->GetDirection();

}

RPostEffectScreenMotionBlur::~RPostEffectScreenMotionBlur(void)
{
}






bool RPostEffectScreenMotionBlur::Init( int nW, int nH)
{
	if(!RPostEffectInterface::Init(nW, nH))
		return false;
	
	RFORMAT eBufferFormat = RFMT_A8R8G8B8;

	if( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;
	if( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	if(!m_pShader)
		m_pShader = REngine::GetDevice().CreateShaderFX("ScreenMotionBlur.fx");

	return true;
}

void RPostEffectScreenMotionBlur::Destroy()
{
	RPostEffectInterface::Destroy();

	if (m_pShader)
		REngine::GetDevice().DeleteShaderFX(m_pShader);
	m_pShader = NULL;
}

//-----------------------------------------------------------------------------
// Name: RenderDOF()
// Desc: Renders Blur effect
//-----------------------------------------------------------------------------
void RPostEffectScreenMotionBlur::Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_THISFUNC;

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


	RTechnique Technique;
	switch( m_nType)
	{
	case SMB_BY_CAM_MOVE: Technique = m_pShader->GetTechnique("ScreenMotionBlurBasisCamMove");	break;
	case SMB_BY_EPICENTER: Technique = m_pShader->GetTechnique("ScreenMotionBlurBasisEpicenter");	break;
	}
	
	REngine::GetDevice().SetTexture(0, pColorBuffer);
	// 카메라 이동 모드일 경우만 뎁스 정보를 사용한다.
	if( m_nType == SMB_BY_CAM_MOVE)
		REngine::GetDevice().SetTexture(1, pDepthBuffer);
	
	if( m_nType == SMB_BY_CAM_MOVE)
	{
		// 뷰프로젝션매트릭스를 그대로 쓰지 않고 위치를 (0,0,0)으로 만든다. 
		RMatrix matDownScaleV;
		RVector3 vDownScalePos(0,0,0);
		RVector3 vDownScaleDir = pCamera->GetDirection();
		RVector3 vDownScaleUp = pCamera->GetUp();
		matDownScaleV.SetLookAtMatrixRH( vDownScalePos, vDownScalePos+vDownScaleDir, vDownScaleUp);
		matDownScaleV.GetInverse( &m_matInvV);
		

		// 이전 매트릭스를 그대로 적용 하면 프레임마다의 간격 및 방향이 일정하지 않아서
		// 이번 매트릭스를 그대로 사용 하지 않고 방향 및 회전 정보를 뽑아내도록 하자
		// 프로젝션 매트릭스는 프레임 단위로 변하지 않으므로( 변한다면 왜!! 설마.. 말도안되.. 후버댐!)
		// 뷰매트릭스를 만들자.

		// TODO: 프레임이 튈경우 1초간 GetFrameRate 가 갱신이 안되기 때문에 이상해집니다. 개선이 필요해보입니다
		float fSlerp = REngine::GetDevice().GetFrameRate() / 30.0f;

		// 한줄로 써 넣는 것보다 가독성 측면에서 이게 낫네..
		// pos
		RVector3 vPos = m_vPrePos - pCamera->GetPosition();
		vPos *= fSlerp;
		//vPos += m_pCameraSceneNode->GetPosition();
		vPos += vDownScalePos;
		// dir
		RVector3 vDir =  m_vPreDir - pCamera->GetDirection();
		vDir *= fSlerp;
		vDir += pCamera->GetDirection();
		// up
		RVector3 vUp =  m_vPreUp - pCamera->GetUp();
		vUp *= fSlerp;
		vUp += pCamera->GetUp();


		// 만들어진 위치 가지고 뷰매트릭스 생성
		RMatrix matMaedenPervView;
		if( m_invalidateRotate)
			// 회전 정보를 무효화 하는 경우는 현재의 정보로 위치와 상향 벡터 설정
			matMaedenPervView.SetLookAtMatrixRH( vPos, vPos + pCamera->GetDirection(), pCamera->GetUp() );
		else
			matMaedenPervView.SetLookAtMatrixRH( vPos, vPos+vDir, vUp );
		// 최종 만들어진 이전 프레임 뷰 프로젝션 매트릭스
		m_matPrevVP = matMaedenPervView * pCamera->GetProjectionMatrix();

		m_pShader->GetConstant("g_previousViewProjectionMatrix").SetMatrix( m_matPrevVP);
		m_pShader->GetConstant("g_ViewInverseMatrix").SetMatrix(m_matInvV);

		RMatrix matView = pCamera->GetViewMatrix();
		RVector3 vP = pCamera->GetViewFrustum().GetPoints()[0];
		matView.TransformVect(vP);
		m_pShader->GetConstant("g_vFarClipCornerInView").SetVector3(RVector3(abs(vP.x),-abs(vP.y),vP.z));
	}
		
	m_pShader->GetConstant("g_numSamples").SetInt( m_nSamplerCnt);
	m_pShader->GetConstant("g_fFarZ").SetFloat( pCamera->GetFarZ());
	m_pShader->GetConstant("g_fSpeed").SetFloat( m_nType == SMB_BY_CAM_MOVE ? m_fSpeed : m_fSpeed * 0.01f);
	m_pShader->GetConstant("g_vEpicenter").SetVector4( m_vEpicenter);

	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 1, RTF_POINT );

	REngine::GetDevice().SetDepthEnable( false, false);

	REngine::GetDevice().SetTextureAddressClamp( 0);
	REngine::GetDevice().SetTextureAddressClamp( 1);

	UINT uiPass, uiPassCount;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidth, m_dwHeight);
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

	// 뷰프로젝션 매트릭스의 이전 값을 셋팅
	m_matPrevVP = pCamera->GetViewProjectionMatrix();
	m_matPrevV = pCamera->GetViewMatrix();
	m_vPrePos = pCamera->GetPosition();
	m_vPreUp = pCamera->GetUp();
	m_vPreDir = pCamera->GetDirection();


	REngine::GetDevice().SetDepthEnable(true, true);


	//RenderOverlay();
}


//-----------------------------------------------------------------------------
// Name: RenderOverlay()
// Desc: Visializes one of the surfaces in the overlay
//-----------------------------------------------------------------------------
void RPostEffectScreenMotionBlur::RenderOverlay( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{

	// 정보 오버레이는 RT1에 관여 하지 않는다.
//	RTexture* pRT1 = REngine::GetDevice().GetRenderTarget( 1);
//	REngine::GetDevice().SetRenderTarget( 1, NULL);

	RTechnique TechniqueOverlay = m_pShader->GetTechnique( "DepthOverlay");

	m_pShader->GetConstant("depthScale").SetFloat( 0.8f);

	REngine::GetDevice().SetTexture(0, pColorBuffer);
	//REngine::GetDevice().SetTexture(0, m_pDepthBuffer);
	REngine::GetDevice().SetTexture(1, NULL);

	REngine::GetDevice().SetDepthEnable( false, false);	// 090625, OZ

	UINT uiPass, uiPassCount;
	TechniqueOverlay.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		TechniqueOverlay.BeginPass(uiPass);
		DrawFullScreenQuad( 0.0f, 0.0f, 256, 256);
		TechniqueOverlay.EndPass();
	}
	TechniqueOverlay.End();

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);

	// 정보 오버레이는 RT1에 관여 하지 않는다.
//	REngine::GetDevice().SetRenderTarget( 1, pRT1);

}


void RPostEffectScreenMotionBlur::PlayScat( float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType)
{
	SetBasisByEpicenter();

	m_dwScatStartTime = REngine::GetTimer().GetTime();

	m_dwPlayTime = (DWORD)(float)( fPlayTime * 1000.0f);
	m_dwScatTime = (DWORD)(float)( fScatTime * 1000.0f);
	m_fScatSpeed = fSpeed;
	m_nScarFactorType = nScarFactorType;

}


void RPostEffectScreenMotionBlur::FrameMove()
{
	RPFC_THISFUNC;

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



void RPostEffectScreenMotionBlur::SetEpicenterProjectionCoord( float x, float y)
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



void RPostEffectScreenMotionBlur::SetEpicenterWorld( RCameraSceneNode* pCurrentCamera, RVector3& v)
{
	// 월드상의 위치를 투영 좌표계로 투영시키고 이를 텍스쳐 좌표로 변환.. 
	RVector4 vProjPos = v * pCurrentCamera->GetViewProjectionMatrix();
	SetEpicenterProjectionCoord( vProjPos.x, vProjPos.y);	

}



}