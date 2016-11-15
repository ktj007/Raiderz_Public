#ifndef _RDEVICE_NULL_H
#define _RDEVICE_NULL_H

#include "RPreDefine.h"
#include "RDevice.h"
#include "RTextureManager.h"

namespace rs3 {

class RS_API RDeviceNull : public RDevice {

private:
	class RMockTexture : public RTexture
	{
//		virtual LOAD_RESULT Load() { return LOADED; }
	};

	virtual RTexture*	NewTexture()						override	{ return new RMockTexture; }

	virtual void OnFlip(){}
	virtual void OnSetViewport(const RViewport &){}

	virtual RShader* _CreateShader( char* vsShaderBuffer,char* psShaderBuffer
		, char *extraDefines = NULL,const int vsLine = 0,const int psLine = 0){ return NULL; }

	virtual void SetVertexBufferFreq( UINT stream, UINT FrequencyParameter) {}

public:
	RDeviceNull()
	{
		m_pTextureManager = new RTextureManager(*this);	// instance를 device에서 생성하고싶은데 template들때문에.. 후에 device로 옮겨가자.
	}

	virtual ~RDeviceNull()
	{
		delete m_pTextureManager;
	}

	virtual bool Create(HWND hWnd) { return true; }
	virtual void Destroy() {}
	virtual void InitDeviceDefault() {}
	virtual bool ResetDevice() { return true; }
	virtual void Flip() { IncreaseFrameCount(); }
	virtual bool QueryFeature(RQUERYFEATURETYPE feature) { return true; }
	virtual RDEVICESTATUS QueryStatus() { return RDEVICE_OK; }
	virtual bool BeginScene() { return true; }
	virtual void EndScene() {}
	virtual bool GenerateMipmapLevels(RTexture* pTexture) { return true; }
	virtual bool LockRect(RTexture* pTex, int nLevel, RLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) { return true; }
	virtual bool UnlockRect(RTexture* pTex, int nLevel) { return true; }
	virtual bool ColorFill(RTexture* pTexture, DWORD dwColor = 0) { return true; }
	virtual bool UpdateTexture(RTexture* pTexture, int nLevel, int nDestX, int nDestY, int nSrcWidth, int nSrcHeight, void *pSrcPixels) { return true; }
	virtual bool UpdateTexture(RTexture* pTexture, int nLevel, int nDestX, int nDestY, int nDestWidth, int nDestHeight, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight, void *pSrcPixels) { return true; }
	virtual HVERTEXBUFFER CreateVertexBuffer(const long size, int nVertexSize, const void *data = NULL, unsigned int flags = 0) { return 1;}
	virtual void DeleteVertexBuffer(HVERTEXBUFFER hVertexBuffer) {}
	virtual void *LockVertexBuffer(const HVERTEXBUFFER hVertexBuffer, const unsigned int flags = 0, const unsigned int nOffsetToLock = 0, const unsigned int nSizeToLock = 0) { return NULL; }
	virtual bool UnlockVertexBuffer(const HVERTEXBUFFER hVertexBuffer) { return true; }
	virtual void Clear(bool bTarget = true, bool bDepth = true, bool bStencil = false, DWORD dwColor = 0, float fDepth = 1.f, DWORD dwStencil = 0, DWORD nIdx = 0) {}
	virtual RTexture* SetDepthStencilBuffer(RTexture* pTexture) { return NULL; }
	virtual RTexture* GetDepthStencilBuffer() { return NULL; }
	virtual RTexture* SetRenderTarget(unsigned int nRenderTargetIndex, RTexture* pTexture, int nSurface = 0) { return NULL; }
	virtual RTexture* GetRenderTarget(unsigned int nRenderTargetIndex) { return NULL; }
	virtual HVERTEXFORMAT CreateVertexFormat(const RVertexAttribute *attribs, const int nAttribs, const int nStream = 0, const int nStartTexCoord = 0) override { return 0; }
	virtual HVERTEXFORMAT CreateVertexFormat(const RVertexAttributeEx *attribs, const int nAttribs) override { return 0; }
	virtual HINDEXBUFFER CreateIndexBuffer(const unsigned int nIndices, RFORMAT inf = RFMT_INDEX16 , const void *data = NULL, unsigned int flags = 0) { return 1; }
	virtual void DeleteIndexBuffer(HINDEXBUFFER hIndexBuffer) {}
	virtual void *LockIndexBuffer(const HINDEXBUFFER hIndexBuffer, const unsigned int flags = 0, const unsigned int nOffsetToLock = 0, const unsigned int nSizeToLock = 0) { return NULL; }
	virtual bool UnlockIndexBuffer(const HINDEXBUFFER hIndexBuffer) { return true; }
	virtual void SetVertexBuffer(const HVERTEXBUFFER hVertexBuffer, const int stream, const unsigned int offset) {}
	virtual void SetVertexFormat(HVERTEXFORMAT hVertexFormat) {}
	virtual void SetupForRenderInstancing(const HVERTEXBUFFER hVertexBuffer, UINT nCnt, unsigned int offset) {};
	virtual void RestoreForRenderInstancing() {};
	virtual void SetIndexBuffer(HINDEXBUFFER hIndexBuffer) {}
	virtual void SetFvF(DWORD fvf) {}
	virtual void ShaderOff() {}
	virtual RShaderFX* CreateShaderFX() { return NULL; }
	virtual RShaderFX* CreateShaderFX(const string& stFilename, const vector<string>& vShaderDefines = vector<string>(0),RShaderFXPool *pShaderPool = NULL) { return NULL; }
	virtual void DeleteShaderFX(RShaderFX* shader) {}
	virtual void SetTexture(int nStage,RTexture* pTexture) {}
	virtual void SetTextureStageState(int nStage, RTEXTURESTAGESTATETYPE nStageStateType, unsigned int value) {}
	virtual void SetTextureFilter(int nSampler, RTEXTUREFILTERTYPE type) {}
	virtual void SetTextureMipmapLodBias(int nStage,float fBias) {}
	virtual void SetTextureMaxAnisotropy(int nStage, DWORD dwValue) {}
	virtual void SetTextureAddress(int nSampler, RTEXTUREADDRESS u, RTEXTUREADDRESS v, RTEXTUREADDRESS w) {}
	virtual void SetTextureBorderColor(int nSampler, const DWORD dwColor) {}
	virtual void SetTransform(const RSETTRANSFORMTYPE type, const RMatrix &matrix) {}
	virtual RMatrix GetTransform(const RSETTRANSFORMTYPE type) const { return RMatrix::IDENTITY; }
	virtual bool DrawIndexedPrimitiveUP(RPRIMITIVETYPE primitiveType, unsigned int nMinVertexIndex, unsigned int nNumVertices, unsigned int nPrimitiveCount, 
		const void* pIndexData, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride, RFORMAT indexFmt = RFMT_INDEX16 ) { return true; }
	virtual bool DrawIndexedPrimitive(RPRIMITIVETYPE primitiveType, unsigned int nBaseVertexIndex, unsigned int nMinIndex, unsigned int nNumVertices,
		unsigned int nStartIndex, unsigned int nPrimitiveCount) { return true; }
	virtual bool DrawPrimitiveUP(RPRIMITIVETYPE primitiveType, unsigned int nPrimitiveCount, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride ) {return true;}
	virtual bool DrawPrimitive(RPRIMITIVETYPE primitiveType, unsigned int nStart, unsigned int nPrimitiveCount) { return true; }
	virtual void SetBlending(RBLEND srcFactor, RBLEND dstFactor = RBLEND_NONE, RBLENDOP blendMode = RBLENDOP_ADD) {}
	virtual void SetSeparateBlending(RBLEND srcFactor, RBLEND dstFactor = RBLEND_NONE, RBLENDOP blendMode = RBLENDOP_ADD) {}
	virtual void SetBlendFactor(const DWORD dwColor) {}
	virtual void SetTextureFactor(const DWORD dwColor) {}
	virtual void SetDepthFunc(RCMPFUNC depthFunc) {}
	virtual void SetDepthEnable(bool bEnable, bool bWriteEnable = true) {}
	virtual void SetColorWriteEnable(bool bEnable) {}
	virtual void SetCullMode(RCULL cullMode) {}
	virtual void SetFillMode(RFILLMODE fillMode) {}
	virtual void GetFillMode( DWORD& _dwMode) {}
	virtual void SetClipPlanes(const RPlane *pPlanes, int nCount) {}
	virtual void SetDepthBias(float fDepthBias = 0, float fSlopeScaleDepthBias = 0) {}
	virtual void SetStencilEnable(bool bEnable) {}
	virtual void SetStencilTwoSide(bool bEnable) {}
	virtual void SetStencilRef(DWORD dwValue) {}
	virtual void SetStencilMask(DWORD dwValue) {}
	virtual void SetStencilWriteMask(DWORD dwValue) {}
	virtual void SetStencilFunc(RCMPFUNC stencilFunc) {}
	virtual void SetStencilOp(RSTENCILOP opPass,RSTENCILOP opFail,RSTENCILOP opZFail) {}
	virtual void SetStencilCCWFunc(RCMPFUNC stencilFunc) {}
	virtual void SetStencilCCWOp(RSTENCILOP opPass,RSTENCILOP opFail,RSTENCILOP opZFail) {}
	virtual void SetLight(int index,void* pLight) {}
	virtual void SetLightEnable(int index,bool bEnable) {}
	virtual void SetLighting(bool bEnable) {}
	virtual void SetMaterial(void* pMtrl) {}
	virtual void SetNormalizeNormals(bool bEnable) {}
	virtual void SetAmbient(DWORD dwColor) {}
	virtual void SetAlphaTestEnable(bool bEnable) {}
	virtual void SetAlphaRef(DWORD dwRef) {}
	virtual void SetAlphaFunc(RCMPFUNC Func) {}
	virtual bool GetAlphaTestEnable() { return true; }
	virtual DWORD GetAlphaRef() { return 0; }
	virtual RCMPFUNC GetAlphaFunc() { return (RCMPFUNC)0; }
	virtual void SetClipping(bool bEnable) {}
	virtual void SetScissorTestEnable(bool bEnable) {}
	virtual void SetScissorRect(const RECT* pRect) {}
	virtual void SetPointSize(DWORD nSize) {}
	virtual void SetFogEnable(bool bEnable) {}
	virtual void SetFog(bool bLinearFog, DWORD dwColor, float fNear, float fFar, float fDensity,
		bool bPixelFog, bool bRangeFog) {}
	virtual void SetFogColor(DWORD dwColor) {}
	virtual void SetFogVertexMode(bool bPixelFog, bool bRangeFog) {}
	virtual void SetVertexBlendEnable(bool bEnable) {}
	virtual void SetIndexedVertexBlendEnable(bool bEnable) {}
	virtual void SetSpecularEnable(bool bEnable) {}
	virtual void SetColorVertex(bool bEnable) {}

	virtual void SetPointSpriteEnable(bool bEnable){}
	virtual void SetPointSizeAndScale( float fSize, float fMin, float fMax, bool bScaleEnable, float fScaleA, float fScaleB, float fScaleC){}
	virtual unsigned int GetVertexBufferMemoryUsed(){ return 0; }

	virtual RTexture* CreateRenderTargetTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter, DWORD flags ){ return new RMockTexture; }
	virtual RTexture* CreateRenderTargetDepthStencilTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter, DWORD flags ){ return new RMockTexture; }
	virtual RTexture* CreateNormalMapFromHeightMap(RTexture* hHeightMap, float fAttitude){ return NULL; }

	virtual bool IsCurrentDisplayModeSupportFullScreenMode() { return false; }

	//Query
	virtual	ROcclusionQuery*	CreateOcclusionQuery(void){ return NULL; }
	virtual void				ReleaseOcclusionQuery(ROcclusionQuery* pQuery){}
	virtual REventQuery*		CreateEventQuery(void){	return NULL; }
	virtual void				ReleaseEventQuery(REventQuery* pEventQuery){}
	virtual RD3DQueryManager*	GetQueryManager(void) { return NULL; }

	// Profile Marker
	virtual void				SetProfileMarker( LPCWSTR wszName) {}
	virtual void				BeginProfileMarker( LPCWSTR wszName) {}
	virtual void				EndProfileMarker() {}
};


} // namespace rs3

#endif
