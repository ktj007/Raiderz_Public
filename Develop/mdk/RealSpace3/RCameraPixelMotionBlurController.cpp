#include "StdAfx.h"
#include "RCameraPixelMotionBlurController.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RShaderCompositeManager.h"



namespace rs3 {

static RTexture* pTargetOrg0 = NULL;
static RTexture* pTargetOrg1 = NULL;
static RTexture* pTargetOrg2 = NULL;
static RTexture* pTargetOrg3 = NULL;


class PixelMotionBlurControllerListener : public RListener
{
	RCameraPixelMotionBlurController* m_pController;
public:
	PixelMotionBlurControllerListener(RCameraPixelMotionBlurController* pController) : m_pController(pController) {}

	virtual void Update() override
	{
		m_pController->Destroy();
		m_pController->Create();
	}
};



RCameraPixelMotionBlurController::RCameraPixelMotionBlurController(  RCameraSceneNode* pCameraSceneNode )
	: RCameraSceneNodeController( RCCT_POSTRENDER, "4_Pixel_MOTION_BLUR", true )
	, m_pCameraSceneNode(pCameraSceneNode)
	//, m_pColorBuffer(NULL)
	, m_pColorBufferOrg(NULL)
	, m_pShader(NULL)
	, m_pVelocityBuffer(NULL)
	, m_pRenderNode(NULL)
	, m_dwWidth(0)
	, m_dwHeight(0)
	, m_dwWidthSmall(0)
	, m_dwHeightSmall(0)
	, m_nSamplerCnt(15)
	, m_nImgSamCnt(1)
{
	m_pColorBuffer[0] = NULL;
	m_pColorBuffer[1] = NULL;

	// 초기값은 false로 하고 그때 그때 필요시 active시키는 걸로 한다.
	m_bActive	= true;
	m_bLockFlag = false;
	// 1/65535로 해도 z-fight일어난다. 좀 더 여유있는 수치로.
	m_fBias	= (-1.f / 65535.f) * 10;

	// event 구독
	m_pEventListener = new PixelMotionBlurControllerListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pEventListener );

	m_nImgSamCnt = 1;

	if( !Create())
		mlog3("Pixel Motion Blur 카메라 생성 실패!\n");
}

RCameraPixelMotionBlurController::~RCameraPixelMotionBlurController(void)
{
	// event 해제
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pEventListener );

	SAFE_DELETE(m_pEventListener);

	Destroy();
}



bool RCameraPixelMotionBlurController::Create()
{
	// 속도 문제로 RFMT_A16B16G16R16F(64bit)를 사용 하지 않고 32bit짜리 사용 
	// 64bit짜리를 사용 하는 HDR과 순서를 잘 설정 해야 할 것이다..
	RFORMAT eColorBufferFormat = RFMT_A8R8G8B8;
	// 속도 버퍼는 스크린 좌표의 속도만 쓰이게 된다. xy두 컴포넌트만 유효한 실수형 버퍼로..
	RFORMAT eVelocityBufferFormat = RFMT_G16R16F;
	//RFORMAT eVelocityBufferFormat = RFMT_A8R8G8B8;

	m_dwWidth	= (DWORD)REngine::GetDevice().GetScreenWidth();
	m_dwHeight	= (DWORD)REngine::GetDevice().GetScreenHeight();

	m_dwWidthSmall	= m_dwWidth / 2;
	m_dwHeightSmall	= m_dwHeight / 2;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	m_pColorBuffer[0] = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eColorBufferFormat);
	if(R_NONE==m_pColorBuffer[0]) return false;
	m_pColorBuffer[1] = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eColorBufferFormat);
	if(R_NONE==m_pColorBuffer[1]) return false;
	m_pVelocityBuffer = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eVelocityBufferFormat);
	if(R_NONE==m_pVelocityBuffer) return false;
	REngine::GetDevice().ColorFill( m_pVelocityBuffer);

	m_pShader = REngine::GetDevice().CreateShaderFX("ObjectMotionBlur.fx");

	return true;
}

void RCameraPixelMotionBlurController::Destroy()
{
	REngine::GetDevicePtr()->DeleteTexture( m_pColorBuffer[0]);
	REngine::GetDevicePtr()->DeleteTexture( m_pColorBuffer[1]);
	REngine::GetDevicePtr()->DeleteTexture( m_pVelocityBuffer);

	if (m_pShader)
		REngine::GetDevice().DeleteShaderFX(m_pShader);

	m_bLockFlag = false;
}


void RCameraPixelMotionBlurController::Restore()
{
	REngine::GetDevice().ColorFill( m_pColorBuffer[0]);
	REngine::GetDevice().ColorFill( m_pColorBuffer[1]);
	REngine::GetDevice().ColorFill( m_pVelocityBuffer);

	m_bLockFlag = false;
}


// 액터 개별 버퍼로 하지 않고 공용 버퍼로 한다.
bool RCameraPixelMotionBlurController::BeginRender( RSceneNode* pActorNode)
{
	m_pRenderNode = pActorNode;

	pTargetOrg0 = REngine::GetDevice().GetRenderTarget(0);
	pTargetOrg1 = REngine::GetDevice().GetRenderTarget(1);
	pTargetOrg2 = REngine::GetDevice().GetRenderTarget(2);
	pTargetOrg3 = REngine::GetDevice().GetRenderTarget(3);

	REngine::GetDevice().SetRenderTarget( 0, m_pVelocityBuffer);
	REngine::GetDevice().SetRenderTarget( 1, NULL);
	REngine::GetDevice().SetRenderTarget( 2, NULL);
	REngine::GetDevice().SetRenderTarget( 3, NULL);

	REngine::GetDevice().SetDepthBias( m_fBias);

	return true;
}

bool RCameraPixelMotionBlurController::EndRender( RSceneNode* pActorNode)
{
	_ASSERT( m_pRenderNode == pActorNode);
	m_pRenderNode = NULL;

	REngine::GetDevice().SetRenderTarget( 0, pTargetOrg0);
	REngine::GetDevice().SetRenderTarget( 1, pTargetOrg1);
	REngine::GetDevice().SetRenderTarget( 2, pTargetOrg2);
	REngine::GetDevice().SetRenderTarget( 3, pTargetOrg3);

	REngine::GetDevice().SetDepthBias( 0);

	return true;
}


//-----------------------------------------------------------------------------
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void RCameraPixelMotionBlurController::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, int bSmallSize)
{
	REngine::GetDevice().SetFillMode(RFILL_SOLID);

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	// For more info see the doc page titled "Directly Mapping Texels to Pixels"
	float fWidth, fHeight;

	if( bSmallSize == 1)
	{
		fWidth		= 256;
		fHeight		= 256;
	}
	else if( bSmallSize == -1)
	{
		fWidth		= (float)m_dwWidthSmall;
		fHeight		= (float)m_dwHeightSmall;
	}
	else
	{
		fWidth		= (float)m_dwWidth;
		fHeight		= (float)m_dwHeight;
	}

	// Draw the quad
	RVt_pos_tex_rhw vQuad[4];
	vQuad[0].vPos	= RVector4( -0.5f, -0.5f, 0.0f, 1.0f );
	vQuad[0].u		= fLeftU;
	vQuad[0].v		= fTopV;

	vQuad[1].vPos	= RVector4( -0.5f, fHeight, 0.0f, 1.0f );
	vQuad[1].u		= fLeftU;
	vQuad[1].v		= fBottomV;

	vQuad[2].vPos	= RVector4( fWidth, -0.5f, 0.0f, 1.0f );
	vQuad[2].u		= fRightU;
	vQuad[2].v		= fTopV;

	vQuad[3].vPos	= RVector4( fWidth, fHeight, 0.0f, 1.0f );
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
void RCameraPixelMotionBlurController::Render()
{
	// 활성화 시만 처리
	if( !m_bActive)
		return;

	FrameMove();

	// 2D 랜더를 위한 상태 설정
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 1, RTF_POINT );

	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetAlphaTestEnable(false);

	REngine::GetDevice().SetDepthEnable( false, false);

	REngine::GetDevice().SetTextureAddressClamp( 0);
	REngine::GetDevice().SetTextureAddressClamp( 1);

	RTechnique Technique;
	UINT uiPass, uiPassCount;

	RTexture* pTargetOrg = REngine::GetDevice().GetRenderTarget(0);
	
	Technique = m_pShader->GetTechnique("Blur");

	m_pShader->GetConstant("g_numSamples").SetInt( m_nSamplerCnt);
	switch( m_nImgSamCnt)
	{
		case 2:		m_pShader->GetConstant("g_fDevideVelocity").SetFloat( 0.6f); break;
		case 3:		m_pShader->GetConstant("g_fDevideVelocity").SetFloat( 0.45f); break;
		default:	m_pShader->GetConstant("g_fDevideVelocity").SetFloat( 1.0f/(float)m_nImgSamCnt);  break;
	}

	REngine::GetDevice().SetTexture(1, m_pVelocityBuffer);

	// nImgSamCnt은 최소 1 이상
	for( int i=0; i<m_nImgSamCnt; i++)
	{
		int nSrcIdx = i%2;
		int nDestIdx = 1 - (i%2);
		if( i == m_nImgSamCnt-1)
		{
			REngine::GetDevice().SetRenderTarget( 0, pTargetOrg);
		}
		else
		{
			REngine::GetDevice().SetRenderTarget( 0, m_pColorBuffer[ nDestIdx]);
		}
		REngine::GetDevice().SetTexture(0, m_pColorBuffer[ nSrcIdx]);
		Technique.Begin(&uiPassCount, 0);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			Technique.BeginPass(uiPass);

			DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f, false );

			Technique.EndPass();
		}
		Technique.End();
	}

	// 상태 돌려 놓기 ----------------------------------------------------------------------------------------------------------
	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR );
	REngine::GetDevice().SetTextureAddressWrap( 0);
	REngine::GetDevice().SetTextureAddressWrap( 1);
	REngine::GetDevice().SetTexture( 0, NULL);
	REngine::GetDevice().SetTexture( 1, NULL);
}


//-----------------------------------------------------------------------------
// Name: RenderOverlay()
// Desc: Visializes one of the surfaces in the overlay
//-----------------------------------------------------------------------------
void RCameraPixelMotionBlurController::RenderOverlay()
{
	// 활성화 시만 처리
	if( !m_bActive)
		return;

	RTechnique Technique = m_pShader->GetTechnique( "Overlay");

	REngine::GetDevice().SetTexture(0, m_pVelocityBuffer);

	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetAlphaTestEnable(false);

	REngine::GetDevice().SetDepthEnable( false, false);

	UINT uiPass, uiPassCount;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f, 1);

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetTexture(0, NULL);

}

void RCameraPixelMotionBlurController::OnRender()
{
	// 활성화 시만 처리
	if(!m_bActive)
		return;

	REngine::GetDevice().SetFogEnable(false);

	Render();

	//TESTOZ
	//RenderOverlay();

	// 속도 버퍼를 비워준다.
	REngine::GetDevice().ColorFill( m_pVelocityBuffer);
}


void RCameraPixelMotionBlurController::OnPostRender()
{
	// 활성화 시만 처리
	if(!m_bActive)
		return;

	REngine::GetDevice().SetRenderTarget( 0, m_pColorBufferOrg );

	// RT1에 관여하지 않으므로 NULL지정 했다가 돌려준다.
	RTexture* pRT1 = REngine::GetDevice().GetRenderTarget( 1);
	REngine::GetDevice().SetRenderTarget( 1, NULL);

	OnRender();

	// RT1을 돌려놓아서 다른 카메라 컨트롤러는 RT1 작동이 제대로 되게 한다.
	REngine::GetDevice().SetRenderTarget( 1, pRT1);

	m_bLockFlag = false;

}


// 랜더 타겟을 관리 - 090617, OZ
void RCameraPixelMotionBlurController::OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy)
{
	RCameraSceneNodeController::OnPreRender( pInferredRenderingStrategy);

	// 활성화 시만 처리
	if(!m_bActive)
		return;

	// 기존 RT 백업
	m_pColorBufferOrg = REngine::GetDevice().GetRenderTarget(0);
	// 새 RT 지정
	REngine::GetDevice().SetRenderTarget( 0, m_pColorBuffer[0] );

	// 랜더 타겟을 셋팅했다. 액티브 플래그가 변경 되면 안된다.
	m_bLockFlag = true;
}


void RCameraPixelMotionBlurController::OnRemoveFromCamera()
{
}

void RCameraPixelMotionBlurController::OnClear()
{
	REngine::GetDevice().Clear( true, true, 
		m_pCameraSceneNode->isHaveStencil(),
		m_pCameraSceneNode->GetClearColor(),
		m_pCameraSceneNode->GetClearDepth(),
		m_pCameraSceneNode->GetClearStencil() );
}

void RCameraPixelMotionBlurController::FrameMove()
{
}





}