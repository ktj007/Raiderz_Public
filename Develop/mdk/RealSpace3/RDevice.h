#ifndef _RDEVICE_H
#define _RDEVICE_H

#include "RPreDefine.h"
#include "MHandle.h"
#include "MSingleton.h"
//#include "d3d9types.h" // D3DLOCKED_RECT
#include "RTexture.h"
#include "RQuery.h"

namespace rs3 {

enum RFILLMODE {	// wireframe ( triangle line ) , solid
	RFILL_POINT = 0,
	RFILL_WIRE,
	RFILL_SOLID
};

enum RBLEND {		// Blending constants
	RBLEND_ZERO = 0,
	RBLEND_ONE,
	RBLEND_SRCCOLOR,
	RBLEND_INVSRCCOLOR,
	RBLEND_DESTCOLOR,
	RBLEND_INVDESTCOLOR,
	RBLEND_SRCALPHA,
	RBLEND_INVSRCALPHA,
	RBLEND_DESTALPHA,
	RBLEND_INVDESTALPHA,
	RBLEND_SRCALPHASAT,
	RBLEND_BLENDFACTOR,
	RBLEND_INVBLENDFACTOR,

	RBLEND_NONE,
};

enum RBLENDOP {		// Blending operation
	RBLENDOP_ADD = 0,
	RBLENDOP_SUBTRACT,
	RBLENDOP_REVSUBTRACT,
	RBLENDOP_MIN,
	RBLENDOP_MAX,
};

enum RCMPFUNC {		// depth compare function
	RCMP_NEVER = 0,
	RCMP_LESS,
	RCMP_EQUAL,
	RCMP_LESSEQUAL,
	RCMP_GREATER,
	RCMP_NOTEQUAL,
	RCMP_GREATEREQUAL,
	RCMP_ALWAYS,
	RCMP_END
};

enum RPRIMITIVETYPE {	// Primitive types
	RPT_POINTLIST = 0,
	RPT_LINELIST,
	RPT_LINESTRIP,
	RPT_TRIANGLELIST,
	RPT_TRIANGLESTRIP,
	RPT_TRIANGLEFAN,
};

enum RCULL {			// Culling
	RCULL_NONE = 0,
	RCULL_CCW,
	RCULL_CW,
};

enum RTEXTURESTAGESTATETYPE {	// stage state
	RTSS_COLOROP = 0,
	RTSS_COLORARG1,
	RTSS_COLORARG2,
	RTSS_ALPHAOP,
	RTSS_ALPHAARG1,
	RTSS_ALPHAARG2,
	RTSS_BUMPENVMAT00,
	RTSS_BUMPENVMAT01,
	RTSS_BUMPENVMAT10,
	RTSS_BUMPENVMAT11,
	RTSS_TEXCOORDINDEX,
	RTSS_BUMPENVLSCALE,
	RTSS_BUMPENVLOFFSET,
	RTSS_TEXTURETRANSFORMFLAGS,
	RTSS_COLORARG0,
	RTSS_ALPHAARG0,
	RTSS_RESULTARG,
	RTSS_CONSTANT,
	
	RTSS_MAX
};

enum RTEXTURECOORDTYPE {

};

enum RTEXTUREOP {			// texture operation
	RTOP_DISABLE = 0,
	RTOP_SELECTARG1,
	RTOP_SELECTARG2,
	RTOP_MODULATE,
	RTOP_MODULATE2X,
	RTOP_MODULATE4X,
	RTOP_ADD,
	RTOP_ADDSIGNED,
	RTOP_ADDSIGNED2X,
	RTOP_SUBTRACT,
	RTOP_ADDSMOOTH,
	RTOP_BLENDDIFFUSEALPHA,
	RTOP_BLENDTEXTUREALPHA,
	RTOP_BLENDFACTORALPHA,
	RTOP_BLENDTEXTUREALPHAPM,
	RTOP_BLENDCURRENTALPHA,
	RTOP_PREMODULATE,
	RTOP_MODULATEALPHA_ADDCOLOR,
	RTOP_MODULATECOLOR_ADDALPHA,
	RTOP_MODULATEINVALPHA_ADDCOLOR,
	RTOP_MODULATEINVCOLOR_ADDALPHA,
	RTOP_BUMPENVMAP,
	RTOP_BUMPENVMAPLUMINANCE,
	RTOP_DOTPRODUCT3,
	RTOP_MULTIPLYADD,
	RTOP_LERP,
};

enum RSTENCILOP {			// stencil operation
    RSTENCILOP_KEEP = 0,
    RSTENCILOP_ZERO,
    RSTENCILOP_REPLACE,
    RSTENCILOP_INCRSAT,
    RSTENCILOP_DECRSAT,
    RSTENCILOP_INVERT,
    RSTENCILOP_INCR,
    RSTENCILOP_DECR,
};

enum RTEXTUREADDRESS {		// texture address
    RTADDRESS_WRAP = 0,
    RTADDRESS_MIRROR,
    RTADDRESS_CLAMP,
    RTADDRESS_BORDER,
    RTADDRESS_MIRRORONCE,
};

enum RCUBEMAP_FACES {
	RCUBEMAP_FACE_POSITIVE_X     = 0,
	RCUBEMAP_FACE_NEGATIVE_X     = 1,
	RCUBEMAP_FACE_POSITIVE_Y     = 2,
	RCUBEMAP_FACE_NEGATIVE_Y     = 3,
	RCUBEMAP_FACE_POSITIVE_Z     = 4,
	RCUBEMAP_FACE_NEGATIVE_Z     = 5,
	RCUBEMAP_FACE_MAX			= 6
};

// Flags field for Issue
#define RISSUE_END (1 << 0) // Tells the runtime to issue the end of a query, changing it's state to "non-signaled".
#define RISSUE_BEGIN (1 << 1) // Tells the runtime to issue the beginng of a query.

// texture 생성 플래그
#define RCT_CLAMP			0x1
#define RCT_CUBEMAP			0x8
#define RCT_NORMALMAP		0x20
#define RCT_AUTOGENMIPMAP	0x40
#define RCT_RENDERTARGET	0x80
#define RCT_DYNAMIC			0x100	// usage 관련
#define RCT_UNREDUCIBLE		0x200	// texture 해상도 옵션으로 낮출수 없는가여부 // 이 프레그가 있으면 해상도를 낮추지 않는다.
#define RCT_SYSTEMMEM		0x400	// usage 관련
#define RCT_DEPTHSTENCIL	0x800	// usage 관련 : Depth Stencil로 사용. D3DUSAGE_DEPTHSTENCIL - 090622, OX

#define RDMAPSAMPLER 256
//Vertex Texture Sampler(0~4)
const int RVERTEXTEXTURESAMPLER0 = (RDMAPSAMPLER+1);
const int RVERTEXTEXTURESAMPLER1 = (RDMAPSAMPLER+2);
const int RVERTEXTEXTURESAMPLER2 = (RDMAPSAMPLER+3);
const int RVERTEXTEXTURESAMPLER3 = (RDMAPSAMPLER+4);

// usage
extern RS_API const int RUSAGE_DYNAMIC;
extern RS_API const int RUSAGE_WRITEONLY;

// Resource locking flags
extern RS_API const int RLF_WRITE_ONLY;
extern RS_API const int RLF_READ_ONLY;
extern RS_API const int RLF_READ_WRITE;

// stage state arg type
extern RS_API const int RTA_DIFFUSE;
extern RS_API const int RTA_CURRENT;
extern RS_API const int RTA_TEXTURE;
extern RS_API const int RTA_TFACTOR;
extern RS_API const int RTA_SPECULAR;
extern RS_API const int RTA_TEMP;
extern RS_API const int RTA_CONSTANT;
extern RS_API const int RTA_COMPLEMENT;
extern RS_API const int RTA_ALPHAREPLICATE;
extern RS_API const int RTA_NOT_INITIALIZED;

extern RS_API const int RTTFF_DISABLE;
extern RS_API const int RTTFF_COUNT1;
extern RS_API const int RTTFF_COUNT2;
extern RS_API const int RTTFF_COUNT3;
extern RS_API const int RTTFF_COUNT4;
extern RS_API const int RTTFF_PROJECTED;

extern RS_API const DWORD RTCI_PASSTHRU;
extern RS_API const DWORD RTCI_CAMERASPACENORMAL;
extern RS_API const DWORD RTCI_CAMERASPACEPOSITION;
extern RS_API const DWORD RTCI_CAMERASPACEREFLECTIONVECTOR;
extern RS_API const DWORD RTCI_SPHEREMAP;


//#define MAX_IMAGEUNIT	20
#define MAX_VERTEXSTREAM 4
const int MAX_RENDERTARGET =	4;

class RShaderFX;
class RShaderFXPool;
class RD3DQueryManager;
class RTextureManager;

struct RDeviceProfileInfo {
	// 폴리곤수
	int nDP;	// drawprimitive
	int nDIP;	// drawindexprimitive
	int nDPUP;	// drawprimitiveup
	int nDIPUP;	// drawindexedprimitiveup

	// call 수
	int nDPCall;	// drawprimitive
	int nDIPCall;	// drawindexprimitive
	int nDPUPCall;	// drawprimitiveup
	int nDIPUPCall;	// drawindexedprimitiveup

	unsigned int nReducedDPCall;
	unsigned int nReducedDIPCall;

	void AddDP(int n, unsigned int nInstanceCount) { nDPCall++; nDP+=n*nInstanceCount; nReducedDPCall+=nInstanceCount-1; }
	void AddDIP(int n, unsigned int nInstanceCount) { nDIPCall++; nDIP+=n*nInstanceCount; nReducedDIPCall+=nInstanceCount-1; }
	void AddDPUP(int n) { nDPUPCall++; nDPUP+=n; }
	void AddDIPUP(int n) { nDIPUPCall++; nDIPUP+=n; }

	int	GetTotalDrawPolygon() const { return nDP+nDIP+nDPUP+nDIPUP; }
	int	GetTotalDrawCall() const { return nDPCall+nDIPCall+nDPUPCall+nDIPUPCall; }
	int GetTotalReducedDrawCall() const { return nReducedDPCall+nReducedDIPCall; }

	RDeviceProfileInfo()
	{
		Reset();
	}

	void Reset()
	{
		nDP = nDIP = nDPUP = nDIPUP = 0;
		nDPCall = nDIPCall = nDPUPCall = nDIPUPCall = 0;
		nReducedDPCall = nReducedDIPCall = 0;
	}
};

struct RLOCKED_RECT
{
	INT                 Pitch;
	void*               pBits;
};

class RS_API RDevice : public MSingletonRestricted< RDevice >
{
	friend RTextureManager;
	friend class REngine;

protected:
	RTextureManager			*m_pTextureManager;

	RDeviceProfileInfo		m_deviceProfileInfoCurrent;
	RDeviceProfileInfo		m_deviceProfileInfoLast;

	// 리소스들
//	MHandle<RTexture>		m_Textures;
//	MHandle<RShader>		m_Shaders;

	// 현재 상태
	RTexture*		currentRenderTarget[MAX_RENDERTARGET];
	int				currentRenderTargetSurface[MAX_RENDERTARGET];
	RTexture*		currentDepthStencilBuffer;

	RTexture*		currentTextures[MAX_IMAGEUNIT];
	RTEXTUREFILTERTYPE currentTextureFilter[MAX_IMAGEUNIT];
	RTEXTUREADDRESS	currentTextureAddress[MAX_IMAGEUNIT][3];	// u,v,w
	unsigned int	currentTextureStageSettings[MAX_IMAGEUNIT][RTSS_MAX];
	DWORD			currentMaxAnisotropy[MAX_IMAGEUNIT];
	float			currentMipmaplodBias[MAX_IMAGEUNIT];
	DWORD			currentTextureBorderColor[MAX_IMAGEUNIT];

	HVERTEXFORMAT	currentVertexFormat;
	HVERTEXBUFFER	currentVertexBuffer[MAX_VERTEXSTREAM];
	unsigned int	currentOffset[MAX_VERTEXSTREAM];
	unsigned int	currentFrequencyParameter[MAX_VERTEXSTREAM];
	HINDEXBUFFER	currentIndexBuffer;
	unsigned int	currentInstanceCount;

	// alpha blend
	bool			currentBlendEnable;
	RBLEND			currentSrcFactor,  currentDstFactor;
	RBLENDOP		currentBlendMode;
	// SEPARATE ALPHABLEND
	bool			currentSeparateBlendEnable;
	RBLEND			currentSeparateSrcFactor,  currentSeparateDstFactor;
	RBLENDOP		currentSeparateBlendMode;
	
	DWORD			currentBlendFactor;
	DWORD			currentTextureFactor;
	RCMPFUNC		currentDepthFunc;
	bool			currentDepthTestEnable, currentDepthWriteEnable;
	RCMPFUNC		currentStencilFunc,currentStencilBackFunc;
	DWORD			currentStencilRef;
	DWORD			currentStencilMask;
	bool			currentStencilEnable;
	bool			currentColorWriteEnable;
	RCULL			currentCullMode;
	RFILLMODE		currentFillMode;
	RMatrix			currentTransform[RST_MAX];

	bool			currentLighting;
	bool			currentNormalizeNormals;
	DWORD			currentAmbientColor;
	//bool			currentAlphaBlendEnable;
	bool			currentAlphaTestEnable;
	DWORD			currentAlphaRef;
	RCMPFUNC		currentAlphaFunc;
	bool			currentClipping;
	bool			currentScissorTestEnable;
	bool			currentFog;
	DWORD			currentFogColor;
	float			currentFogNear,currentFogFar;
	bool			currentVertexBlendEnable;
	bool			currentIndexedVertexBlendEnable;
	bool			currentSpecularEnable;

	bool			currentColorVertexEnable;
	DWORD			currentClipPlaneEnable;

	float			currentDepthBias;
	float			currentSlopeScaleDepthBias;

	RSTENCILOP		currentStencilPass;
	RSTENCILOP		currentStencilFail;
	RSTENCILOP		currentStencilZFail;

	// 기타
	bool			m_bCursor;
//	RConfiguration	m_currentConfig;
	int				m_nScreenWidth;
	int				m_nScreenHeight;
	RFORMAT			m_depthFormat;

	vector<RShaderFX*>	m_vShaderFXs;

	RTexture*		m_pDefaultTexture;
	RTexture*		m_pDefaultNoiseTexture;

	// flip관련
	unsigned int	m_nFrameCount;
	unsigned int	m_nLastFPSFrameCount;

	float			m_fFPS;
	DWORD			m_dwLastFPSTime;

	DWORD			m_dwLastFlipTime;
	DWORD			m_dwLastElapsedTime;

	RViewport		m_viewport;

	RVector4		m_vDofParams;

protected:
	virtual void OnFlip() = 0;
	virtual void OnSetViewport(const RViewport &) = 0;
	virtual void OnResetDevice() { }
	virtual void OnShowCursor(bool bShow) { }

	// sampler 번호 -> 내부 저장용 index 로 변환
	unsigned int GetSamplerNumberToSaveIndex(unsigned int nStage);
	//  내부 저장용 index -> sampler 번호 로 변환
	unsigned int GetSaveIndexToSamplerNumber(unsigned int nIndex);

	void IncreaseFrameCount() { m_nFrameCount++; }

	virtual void SetVertexBufferFreq( UINT stream, UINT FrequencyParameter) = 0;

public:
	RDevice();
	virtual ~RDevice(){}

	virtual bool Create(HWND hWnd) = 0;
	virtual void Destroy() = 0;

	int				GetScreenWidth()		{ return m_nScreenWidth;	}
	int				GetScreenHeight()		{ return m_nScreenHeight; }
	RFORMAT			GetDepthFormat()		{ return m_depthFormat; }
	DWORD			GetLastElapsedTime()	{ return m_dwLastElapsedTime; }
	unsigned int	GetFrameCount()			{ return m_nFrameCount; }
	float			GetFrameRate()			{ return m_fFPS; }
	unsigned int	GetTextureMemoryUsed();
	virtual unsigned int	GetVertexBufferMemoryUsed() = 0;

	const RTextureManager*	GetTextureManager()		{	return m_pTextureManager; }

	const RDeviceProfileInfo& GetDeviceProfileInfo() { return m_deviceProfileInfoLast; }

	bool	IsCursorVisible();
	bool	ShowCursor(bool bShow);

	virtual void InitDeviceDefault();
	virtual bool ResetDevice() = 0;	// rs3::config로부터 설정을 다시 읽어서 디바이스를 reset한다

	virtual void Flip();
	void SetViewport(const RViewport &viewport);
	void SetViewport(int x, int y, int nWidth, int nHeight, float fMinZ = 0, float fMaxZ = 1);
	RViewport GetViewport() const { return m_viewport; }
	
	virtual bool QueryFeature(RQUERYFEATURETYPE feature) = 0;
	virtual RDEVICESTATUS QueryStatus() = 0;

	virtual bool BeginScene() = 0;
	virtual void EndScene() = 0;

	virtual void Clear(bool bTarget = true, bool bDepth = true, bool bStencil = false, DWORD dwColor = 0, float fDepth = 1.f, DWORD dwStencil = 0, DWORD nIdx = 0) = 0;	// 스테이지 인자 추가 - 090625, OZ

	//////////////////////////////////////////////////////////////////////////
	// Render Target / Depth Buffer
	// 리턴값은 이전 깊이버퍼/렌더타켓
	virtual RTexture* SetRenderTarget(unsigned int nRenderTargetIndex, RTexture* pTexture, int nSurface = 0) = 0;
	virtual RTexture* GetRenderTarget(unsigned int nRenderTargetIndex) = 0;
	virtual RTexture* SetDepthStencilBuffer(RTexture* pTexture) = 0;
	virtual RTexture* GetDepthStencilBuffer() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Vertex Format
	virtual HVERTEXFORMAT CreateVertexFormat(const RVertexAttribute *attribs, const int nAttribs, const int nStream = 0, const int nStartTexCoord = 0) = 0;
	virtual HVERTEXFORMAT	CreateVertexFormat(const RVertexAttributeEx *attribs, const int nAttribs) = 0;
	virtual void SetFvF(DWORD fvf) = 0;

	//////////////////////////////////////////////////////////////////////////
	// texture 
	virtual RTexture*	NewTexture() = 0;
	virtual void		ReleaseTexture(RTexture* pTexture){} //TODO_ignore444 : 원래는 없던건데 컴파일이 안되어서 삽입

	const RTextureInfo *GetTextureInfo(RTexture* pTexture);

	RTexture* CreateTextureForBuildLater(const char *fileName, const RTEXTUREFILTERTYPE filter = RTF_LINEAR,
		DWORD flags = RCT_AUTOGENMIPMAP, RFORMAT pf = RFMT_NONE, bool bBackground = false, RResource::LoadingController* pController = NULL);
	RTexture* CreateTexture(const char *fileName, const RTEXTUREFILTERTYPE filter = RTF_LINEAR,
		DWORD flags = RCT_AUTOGENMIPMAP, RFORMAT pf = RFMT_NONE, bool bBackground = false, RResource::LoadingController* pController = NULL);
	RTexture* CreateTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = 0, void *pixels = NULL, const char* szName = NULL);
	//	RTexture* CreateTextureByName(const char *pSzTextureName, RTexture* pDefaultPixelSrcTexture, const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = 0, RFORMAT pf = RFMT_NONE);
	RTexture* CreateGarbageRenderTargetTexture( int nWidth, int nHeight, RFORMAT pf);

	// GetTextureByName 와 CreateTextureByName 을 분리하는게 좋을듯 합니다
//	RTexture* GetTextureByName( const char* pSzTextureName, bool bCreateIfNotExist = true, RTexture* pSrcDefaultPixelTexture = NULL);

	// create special texture - render target and normal map
	virtual RTexture* CreateRenderTargetTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = 0);
	virtual RTexture* CreateRenderTargetDepthStencilTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = 0);	// D3DUSAGE_DEPTHSTENCIL을 사용하는 - 090622, OZ
	virtual RTexture* CreateNormalMapFromHeightMap(RTexture* hHeightMap, float fAttitude) = 0;
	//	TODO: file in memory 에서 텍스쳐 생성하는 펑션 필요
	//	virtual HTEXTURE CreateCubeMapTexture();
	//	virtual HTEXTURE CreateDepthStencil();

	void DeleteTexture(RTexture* pTexture);
	void SafeDeleteTexture(RTexture*& pTexture); ///< SAFE_DELETE 매크로와 비슷한 역할을 하도록 만들어진 함수
	RTexture* GetTexture(const char* pSzTextureName);


	virtual bool GenerateMipmapLevels(RTexture* pTexture) = 0;
	virtual bool LockRect(RTexture* pTex, int nLevel, RLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) = 0;
	virtual bool UnlockRect(RTexture* pTex, int nLevel) = 0;
	virtual bool UpdateTexture(RTexture* pTexture, int nLevel, int nDestX, int nDestY, int nSrcWidth, int nSrcHeight, void *pSrcPixels) = 0;
	virtual bool ColorFill(RTexture* pTexture, DWORD dwColor = 0) = 0;
	
	/// nSrcWidth, nSrcHeight 크기의 SrcPixel을, nSrcX, nSrcY 을 기준으로 nDestWidth, nDestHeight 크기만큼 nDestX, nDestY에 복사한다.
	virtual bool UpdateTexture(RTexture* pTexture, int nLevel, int nDestX, int nDestY, int nDestWidth, int nDestHeight, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight, void *pSrcPixels) = 0;

	RTexture* GetDefaultTexture();
	RTexture* GetDefaultNoiseTexture();

	//////////////////////////////////////////////////////////////////////////
	// Vertex Buffer
	virtual HVERTEXBUFFER CreateVertexBuffer(const long size, int nVertexSize, const void *data = NULL, unsigned int flags = 0) = 0;
	virtual void DeleteVertexBuffer(HVERTEXBUFFER hVertexBuffer) = 0;
	virtual void *LockVertexBuffer(const HVERTEXBUFFER hVertexBuffer, const unsigned int flags = 0, const unsigned int nOffsetToLock = 0, const unsigned int nSizeToLock = 0) = 0;
	virtual bool UnlockVertexBuffer(const HVERTEXBUFFER hVertexBuffer) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Index Buffer
	virtual HINDEXBUFFER CreateIndexBuffer(const unsigned int nIndices, RFORMAT inf = RFMT_INDEX16 , const void *data = NULL, unsigned int flags = 0) = 0;
	virtual void DeleteIndexBuffer(HINDEXBUFFER hIndexBuffer) = 0;
	virtual void *LockIndexBuffer(const HINDEXBUFFER hIndexBuffer, const unsigned int flags = 0, const unsigned int nOffsetToLock = 0, const unsigned int nSizeToLock = 0) = 0;
	virtual bool UnlockIndexBuffer(const HINDEXBUFFER hIndexBuffer) = 0;

	//////////////////////////////////////////////////////////////////////////
	// DrawPrimitives
	virtual bool DrawIndexedPrimitiveUP(RPRIMITIVETYPE primitiveType, unsigned int nMinVertexIndex, unsigned int nNumVertices, unsigned int nPrimitiveCount, 
		const void* pIndexData, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride, RFORMAT indexFmt = RFMT_INDEX16 ) = 0;
	virtual bool DrawIndexedPrimitive(RPRIMITIVETYPE primitiveType, unsigned int nBaseVertexIndex, unsigned int nMinIndex, unsigned int nNumVertices,
		unsigned int nStartIndex, unsigned int nPrimitiveCount) = 0;
	virtual bool DrawPrimitiveUP(RPRIMITIVETYPE primitiveType, unsigned int nPrimitiveCount, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride ) = 0;
	virtual bool DrawPrimitive(RPRIMITIVETYPE primitiveType, unsigned int nStart, unsigned int nPrimitiveCount) = 0;

// TODO:
	// FileManager 에서 읽도록 수정필요 + 컴파일된 쉐이더 파일도 읽도록 수정
	// 쉐이더 파일 에서 vs ps 식별탭의 이름을 바꿔주기

	// ogl 과 d3d 에따라 다른 세부코드
	// asm file 에서 읽기..
	// hlsl file 에서 읽기..

//	virtual HSHADER CreateShader( char* vsShaderBuffer,char* psShaderBuffer, char *extraDefines = NULL,const int vsLine = 0,const int psLine = 0, const char **attribs = NULL, const int nAttribs = 0) = 0;
//	        HSHADER CreateShaders( const char *vsFileName, const char *psFileName, char *extraDefines = NULL, const char** attribs = NULL, const int nAttribs = 0);
//	        HSHADER CreateShader( const char *shaderFile, const char **attribs = NULL, const int nAttribs = 0, char *extraDefines = NULL);
//	        HSHADER CreateShader( const char *shaderFile, char *extraDefines) { return CreateShader(shaderFile, NULL, 0, extraDefines); }

	//////////////////////////////////////////////////////////////////////////
	// Shader 관련
	virtual void ShaderOff() = 0;

	//////////////////////////////////////////////////////////////////////////
	// ShaderFX
	virtual RShaderFX* CreateShaderFX() = 0;
	virtual RShaderFX* CreateShaderFX(const string& stFilename, const vector<string>& vShaderDefines = vector<string>(0),RShaderFXPool *pShaderPool = NULL) = 0;
	virtual void DeleteShaderFX(RShaderFX* shader) = 0;

	//////////////////////////////////////////////////////////////////////////
	// State 관리
	virtual void SetVertexBuffer(const HVERTEXBUFFER hVertexBuffer, const int stream, const unsigned int offset) = 0;
	virtual void SetVertexFormat(HVERTEXFORMAT hVertexFormat) = 0;

	virtual void SetupForRenderInstancing(const HVERTEXBUFFER hVertexBuffer, UINT nCnt, unsigned int offset = 0) = 0;
	virtual void RestoreForRenderInstancing() = 0;

	virtual void SetIndexBuffer(HINDEXBUFFER hIndexBuffer) = 0;

	virtual void SetTexture(int nStage,RTexture* pTexture) = 0;
	RTexture* GetTexture(int nStage);
	virtual void SetTextureStageState(int nStage, RTEXTURESTAGESTATETYPE nStageStateType, unsigned int value) = 0;
	virtual void SetTextureFilter(int nSampler, RTEXTUREFILTERTYPE type) = 0;
	virtual void SetTextureMipmapLodBias(int nStage,float fBias) = 0;
	virtual void SetTextureMaxAnisotropy(int nStage, DWORD dwValue) = 0;
	virtual DWORD GetTextureMaxAnisotropy(int nStage);
	inline float GetTextureMipmapLodBias(int nStage);
	virtual void SetTextureAddress(int nSampler, RTEXTUREADDRESS u, RTEXTUREADDRESS v, RTEXTUREADDRESS w) = 0;
	virtual void SetTextureBorderColor(int nSampler, const DWORD dwColor) = 0;
	inline void SetTextureAddressClamp(int nSampler);
	inline void SetTextureAddressWrap(int nSampler);

	virtual void SetTransform(const RSETTRANSFORMTYPE type, const RMatrix &matrix) = 0;
	virtual RMatrix GetTransform(const RSETTRANSFORMTYPE type) const = 0;

	// blending
	virtual void SetBlending(RBLEND srcFactor, RBLEND dstFactor = RBLEND_NONE, RBLENDOP blendMode = RBLENDOP_ADD) = 0;
	virtual void SetSeparateBlending(RBLEND srcFactor, RBLEND dstFactor = RBLEND_NONE, RBLENDOP blendMode = RBLENDOP_ADD) = 0;
	virtual void SetBlendFactor(const DWORD dwColor) = 0;
	virtual void SetTextureFactor(const DWORD dwColor) = 0;

	// alpha test
	virtual void SetAlphaTestEnable(bool bEnable) = 0;
	virtual void SetAlphaRef(DWORD dwRef) = 0;
	virtual void SetAlphaFunc(RCMPFUNC Func) = 0;
	virtual bool GetAlphaTestEnable() = 0;
	virtual DWORD GetAlphaRef() = 0;
	virtual RCMPFUNC GetAlphaFunc() = 0;

	virtual void SetDepthFunc(RCMPFUNC depthFunc) = 0;
	virtual void SetDepthEnable(bool bEnable, bool bWriteEnable = true) = 0;
	virtual void SetColorWriteEnable(bool bEnable) = 0;
	virtual void SetCullMode(RCULL cullMode) = 0;
	virtual void SetFillMode(RFILLMODE fillMode) = 0;
	inline RFILLMODE GetFillMode();

	virtual void SetClipPlanes(const RPlane *pPlanes, int nCount) = 0;
	virtual void SetDepthBias(float fDepthBias = 0, float fSlopeScaleDepthBias = 0) = 0;

	// stencil
	virtual void SetStencilEnable(bool bEnable) = 0;
	virtual void SetStencilTwoSide(bool bEnable) = 0;
	virtual void SetStencilRef(DWORD dwValue) = 0;
	virtual void SetStencilMask(DWORD dwValue) = 0;
	virtual void SetStencilWriteMask(DWORD dwValue) = 0;
	virtual void SetStencilFunc(RCMPFUNC stencilFunc) = 0;
	virtual void SetStencilOp(RSTENCILOP opPass,RSTENCILOP opFail,RSTENCILOP opZFail) = 0;
	virtual void SetStencilCCWFunc(RCMPFUNC stencilFunc) = 0;	// twoside의 뒷면
	virtual void SetStencilCCWOp(RSTENCILOP opPass,RSTENCILOP opFail,RSTENCILOP opZFail) = 0;

	virtual void SetPointSpriteEnable(bool bEnable) = 0;
	virtual void SetPointSizeAndScale( float fSize, float fMin, float fMax, bool bScaleEnable, float fScaleA, float fScaleB, float fScaleC) = 0;

	// light
	virtual void SetLight(int index,void* pLight) = 0;
	virtual void SetLightEnable(int index,bool bEnable) = 0;
	virtual void SetLighting(bool bEnable) = 0;

	// material 
	virtual void SetMaterial(void* pMtrl) = 0;

	// fog
	virtual void SetFogEnable(bool bEnable) = 0;
	virtual void SetFog(bool bLinearFog, DWORD dwColor, float fNear, float fFar, float fDensity,
		bool bPixelFog, bool bRangeFog) {}
	virtual void SetFogColor(DWORD dwColor) {}
	virtual void SetFogVertexMode(bool bPixelFog, bool bRangeFog) {}
	inline bool GetFogEnable();
	inline float GetFogNear();
	inline float GetFogFar();
	inline DWORD GetFogColor();

	// etc
	virtual void SetNormalizeNormals(bool bEnable) = 0;
	virtual void SetAmbient(DWORD dwColor) = 0;
	virtual void SetClipping(bool bEnable) = 0;		// Direct3D 클리핑(뷰프러스텀면)을 켜거나 끈다.

	virtual void SetScissorTestEnable(bool bEnable) = 0;
	virtual void SetScissorRect(const RECT* pRect) = 0;

	virtual void SetPointSize(DWORD nSize) = 0;

	virtual void SetVertexBlendEnable(bool bEnable) = 0;
	virtual void SetIndexedVertexBlendEnable(bool bEnable) = 0;
	virtual void SetSpecularEnable(bool bEnable) = 0;

	virtual void SetColorVertex(bool bEnable) = 0;

	virtual RImage* CreateImage(int nWidth, int nHeight, RFORMAT format, void* pSrcPixels, int nSourcePitch, int nSrcX, int nSrcY);
	virtual void DeleteImage(RImage*);

	virtual bool SetCursorProperties(int XHotSpot, int YHotSpot, RImage* pImage);

	inline void			SetDofParams( RVector4& vParams);
	inline const RVector4&	GetDofParams() const;

	virtual bool IsCurrentDisplayModeSupportFullScreenMode() = 0;

	//Query
	virtual	ROcclusionQuery*	CreateOcclusionQuery(void) = 0;
	virtual void				ReleaseOcclusionQuery(ROcclusionQuery* pQuery) = 0;
	virtual REventQuery*		CreateEventQuery(void) = 0;
	virtual void				ReleaseEventQuery(REventQuery* pEventQuery) = 0;
	virtual RD3DQueryManager*	GetQueryManager(void) { return NULL; }

	// Profile Marker
	virtual void				SetProfileMarker( LPCWSTR wszName) = 0;
	virtual void				BeginProfileMarker( LPCWSTR wszName) = 0;
	virtual void				EndProfileMarker() = 0;
	

	//////////////////////////////////////////////////////////////////////////
	// 외부엔진과 연결할때 필요한 함수들,
	// 미들웨어와의 연동 이외의 용도로 사용하지 말것.

	// 외부 라이브러리와의 연동등의 이유로 LPDIRECT3DDEVICE9 를 얻고 싶을때 이걸로 얻어서 캐스팅해서 쓰세요
	virtual void* GetLPDIRECT3DDEVICE9() { return NULL; }

	virtual void* GetD3DPRESENT_PARAMETERS() { return NULL; }

	/// 스크린샷 저장
	virtual bool SaveScreenShot( const char* szFileName ) { return false; }
	virtual RImage* GetSaveScreenImage() { return NULL; }
};

//////////////////////////////////////////////////////////////////////////
// inline functions
inline	void RDevice::SetTextureAddressClamp(int nSampler) {
	SetTextureAddress(nSampler,RTADDRESS_CLAMP,RTADDRESS_CLAMP,RTADDRESS_CLAMP);
}

inline void RDevice::SetTextureAddressWrap(int nSampler) {
	SetTextureAddress(nSampler,RTADDRESS_WRAP,RTADDRESS_WRAP,RTADDRESS_WRAP);
}

inline float RDevice::GetTextureMipmapLodBias(int nStage) {
	return currentMipmaplodBias[nStage];
}

inline DWORD RDevice::GetTextureMaxAnisotropy(int nStage) {
	return currentMaxAnisotropy[nStage];
}

inline RTexture* RDevice::GetTexture(int nStage) {
	int nSaveIndex = GetSamplerNumberToSaveIndex(nStage);
	return currentTextures[nSaveIndex];
}

inline bool RDevice::GetFogEnable() { return currentFog; }
inline float RDevice::GetFogNear() { return currentFogNear; }
inline float RDevice::GetFogFar() { return currentFogFar; }
inline DWORD RDevice::GetFogColor() { return currentFogColor; }

inline RTexture* RDevice::GetDefaultTexture() { return m_pDefaultTexture; }
inline RTexture* RDevice::GetDefaultNoiseTexture() { return m_pDefaultNoiseTexture; }


inline unsigned int RDevice::GetSamplerNumberToSaveIndex(unsigned int nStage) 
{ 
	// 0 ~ 15 or RVERTEXTEXTURESAMPLER0 ~ RVERTEXTEXTURESAMPLER3
	_ASSERT(nStage<MAX_IMAGEUNIT-MAX_VERTEXSAMPLER || 
		(RVERTEXTEXTURESAMPLER0<=nStage && nStage<=RVERTEXTEXTURESAMPLER3) );
	if(nStage >= RVERTEXTEXTURESAMPLER0)
		return nStage - RVERTEXTEXTURESAMPLER0 + MAX_IMAGEUNIT-MAX_VERTEXSAMPLER;
	return nStage;
}

inline unsigned int RDevice::GetSaveIndexToSamplerNumber(unsigned int nIndex)
{
	_ASSERT(nIndex<MAX_IMAGEUNIT);
	if(nIndex >= MAX_IMAGEUNIT-MAX_VERTEXSAMPLER)
		return nIndex - (MAX_IMAGEUNIT-MAX_VERTEXSAMPLER ) + RVERTEXTEXTURESAMPLER0;
	return nIndex;
}


inline void			RDevice::SetDofParams( RVector4& vParams)	{ m_vDofParams = vParams; }
inline const RVector4&	RDevice::GetDofParams()	const				{ return m_vDofParams;	}

inline RFILLMODE	RDevice::GetFillMode()	{ return currentFillMode; }

}

#endif
