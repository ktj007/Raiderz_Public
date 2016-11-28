#include "stdafx.h"
#include "windows.h"
#include "RDevice.h"
#include "mmsystem.h"
#include "RTypes.h"
#include "MDebug.h"
#include "RResource.h"
#include "MFileSystem.h"
#include "REngine.h"
#include "RTextureManager.h"

namespace rs3 {

RDevice::RDevice() : m_nFrameCount(1), m_pDefaultTexture(NULL), m_pDefaultNoiseTexture(NULL)
{
}

void RDevice::InitDeviceDefault()
{
	// 디바이스마다 기본값이 다르다면 오버라이드
	currentRenderTarget[0] = RT_FRAMEBUFFER;
	currentDepthStencilBuffer = RT_FRAMEBUFFER;

	for ( int i = 1; i<MAX_RENDERTARGET; ++i )
	{
		currentRenderTarget[i] = R_NONE;
		currentRenderTargetSurface[i] = 0;
	}

	currentVertexFormat = R_NONE;
	memset(currentOffset, 0, sizeof(currentOffset));
	memset(currentFrequencyParameter, 0, sizeof(currentOffset));
	currentIndexBuffer = R_NONE;
	currentInstanceCount = 1;

	// alpha blend
	currentSrcFactor = RBLEND_ONE;		/// D3DRS_SRCBLEND : One member of the D3DBLEND enumerated type. The default value is D3DBLEND_ONE.
	currentDstFactor = RBLEND_ZERO;
	currentBlendMode = RBLENDOP_ADD;
	currentBlendEnable = false;
	// separate alpha
	currentSeparateSrcFactor = RBLEND_ONE;
	currentSeparateDstFactor = RBLEND_ZERO;
	currentSeparateBlendMode = RBLENDOP_ADD;
	currentSeparateBlendEnable = false;

	currentDepthFunc = RCMP_LESSEQUAL;
	currentDepthTestEnable = true;
	currentDepthWriteEnable = true;
	currentColorWriteEnable = true;
	currentCullMode = RCULL_CCW;
	currentStencilEnable = false;
	currentStencilFunc = RCMP_ALWAYS;		/// D3DRS_STENCILFUNC : Comparison function for the stencil test. Values are from the D3DCMPFUNC enumerated type. The default value is D3DCMP_ALWAYS. 
	currentStencilRef = 0;					/// D3DRS_STENCILREF : An int reference value for the stencil test. The default value is 0. 
	currentStencilMask = 0xFFFFFFFF;		/// D3DRS_STENCILMASK : Mask applied to the reference value and each stencil buffer entry to determine the significant bits for the stencil test. The default mask is 0xFFFFFFFF.
	currentStencilBackFunc = RCMP_ALWAYS;
	currentClipPlaneEnable = false;
	currentAlphaTestEnable = false;
	currentAlphaFunc = RCMP_ALWAYS;
	currentAlphaRef = 0;
	currentLighting = true;
	currentNormalizeNormals = false;
	currentAmbientColor = 0;
	currentClipping = true;
	currentScissorTestEnable = false;
	currentFog = false;
	currentVertexBlendEnable = false;
	currentIndexedVertexBlendEnable = false;
	currentSpecularEnable = false;
	currentColorVertexEnable = true;
	currentFillMode = RFILL_SOLID;
	currentStencilPass = RSTENCILOP_KEEP;	/// D3DRS_STENCILPASS : Stencil operation to perform if both the stencil and the depth (z) tests pass. Values are from the D3DSTENCILOP enumerated type. The default value is D3DSTENCILOP_KEEP.
	currentStencilFail = RSTENCILOP_KEEP;	/// D3DRS_STENCILFAIL : Stencil operation to perform if the stencil test fails. Values are from the D3DSTENCILOP enumerated type. The default value is D3DSTENCILOP_KEEP. 
	currentStencilZFail = RSTENCILOP_KEEP;	/// D3DRS_STENCILZFAIL : Stencil operation to perform if the stencil test passes and the depth test (z-test) fails. Values are from the D3DSTENCILOP enumerated type. The default value is D3DSTENCILOP_KEEP.
	currentDepthBias = 0;					/// D3DRS_DEPTHBIAS : A floating-point value that is used for comparison of depth values. See Depth Bias (Direct3D 9). The default value is 0.
	currentSlopeScaleDepthBias = 0;			/// D3DRS_SLOPESCALEDEPTHBIAS : Used to determine how much bias can be applied to co-planar primitives to reduce z-fighting. The default value is 0.

	for(int i=0;i<MAX_IMAGEUNIT;i++) {
		currentTextures[i]=R_NONE;
		currentTextureFilter[i]=RTF_POINT;
		currentTextureAddress[i][0]= RTADDRESS_WRAP;
		currentTextureAddress[i][1]= RTADDRESS_WRAP;
		currentTextureAddress[i][2]= RTADDRESS_WRAP;
		currentMaxAnisotropy[i] = 1;	/// D3DSAMP_MAXANISOTROPY : DWORD maximum anisotropy. The default value is 1. 
		currentMipmaplodBias[i] = 0.0f;
		currentTextureBorderColor[i] = 0;	/// D3DSAMP_BORDERCOLOR : Border color or type D3DCOLOR. The default color is 0x00000000. 

		currentTextureStageSettings[i][RTSS_ALPHAOP] = RTOP_DISABLE;	/// D3DTSS_COLOROP : Texture-stage state is a texture color blending operation identified by one member of the D3DTEXTUREOP enumerated type. The default value for the first texture stage (stage 0) is D3DTOP_MODULATE; for all other stages the default is D3DTOP_DISABLE. 
		currentTextureStageSettings[i][RTSS_COLOROP] = RTOP_DISABLE;	/// D3DTSS_ALPHAOP : Texture-stage state is a texture alpha blending operation identified by one member of the D3DTEXTUREOP enumerated type. The default value for the first texture stage (stage 0) is D3DTOP_SELECTARG1, and for all other stages the default is D3DTOP_DISABLE. 
		currentTextureStageSettings[i][RTSS_ALPHAARG1] = RTA_NOT_INITIALIZED;	/// D3DTSS_ALPHAARG1 : 텍스쳐 셋팅 여부에 따라 기본 값이 다르다. 그러므로 유효하지 않은 기본 값을 셋팅. 최초의 중복 설정은 어쩔 수 없다. Texture-stage state is the first alpha argument for the stage, identified by by D3DTA. The default argument is D3DTA_TEXTURE. If no texture is set for this stage, the default argument is D3DTA_DIFFUSE. 
		currentTextureStageSettings[i][RTSS_ALPHAARG2] = RTA_CURRENT;			/// D3DTSS_ALPHAARG2 : Texture-stage state is the second alpha argument for the stage, identified by by D3DTA. The default argument is D3DTA_CURRENT. 
		currentTextureStageSettings[i][RTSS_COLORARG1] = RTA_TEXTURE;			/// D3DTSS_COLORARG1 : Texture-stage state is the first color argument for the stage, identified by one of the D3DTA. The default argument is D3DTA_TEXTURE. 
		currentTextureStageSettings[i][RTSS_COLORARG2] = RTA_CURRENT;			/// D3DTSS_COLORARG2 : Texture-stage state is the second color argument for the stage, identified by D3DTA. The default argument is D3DTA_CURRENT.
	}

	/// 첫번째 스테이지의 기본 값은 다른 스테이지의 값과 다르다.
	currentTextureStageSettings[0][RTSS_ALPHAOP] = RTOP_SELECTARG1;		/// D3DTSS_ALPHAOP : Texture-stage state is a texture alpha blending operation identified by one member of the D3DTEXTUREOP enumerated type. The default value for the first texture stage (stage 0) is D3DTOP_SELECTARG1, and for all other stages the default is D3DTOP_DISABLE. 
	currentTextureStageSettings[0][RTSS_COLOROP] = RTOP_MODULATE;		/// D3DTSS_COLOROP : Texture-stage state is a texture color blending operation identified by one member of the D3DTEXTUREOP enumerated type. The default value for the first texture stage (stage 0) is D3DTOP_MODULATE; for all other stages the default is D3DTOP_DISABLE. 

	for(int i=0;i<MAX_VERTEXSTREAM;i++) {
		currentVertexBuffer[i]=R_NONE;
	}

	for(int i=0;i<RST_MAX;i++)
		currentTransform[i].MakeIdentity();

	m_dwLastFPSTime = timeGetTime();
	m_nLastFPSFrameCount = 0;
	m_fFPS = 0;
	m_dwLastFlipTime = m_dwLastFPSTime;
	m_dwLastElapsedTime = 0;
	m_bCursor = false;

	REngine::GetResourceProfiler().Reset();

	OnResetDevice();
}

#define FPS_INTERVAL	1000

void RDevice::Flip()
{
	OnFlip();

	IncreaseFrameCount();

	DWORD currentTime=timeGetTime();
	m_dwLastElapsedTime = currentTime - m_dwLastFlipTime;
	if(m_dwLastFPSTime+FPS_INTERVAL<currentTime)
	{
		m_fFPS=(m_nFrameCount-m_nLastFPSFrameCount)*FPS_INTERVAL/((float)(currentTime-m_dwLastFPSTime)*(FPS_INTERVAL/1000));
		m_dwLastFPSTime=currentTime;
		m_nLastFPSFrameCount=m_nFrameCount;
	}

	m_dwLastFlipTime = currentTime;

	REngine::GetResourceProfiler().Reset();
	REngine::GetResourceProfiler().UpdateTimeQueue("FrameTime", m_dwLastElapsedTime );
	REngine::GetResourceProfiler().UpdateTimeProfileNodes();

	m_deviceProfileInfoLast = m_deviceProfileInfoCurrent;
	m_deviceProfileInfoCurrent.Reset();
}

void RDevice::SetViewport(const RViewport &viewport)
{
	m_viewport = viewport;
	OnSetViewport(viewport);
}

void RDevice::SetViewport(int x, int y, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	SetViewport(RViewport(x,y,nWidth,nHeight,fMinZ,fMaxZ));
}


RTexture* RDevice::CreateTextureForBuildLater(const char *fileName, const RTEXTUREFILTERTYPE filter,
											  DWORD flags, RFORMAT pf, bool bBackground, RResource::LoadingController* pController)
{
	return m_pTextureManager->CreateTextureForBuildLater(fileName, filter, flags, pf, bBackground, pController);
}

RTexture* RDevice::CreateTexture(const char *fileName, const RTEXTUREFILTERTYPE filter, DWORD flags, RFORMAT pf, bool bBackground, RResource::LoadingController* pController)
{
	return m_pTextureManager->CreateTexture(fileName, filter, flags, pf, bBackground, pController);
}

RTexture* RDevice::CreateTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter, DWORD flags, void *pixels, const char* szName)
{
	return m_pTextureManager->CreateTexture(nWidth, nHeight, pf, filter, flags, pixels, szName);
}

RTexture* RDevice::CreateRenderTargetTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter, DWORD flags )
{
	return m_pTextureManager->CreateRenderTargetTexture(nWidth, nHeight, pf, filter, flags);
}

RTexture* RDevice::CreateRenderTargetDepthStencilTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter, DWORD flags )
{
	return m_pTextureManager->CreateRenderTargetTexture(nWidth, nHeight, pf, filter, flags | RCT_DEPTHSTENCIL);
}

void RDevice::DeleteTexture(RTexture* pTexture)
{
	if(pTexture == R_NONE) return;

	if(m_pTextureManager->Get(pTexture)->GetRefCount()==1)
	{
		// 삭제될 텍스쳐가 현재 선택된것이라면, 해제한다
		for(int i=0;i<MAX_IMAGEUNIT;i++) 
		{
			if(currentTextures[i] == pTexture) {
				int nStage = GetSaveIndexToSamplerNumber(i);
				SetTexture(nStage,R_NONE);
			}
		}
	}

	m_pTextureManager->DeleteTexture(pTexture);
}

void RDevice::SafeDeleteTexture(RTexture*& pTexture)
{
	if( pTexture != NULL )
	{
		DeleteTexture( pTexture );
		pTexture = NULL;
	}
}


RTexture* RDevice::GetTexture( const char* pSzTextureName )
{
	return m_pTextureManager->GetTexture(pSzTextureName);
}

unsigned int RDevice::GetTextureMemoryUsed()
{
	return m_pTextureManager->GetUsingVideoMemory();
}

const RTextureInfo *RDevice::GetTextureInfo(RTexture* pTexture)
{
	return &pTexture->info;
}

RImage* RDevice::CreateImage(int nWidth, int nHeight, RFORMAT format, void* pSrcPixels, int nSourcePitch, int nSrcX, int nSrcY)
{
	return NULL;
}

void RDevice::DeleteImage(RImage* pImage) 
{
	delete pImage;
}

bool RDevice::IsCursorVisible()
{
	return m_bCursor;
}

bool RDevice::ShowCursor(bool bShow)
{
	OnShowCursor(bShow);
	bool bPrevious = m_bCursor;
	m_bCursor = bShow;
	return bPrevious;
}

bool RDevice::SetCursorProperties(int XHotSpot, int YHotSpot, RImage* pImage)
{
	return false;
}

}