#ifndef _RDEVICE_D3D_H
#define _RDEVICE_D3D_H

#include "RPreDefine.h"
#include "RDevice.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "RD3DQuery.h"

namespace rs3 {

struct RTextureFilterParams {
	D3DTEXTUREFILTERTYPE minFilter;
	D3DTEXTUREFILTERTYPE magFilter;
	D3DTEXTUREFILTERTYPE mipFilter;
	D3DTEXTUREADDRESS wrapS;
	D3DTEXTUREADDRESS wrapT;
	D3DTEXTUREADDRESS wrapR;
};

class RFont;
class RTextureD3D;
class RShaderFX;
class RShaderFXPool;
class RShaderConstant;

class RS_API RDeviceD3D : public RDevice {

	RTextureD3D* GetTexture(RTexture* pTexture);
	friend class RShaderConstant;
	RShader*	GetShader(HSHADER hShader);

	HMODULE					m_hd3dLibrary;
	LPDIRECT3D9				m_pd3d;
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	D3DADAPTER_IDENTIFIER9	m_deviceID;

	D3DFORMAT				m_d3dPixelFormat;
	D3DPRESENT_PARAMETERS	m_d3dpp; 
	D3DCAPS9				m_d3dcaps;
	D3DDISPLAYMODE			m_d3ddm;
	D3DVIEWPORT9			m_d3dViewport;

	LPDIRECT3DSURFACE9		m_pFrameBuffer;
	LPDIRECT3DSURFACE9		m_pDepthStencilBuffer;
	HWND					m_hWnd;

	bool	m_bSupportVS;

	int		m_nPicmip;

	bool CreateDirect3D9();

	list<RFont*>			m_fonts;

	MHandle<RVertexFormat>	m_VertexFormats;
	MHandle<RVertexBuffer>	m_VertexBuffers;
	MHandle<RIndexBuffer>	m_IndexBuffers;

	//쿼리매니저
	RD3DQueryManager		m_QueryMgr;

	// default texture ( 1x1 no-mip map )
//	LPDIRECT3DTEXTURE9		m_lpDefaultTexture;

	DWORD					m_currentFVF;
	bool					m_bInRenderLoop;

private:
	// helpers
	void	InitPresentParameters();
	HRESULT CheckResourceFormat(D3DFORMAT fmt, D3DRESOURCETYPE resType, DWORD dwUsage);

protected:
	virtual RTexture* NewTexture() override;

	void		OnLostDevice();
	void		OnRestoreDevice();

	virtual void OnFlip();
	virtual void OnSetViewport(const RViewport &);

	virtual void OnShowCursor(bool bShow);

	virtual void SetVertexBufferFreq( UINT stream, UINT FrequencyParameter);

public:
	RDeviceD3D();
	virtual ~RDeviceD3D();

	virtual bool Create(HWND hWnd);
	virtual void Destroy();			// 가급적이면 명시적으로 호출하는 편이 좋을 것 같습니다. 소멸자에서 호출하게 되면 refCount 체크에 문제가 발생.

	virtual bool			QueryFeature(RQUERYFEATURETYPE feature);
	virtual RDEVICESTATUS	QueryStatus();
	virtual unsigned int	GetVertexBufferMemoryUsed();

	virtual bool			ResetDevice();

	bool BeginScene();
	void EndScene();

	void RegisterFont(RFont* pFont);	// lost 관리를 위해 등록한다
	void UnregisterFont(RFont* pFont);

	virtual void Clear(bool bTarget = true, bool bDepth = true, bool bStencil = false, DWORD dwColor = 0, float fDepth = 1.f, DWORD dwStencil = 0, DWORD nIdx = 0);

	//////////////////////////////////////////////////////////////////////////
	// Render Target / Depth Buffer
	virtual RTexture* SetRenderTarget(unsigned int nRenderTargetIndex, RTexture* pTexture, int nSurface = 0);
	virtual RTexture* GetRenderTarget(unsigned int nRenderTargetIndex);

	virtual RTexture* SetDepthStencilBuffer(RTexture* pTexture);
	virtual RTexture* GetDepthStencilBuffer();

	//////////////////////////////////////////////////////////////////////////
	// Vertex Format
	virtual HVERTEXFORMAT	CreateVertexFormat(const RVertexAttribute *attribs, const int nAttribs, const int nStream = 0, const int nStartTexCoord = 0) override;
	virtual HVERTEXFORMAT	CreateVertexFormat(const RVertexAttributeEx *attribs, const int nAttribs) override;
	virtual void			SetFvF(DWORD fvf);

	//////////////////////////////////////////////////////////////////////////
	// 텍스쳐 관련
	virtual bool GenerateMipmapLevels(RTexture* pTexture);

	virtual bool LockRect(RTexture* pTex, int nLevel, RLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
	virtual bool UnlockRect(RTexture* pTex, int nLevel);
	virtual bool UpdateTexture(RTexture* pTexture,int nLevel, int nDestX, int nDestY, int nSrcWidth, int nSrcHeight, void *pSrcPixels);
	virtual bool UpdateTexture(RTexture* pTexture, int nLevel, int nDestX, int nDestY, int nDestWidth, int nDestHeight, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight, void *pSrcPixels);

	virtual bool ColorFill(RTexture* pTexture, DWORD dwColor = 0);

	/// 렌더타겟 텍스쳐 복사. 필요할때에 CreateCopyOfTexture 로 일반화할수 있을듯합니다.
	void		CopyRenderTargetTexture( RTexture* pSrcTexture, RTexture* pDestTexture );

	virtual RTexture* CreateNormalMapFromHeightMap(RTexture* hHeightMap, float fAttitude);

	//////////////////////////////////////////////////////////////////////////
	// Vertex Buffer
	virtual HVERTEXBUFFER CreateVertexBuffer(const long size, int nVertexSize, const void *data = NULL, unsigned int flags = 0);
	virtual void DeleteVertexBuffer(HVERTEXBUFFER hVertexBuffer);
	virtual void *LockVertexBuffer(const HVERTEXBUFFER hVertexBuffer, const unsigned int flags = 0, const unsigned int nOffsetToLock = 0, const unsigned int nSizeToLock = 0);
	virtual bool UnlockVertexBuffer(const HVERTEXBUFFER hVertexBuffer);

	//////////////////////////////////////////////////////////////////////////
	// Index Buffer
	virtual HINDEXBUFFER CreateIndexBuffer(const unsigned int nIndices, RFORMAT inf = RFMT_INDEX16, const void *data = NULL, unsigned int flags = 0);
	virtual void DeleteIndexBuffer(HINDEXBUFFER hIndexBuffer);
	virtual void *LockIndexBuffer(const HINDEXBUFFER hIndexBuffer, const unsigned int flags = 0, const unsigned int nOffsetToLock = 0, const unsigned int nSizeToLock = 0);
	virtual bool UnlockIndexBuffer(const HINDEXBUFFER hIndexBuffer);

	//////////////////////////////////////////////////////////////////////////
	// DrawPrimitives
	bool DrawIndexedPrimitiveUP(RPRIMITIVETYPE primitiveType, unsigned int nMinVertexIndex, unsigned int nNumVertices, unsigned int nPrimitiveCount, 
		const void* pIndexData, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride, RFORMAT indexFmt = RFMT_INDEX16 );
	bool DrawIndexedPrimitive(RPRIMITIVETYPE primitiveType, unsigned int nBaseVertexIndex, unsigned int nMinIndex, unsigned int nNumVertices,
		unsigned int nStartIndex, unsigned int nPrimitiveCount);
	bool DrawPrimitiveUP(RPRIMITIVETYPE primitiveType, unsigned int nPrimitiveCount, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride );
	bool DrawPrimitive(RPRIMITIVETYPE primitiveType, unsigned int nStart, unsigned int nPrimitiveCount);

	//////////////////////////////////////////////////////////////////////////
	// Shader 관련
	virtual void ShaderOff();

	//////////////////////////////////////////////////////////////////////////
	// ShaderFX
//	virtual RShaderFX*	CreateShaderFXFromString(const string& strShader, const vector<string>& vShaderDefines = vector<string>(0), RShaderFXPool *pShaderPool = NULL);
	virtual RShaderFX*	CreateShaderFX();
	virtual RShaderFX*	CreateShaderFX(const string& strFilename, const vector<string>& vShaderDefines = vector<string>(0), RShaderFXPool *pShaderPool = NULL);
	virtual void		DeleteShaderFX(RShaderFX* shader);

	//////////////////////////////////////////////////////////////////////////
	// 스테이트 관련

	virtual void SetVertexBuffer(const HVERTEXBUFFER hVertexBuffer, const int stream, const unsigned int offset);
	virtual void SetVertexFormat(HVERTEXFORMAT hVertexFormat);

	virtual void SetupForRenderInstancing(const HVERTEXBUFFER hVertexBuffer, UINT nCnt, unsigned int offset);
	virtual void RestoreForRenderInstancing();

	virtual void SetIndexBuffer(HINDEXBUFFER hIndexBuffer);

	virtual void SetTexture(int nStage,RTexture* pTexture);
	virtual void SetTextureStageState(int nStage, RTEXTURESTAGESTATETYPE nStageStateType, unsigned int value);
	virtual void SetTextureFilter(int nSampler, RTEXTUREFILTERTYPE type);
	virtual void SetTextureMipmapLodBias(int nStage, float fBias);
	virtual void SetTextureMaxAnisotropy(int nStage, DWORD dwValue);
	virtual void SetTextureAddress(int nSampler, RTEXTUREADDRESS u, RTEXTUREADDRESS v, RTEXTUREADDRESS w);
	virtual void SetTextureBorderColor(int nSampler, const DWORD dwColor);

	virtual void SetTransform(const RSETTRANSFORMTYPE type, const RMatrix &matrix);
	virtual RMatrix GetTransform(const RSETTRANSFORMTYPE type) const;

	// blending
	virtual void SetBlending(RBLEND srcFactor, RBLEND dstFactor, RBLENDOP blendMode = RBLENDOP_ADD);
	virtual void SetSeparateBlending(RBLEND srcFactor, RBLEND dstFactor, RBLENDOP blendMode = RBLENDOP_ADD);
	virtual void SetBlendFactor(const DWORD dwColor);
	virtual void SetTextureFactor(const DWORD dwColor);

	// alpha test
	virtual void SetAlphaTestEnable(bool bEnable);
	virtual void SetAlphaRef(DWORD dwRef);
	virtual void SetAlphaFunc(RCMPFUNC Func);
	virtual bool		GetAlphaTestEnable() { return currentAlphaTestEnable; }
	virtual DWORD		GetAlphaRef() { return currentAlphaRef; }
	virtual RCMPFUNC	GetAlphaFunc() { return currentAlphaFunc; }


	virtual void SetDepthFunc(RCMPFUNC depthFunc);
	virtual void SetDepthEnable(bool bEnable, bool bWriteEnable = true);
	virtual void SetColorWriteEnable(bool bEnable);
	virtual void SetCullMode(RCULL cullMode);
	virtual void SetFillMode(RFILLMODE fillMode);

	virtual void SetClipPlanes(const RPlane *pPlanes, int nCount);
	virtual void SetDepthBias(float fDepthBias = 0, float fSlopeScaleDepthBias = 0);

	// stencil
	virtual void SetStencilEnable(bool bEnable);
	virtual void SetStencilTwoSide(bool bEnable);
	virtual void SetStencilRef(DWORD dwValue);
	virtual void SetStencilMask(DWORD dwValue);
	virtual void SetStencilWriteMask(DWORD dwValue);
	virtual void SetStencilFunc(RCMPFUNC stencilFunc);
	virtual void SetStencilOp(RSTENCILOP opPass,RSTENCILOP opFail,RSTENCILOP opZFail);
	virtual void SetStencilCCWFunc(RCMPFUNC stencilFunc);	// twoside의 뒷면
	virtual void SetStencilCCWOp(RSTENCILOP opPass,RSTENCILOP opFail,RSTENCILOP opZFail);

	virtual void SetPointSpriteEnable(bool bEnable);
	virtual void SetPointSizeAndScale( float fSize, float fMin, float fMax, bool bScaleEnable, float fScaleA = 1.0f, float fScaleB = 1.0f, float fScaleC = 1.0f);
	
	// light
	virtual void SetLight(int index,void* pLight);
	virtual void SetLightEnable(int index,bool bEnable);
	virtual void SetLighting(bool bEnable);
	
	// material 
	virtual void SetMaterial(void* pMtrl);

	// fog
	virtual void SetFogEnable(bool bEnable);
	virtual void SetFog(bool bLinearFog, DWORD dwColor, float fNear, float fFar, float fDensity,
		bool bPixelFog, bool bRangeFog);
	virtual void SetFogColor(DWORD dwColor);
	virtual void SetFogVertexMode(bool bPixelFog, bool bRangeFog);

	// occlusion query
	virtual	ROcclusionQuery*	CreateOcclusionQuery(void);
	virtual void				ReleaseOcclusionQuery(ROcclusionQuery* pQuery);

	// Event Query
	virtual REventQuery*		CreateEventQuery(void);
	virtual void				ReleaseEventQuery(REventQuery* pEventQuery);

	virtual RD3DQueryManager*	GetQueryManager(void) { return &m_QueryMgr; }

	// Profile Marker
	virtual void				SetProfileMarker( LPCWSTR wszName);
	virtual void				BeginProfileMarker( LPCWSTR wszName);
	virtual void				EndProfileMarker();

	// etc
	virtual void SetNormalizeNormals(bool bEnable);
	virtual void SetAmbient(DWORD dwColor);
	virtual void SetClipping(bool bEnable);

	virtual void SetScissorTestEnable(bool bEnable);
	virtual void SetScissorRect( const RECT* pRect );

	virtual void SetPointSize(DWORD nSize);

	virtual void SetVertexBlendEnable(bool bEnable);
	virtual void SetIndexedVertexBlendEnable(bool bEnable);
	virtual void SetSpecularEnable(bool bEnable);

	virtual void SetColorVertex(bool bEnable);
	static RFORMAT GetPixelFormat(D3DFORMAT Format);
	static DWORD GetVideoMemorySize();

	virtual bool IsCurrentDisplayModeSupportFullScreenMode();

	/// 스크린샷 저장
	virtual bool SaveScreenShot( const char* szFileName ) override;
	virtual RImage* GetSaveScreenImage() override;

	RImage* CreateImage(int nWidth, int nHeight, RFORMAT format, void* pSrcPixels, int nSourcePitch, int nSrcX, int nSrcY);
	bool SetCursorProperties(int XHotSpot, int YHotSpot, RImage* pImage);

	//////////////////////////////////////////////////////////////////////////
	// TODO: 앞으로 제거 해야할 d3d 직접쓴 부분들

	LPDIRECT3DBASETEXTURE9 GetD3DTexture(RTexture* pTexture);
	LPDIRECT3DSURFACE9 GetD3DDepthSurface(RTexture* hDepth);
	LPDIRECT3DSURFACE9 GetD3DRenderTargetSurface(RTexture* pTexture);

	//////////////////////////////////////////////////////////////////////////
	// 외부엔진과 연결할때 필요한 함수들,

	// 아래의 함수로 얻어서 타입캐스팅해서 사용하세요
	// deprecated되었었지만, 엔진 외부에서 필요에 의해 다시 사용하게 되었습니다. - birdkr
	LPDIRECT3DDEVICE9 GetD3DDevice() { return m_pd3dDevice; }

	// 미들웨어와의 연동 이외의 용도로 사용하지 말것. 위의 함수 대신 이걸 사용하세요
	virtual void* GetLPDIRECT3DDEVICE9() override { return (void*)m_pd3dDevice; }

	// 미들웨어와의 연동 이외의 용도로 사용하지 말것. present parameter 얻기
	virtual void* GetD3DPRESENT_PARAMETERS() override { return (void*)&m_d3dpp; }

	// TODO by pok
	// D3D Device의 함수들을 일단 감싸기만 했습니다.
	// 관련있는것들 끼리 모으로 분리해야 될것 같습니다.
	// 나중에 깔끔하게 옮겨요~

	// D3DTextureToHDC 때문인듯
	HRESULT D3DXCreateTextureFromFile(const char* _pFileName, LPDIRECT3DTEXTURE9* ppTexture)
	{
		return ::D3DXCreateTextureFromFileA( m_pd3dDevice, _pFileName, ppTexture);
	}
	HRESULT D3DXCreateTextureFromFileEx(const char* _pFileName, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
		DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO& SrcInfo, PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture)
	{
		return ::D3DXCreateTextureFromFileExA( m_pd3dDevice, _pFileName, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter,
			ColorKey, &SrcInfo, pPalette, ppTexture );
	}

	// 툴의 커서 때문에
	HRESULT D3DXCreateBox(FLOAT Width, FLOAT Height, FLOAT Depth, LPD3DXMESH& _pMesh)
	{
		return ::D3DXCreateBox(m_pd3dDevice, Width, Height, Depth, &_pMesh, NULL);
	}
	HRESULT D3DXCreateSphere( FLOAT Radius, UINT Slices, UINT Stacks, LPD3DXMESH& pMesh )
	{
		return ::D3DXCreateSphere( m_pd3dDevice, Radius, Slices, Stacks, &pMesh, NULL);
	}
	HRESULT D3DXCreateCylinder(FLOAT Radius1, FLOAT Radius2, FLOAT Length, UINT Slices, UINT Stacks, LPD3DXMESH& pMesh)
	{
		return ::D3DXCreateCylinder(m_pd3dDevice, Radius1, Radius2, Length, Slices, Stacks, &pMesh, NULL);
	}

	HRESULT D3DXSaveTextureToFileInMemory(LPD3DXBUFFER*ppDestBuf, D3DXIMAGE_FILEFORMAT DestFormat, LPDIRECT3DBASETEXTURE9 pSrcTexture, const PALETTEENTRY* pSrcPallette = NULL)
	{
		return ::D3DXSaveTextureToFileInMemory(ppDestBuf, DestFormat, pSrcTexture, pSrcPallette);
	}
};

}

#endif
