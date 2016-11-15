#include "stdafx.h"
#include "RDeviceD3D.h"
#include "MDebug.h"
#include "RTypes.h"
#include "d3dx9.h"
#include "RConfiguration.h"
#include "RFont.h"
#include "RShaderFX.h"
#include "RTechnique.h"
#include "RUtil.h"
#include "MFileLoader.h"
#include "RQuery.h"
#include <cctype>	// std::tolower
#include "MProfiler.h"
#include "RShaderCompositeManager.h"
#include "RTextureManager.h"

#pragma comment (lib , "d3dx9.lib")

// for GetVideoMemory
#include "ddraw.h"
#pragma comment(lib,"dxguid.lib")

//#define SHADER_USE_LEGACY_D3DX9_31
//#define DEBUG_VS		// shaderfx 는 shaderfx.cpp 에서 디파인 풀어주세요
//#define DEBUG_PS 
//#define _NVPERFHUD

namespace rs3 {

#define USING_SHADER_CONST_WARNING

#ifndef NEW_LOADER
#else
	RTexture* RT_FRAMEBUFFER = (RTexture*)-1;	///< frame buffer 를 나타낼 포인터, 내용자체는 의미없다
#endif


// RS3 -> D3D 변환 테이블들
const D3DBLEND d3dBlendTable[] = {
	D3DBLEND_ZERO,
	D3DBLEND_ONE,
	D3DBLEND_SRCCOLOR,
	D3DBLEND_INVSRCCOLOR,
	D3DBLEND_DESTCOLOR,
	D3DBLEND_INVDESTCOLOR,
	D3DBLEND_SRCALPHA,
	D3DBLEND_INVSRCALPHA,
	D3DBLEND_DESTALPHA,
	D3DBLEND_INVDESTALPHA,
	D3DBLEND_SRCALPHASAT,
	D3DBLEND_BLENDFACTOR,
	D3DBLEND_INVBLENDFACTOR,
};

const D3DBLENDOP d3dBlendOPTable[] = {
	D3DBLENDOP_ADD,
	D3DBLENDOP_SUBTRACT,
	D3DBLENDOP_REVSUBTRACT,
	D3DBLENDOP_MIN,
	D3DBLENDOP_MAX,
};

const D3DCMPFUNC d3dCmpFuncTable[] = {
	D3DCMP_NEVER,
	D3DCMP_LESS,
	D3DCMP_EQUAL,
	D3DCMP_LESSEQUAL,
	D3DCMP_GREATER,
	D3DCMP_NOTEQUAL,
	D3DCMP_GREATEREQUAL,
	D3DCMP_ALWAYS
};

const D3DPRIMITIVETYPE d3dPrimitiveTypeTable[] = {
	D3DPT_POINTLIST,
	D3DPT_LINELIST,
	D3DPT_LINESTRIP,
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN,
};

const D3DCULL d3dCullTable[] = {
	D3DCULL_NONE,
	D3DCULL_CCW,
	D3DCULL_CW,
};

const D3DFILLMODE d3dFillTable[] = {
	D3DFILL_POINT,
	D3DFILL_WIREFRAME,
	D3DFILL_SOLID
};

const D3DTEXTURESTAGESTATETYPE d3dTextureStageStateTypeTable[] = {
	D3DTSS_COLOROP,
	D3DTSS_COLORARG1,
	D3DTSS_COLORARG2,
	D3DTSS_ALPHAOP,
	D3DTSS_ALPHAARG1,
	D3DTSS_ALPHAARG2,
	D3DTSS_BUMPENVMAT00,
	D3DTSS_BUMPENVMAT01,
	D3DTSS_BUMPENVMAT10,
	D3DTSS_BUMPENVMAT11,
	D3DTSS_TEXCOORDINDEX,
	D3DTSS_BUMPENVLSCALE,
	D3DTSS_BUMPENVLOFFSET,
	D3DTSS_TEXTURETRANSFORMFLAGS,
	D3DTSS_COLORARG0,
	D3DTSS_ALPHAARG0,
	D3DTSS_RESULTARG,
	D3DTSS_CONSTANT,
};

const D3DTEXTUREOP d3dTextureOPTable[] = {
	D3DTOP_DISABLE,
	D3DTOP_SELECTARG1,
	D3DTOP_SELECTARG2,
	D3DTOP_MODULATE,
	D3DTOP_MODULATE2X,
	D3DTOP_MODULATE4X,
	D3DTOP_ADD,
	D3DTOP_ADDSIGNED,
	D3DTOP_ADDSIGNED2X,
	D3DTOP_SUBTRACT,
	D3DTOP_ADDSMOOTH,
	D3DTOP_BLENDDIFFUSEALPHA,
	D3DTOP_BLENDTEXTUREALPHA,
	D3DTOP_BLENDFACTORALPHA,
	D3DTOP_BLENDTEXTUREALPHAPM,
	D3DTOP_BLENDCURRENTALPHA,
	D3DTOP_PREMODULATE,
	D3DTOP_MODULATEALPHA_ADDCOLOR,
	D3DTOP_MODULATECOLOR_ADDALPHA,
	D3DTOP_MODULATEINVALPHA_ADDCOLOR,
	D3DTOP_MODULATEINVCOLOR_ADDALPHA,
	D3DTOP_BUMPENVMAP,
	D3DTOP_BUMPENVMAPLUMINANCE,
	D3DTOP_DOTPRODUCT3,
	D3DTOP_MULTIPLYADD,
	D3DTOP_LERP,
};

const D3DSAMPLERSTATETYPE d3dSamplerStateTypeTable[] = {
    D3DSAMP_ADDRESSU,
    D3DSAMP_ADDRESSV,
    D3DSAMP_ADDRESSW,
    D3DSAMP_BORDERCOLOR,
    D3DSAMP_MAGFILTER,
    D3DSAMP_MINFILTER,
    D3DSAMP_MIPFILTER,
    D3DSAMP_MIPMAPLODBIAS,
    D3DSAMP_MAXMIPLEVEL,
    D3DSAMP_MAXANISOTROPY,
    D3DSAMP_SRGBTEXTURE,
    D3DSAMP_ELEMENTINDEX,
    D3DSAMP_DMAPOFFSET,
};

// rtypes 에 정의되어있는 포맷들에 대응. RFORMAT과 순서가 맞아야 합니다.
const D3DFORMAT d3dFormatTable[] = {
	D3DFMT_UNKNOWN,
	D3DFMT_R5G6B5,
	D3DFMT_R8G8B8,
	D3DFMT_X8R8G8B8,
	D3DFMT_A8R8G8B8,

	D3DFMT_A8,
	D3DFMT_A8R8G8B8,

	D3DFMT_A2R10G10B10,
	D3DFMT_G16R16,

	// 실수형 포맷
	D3DFMT_R16F,
	D3DFMT_G16R16F,
	D3DFMT_G32R32F,
	D3DFMT_R32F,
	D3DFMT_A16B16G16R16F,
	D3DFMT_A32B32G32R32F,

	// depth stencil buffer 용
	D3DFMT_D24S8,
	D3DFMT_D24X8,
	D3DFMT_D16,
	D3DFMT_D24FS8,			
	D3DFMT_D32F_LOCKABLE,	
	D3DFMT_D32,				

	D3DFMT_L8,		// 8-bit luminance only.

	// index buffer 포맷
	D3DFMT_INDEX16,
	D3DFMT_INDEX32,

	// 범프맵 포맷
	D3DFMT_Q8W8V8U8,
	D3DFMT_V16U16,
	D3DFMT_V8U8,

	// 압축텍스쳐 포맷
	D3DFMT_DXT1,
	D3DFMT_DXT3,
	D3DFMT_DXT5,
};

// rtypes 에 정의된 texturefiltertype 에 대응
struct RTEXTURE_FILTER_SET
{
	D3DTEXTUREFILTERTYPE minFilter;
	D3DTEXTUREFILTERTYPE magFilter;
	D3DTEXTUREFILTERTYPE mipFilter;
};

const RTEXTURE_FILTER_SET d3dTextureFilterSets[] = {
    D3DTEXF_POINT,		D3DTEXF_POINT,		D3DTEXF_NONE,		// RTF_POINT
	D3DTEXF_LINEAR,		D3DTEXF_LINEAR,		D3DTEXF_NONE,		// RTF_LINEAR
	D3DTEXF_LINEAR,		D3DTEXF_LINEAR,		D3DTEXF_POINT,		// RTF_BILINEAR
	D3DTEXF_LINEAR,		D3DTEXF_LINEAR,		D3DTEXF_LINEAR,		// RTF_TRILINEAR
	D3DTEXF_ANISOTROPIC,D3DTEXF_ANISOTROPIC,D3DTEXF_POINT,		// RTF_BILINEAR_ANISO
	D3DTEXF_ANISOTROPIC,D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,		// RTF_TRILINEAR_ANISO
	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,		D3DTEXF_NONE,		// RTF_BILINEAR_ANISO_LINEAR_FOR_FONT
};

const D3DSTENCILOP d3dStencilOpTable[] = {
    D3DSTENCILOP_KEEP,
    D3DSTENCILOP_ZERO,
    D3DSTENCILOP_REPLACE,
    D3DSTENCILOP_INCRSAT,
    D3DSTENCILOP_DECRSAT,
    D3DSTENCILOP_INVERT,
    D3DSTENCILOP_INCR,
    D3DSTENCILOP_DECR,
};

const DWORD d3dTexCoordTable[] = {
	D3DTSS_TCI_CAMERASPACEPOSITION,
	D3DTSS_TCI_CAMERASPACENORMAL
};

const D3DTEXTUREADDRESS d3dTextureAddressTable[] = {
    D3DTADDRESS_WRAP,
    D3DTADDRESS_MIRROR,
    D3DTADDRESS_CLAMP,
    D3DTADDRESS_BORDER,
    D3DTADDRESS_MIRRORONCE,
};

// stage state arg type
const int RTA_DIFFUSE			= D3DTA_DIFFUSE;
const int RTA_CURRENT			= D3DTA_CURRENT;
const int RTA_TEXTURE			= D3DTA_TEXTURE;
const int RTA_TFACTOR			= D3DTA_TFACTOR;
const int RTA_SPECULAR			= D3DTA_SPECULAR;
const int RTA_TEMP				= D3DTA_TEMP;
const int RTA_CONSTANT			= D3DTA_CONSTANT;
const int RTA_COMPLEMENT		= D3DTA_COMPLEMENT;
const int RTA_ALPHAREPLICATE	= D3DTA_ALPHAREPLICATE;
const int RTA_NOT_INITIALIZED	= 0xffffff;

const int RTTFF_DISABLE		= D3DTTFF_DISABLE;
const int RTTFF_COUNT1		= D3DTTFF_COUNT1;
const int RTTFF_COUNT2		= D3DTTFF_COUNT2;
const int RTTFF_COUNT3		= D3DTTFF_COUNT3;
const int RTTFF_COUNT4		= D3DTTFF_COUNT4;
const int RTTFF_PROJECTED	= D3DTTFF_PROJECTED;

const DWORD RTCI_PASSTHRU					= D3DTSS_TCI_PASSTHRU;
const DWORD RTCI_CAMERASPACENORMAL			= D3DTSS_TCI_CAMERASPACENORMAL;
const DWORD RTCI_CAMERASPACEPOSITION		= D3DTSS_TCI_CAMERASPACEPOSITION;
const DWORD RTCI_CAMERASPACEREFLECTIONVECTOR= D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
const DWORD RTCI_SPHEREMAP					= D3DTSS_TCI_SPHEREMAP;

// usage
const int RUSAGE_DYNAMIC	=	D3DUSAGE_DYNAMIC;
const int RUSAGE_WRITEONLY	=	D3DUSAGE_WRITEONLY;

// Lock flags
const int RLF_WRITE_ONLY = D3DLOCK_DISCARD;
const int RLF_READ_ONLY  = D3DLOCK_READONLY | D3DLOCK_NOOVERWRITE;
const int RLF_READ_WRITE = 0;

// 일단 여기다 둡니다. 좋은 위치가 있으면 옮겨주세요
D3DXIMAGE_FILEFORMAT GetFileFormatByFileName( const char* szFileName )
{
	string strExt = MGetPureExtension(szFileName);
	// convert strExt to lower case
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), std::tolower);

	if(strExt==".jpg")
		return D3DXIFF_JPG;
	else if(strExt==".bmp")
		return D3DXIFF_BMP;
	else if(strExt==".tga")
		return D3DXIFF_TGA;
	else if(strExt==".dds")
		return D3DXIFF_DDS;
	else if(strExt==".png")
		return D3DXIFF_PNG;
	else if(strExt==".hdr")
		return D3DXIFF_HDR;

	return D3DXIFF_FORCE_DWORD;
}

class RImageD3D : public RImage {
	friend RDeviceD3D;
	LPDIRECT3DSURFACE9 m_pSurface;
public:
	RImageD3D() : m_pSurface(NULL)	{}
	virtual ~RImageD3D()			{ SAFE_RELEASE(m_pSurface); }

	bool Save(const char* szFileName ) override
	{
		//////////////////////////////////////////////////////////////////////////
		// 확장자 알아오기
		D3DXIMAGE_FILEFORMAT imageFormat = GetFileFormatByFileName(szFileName);
		if(imageFormat == D3DXIFF_FORCE_DWORD)
		{
			// 지원 안함
			_ASSERT(FALSE);
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// 저장
		return D3D_OK == D3DXSaveSurfaceToFile(szFileName, imageFormat, m_pSurface, NULL, NULL);
	}
};

// 리소스들 RTexture, RVertexFormat, RVertexBuffer, RIndexBuffer 에 virtual 함수 쓰지 말것.
class RS_API RTextureD3D : public RTexture {
public:
	RTextureD3D() 
		: nRTSurfaces(0)
		, rtSurfaces(NULL)
		, m_pD3Dtexture(NULL)
		, defaultTexture(NULL)
		, m_buffer(NULL)
		, usage(0)
		, pool(D3DPOOL_MANAGED)
	{ }
	~RTextureD3D();

	// 실제 텍스쳐
	LPDIRECT3DBASETEXTURE9 m_pD3Dtexture;

	// 외부에서부터 링크된 기본 텍스쳐. RS_LOADED 가 아니더라도 렌더링이 가능하게 한다.
	// 기본텍스쳐는 링크된 포인터이므로 지우지 않아야 한다.
	LPDIRECT3DBASETEXTURE9 defaultTexture;

	LPDIRECT3DSURFACE9 *rtSurfaces;
	D3DFORMAT format;
	DWORD usage;
	D3DPOOL pool;
	int nRTSurfaces;

	char* m_buffer;
	int m_nFileSize;

	static LPDIRECT3DDEVICE9 m_pD3DDevice;
//	static const std::vector< std::string >* m_pVecPath;

	virtual bool Invalidate();
	virtual bool Restore();

	virtual void SetDefaultPixel( RTexture* pSrcTexture )
	{
		m_pD3Dtexture = static_cast<RTextureD3D*>(pSrcTexture)->m_pD3Dtexture;
		defaultTexture = m_pD3Dtexture;
	}

	virtual bool SaveToFile(const char* szFileName, RFORMAT format)
	{
		D3DXIMAGE_FILEFORMAT imageFormat = GetFileFormatByFileName(szFileName);
		if(imageFormat == D3DXIFF_FORCE_DWORD)
		{
			// 지원 안함
			_ASSERT(FALSE);
			return false;
		}

		if(format!=RFMT_NONE && format!=info.pixelFormat)
			return ConvertTexture(m_pD3DDevice,m_pD3Dtexture,d3dFormatTable[format],szFileName, imageFormat );

		return (D3D_OK == D3DXSaveTextureToFile(szFileName, imageFormat, m_pD3Dtexture, NULL));
	}

	virtual int GetUsingSystemMemory()
	{
		if (pool != D3DPOOL_DEFAULT)
			return info.GetSize();

		return 0;
	}

	virtual int GetUsingVideoMemory()
	{
		return info.GetSize();
	}

protected:
	virtual bool		Fill() override;
	virtual LOAD_RESULT Load() override;

private:

	void CheckFailedCreateTexture( HRESULT hr );

	bool ConvertTexture(LPDIRECT3DDEVICE9 device, LPDIRECT3DBASETEXTURE9 pSrcTexture, D3DFORMAT targetFormat, const char* szOutput, D3DXIMAGE_FILEFORMAT imageFormat )
	{
		//////////////////////////////////////////////////////////////////////////
		// 만든 텍스쳐를 메모리에 저장해둔다
		LPD3DXBUFFER pBuffer = NULL;
		D3DXSaveTextureToFileInMemory(&pBuffer,D3DXIFF_DDS, pSrcTexture, NULL);

		//////////////////////////////////////////////////////////////////////////
		// 밉맵을 만들고 압축텍스쳐로 변환한다

		D3DXIMAGE_INFO diffuseInfo;
		LPDIRECT3DTEXTURE9 pTargetTexture;
		if( FAILED(D3DXCreateTextureFromFileInMemoryEx(

			device,pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), 

			D3DX_DEFAULT, D3DX_DEFAULT, 
			1 , // mipmap
			0 , // usage
			targetFormat ,D3DPOOL_SYSTEMMEM,
			D3DX_FILTER_NONE, 
			D3DX_FILTER_NONE, 
			0, &diffuseInfo, NULL, &pTargetTexture )))
		{
			_ASSERT(FALSE);
			return false;
		}

		bool bSuccess = (D3D_OK == D3DXSaveTextureToFile(szOutput, imageFormat, pTargetTexture, NULL));

		pTargetTexture->Release();
		pBuffer->Release();

		return bSuccess;
	}
};

LPDIRECT3DDEVICE9 RTextureD3D::m_pD3DDevice = NULL;
//const std::vector< std::string >* RTextureD3D::m_pVecPath = NULL;

RTextureD3D::~RTextureD3D()
{
	SAFE_DELETE_ARRAY( m_buffer );

	if(rtSurfaces) {
		for(int i=0;i<nRTSurfaces;++i) {
			SAFE_RELEASE(rtSurfaces[i]);
		}
		SAFE_DELETE_ARRAY(rtSurfaces);
	}
	if( m_pD3Dtexture != defaultTexture )
		SAFE_RELEASE(m_pD3Dtexture);
}

bool RTextureD3D::Fill()
{
	if(!m_bCreateFromFile) 
		return true;

	if( GetName().empty() )
	{
//		m_state = RRS_WRONG;
		return false;
	}

//	_ASSERT(m_state==RRS_EMPTY);

	INIT_TCHECK("/check_texture_filltime");
	BEGIN_TCHECK;


	MFile file;
	if ( file.Open( GetName().c_str()) == false)
	{
		if ( file.Open( string( GetName() + ".dds").c_str()) == false)
		{
			//	m_state = RRS_WRONG;
			mlog("Texture file load failed -> %s\n", GetName().c_str());
			return false;
		}
	}


	m_nFileSize = file.GetLength();
	_ASSERT(m_nFileSize>=0);
//	m_buffer = new char[m_nFileSize];
	m_buffer = new(std::nothrow) char[m_nFileSize];
	if ( !m_buffer )
	{
		mlog("#Texture Memory Allocation Failed (%s)\n", GetName().c_str());
		return false;
	}
	if ( !file.Read(m_buffer, m_nFileSize))
	{
		mlog("#Failed TextureD3D Fill (%s)\n", GetName().c_str());
		SAFE_DELETE_ARRAY(m_buffer);
		
		file.Close();
		return false;
	}

	file.Close();

	END_TCHECK("RTextureD3D::Fill / " + GetName());
// 	m_state = RRS_FILLED;
	return true;
}


void RTextureD3D::CheckFailedCreateTexture( HRESULT hr )
{
	if( D3D_OK != hr )
	{
		if( D3DERR_OUTOFVIDEOMEMORY == hr )
		{
			mlog("#Create Texture Faild. Out of Video Memory. (%s)\n", GetName().c_str());
		}
		else
		{
			if( E_OUTOFMEMORY == hr )
				mlog("#Create Texture Faild. Out of Memory. (%s)\n", GetName().c_str());
			else
				mlog("#Create Texture Faild. Invalid Call or Invalid Data. (%s)\n", GetName().c_str());
		}
	}
}

RResource::LOAD_RESULT RTextureD3D::Load()
{
// 	_ASSERT(m_state==RRS_FILLED || m_state==RRS_LOST);

	bool bAutoMipmap = (flags&RCT_AUTOGENMIPMAP ? true : false);
	UINT nMipLevel = (bAutoMipmap ? 0 : 1);

	INIT_TCHECK("/check_texture_filltime");
	if(m_buffer)	// 파일에서 읽은 경우
	{
		BEGIN_TCHECK;

		LPDIRECT3DTEXTURE9 pTexture = NULL;
		D3DXIMAGE_INFO imageInfo;

		UINT uWidth = D3DX_FROM_FILE;
		UINT uHeight = D3DX_FROM_FILE;

		HRESULT hr;

		if( D3D_OK != (hr = D3DXGetImageInfoFromFileInMemory( m_buffer, m_nFileSize, &imageInfo) ) )
		{
			mlog("#Get Texture Image Info Failed. (%s)\n", GetName().c_str());
			SAFE_DELETE_ARRAY( m_buffer );
			return WRONG;
		}

		// 실제 로딩된 이미지 크기
		UINT uLoadedImageWidth = imageInfo.Width;
		UINT uLoadedImageHeight = imageInfo.Height;

		// config 의 texture reduction 옵션에 따라 해상도를 줄인다
		if( !CheckFlags(RCT_UNREDUCIBLE) )
		{
 			uWidth = max(2,imageInfo.Width >> REngine::GetConfig().m_nTextureReduction );
 			uHeight = max(2,imageInfo.Height >> REngine::GetConfig().m_nTextureReduction );

			uLoadedImageWidth = uWidth;
			uLoadedImageHeight = uHeight;
		}


		if( flags & RCT_CUBEMAP )
		{
			LPDIRECT3DCUBETEXTURE9 pCubeTexture;

			if ( D3D_OK != ( hr = D3DXCreateCubeTextureFromFileInMemoryEx(m_pD3DDevice, m_buffer, m_nFileSize, 
				uWidth, nMipLevel, usage, d3dFormatTable[RFMT_NONE], D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,&imageInfo, NULL,&pCubeTexture)) )
			{
				CheckFailedCreateTexture( hr );
				return WRONG;

			}
				
			m_pD3Dtexture = pCubeTexture;
		}
		else
		{
			if( D3D_OK != (hr = D3DXCreateTextureFromFileInMemoryEx( m_pD3DDevice, m_buffer, m_nFileSize
			, uWidth, uHeight, nMipLevel, usage, d3dFormatTable[RFMT_NONE], D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,&imageInfo, NULL,&pTexture)) )
			{
				CheckFailedCreateTexture( hr );				
				return WRONG;
			}
			
			m_pD3Dtexture = pTexture;
		}

		SAFE_DELETE_ARRAY( m_buffer );

		info.nTextureWidth = uLoadedImageWidth;
		info.nTextureHeight = uLoadedImageHeight;
		info.pixelFormat = RDeviceD3D::GetPixelFormat(imageInfo.Format);//pf;	// TODO: 개선 필요
		info.bMipmap = bAutoMipmap;

		pool = D3DPOOL_MANAGED;
		format = imageInfo.Format;

		if ((fmod(MMath::Log2((float)imageInfo.Width), 1.0f)!= 0.0 
			|| fmod(MMath::Log2((float)imageInfo.Height), 1.0f)!= 0.0) // 2제곱 아니면서
			&& bAutoMipmap)	// 밉맵을 사용하려고 한다면
		{
			const char *pWarningMessage = "Warning : 이 텍스쳐는 2 제곱이 아님에도, 밉맵을 생성하려고 합니다.\n";
			mlog(pWarningMessage);
		//	_ASSERT(pWarningMessage==NULL);
		}

		END_TCHECK("RTextureD3D::Load / " + GetName());
	}else	// 빈 텍스쳐를 만드는 경우
	{
		BEGIN_TCHECK;

		// 렌더타켓이나 다이나믹은 default 로 생성
		if((flags&RCT_RENDERTARGET)
			|| (flags&RCT_DYNAMIC))
		{
			// RenderTarget과 Dynamic은 동시에 지정 불가능
			if ( flags&RCT_RENDERTARGET)
				usage |= D3DUSAGE_RENDERTARGET;
			else if (flags&RCT_DYNAMIC)
				usage |= D3DUSAGE_DYNAMIC;

			// Depth Stencil로 사용. 주로 D?포맷을 사용 하지 않는 랜더타겟용 - 090622, OZ
			// 실패하네.. 왜있는거야 이 유세이지는..
			if( flags & RCT_DEPTHSTENCIL)
				usage |= D3DUSAGE_DEPTHSTENCIL;

			pool=D3DPOOL_DEFAULT;

			if(flags&RCT_CUBEMAP)
			{
				nRTSurfaces = 6;
				SAFE_DELETE_ARRAY(rtSurfaces);
				rtSurfaces = new LPDIRECT3DSURFACE9[6];
				ZeroMemory(rtSurfaces, sizeof(LPDIRECT3DSURFACE9)*6);
			}else
			{
				nRTSurfaces = 1;
				SAFE_DELETE_ARRAY(rtSurfaces);
				rtSurfaces = new LPDIRECT3DSURFACE9[1];
				rtSurfaces[0] = NULL;
			}
		}else
		{
			if(flags&RCT_SYSTEMMEM)
				pool=D3DPOOL_SYSTEMMEM;
		}

		format = d3dFormatTable[info.pixelFormat];

		if(format>=D3DFMT_D16_LOCKABLE && format<=D3DFMT_D24FS8 && format!=D3DFMT_L16) // depth stencil
		{
			if (D3D_OK != m_pD3DDevice->CreateDepthStencilSurface(info.nTextureWidth, info.nTextureHeight, format, D3DMULTISAMPLE_NONE, 0, TRUE, rtSurfaces, NULL))
				return WRONG;
		}
		else
		{
			if(flags&RCT_CUBEMAP)
			{
				if(info.nTextureHeight!=info.nTextureWidth)
				{
					mlog("create cubemap error: height!=width\n");
					return WRONG;
				}
				LPDIRECT3DCUBETEXTURE9 pTexture;
				if (m_pD3DDevice->CreateCubeTexture(info.nTextureWidth, nMipLevel, usage, format, pool, (LPDIRECT3DCUBETEXTURE9 *) &pTexture, NULL) != D3D_OK)
					return WRONG;

				for (unsigned int i = 0; i < 6; ++i){
					pTexture->GetCubeMapSurface((D3DCUBEMAP_FACES) i, 0, &rtSurfaces[i]);
				}
				m_pD3Dtexture = pTexture;
			}else
			{
				LPDIRECT3DTEXTURE9 pTexture;	
				if (D3D_OK != m_pD3DDevice->CreateTexture(info.nTextureWidth, info.nTextureHeight, nMipLevel, usage, format, pool,  &pTexture, NULL))
					return WRONG;
				if(rtSurfaces)
				{
					HRESULT hr = pTexture->GetSurfaceLevel(0,rtSurfaces);
					_ASSERT(hr == D3D_OK);
				}
				m_pD3Dtexture = pTexture;
			}
		}

		END_TCHECK("RTextureD3D::Load , empty texture");
	}

// 	m_state = RRS_LOADED;

	return LOADED;
}

bool RTextureD3D::Invalidate()
{
	if(D3DPOOL_DEFAULT!=pool) return true;	// default 만 해당

	if(rtSurfaces) {
		for(int i=0;i<nRTSurfaces;++i) {
			SAFE_RELEASE(rtSurfaces[i]);
		}
		SAFE_DELETE(rtSurfaces);
	}
	SAFE_RELEASE(m_pD3Dtexture);
//	m_state = RRS_LOST;
	ForceSetState( RRS_LOST );
	return true;
}

bool RTextureD3D::Restore()
{
	if(D3DPOOL_DEFAULT==pool)
		return (Load()==LOADED);
	return true;
}


struct RVertexFormat {
	LPDIRECT3DVERTEXDECLARATION9 vDecl;
	unsigned int vertexSize[MAX_VERTEXSTREAM];
};

struct RVertexBuffer {
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	long size;
	DWORD flags;
	int	vertexSize;
	D3DPOOL pool;

	void Invalidate();
	bool Restore(LPDIRECT3DDEVICE9 pd3dDevice);
	bool IsDynamic() { return (flags & D3DUSAGE_DYNAMIC)!=0; }
};

void RVertexBuffer::Invalidate()
{
	if(!IsDynamic()) return;
	SAFE_RELEASE(vertexBuffer);
}

bool RVertexBuffer::Restore(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr;
	if ( (hr = pd3dDevice->CreateVertexBuffer(size, flags, 0, pool, &vertexBuffer, NULL)) != D3D_OK){
		mlog("Vertex buffer creation failed. hr = %x\n", hr);
		return false;
	}
	return true;
}

struct RIndexBuffer {
	LPDIRECT3DINDEXBUFFER9 indexBuffer;
	DWORD nIndices;
	DWORD indexSize;
	DWORD flags;
	D3DPOOL pool;

	void Invalidate();
	bool Restore(LPDIRECT3DDEVICE9 pd3dDevice);
	bool IsDynamic() { return (flags & D3DUSAGE_DYNAMIC)!=0; }
};

void RIndexBuffer::Invalidate()
{
	SAFE_RELEASE(indexBuffer);
}

bool RIndexBuffer::Restore(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (pd3dDevice->CreateIndexBuffer(nIndices * indexSize, flags, indexSize==2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, 
		pool, &indexBuffer, NULL) != D3D_OK)
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// RDeviceD3D

RTextureD3D* RDeviceD3D::GetTexture(RTexture* pTexture)	{ return (RTextureD3D*)pTexture; }

RDeviceD3D::RDeviceD3D() : m_hd3dLibrary(NULL), m_pd3d(NULL), m_pd3dDevice(NULL), m_pFrameBuffer(NULL), m_pDepthStencilBuffer(NULL)
, m_currentFVF(0), m_bInRenderLoop(false)
, m_hWnd( 0 )
{
//	m_dwClearFlag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
	m_pTextureManager = new RTextureManager(*this);	// instance를 device에서 생성하고싶은데 template들때문에.. 후에 device로 옮겨가자.
	m_vDofParams = RVector4( 0,0,0,0);

	//MInitLog(MLOG_LEVEL_INFORMATION, MLOG_FILE|MLOG_DEBUG);
}

RDeviceD3D::~RDeviceD3D()
{
	if (m_hd3dLibrary != NULL)
		Destroy();
	delete m_pTextureManager;						// 후에 device로 옮겨가자.
}

void RDeviceD3D::Destroy()
{
	SAFE_RELEASE(m_pFrameBuffer);
	SAFE_RELEASE(m_pDepthStencilBuffer);

	// device 리소스와 관련된 매니져들 리소스 해제
	if(m_pDefaultTexture)
	{
		DeleteTexture(m_pDefaultTexture);
		m_pDefaultTexture = NULL;
	}

	if(m_pDefaultNoiseTexture)
	{
		DeleteTexture(m_pDefaultNoiseTexture);
		m_pDefaultNoiseTexture = NULL;
	}

	m_pTextureManager->Destroy();

	while(m_VertexFormats.GetCount()) {
		m_VertexFormats[m_VertexFormats.GetFirst()].vDecl->Release();
		m_VertexFormats.Remove(m_VertexFormats.GetFirst());
	}
	while(m_VertexBuffers.GetCount()) {
		DeleteVertexBuffer(m_VertexBuffers.GetFirst());
	}
	while(m_IndexBuffers.GetCount()) {
		DeleteIndexBuffer(m_IndexBuffers.GetFirst());
	}
	while(m_vShaderFXs.size() ) {
		DeleteShaderFX( m_vShaderFXs.front() );
	}

	m_QueryMgr.Release();

	// RefCount를 검사하여, D3D 리소스가 제대로 Release되어 있는지를 알 수 있음.
	if ( m_pd3dDevice )
	{
		if ( m_pd3dDevice->Release() > 0 )
			OutputDebugString(_T("D3D 참조 카운터 0이 아님. 아직 해제되지 않은 리소스 있음\n"));
		m_pd3dDevice = NULL;
	}
	SAFE_RELEASE(m_pd3d);
	FreeLibrary(m_hd3dLibrary);
	m_hd3dLibrary = NULL;
}

bool RDeviceD3D::CreateDirect3D9()
{
	if (!m_pd3d)
	{
		m_hd3dLibrary = LoadLibrary( "d3d9.dll" );

		if (!m_hd3dLibrary)
		{
			mlog_setlasterror("Error, failed to load d3d9.dll.");
			return false;
		}

		typedef IDirect3D9 * (__stdcall *D3DCREATETYPE)(UINT);
		D3DCREATETYPE d3dCreate = (D3DCREATETYPE) GetProcAddress(m_hd3dLibrary, "Direct3DCreate9");

		if (!d3dCreate)
		{
			mlog_setlasterror("Error, failed to get the process adress of Direct3DCreate9.");
			return false;
		}

		//just like pID3D = Direct3DCreate9(D3D_SDK_VERSION);
		m_pd3d = (*d3dCreate)(D3D_SDK_VERSION);

		if (!m_pd3d)
		{
			mlog_setlasterror("Error, failed to create Direct3DCreate9.");
			return false;
		}

		// D3DXEFFECTCOMPILER 리소스가 많은 메모리를 점유하기 때문에 
		// RShaderFX에서 셰이더 컴파일이 끝나면 리소스를 해제하는데
		// 이 때 레퍼런스 카운터가 0이 되어 DLL언로드, 로드가 반복이 되어 부하가 걸린다.
		// 부하 걸리는 걸 막기위해 초기화 할 때 D3DXCreateEffectCompiler()를 한번 호출해준다.
		LPD3DXEFFECTCOMPILER pEffectCompiler;
		D3DXCreateEffectCompiler( "", 1, NULL, NULL, 0, &pEffectCompiler, NULL); // D3DXCreateEffectCompiler가 호출되면 셰이더 컴파일러 DLL이 로드 된다.
	}

	m_pd3d->GetAdapterIdentifier(0,0,&m_deviceID);
	return true;
}

bool RDeviceD3D::QueryFeature(RQUERYFEATURETYPE feature)
{
	switch(feature)
	{
		case RQF_HARDWARETNL : return (m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) != 0;
		case RQF_USERCLIPPLANE : return (m_d3dcaps.MaxUserClipPlanes > 0 );
		case RQF_VS11 : return ( m_d3dcaps.VertexShaderVersion >= D3DVS_VERSION(1, 1));
		case RQF_VS20 : return ( m_d3dcaps.VertexShaderVersion >= D3DVS_VERSION(2, 0));
		case RQF_PS10 : return ( m_d3dcaps.PixelShaderVersion >= D3DPS_VERSION(1, 0));
		case RQF_PS20 : return ( m_d3dcaps.PixelShaderVersion >= D3DPS_VERSION(2, 0));
		case RQF_PS30 : return ( m_d3dcaps.PixelShaderVersion >= D3DPS_VERSION(3, 0));
		// TODO : Device에서 검사하는 걸로 수정할것
		case RQF_R32F :
			return D3D_OK==CheckResourceFormat( D3DFMT_R32F, D3DRTYPE_TEXTURE, D3DUSAGE_RENDERTARGET );
		case RQF_A32B32G32R32F:
			return D3D_OK==CheckResourceFormat( D3DFMT_A32B32G32R32F, D3DRTYPE_TEXTURE, D3DUSAGE_RENDERTARGET );
		case RQF_A16B16G16R16F:
			return D3D_OK==CheckResourceFormat( D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_RENDERTARGET );
		case RQF_R16F:
			return D3D_OK==CheckResourceFormat( D3DFMT_R16F, D3DRTYPE_TEXTURE, D3DUSAGE_RENDERTARGET );
		case RQF_RGB16:
			return D3D_OK==CheckResourceFormat( D3DFMT_R5G6B5, D3DRTYPE_TEXTURE, D3DUSAGE_RENDERTARGET );
		case RQF_G16R16F:
			return D3D_OK==CheckResourceFormat( D3DFMT_G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_RENDERTARGET );
		case RQF_G32R32F:
			return D3D_OK==CheckResourceFormat( D3DFMT_G32R32F, D3DRTYPE_TEXTURE, D3DUSAGE_RENDERTARGET );
		case RQF_VERTEXTEXTURE : 
//			return (m_d3dcaps.VertexTextureFilterCaps & 0x0010000L/*D3DUSAGE_QUERY_VERTEXTEXTURE*/) != 0;
			return D3D_OK==CheckResourceFormat( D3DFMT_R32F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_VERTEXTEXTURE );
		case RQF_HWSHADOWMAP : 
			//////////////////////////////////////////////////////////////////////////
			// "Hardware Shadow Support" means that shadow depth maps are automatically sampled using PCF (Percentage Closer Filtering), 
			//  hardware shadow maps are enabled by creating a texture with a depth format (D16, D24X8, D24S8),
			//  with usage DEPTHSTENCIL set.
			return SUCCEEDED( CheckResourceFormat(D3DFMT_D24S8, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL) );
		case RQF_WFOG : return (m_d3dcaps.RasterCaps & D3DPRASTERCAPS_WFOG ) != 0;
		case RQF_MRTINDEPENDENTBITDEPTHS : return ( m_d3dcaps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS ) != 0;
			// 텍스쳐 필터 지원 여부
			// To check if a format supports texture filter types other than D3DTEXF_POINT (which is always supported), call IDirect3D9::CheckDeviceFormat with D3DUSAGE_QUERY_FILTER.
			// SUCCEEDED 매크로는 너무 너그럽다. 무조건 S_OK만 걸러내자.
		case RQF_RGB16_RTF :
			return  ( S_OK == CheckResourceFormat(D3DFMT_R5G6B5, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER) );
		case RQF_R32F_RTF :
			return ( S_OK == CheckResourceFormat(D3DFMT_R32F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER) );
		case RQF_A8R8G8B8_RTF :
			return ( S_OK == CheckResourceFormat(D3DFMT_A8R8G8B8, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER) );
		case RQF_A32B32G32R32F_RTF :
			return ( S_OK == CheckResourceFormat(D3DFMT_A32B32G32R32F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER) );
		case RQF_A16B16G16R16F_RTF :
			return ( S_OK == CheckResourceFormat(D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER) );
		case RQF_R16F_RTF :
			return ( S_OK == CheckResourceFormat(D3DFMT_R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER) );
		case RQF_G32R32F_RTF :
			return ( S_OK == CheckResourceFormat(D3DFMT_G32R32F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER) );
		case RQF_MRTBLEND_R32F:
			return ( S_OK == CheckResourceFormat(D3DFMT_R32F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING) );
		case RQF_MRTBLEND_G16R16F:
			return ( S_OK == CheckResourceFormat(D3DFMT_G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING) );
		case RQF_MRTBLEND_A8R8G8B8:
			return ( S_OK == CheckResourceFormat(D3DFMT_A8R8G8B8, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING) );

		default : _ASSERT(FALSE);
	}
	return false;
}

void RDeviceD3D::InitPresentParameters()
{
	m_nScreenWidth = REngine::GetConfig().m_nWidth;
	m_nScreenHeight = REngine::GetConfig().m_nHeight;

	m_d3dPixelFormat = REngine::GetConfig().m_bFullScreen ? d3dFormatTable[REngine::GetConfig().pixelFormat] : m_d3ddm.Format;

	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferWidth = REngine::GetConfig().m_nWidth;
	m_d3dpp.BackBufferHeight = REngine::GetConfig().m_nHeight;

	const bool bTripleBuffer = false;
	m_d3dpp.BackBufferCount = bTripleBuffer ? 2 : 1;						// or 2 ??
	m_d3dpp.Windowed   = !REngine::GetConfig().m_bFullScreen;
	m_d3dpp.BackBufferFormat = m_d3dPixelFormat;
	m_d3dpp.EnableAutoDepthStencil = TRUE;

	//m_bStencilBuffer = true;
	//if(FAILED(m_pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_X8R8G8B8,D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,D3DFMT_D24S8)) ||
	//	FAILED(m_pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_R5G6B5,D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,D3DFMT_D24S8))) 
	//{
	//	mlog("Does not provide D24S8 DepthStencil Buffer Format\n");
	//	m_bStencilBuffer = false;
	//}
	//else if( FAILED(m_pd3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_d3dPixelFormat,m_d3dPixelFormat,D3DFMT_D24S8)))
	//{
	//	mlog("D24S8 DepthStencil Buffer Format doesn't match for current display mode\n");
	//	m_bStencilBuffer = false;
	//}
	//if(m_bStencilBuffer)
	//	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	//else

	m_d3dpp.AutoDepthStencilFormat = d3dFormatTable[REngine::GetConfig().depthFormat];

	if ( REngine::GetConfig().m_bFullScreen )
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	else
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

bool RDeviceD3D::Create(HWND hWnd)
{
	if (CreateDirect3D9() == false)
	{
		return false;
	}

	/*
	// 가능한 그래픽카드 디바이스 체크
	if (CheckVideoAdapterSupported()==false)
	{
		if (RError(RERROR_INVALID_DEVICE) != R_OK) return false;
	}
	*/

	m_pd3d->GetDeviceCaps(D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , &m_d3dcaps );
	
	if(QueryFeature(RQF_WFOG)) mlog("WFog Enabled Device.\n");

	m_bSupportVS = ( m_d3dcaps.VertexShaderVersion >= D3DVS_VERSION(1, 1));

	if(!QueryFeature(RQF_VS11)) mlog("Vertex Shader isn't supported\n");
	else
	{
		if( m_d3dcaps.MaxVertexShaderConst < 60 )
		{
			mlog("Too small Constant Number to use Hardware Skinning so Disable Vertex Shader\n");
			m_bSupportVS = false;
		}
	}

	// get screen information
	HRESULT hr;
	hr=m_pd3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &m_d3ddm );

	/*
	m_bFullScreen = REngine::GetConfig().m_bFullScreen;
	m_nScreenWidth = REngine::GetConfig().m_nWidth;
	m_nScreenHeight = REngine::GetConfig().m_nHeight;
	*/
	
	InitPresentParameters();


	DWORD BehaviorFlags=D3DCREATE_FPU_PRESERVE |
		(QueryFeature(RQF_HARDWARETNL) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING);
	//BehaviorFlags=D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DDEVTYPE d3dDevType = D3DDEVTYPE_HAL;

	BehaviorFlags |= D3DCREATE_PUREDEVICE;
	//BehaviorFlags |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT;

#ifdef DEBUG_VS
	if( d3dDevType != D3DDEVTYPE_REF )
	{
		BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		BehaviorFlags &= ~D3DCREATE_PUREDEVICE;                            
		BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
#endif
#ifdef DEBUG_PS
	d3dDevType = D3DDEVTYPE_REF;
#endif

//#ifdef _MT
//	mlog("multithread.\n");
	//BehaviorFlags|=D3DCREATE_MULTITHREADED;
//#endif

#ifndef _NVPERFHUD
	hr = m_pd3d->CreateDevice( D3DADAPTER_DEFAULT, d3dDevType ,hWnd,BehaviorFlags,&m_d3dpp,&m_pd3dDevice);
	if( FAILED( hr ) )
	{
		SAFE_RELEASE(m_pd3d);
		mlog("can't create device\n");
		return false;
	}

#else
	// 디폴트 세팅
	UINT AdapterToUse = D3DADAPTER_DEFAULT;

	// 'NVIDIA NVPerfHUD' 어댑터를 찾아보고, 있으면 디폴트 세팅 대신 그것을 사용한다.
	for( UINT Adapter = 0; Adapter < m_pd3d->GetAdapterCount(); ++Adapter ) 
	{    
		D3DADAPTER_IDENTIFIER9 Identifier;    
		HRESULT Res = m_pd3d->GetAdapterIdentifier( Adapter, 0, &Identifier );    
		if( strstr( Identifier.Description, "PerfHUD" ) != 0 )    
		{        
			AdapterToUse = Adapter;        
			d3dDevType = D3DDEVTYPE_REF;
			mlog("NVPerfHUD Mode\n");
			break;    
		}
	}

	if( FAILED( hr = m_pd3d->CreateDevice( AdapterToUse, d3dDevType, hWnd, BehaviorFlags, &m_d3dpp, &m_pd3dDevice ) ) )
	{ 
		SAFE_RELEASE(m_pd3d);
		mlog_setlasterror("Failed to create Device.\n");
		return false;
	}

#endif	// #ifndef _NVPERFHUD

	// by pok, for texture
	RTextureD3D::m_pD3DDevice = m_pd3dDevice;
//	RTextureD3D::m_pVecPath = &m_pTextureManager->GetPath().GetPathList();

	RD3DQueryManager::m_pD3DDevice = m_pd3dDevice;

	mlog("RDeviceD3D created.\n");

	/*
	if( FAILED( m_pd3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &g_d3ddm ) ) )
		return false;

	hr=m_pd3dDevice->GetDisplayMode( &g_d3ddm );
	m_d3dPixelFormat=g_d3ddm.Format;
	*/

//	if(m_currentConfig.depthFormat == RFMT_D24S8) // stencil 지원 포멧은 아직까지 하나.
//		m_dwClearFlag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
//	else 
//		m_dwClearFlag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;

	SetViewport(0,0,GetScreenWidth(),GetScreenHeight());
	Clear();
		
	m_pd3dDevice->GetRenderTarget(0,&m_pFrameBuffer);
	m_pd3dDevice->GetDepthStencilSurface(&m_pDepthStencilBuffer);

	InitDeviceDefault();

	// default texture set
	DWORD dwDefaultPixelColor = 0xffffffff;
	m_pDefaultTexture = CreateTexture( 1, 1, RFMT_A8R8G8B8, RTF_LINEAR, 0, &dwDefaultPixelColor);

	m_pDefaultNoiseTexture = CreateTexture( "noisemono.jpg", RTF_POINT, 0);

	SetTexture(0, R_NONE);

	RTechnique::m_pDevice = this;
	m_hWnd = hWnd;

	return true;
}

void RDeviceD3D::Clear(bool bTarget, bool bDepth, bool bStencil, DWORD dwColor, float fDepth, DWORD dwStencil, DWORD nIdx)
{
	RPFC_THISFUNC;
	DWORD flag =  (bTarget ? D3DCLEAR_TARGET : 0) |
					(bDepth ? D3DCLEAR_ZBUFFER : 0) |
					(bStencil ? D3DCLEAR_STENCIL : 0);
	m_pd3dDevice->Clear(nIdx, NULL, flag, dwColor, fDepth, dwStencil );
}


void RDeviceD3D::OnSetViewport(const RViewport &vp)
{
	D3DVIEWPORT9 *pViewport=&m_d3dViewport;

	pViewport->X=vp.x;
	pViewport->Y=vp.y;
	pViewport->Width=vp.nWidth;
	pViewport->Height=vp.nHeight;
	pViewport->MinZ=vp.fMinZ;
	pViewport->MaxZ=vp.fMaxZ;
	
	HRESULT hr = m_pd3dDevice->SetViewport(pViewport);
	_ASSERT(hr==D3D_OK);

	/*
	float RSwx=(float)(x2-x1)/2;
	float RSwy=(float)(y2-y1)/2;
	float RScx=(float)RSwx+x1;
	float RScy=(float)RSwy+y1;

	D3DXMatrixIdentity(&RViewport);
	RViewport._11=RSwx;
	RViewport._22=-RSwy;
	RViewport._41=RScx;
	RViewport._42=RScy;
	*/
}

bool RDeviceD3D::BeginScene()
{
	_ASSERT(m_bInRenderLoop == false);
	if (m_pd3dDevice->BeginScene() == D3D_OK) 
	{
		m_bInRenderLoop = true;
		return true;
	}
	return false;
}

void RDeviceD3D::EndScene()
{
	_ASSERT(m_bInRenderLoop);
	m_bInRenderLoop = false;

	m_pd3dDevice->EndScene();
}

void RDeviceD3D::OnFlip()
{
	RPFC_THISFUNC;
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	
	// 펑션을 실행하고 나면 이전세팅이 무효화되는 문제가 있어서 삽입
	SetIndexBuffer(R_NONE);
	SetVertexBuffer(R_NONE,0,0);
	currentVertexFormat = R_NONE;

	RPFC_GPU_CALL;
}

bool RDeviceD3D::DrawIndexedPrimitiveUP(RPRIMITIVETYPE primitiveType, unsigned int nMinVertexIndex, unsigned int nNumVertices, unsigned int nPrimitiveCount, 
										const void* pIndexData, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride, RFORMAT indexFmt )
{
	SetVertexBuffer(R_NONE,0,0);	// 역시 문제가 있어서 삽입.
	SetIndexBuffer(R_NONE);	// 펑션을 실행하고 나면 이전setindices()가 무효화 되는 문제가 있어서 삽입

	m_deviceProfileInfoCurrent.AddDIPUP(nPrimitiveCount);
	return m_pd3dDevice->DrawIndexedPrimitiveUP(d3dPrimitiveTypeTable[primitiveType], nMinVertexIndex, nNumVertices, nPrimitiveCount, pIndexData, d3dFormatTable[indexFmt], pVertexStreamZeroData, VertexStreamZeroStride ) == D3D_OK;
}

bool RDeviceD3D::DrawPrimitiveUP(RPRIMITIVETYPE primitiveType, unsigned int nPrimitiveCount, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride )
{
	RPFC_THISFUNC;
	SetVertexBuffer(R_NONE,0,0);	/// 역시 문제가 있어서 삽입.
	m_deviceProfileInfoCurrent.AddDPUP(nPrimitiveCount);
	return m_pd3dDevice->DrawPrimitiveUP(d3dPrimitiveTypeTable[primitiveType], nPrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride ) == D3D_OK;
}

bool RDeviceD3D::DrawIndexedPrimitive(RPRIMITIVETYPE primitiveType, unsigned int nBaseVertexIndex, unsigned int nMinIndex, unsigned int nNumVertices,
										unsigned int nStartIndex, unsigned int nPrimitiveCount)
{
	RPFC_THISFUNC;
	m_deviceProfileInfoCurrent.AddDIP(nPrimitiveCount, currentInstanceCount);
	return m_pd3dDevice->DrawIndexedPrimitive(d3dPrimitiveTypeTable[primitiveType], nBaseVertexIndex, nMinIndex, nNumVertices, nStartIndex, nPrimitiveCount ) == D3D_OK;
}

bool RDeviceD3D::DrawPrimitive(RPRIMITIVETYPE primitiveType, unsigned int nStart, unsigned int nPrimitiveCount)
{
	RPFC_THISFUNC;
	m_deviceProfileInfoCurrent.AddDP(nPrimitiveCount, currentInstanceCount);
	return m_pd3dDevice->DrawPrimitive(d3dPrimitiveTypeTable[primitiveType], nStart, nPrimitiveCount ) == D3D_OK;
}

void RDeviceD3D::SetTexture(int nStage, RTexture* pTexture)
{
	RPFC_THISFUNC;
	unsigned int nSaveIndex = GetSamplerNumberToSaveIndex(nStage);

	// 중복된 텍스쳐 변환 생략
	if(currentTextures[nSaveIndex]==pTexture) return;
	currentTextures[nSaveIndex]=pTexture;

	if ( pTexture == R_NONE )
		m_pd3dDevice->SetTexture(nStage, NULL );
	else
		m_pd3dDevice->SetTexture(nStage, GetTexture(pTexture)->m_pD3Dtexture);		
}

void RDeviceD3D::SetTextureStageState(int nStage, RTEXTURESTAGESTATETYPE nStageStateType, unsigned int value)
{
	DWORD dwValue = value;

	unsigned int nSaveIndex = GetSamplerNumberToSaveIndex(nStage);
	if(currentTextureStageSettings[nSaveIndex][nStageStateType] != value)
	{
		currentTextureStageSettings[nSaveIndex][nStageStateType] = value;
		if(nStageStateType==RTSS_COLOROP || nStageStateType==RTSS_ALPHAOP)
			dwValue = d3dTextureOPTable[value];
		m_pd3dDevice->SetTextureStageState(nStage, d3dTextureStageStateTypeTable[nStageStateType], dwValue);
	}
}

RFORMAT RDeviceD3D::GetPixelFormat(D3DFORMAT Format)
{
	RFORMAT pf = RFMT_NONE;

	for( int i = 0; i < sizeof(d3dFormatTable)/sizeof(D3DFORMAT); ++i) {
		if(d3dFormatTable[i] == Format)	{
			pf = (RFORMAT)i;
			break;
		}
	}

	return pf;
}

RTexture* RDeviceD3D::NewTexture()
{
	return new RTextureD3D;
}

LPDIRECT3DBASETEXTURE9 RDeviceD3D::GetD3DTexture(RTexture* pTexture)
{
	_ASSERT( NULL != pTexture );
	_ASSERT( R_NONE != pTexture );

	if ( pTexture == R_NONE )
		return NULL;

	LPDIRECT3DBASETEXTURE9 pD3DTexture = GetTexture(pTexture)->m_pD3Dtexture;
	return pD3DTexture;
}

LPDIRECT3DSURFACE9 RDeviceD3D::GetD3DDepthSurface(RTexture* hDepth)
{
	if ( ( GetTexture(hDepth)->format == D3DFMT_D16 )
		|| (GetTexture(hDepth)->format == D3DFMT_D24S8))
	{
		return (*GetTexture(hDepth)->rtSurfaces);
	}
	
	_ASSERT(NULL);
	return NULL;
}

LPDIRECT3DSURFACE9 RDeviceD3D::GetD3DRenderTargetSurface(RTexture* pTexture)
{
	if( GetTexture(pTexture)->nRTSurfaces > 0 )
	{
		return GetTexture(pTexture)->rtSurfaces[0];
	}

	_ASSERT(NULL);
	return NULL;
}

bool RDeviceD3D::GenerateMipmapLevels(RTexture* pTexture)
{
	RPFC_THISFUNC;
	// D3DX 함수를 이용하여 TopLevel 텍스쳐를 기준으로 Mipmap을 생성, 2DTexture에 대해서만 유효
	_ASSERT(GetTexture(pTexture)->m_pD3Dtexture->GetType() == D3DRTYPE_TEXTURE);

	D3DFORMAT textureFormat = GetTexture(pTexture)->format;
	LPDIRECT3DTEXTURE9 pD3DTexture = (LPDIRECT3DTEXTURE9)GetTexture(pTexture)->m_pD3Dtexture;

	DWORD dwLevelCount = pD3DTexture->GetLevelCount();
	DWORD nLevel = 0;
	// Main Surface Update
	LPDIRECT3DSURFACE9 pTopSurface = NULL;
	if ( SUCCEEDED(pD3DTexture->GetSurfaceLevel(nLevel,&pTopSurface)) )
	{
		++nLevel;
		for ( ; nLevel<dwLevelCount; ++nLevel)
		{
			LPDIRECT3DSURFACE9 pSurface = NULL;
			if (SUCCEEDED(pD3DTexture->GetSurfaceLevel(nLevel,&pSurface)) )
			{
				_ASSERT(pSurface!=NULL);
				HRESULT hResult = D3DXLoadSurfaceFromSurface(pSurface, NULL, NULL,pTopSurface, NULL, NULL, D3DX_DEFAULT, 0);
				_ASSERT(SUCCEEDED(hResult));
				
				SAFE_RELEASE(pSurface);
			}
		}

		SAFE_RELEASE(pTopSurface);
	}

	return true;
}

bool RDeviceD3D::LockRect(RTexture* pTex, int nLevel, RLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags )
{
	LPDIRECT3DTEXTURE9 pD3DTexture = (LPDIRECT3DTEXTURE9)GetTexture(pTex)->m_pD3Dtexture;
	_ASSERT(NULL != pD3DTexture);
	D3DLOCKED_RECT rect;
	if( pD3DTexture->LockRect(nLevel,&rect,pRect,Flags) != S_OK )
		return false;

	pLockedRect->pBits = rect.pBits;
	pLockedRect->Pitch = rect.Pitch;

	return true;
}
bool RDeviceD3D::UnlockRect(RTexture* pTex, int nLevel )
{
	LPDIRECT3DTEXTURE9 pD3DTexture = (LPDIRECT3DTEXTURE9)GetTexture(pTex)->m_pD3Dtexture;
	_ASSERT(NULL != pD3DTexture);
	if( pD3DTexture->UnlockRect(nLevel) != S_OK )
		return false;

	return true;
}

void RDeviceD3D::CopyRenderTargetTexture( RTexture* pSrcTexture, RTexture* pDestTexture )
{
	if(GetTexture(pSrcTexture)->nRTSurfaces)	// 렌더타켓이면 복사
	{
		// source surface 를 얻는다
		_ASSERT(GetTexture(pSrcTexture)->m_pD3Dtexture->GetType() == D3DRTYPE_TEXTURE);
		LPDIRECT3DTEXTURE9 pd3dTextureSrc = (LPDIRECT3DTEXTURE9)GetTexture(pSrcTexture)->m_pD3Dtexture;

		_ASSERT(GetTexture(pDestTexture)->m_pD3Dtexture->GetType() == D3DRTYPE_TEXTURE);
		LPDIRECT3DTEXTURE9 pd3dTextureDest = (LPDIRECT3DTEXTURE9)GetTexture(pDestTexture)->m_pD3Dtexture;

		LPDIRECT3DSURFACE9 pDestSurface = NULL;
		HRESULT hr = pd3dTextureDest->GetSurfaceLevel( 0, &pDestSurface );
		_ASSERT( SUCCEEDED(hr) );

		// rendertarget 을 복사한다
		hr = m_pd3dDevice->GetRenderTargetData( GetTexture(pSrcTexture)->rtSurfaces[0], pDestSurface );
		_ASSERT( SUCCEEDED(hr) );
		pDestSurface->Release();
	}
}

bool RDeviceD3D::UpdateTexture(RTexture* pTexture, int nLevel, int nDestX, int nDestY, int nDestWidth, int nDestHeight
							   , int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight, void *pSrcPixels)
{
	RPFC_THISFUNC;

	int nBPP;
	switch(GetTexture(pTexture)->format)
	{
	case D3DFMT_L8 : 
	case D3DFMT_A8 :
		nBPP = 1;break;
	case D3DFMT_R5G6B5 : 
	case D3DFMT_V8U8 : 
		nBPP = 2; break;
	case D3DFMT_A8R8G8B8 : 
		nBPP = 4; break;
	default: _ASSERT(FALSE);
	}

	// 현재 texture9 에 대해서만 사용
	_ASSERT(GetTexture(pTexture)->m_pD3Dtexture->GetType() == D3DRTYPE_TEXTURE);
	int nTextureWidth = GetTextureInfo(pTexture)->nTextureWidth;
	int nTextureHeight = GetTextureInfo(pTexture)->nTextureHeight;

	LPDIRECT3DTEXTURE9 pD3DTexture = (LPDIRECT3DTEXTURE9)GetTexture(pTexture)->m_pD3Dtexture;

	D3DLOCKED_RECT d3dlr;
	HRESULT hr = pD3DTexture->LockRect(nLevel,&d3dlr,NULL,NULL);
	if(hr != D3D_OK) return false;

	for ( int nY = 0; nY<nDestHeight; ++nY )
	{
		for ( int nX = 0; nX<nDestWidth; ++nX )
		{
			// Clamp the src-data
			int x = nSrcX + nX;
			int y = nSrcY + nY;
			
			if ( x < 0 )	x = 0;
			if ( x >= nSrcWidth )	x = nSrcWidth-1;
			if ( y < 0 )	y = 0;
			if ( y >= nSrcHeight )	y = nSrcHeight-1;

			BYTE *pSrcData = (BYTE*)pSrcPixels + y*nSrcWidth*nBPP;
			BYTE *pDestData = (BYTE*)d3dlr.pBits + (nDestY + nY)*d3dlr.Pitch;

			pSrcData += nBPP*x;
			pDestData += nBPP*(nDestX + nX);

			_ASSERT(nTextureHeight>(nY+nDestY));
			_ASSERT(nTextureWidth>(nX+nDestX));
			switch ( nBPP )
			{
			case 4: 
				*pDestData = *pSrcData;
				++pDestData;
				++pSrcData;
				*pDestData = *pSrcData;
				++pDestData;
				++pSrcData;
			case 2:
				*pDestData = *pSrcData;
				++pDestData;
				++pSrcData;
			case 1:
				*pDestData = *pSrcData;
				++pDestData;
				++pSrcData;
			}
		}
	}

	hr = pD3DTexture->UnlockRect(nLevel);
	_ASSERT(hr==D3D_OK);

	return true;
}

bool RDeviceD3D::UpdateTexture(RTexture* pTexture,int nLevel, int nDestX, int nDestY, int nSrcWidth, int nSrcHeight, void *pSrcPixels)
{
	RPFC_THISFUNC;

	int nBPP;
	switch(GetTexture(pTexture)->format)
	{
	case D3DFMT_L8 : 
	case D3DFMT_A8 :
		nBPP = 1;break;
	case D3DFMT_R5G6B5 : 
	case D3DFMT_V8U8 : 
		nBPP = 2; break;
	case D3DFMT_A8R8G8B8 : 
		nBPP = 4; break;
	default: _ASSERT(FALSE);
	}

	// 현재 texture9 에 대해서만 사용
	_ASSERT(GetTexture(pTexture)->m_pD3Dtexture->GetType() == D3DRTYPE_TEXTURE);

	LPDIRECT3DTEXTURE9 pD3DTexture = (LPDIRECT3DTEXTURE9)GetTexture(pTexture)->m_pD3Dtexture;

	D3DLOCKED_RECT d3dlr;
	HRESULT hr = pD3DTexture->LockRect(nLevel,&d3dlr,NULL,NULL);
	if(hr != D3D_OK) return false;

	switch(nBPP)
	{
	case 4: 
		{
			BYTE *pDest = (BYTE*)d3dlr.pBits;
			DWORD *pDestTemp = NULL;
			DWORD *pSrcTemp = NULL;
			for(int i=0;i<nSrcHeight; ++i)
			{
				pDestTemp = (DWORD*)(pDest+((i+nDestY)*d3dlr.Pitch)) + nDestX;
				pSrcTemp = (DWORD*)pSrcPixels+(i*nSrcWidth);
				for(int j=0;j<nSrcWidth; ++j)
				{
					pDestTemp[j] = pSrcTemp[j];
				}
			}
		}break;
	case 2: 
		{
			BYTE *pDest = (BYTE*)d3dlr.pBits;
			SHORT *pDestTemp = NULL;
			SHORT *pSrcTemp = NULL;
			for(int i=0;i<nSrcHeight; ++i)
			{
				pDestTemp = (SHORT*)(pDest+((i+nDestY)*d3dlr.Pitch)) + nDestX;
				pSrcTemp = (SHORT*)pSrcPixels+(i*nSrcWidth);
				for(int j=0;j<nSrcWidth; ++j)
				{
					pDestTemp[j] = pSrcTemp[j];
				}
			}
		}break;
	case 1:
		{
			BYTE *pDest = (BYTE*)d3dlr.pBits;
			BYTE *pDestTemp = NULL;
			BYTE *pSrcTemp = NULL;
			for(int i=0;i<nSrcHeight; ++i)
			{
				pDestTemp = (pDest+((i+nDestY)*d3dlr.Pitch)) + nDestX;
				pSrcTemp = (BYTE*)pSrcPixels+(i*nSrcWidth);
				for(int j=0;j<nSrcWidth; ++j)
				{
					pDestTemp[j] = pSrcTemp[j];
				}
			}
		}break;
	default:
		_ASSERT(false);
	}
	hr = pD3DTexture->UnlockRect(nLevel);
	_ASSERT(hr==D3D_OK);

	return true;
}

bool RDeviceD3D::ColorFill(RTexture* pTexture, DWORD dwColor)
{
	RPFC_THISFUNC;
	HRESULT hr = S_OK;
	PDIRECT3DSURFACE9 pSurface = NULL;

	LPDIRECT3DTEXTURE9 pD3DTexture = (LPDIRECT3DTEXTURE9)GetTexture(pTexture)->m_pD3Dtexture;
	hr = pD3DTexture->GetSurfaceLevel( 0, &pSurface );
	if( SUCCEEDED( hr ) )
		m_pd3dDevice->ColorFill( pSurface, NULL, dwColor );

	SAFE_RELEASE( pSurface );
	return hr == S_OK;
}

RTexture* RDeviceD3D::SetRenderTarget(unsigned int nRenderTargetIndex, RTexture* pTexture, int nSurface )
{
	RPFC_THISFUNC;
	if(currentRenderTarget[nRenderTargetIndex] == pTexture
		&& currentRenderTargetSurface[nRenderTargetIndex] == nSurface ) return pTexture;

	RTexture* hOld = currentRenderTarget[nRenderTargetIndex];
	currentRenderTarget[nRenderTargetIndex] = pTexture;
	currentRenderTargetSurface[nRenderTargetIndex] = nSurface;

	if(pTexture==R_NONE)
		m_pd3dDevice->SetRenderTarget(nRenderTargetIndex,NULL);
	else if(pTexture==RT_FRAMEBUFFER)
		m_pd3dDevice->SetRenderTarget(nRenderTargetIndex,m_pFrameBuffer);
	else
	{
		HRESULT hr = m_pd3dDevice->SetRenderTarget(nRenderTargetIndex,GetTexture(pTexture)->rtSurfaces[nSurface]);
		_ASSERT(hr==D3D_OK);
	}
	return hOld;
}

RTexture* RDeviceD3D::SetDepthStencilBuffer(RTexture* pTexture)
{
	RPFC_THISFUNC;
	if(currentDepthStencilBuffer == pTexture) return pTexture;

	RTexture* hOld = currentDepthStencilBuffer;
	currentDepthStencilBuffer = pTexture;

	if(pTexture==R_NONE)
		m_pd3dDevice->SetDepthStencilSurface(NULL);
	else if(pTexture==RT_FRAMEBUFFER)
		m_pd3dDevice->SetDepthStencilSurface(m_pDepthStencilBuffer);
	else
		m_pd3dDevice->SetDepthStencilSurface(*GetTexture(pTexture)->rtSurfaces);
	return hOld;
}

RTexture* RDeviceD3D::GetRenderTarget(unsigned int nRenderTargetIndex)
{
	return currentRenderTarget[nRenderTargetIndex];
}

RTexture* RDeviceD3D::GetDepthStencilBuffer()
{
	return currentDepthStencilBuffer;
}

HVERTEXBUFFER RDeviceD3D::CreateVertexBuffer(const long size, int nVertexSize, const void *data, unsigned int flags )
{
	RVertexBuffer vb;

	vb.size = size;
	vb.flags = flags;
	vb.vertexSize = nVertexSize;
	vb.pool = vb.IsDynamic() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	if(!vb.Restore(m_pd3dDevice))
		return R_NONE;

	// Upload the provided vertex data if any
	if (data != NULL){
		void *dest;
		if (vb.vertexBuffer->Lock(0, size, &dest, vb.IsDynamic() ? D3DLOCK_DISCARD : 0) == D3D_OK){
			memcpy(dest, data, size);
			vb.vertexBuffer->Unlock();
		} else {
            mlog("Couldn't lock vertex buffer\n");
		}
	}

	return m_VertexBuffers.Add(vb);
}

void RDeviceD3D::DeleteVertexBuffer(HVERTEXBUFFER hVertexBuffer)
{
	if(hVertexBuffer == R_NONE) return;
	
	// 삭제될것이 현재 선택된것이라면, 해제한다
	for(int i=0;i<MAX_VERTEXSTREAM;++i) 
	{
		if(currentVertexBuffer[i] == hVertexBuffer) {
			SetVertexBuffer(R_NONE,i,0);
		}
	}

	SAFE_RELEASE(m_VertexBuffers[hVertexBuffer].vertexBuffer);
	m_VertexBuffers.Remove(hVertexBuffer);
}

void *RDeviceD3D::LockVertexBuffer(const HVERTEXBUFFER hVertexBuffer, const unsigned int flags, const unsigned int nOffsetToLock, const unsigned int nSizeToLock)
{
	void *pointer = NULL;
	HRESULT hr = m_VertexBuffers[hVertexBuffer].vertexBuffer->Lock( nOffsetToLock * m_VertexBuffers[hVertexBuffer].vertexSize, nSizeToLock * m_VertexBuffers[hVertexBuffer].vertexSize, &pointer, flags);
	if(hr!=D3D_OK) return NULL;
	return pointer;
}

bool RDeviceD3D::UnlockVertexBuffer(const HVERTEXBUFFER hVertexBuffer)
{
	return (m_VertexBuffers[hVertexBuffer].vertexBuffer->Unlock() == D3D_OK);
}

void *RDeviceD3D::LockIndexBuffer(const HINDEXBUFFER hIndexBuffer, const unsigned int flags, const unsigned int nOffsetToLock, const unsigned int nSizeToLock)
{
	void *pointer = NULL;

	RIndexBuffer in = m_IndexBuffers[hIndexBuffer];

	HRESULT hr = m_IndexBuffers[hIndexBuffer].indexBuffer->Lock( nOffsetToLock * m_IndexBuffers[hIndexBuffer].indexSize, nSizeToLock * m_IndexBuffers[hIndexBuffer].indexSize, &pointer, flags);
	if(hr!=D3D_OK) return NULL;
	return pointer;
}

bool RDeviceD3D::UnlockIndexBuffer(const HINDEXBUFFER hIndexBuffer)
{
	return (m_IndexBuffers[hIndexBuffer].indexBuffer->Unlock() == D3D_OK);
}

void RDeviceD3D::ShaderOff()
{
	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetPixelShader(NULL);
}

//////////////////////////////////////////////////////////////////////////

// Format lookup table
static const D3DDECLTYPE declTypes[][4] = {
	D3DDECLTYPE_FLOAT1, D3DDECLTYPE_FLOAT2,    D3DDECLTYPE_FLOAT3, D3DDECLTYPE_FLOAT4,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,    D3DDECLTYPE_UNUSED, D3DDECLTYPE_UBYTE4N,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,    D3DDECLTYPE_UNUSED, D3DDECLTYPE_UBYTE4,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_SHORT2N,   D3DDECLTYPE_UNUSED, D3DDECLTYPE_SHORT4N,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_USHORT2N,  D3DDECLTYPE_UNUSED, D3DDECLTYPE_USHORT4N,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_SHORT2,    D3DDECLTYPE_UNUSED, D3DDECLTYPE_SHORT4,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_FLOAT16_2, D3DDECLTYPE_UNUSED, D3DDECLTYPE_FLOAT16_4,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,    D3DDECLTYPE_DEC3N,  D3DDECLTYPE_UNUSED,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,    D3DDECLTYPE_UDEC3,  D3DDECLTYPE_UNUSED,
	D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,    D3DDECLTYPE_UNUSED, D3DDECLTYPE_D3DCOLOR,
};

static const D3DDECLUSAGE declUsages[] = {
	(D3DDECLUSAGE) (-1),
	D3DDECLUSAGE_POSITION,
	D3DDECLUSAGE_NORMAL,
	D3DDECLUSAGE_TEXCOORD,
	D3DDECLUSAGE_COLOR,
	D3DDECLUSAGE_TANGENT,
	D3DDECLUSAGE_BINORMAL,
	D3DDECLUSAGE_POSITIONT,
	D3DDECLUSAGE_BLENDINDICES,
	D3DDECLUSAGE_BLENDWEIGHT,
};

int vfSizes[] = {
	sizeof(float),
	sizeof(unsigned char),
	sizeof(unsigned char),
	sizeof(short),
	sizeof(unsigned short),
	sizeof(short),
	sizeof(unsigned short),	// half
};

// Small common utility function for the vertex format creation functions below
void fillVertexElement(D3DVERTEXELEMENT9 &vElem, RVertexFormat &vf, const int stream, const AttribType type, const AttribFormat format, const int size, const int usageIndex){
	vElem.Stream = stream;
	vElem.Offset = vf.vertexSize[stream];
	vElem.Type = declTypes[format][size - 1];
	vElem.Method = D3DDECLMETHOD_DEFAULT;
	vElem.Usage = declUsages[type];
	vElem.UsageIndex = usageIndex;

	if (format >= FORMAT_DEC){
		vf.vertexSize[stream] += 4;
	} else {
		vf.vertexSize[stream] += size * vfSizes[format];
	}
}

HVERTEXFORMAT RDeviceD3D::CreateVertexFormat(const RVertexAttribute *attribs, const int nAttribs, const int nStream, const int nStartTexCoord)
{
	RVertexFormat vf;
	memset(vf.vertexSize, 0, sizeof(vf.vertexSize));

	D3DVERTEXELEMENT9 *vElem = new D3DVERTEXELEMENT9[nAttribs + 1];

	// Fill the D3DVERTEXELEMENT9 array
	int nTexCoords = nStartTexCoord;
	for (int i = 0; i < nAttribs; ++i)
	{
		fillVertexElement(vElem[i], vf, nStream, attribs[i].type, attribs[i].format, attribs[i].size, nTexCoords);

		if (attribs[i].type == TYPE_TEXCOORD)
			++nTexCoords;
	}
	// Terminating element
	memset(vElem + nAttribs, 0, sizeof(D3DVERTEXELEMENT9));
	vElem[nAttribs].Stream = 0xFF;
	vElem[nAttribs].Type = D3DDECLTYPE_UNUSED;

	HRESULT hr = m_pd3dDevice->CreateVertexDeclaration(vElem, &vf.vDecl);
    delete[] vElem;

	if (hr != D3D_OK){
		mlog("Couldn't create vertex declaration");
		return R_NONE;
	} else {
		return m_VertexFormats.Add(vf);
	}
}

HVERTEXFORMAT RDeviceD3D::CreateVertexFormat(const RVertexAttributeEx *attribs, const int nAttribs)
{
	RVertexFormat vf;
	memset(vf.vertexSize, 0, sizeof(vf.vertexSize));

	D3DVERTEXELEMENT9 *vElem = new D3DVERTEXELEMENT9[nAttribs + 1];

	// Fill the D3DVERTEXELEMENT9 array
	for (int i = 0; i < nAttribs; ++i){
		fillVertexElement(vElem[i], vf, attribs[i].stream, attribs[i].type, attribs[i].format, attribs[i].size, attribs[i].usage);
	}
	// Terminating element
	memset(vElem + nAttribs, 0, sizeof(D3DVERTEXELEMENT9));
	vElem[nAttribs].Stream = 0xFF;
	vElem[nAttribs].Type = D3DDECLTYPE_UNUSED;

	HRESULT hr = m_pd3dDevice->CreateVertexDeclaration(vElem, &vf.vDecl);
	delete[] vElem;

	if (hr != D3D_OK){
		mlog("Couldn't create vertex declaration\n");
		return R_NONE;
	} else {
		return m_VertexFormats.Add(vf);
	}
}

HINDEXBUFFER RDeviceD3D::CreateIndexBuffer(const unsigned int nIndices, RFORMAT inf, const void *data, unsigned int flags) {

	if(inf!=RFMT_INDEX16 && inf!=RFMT_INDEX32) {
		mlog("unknown index buffer format\n");
		return R_NONE;
	}

	RIndexBuffer ib;
	ib.nIndices = nIndices;
	ib.indexSize = (inf == RFMT_INDEX16 ? 2 : 4);
	ib.flags = flags;
	ib.pool = ib.IsDynamic() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	if(!ib.Restore(m_pd3dDevice))
	{
		mlog("Index buffer creation failed\n");
		return R_NONE;
	}

	unsigned int size = ib.nIndices * ib.indexSize;

	// Upload the provided index data if any
	if (data != NULL){
		void *dest;
		if (ib.indexBuffer->Lock(0, size, &dest, ib.IsDynamic()? D3DLOCK_DISCARD : 0) == D3D_OK){
			memcpy(dest, data, size);
			ib.indexBuffer->Unlock();
		} else {
            mlog("Couldn't lock index buffer\n");
		}
	}

	return m_IndexBuffers.Add(ib);
}

void RDeviceD3D::DeleteIndexBuffer(HINDEXBUFFER hIndexBuffer)
{
	if(hIndexBuffer == R_NONE) return;
	
	// 삭제할 것이 현재 선택된 것이다.
	if(hIndexBuffer == currentIndexBuffer)
		SetIndexBuffer(R_NONE);

	SAFE_RELEASE(m_IndexBuffers[hIndexBuffer].indexBuffer);
	m_IndexBuffers.Remove(hIndexBuffer);
}

void RDeviceD3D::SetVertexBuffer(const HVERTEXBUFFER hVertexBuffer, const int stream, const unsigned int offset)
{
	RPFC_THISFUNC;
	if (hVertexBuffer != currentVertexBuffer[stream] || offset != currentOffset[stream])
	{
		if (hVertexBuffer == R_NONE) {
			HRESULT hr = m_pd3dDevice->SetStreamSource(stream, NULL, 0, 0);
			_ASSERT(SUCCEEDED(hr));
		} else {
			int nVertexSize = m_VertexBuffers[hVertexBuffer].vertexSize;
			HRESULT hr = m_pd3dDevice->SetStreamSource(stream, m_VertexBuffers[hVertexBuffer].vertexBuffer, (UINT)offset, nVertexSize);
			_ASSERT(SUCCEEDED(hr));
		}

		currentVertexBuffer[stream] = hVertexBuffer;
		currentOffset[stream] = offset;
	}
}

void RDeviceD3D::SetVertexBufferFreq( UINT stream, UINT FrequencyParameter){
	
	if( FrequencyParameter != currentFrequencyParameter[stream])
	{
		HRESULT hr = m_pd3dDevice->SetStreamSourceFreq( stream, FrequencyParameter);
		_ASSERT(SUCCEEDED(hr));
		currentFrequencyParameter[stream] = FrequencyParameter;
	}
}

void RDeviceD3D::SetVertexFormat(HVERTEXFORMAT hVertexFormat)
{
	RPFC_THISFUNC;
	if (hVertexFormat != currentVertexFormat){
		if (hVertexFormat == R_NONE){
 			//HRESULT hr = m_pd3dDevice->SetVertexDeclaration(NULL);
 			//_ASSERT(hr==D3D_OK);
		} else {
			HRESULT hr = m_pd3dDevice->SetVertexDeclaration(m_VertexFormats[hVertexFormat].vDecl);
			_ASSERT(hr==D3D_OK);
		}

		currentVertexFormat = hVertexFormat;
		m_currentFVF = 0;
	}
}

void RDeviceD3D::SetupForRenderInstancing(const HVERTEXBUFFER hVertexBuffer, UINT nCnt, unsigned int offset)
{
	RPFC_THISFUNC;
	if( hVertexBuffer == R_NONE)
	{
		SetVertexBuffer( R_NONE, 1, 0);
		RestoreForRenderInstancing();
	}
	else
	{
		// 스트림 0의 SetStreamSourceFreq
		SetVertexBufferFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | nCnt);

		// 스트림 1의 버텍스 버퍼
		SetVertexBuffer( hVertexBuffer, 1, offset);
		SetVertexBufferFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);

		currentInstanceCount = nCnt;
	}
	
}

void RDeviceD3D::RestoreForRenderInstancing()
{
	SetVertexBufferFreq( 0, 1);
	SetVertexBufferFreq( 1, 1);

	currentInstanceCount = 1;
}

void RDeviceD3D::SetIndexBuffer(HINDEXBUFFER hIndexBuffer)
{
	RPFC_THISFUNC;
	if (hIndexBuffer != currentIndexBuffer){
		if (hIndexBuffer == R_NONE){
			HRESULT hr = m_pd3dDevice->SetIndices(NULL);
			_ASSERT(hr==D3D_OK);
		} else {
			HRESULT hr = m_pd3dDevice->SetIndices(m_IndexBuffers[hIndexBuffer].indexBuffer);
			_ASSERT(hr==D3D_OK);
		}

		currentIndexBuffer = hIndexBuffer;
	}
}

void RDeviceD3D::SetFvF(DWORD fvf)
{
	if(m_currentFVF!=fvf)
	{
		m_currentFVF = fvf;
		m_pd3dDevice->SetFVF(fvf);
		currentVertexFormat = R_NONE;
	}
}

//void RDeviceD3D::SetClearFlag(DWORD dwClearFlag)
//{
//	m_dwClearFlag = dwClearFlag;
//}

void RDeviceD3D::SetTransform(RSETTRANSFORMTYPE type, const RMatrix &matrix)
{
	D3DTRANSFORMSTATETYPE d3dtransformtypes[] = { D3DTS_WORLD, D3DTS_VIEW, D3DTS_PROJECTION, D3DTS_TEXTURE0, D3DTS_TEXTURE1, D3DTS_TEXTURE2, D3DTS_TEXTURE3 };
	m_pd3dDevice->SetTransform(d3dtransformtypes[type],(D3DMATRIX*)&matrix);
	currentTransform[type] = matrix;
}

RMatrix RDeviceD3D::GetTransform(RSETTRANSFORMTYPE type) const 
{
	return currentTransform[type];
}

void RDeviceD3D::SetBlending(RBLEND srcFactor, RBLEND dstFactor, RBLENDOP blendMode)
{
	RPFC_THISFUNC;
	bool blendEnable = !(dstFactor==RBLEND_NONE && (srcFactor==RBLEND_NONE || srcFactor==RBLEND_ONE));	// none,none or one,none 인경우 blend가아니다

	// Update blend enable state if needed
	if (blendEnable != currentBlendEnable){
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, currentBlendEnable = blendEnable);
	}

	// Update blend factors and blend mode if needed
	if (blendEnable){
		if (srcFactor != currentSrcFactor) 
			m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,  d3dBlendTable[currentSrcFactor = srcFactor]);
		if (dstFactor != currentDstFactor) 
			m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, d3dBlendTable[currentDstFactor = dstFactor]);
		if (blendMode != currentBlendMode) 
			m_pd3dDevice->SetRenderState(D3DRS_BLENDOP,   d3dBlendOPTable[currentBlendMode = blendMode]);
	}
}

void RDeviceD3D::SetSeparateBlending(RBLEND srcFactor, RBLEND dstFactor, RBLENDOP blendMode)
{
	bool blendEnable = !(dstFactor==RBLEND_NONE && (srcFactor==RBLEND_NONE || srcFactor==RBLEND_ONE));	// none,none or one,none 인경우 blend가아니다

	// Update blend enable state if needed
	if (blendEnable != currentSeparateBlendEnable){
		m_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE , currentSeparateBlendEnable = blendEnable);
	}

	// Update blend factors and blend mode if needed
	if (blendEnable){
		if (srcFactor != currentSeparateSrcFactor) 
			m_pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA,  d3dBlendTable[currentSeparateSrcFactor = srcFactor]);
		if (dstFactor != currentSeparateDstFactor) 
			m_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, d3dBlendTable[currentSeparateDstFactor = dstFactor]);
		if (blendMode != currentSeparateBlendMode) 
			m_pd3dDevice->SetRenderState(D3DRS_BLENDOPALPHA,   d3dBlendOPTable[currentSeparateBlendMode = blendMode]);
	}
}

void RDeviceD3D::SetBlendFactor(const DWORD dwColor){
	if (dwColor != currentBlendFactor){
		m_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR, currentBlendFactor = dwColor);
	}
}

void RDeviceD3D::SetTextureFactor(const DWORD dwColor){
	if (dwColor != currentTextureFactor){
		m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, currentTextureFactor = dwColor);
	}
}

void RDeviceD3D::SetDepthFunc(RCMPFUNC depthFunc){
	if (depthFunc != currentDepthFunc){
		m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, d3dCmpFuncTable[currentDepthFunc = depthFunc]);
	}
}

void RDeviceD3D::SetDepthEnable(bool bEnable, bool bWriteEnable)
{
	RPFC_THISFUNC;
	if (bEnable != currentDepthTestEnable){
		m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, currentDepthTestEnable = bEnable);
	}
	if (bWriteEnable != currentDepthWriteEnable){
		m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, currentDepthWriteEnable = bWriteEnable);
	}
}


void RDeviceD3D::SetCullMode(RCULL cullMode){
	if (cullMode != currentCullMode){
		m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, d3dCullTable[currentCullMode = cullMode]);
	}
}


void RDeviceD3D::SetFillMode(RFILLMODE fillMode) {
	if (fillMode != currentFillMode){
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, d3dFillTable[currentFillMode = fillMode]);
	}
}

void RDeviceD3D::SetTextureFilter(int nSampler, RTEXTUREFILTERTYPE type)
{
	RPFC_THISFUNC;
	unsigned int nSaveIndex = GetSamplerNumberToSaveIndex(nSampler);

	if(type != currentTextureFilter[nSaveIndex]){
		if( type < RTF_COUNT)
		{
			/// RTEXTUREFILTERTYPE은 min,mag,mip을 뭉뚱그려 관리된다. RTEXTUREFILTERTYPE 내에서도 각각 중복되는 것이 있는지를 체크하여 셋팅하여야 함.
			if( d3dTextureFilterSets[type].minFilter != d3dTextureFilterSets[currentTextureFilter[nSaveIndex]].minFilter)
				m_pd3dDevice->SetSamplerState(nSampler, D3DSAMP_MINFILTER, d3dTextureFilterSets[type].minFilter);
			if( d3dTextureFilterSets[type].magFilter != d3dTextureFilterSets[currentTextureFilter[nSaveIndex]].magFilter)
				m_pd3dDevice->SetSamplerState(nSampler, D3DSAMP_MAGFILTER, d3dTextureFilterSets[type].magFilter);
			if( d3dTextureFilterSets[type].mipFilter != d3dTextureFilterSets[currentTextureFilter[nSaveIndex]].mipFilter)
				m_pd3dDevice->SetSamplerState(nSampler, D3DSAMP_MIPFILTER, d3dTextureFilterSets[type].mipFilter);
		}
		currentTextureFilter[nSaveIndex] = type;
	}
}

void RDeviceD3D::SetTextureMaxAnisotropy(int nStage, DWORD dwValue)
{
	unsigned int nSaveIndex = GetSamplerNumberToSaveIndex(nStage);
	if(currentMaxAnisotropy[nSaveIndex] != dwValue) {
		m_pd3dDevice->SetSamplerState( nStage, D3DSAMP_MAXANISOTROPY, dwValue);
		currentMaxAnisotropy[nSaveIndex] = dwValue;
	}
}

void RDeviceD3D::SetTextureMipmapLodBias(int nStage,float fBias)
{
	RPFC_THISFUNC;
	unsigned int nSaveIndex = GetSamplerNumberToSaveIndex(nStage);
	if(currentMipmaplodBias[nSaveIndex] != fBias) {
		m_pd3dDevice->SetSamplerState( nStage, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&fBias)) );
		currentMipmaplodBias[nSaveIndex] = fBias;
	}
}

void RDeviceD3D::SetTextureAddress(int nSampler, const RTEXTUREADDRESS u,const RTEXTUREADDRESS v,const RTEXTUREADDRESS w)
{
	unsigned int nSaveIndex = GetSamplerNumberToSaveIndex(nSampler);
	if(u!=currentTextureAddress[nSaveIndex][0])
		m_pd3dDevice->SetSamplerState(nSampler, D3DSAMP_ADDRESSU, d3dTextureAddressTable[currentTextureAddress[nSaveIndex][0] = u]);
	if(v!=currentTextureAddress[nSaveIndex][1])
		m_pd3dDevice->SetSamplerState(nSampler, D3DSAMP_ADDRESSV, d3dTextureAddressTable[currentTextureAddress[nSaveIndex][1] = v]);
	if(w!=currentTextureAddress[nSaveIndex][2])
		m_pd3dDevice->SetSamplerState(nSampler, D3DSAMP_ADDRESSW, d3dTextureAddressTable[currentTextureAddress[nSaveIndex][2] = w]);
}

void RDeviceD3D::SetTextureBorderColor(int nSampler, const DWORD dwColor)
{
	unsigned int nSaveIndex = GetSamplerNumberToSaveIndex(nSampler);
	if( dwColor != currentTextureBorderColor[nSaveIndex])
		m_pd3dDevice->SetSamplerState(nSampler, D3DSAMP_BORDERCOLOR, currentTextureBorderColor[nSaveIndex] = dwColor);
}

void RDeviceD3D::SetClipPlanes(const RPlane *pPlanes, int nCount)
{
	_ASSERT(nCount<=D3DMAXUSERCLIPPLANES);

	DWORD flag;

	if(NULL==pPlanes || 0==nCount)
		flag = 0;
	else
		flag = (1 << nCount) -1;	// dx 문서 참조

	if(flag!=currentClipPlaneEnable)
		m_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, currentClipPlaneEnable = flag);

	for(int i=0;i<nCount;++i) {
		m_pd3dDevice->SetClipPlane(i,(const float*)pPlanes[i]);
	}
}

void RDeviceD3D::SetDepthBias(float fDepthBias, float fSlopeScaleDepthBias)
{
	if( fDepthBias != currentDepthBias)
		m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD *) &fDepthBias);
	if( fSlopeScaleDepthBias != currentSlopeScaleDepthBias)
		m_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *) &fSlopeScaleDepthBias);
	currentDepthBias = fDepthBias;
	currentSlopeScaleDepthBias = fSlopeScaleDepthBias;
}

void RDeviceD3D::SetColorWriteEnable(bool bEnable)
{
	if(bEnable != currentColorWriteEnable) {
		m_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, (currentColorWriteEnable = bEnable) ? 0x0000000F : 0);
	}
}

void RDeviceD3D::SetStencilEnable(bool bEnable)
{
	RPFC_THISFUNC;
	if (bEnable != currentStencilEnable){
		m_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, currentStencilEnable = bEnable);
	}
}

void RDeviceD3D::SetStencilTwoSide(bool bEnable)
{
	m_pd3dDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, bEnable);
}

void RDeviceD3D::SetStencilRef(DWORD dwValue)
{
	RPFC_THISFUNC;
	if ( dwValue != currentStencilRef)
		m_pd3dDevice->SetRenderState(D3DRS_STENCILREF, currentStencilRef = dwValue);
}

void RDeviceD3D::SetStencilMask(DWORD dwValue)
{
	RPFC_THISFUNC;
	if ( dwValue != currentStencilMask)
		m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, currentStencilMask = dwValue);
}

void RDeviceD3D::SetStencilWriteMask(DWORD dwValue)
{
	m_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, dwValue);
}

void RDeviceD3D::SetStencilFunc(RCMPFUNC stencilFunc)
{
	RPFC_THISFUNC;
	if (stencilFunc != currentStencilFunc)
		m_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, d3dCmpFuncTable[currentStencilFunc = stencilFunc]);
}

void RDeviceD3D::SetStencilOp(RSTENCILOP opPass,RSTENCILOP opFail,RSTENCILOP opZFail)
{
	RPFC_THISFUNC;
	if( currentStencilPass != opPass)
		m_pd3dDevice->SetRenderState(D3DRS_STENCILPASS,d3dStencilOpTable[currentStencilPass=opPass]);
	if( currentStencilFail != opFail)
		m_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL,d3dStencilOpTable[currentStencilFail=opFail]);
	if( currentStencilZFail != opZFail)
		m_pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL,d3dStencilOpTable[currentStencilZFail=opZFail]);
}

void RDeviceD3D::SetStencilCCWFunc(RCMPFUNC stencilFunc)	// twoside의 뒷면
{
//	if (stencilFunc != currentStencilBackFunc)
	{
		m_pd3dDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, d3dCmpFuncTable[currentStencilBackFunc = stencilFunc]);
	}
}

void RDeviceD3D::SetStencilCCWOp(RSTENCILOP opPass,RSTENCILOP opFail,RSTENCILOP opZFail)
{
	m_pd3dDevice->SetRenderState(D3DRS_CCW_STENCILPASS,d3dStencilOpTable[opPass]);
	m_pd3dDevice->SetRenderState(D3DRS_CCW_STENCILFAIL,d3dStencilOpTable[opFail]);
	m_pd3dDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL,d3dStencilOpTable[opZFail]);
}

void RDeviceD3D::SetLight(int index,void* pLight)
{
	// void 형이 싫거나 상태비교를 하려면 light 구조체를 다시 만들어서 설정.
	m_pd3dDevice->SetLight(index,(D3DLIGHT9*)pLight);
}

void RDeviceD3D::SetLightEnable(int index,bool bEnable)
{
//	if(currentLighting[index] != bEnable) {
//		currentLighting[index] = bEnable;
		m_pd3dDevice->LightEnable(index,bEnable);
//	}
}

void RDeviceD3D::SetLighting(bool bEnable)
{
	if(currentLighting != bEnable) {
		currentLighting = bEnable;
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, bEnable);
	}
}

void RDeviceD3D::SetMaterial(void* pMtrl)
{
	// void 형이 싫거나 상태비교를 하려면 mtrl 구조체를 다시 만들어서 설정.

	m_pd3dDevice->SetMaterial((D3DMATERIAL9*)pMtrl);
}

void RDeviceD3D::SetNormalizeNormals(bool bEnable)
{
	if(currentNormalizeNormals != bEnable) {
		currentNormalizeNormals = bEnable;
		m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, bEnable);
	}
}

void RDeviceD3D::SetAmbient(DWORD dwColor)
{
	if(currentAmbientColor != dwColor) {
		currentAmbientColor = dwColor;
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwColor );
	}
}

void RDeviceD3D::SetPointSpriteEnable(bool bEnable)
{
	m_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, bEnable);
}

void RDeviceD3D::SetPointSizeAndScale( float fSize, float fMin, float fMax, bool bScaleEnable, float fScaleA, float fScaleB, float fScaleC)
{
	m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE, *(DWORD *)&fSize);
	m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MAX,*(DWORD *)&fMax);
	m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN,*(DWORD *)&fMin );  // Float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering. 
	if (bScaleEnable)
	{
		m_pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
		m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A, *(DWORD *)&fScaleA ); // Default 1.0
		m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B, *(DWORD *)&fScaleB ); // Default 0.0
		m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C, *(DWORD *)&fScaleC );  // Default 0.0
	}
}

//void RDeviceD3D::SetAlphaBlendEnable(bool bEnable)
//{
//	if(currentAlphaBlendEnable != bEnable) {
//		currentAlphaBlendEnable = bEnable;
//		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,bEnable);
//	}
//}
//
void RDeviceD3D::SetAlphaTestEnable(bool bEnable)
{
	if(currentAlphaTestEnable != bEnable) {
		currentAlphaTestEnable = bEnable;
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,bEnable);
	}
}

void RDeviceD3D::SetAlphaRef(DWORD dwRef)
{
	if(currentAlphaRef != dwRef) {
		currentAlphaRef = dwRef;
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, dwRef );
	}
}

void RDeviceD3D::SetAlphaFunc(RCMPFUNC Func)
{
	if(currentAlphaFunc != Func) {
		currentAlphaFunc = Func;
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  d3dCmpFuncTable[Func] );
	}
}

void RDeviceD3D::SetClipping(bool bEnable)
{
	if(currentClipping != bEnable) {
		currentClipping = bEnable;
		m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,bEnable);
	}
}

void RDeviceD3D::SetScissorTestEnable(bool bEnable)
{
	if(currentScissorTestEnable != bEnable) {
		currentScissorTestEnable = bEnable;
		m_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, bEnable ? TRUE : FALSE);
	}
}

void RDeviceD3D::SetScissorRect( const RECT* pRect )
{
	m_pd3dDevice->SetScissorRect(pRect);
}

void RDeviceD3D::SetPointSize(DWORD nSize)
{
	m_pd3dDevice->SetRenderState(D3DRS_POINTSIZE, nSize);
}

void RDeviceD3D::SetFogEnable(bool bEnable)
{
 	if(currentFog != bEnable) {
 		currentFog = bEnable;
		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE , bEnable );
	}
}

void RDeviceD3D::SetVertexBlendEnable(bool bEnable)
{
	if(currentVertexBlendEnable != bEnable ) {
		currentVertexBlendEnable = bEnable;
		m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, bEnable );
	}
}

void RDeviceD3D::SetIndexedVertexBlendEnable(bool bEnable)
{
	if(currentIndexedVertexBlendEnable!=bEnable) {
		currentIndexedVertexBlendEnable = bEnable;
		m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, bEnable );
	}
}

void RDeviceD3D::SetSpecularEnable(bool bEnable)
{
	if(currentSpecularEnable != bEnable) {
		currentSpecularEnable = bEnable;
		m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, bEnable );
	}
}

void RDeviceD3D::SetColorVertex(bool bEnable)
{
	if(currentColorVertexEnable != bEnable) {
		currentColorVertexEnable = bEnable;
		m_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, bEnable );
	}
}

void RDeviceD3D::SetFogColor(DWORD dwColor)
{
	currentFogColor = dwColor;
	m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR,	dwColor);
}

void RDeviceD3D::SetFog(bool bLinearFog, DWORD dwColor, float fNear, float fFar, float fDensity,
						bool bPixelFog, bool bRangeFog)
{
	m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR,	dwColor);

	m_pd3dDevice->SetRenderState(
		bPixelFog ? D3DRS_FOGTABLEMODE : D3DRS_FOGVERTEXMODE,
		bPixelFog ? D3DFOG_EXP : D3DFOG_LINEAR );

	currentFogColor = dwColor;
	if (bLinearFog) 
	{
		currentFogNear = fNear;
		currentFogFar = fFar;

		m_pd3dDevice->SetRenderState(D3DRS_FOGSTART,	*(DWORD*)(&fNear)); 
		m_pd3dDevice->SetRenderState(D3DRS_FOGEND,		*(DWORD*)(&fFar)); 
	}
	else
		m_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)(&fDensity)); 

	m_pd3dDevice->SetRenderState	(D3DRS_RANGEFOGENABLE, ( bRangeFog ? TRUE : FALSE));	
}

void RDeviceD3D::SetFogVertexMode(bool bPixelFog, bool bRangeFog)
{
	if (!bPixelFog && !bRangeFog)
	{
		m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE); 
		m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE); 
	}
	else
		m_pd3dDevice->SetRenderState(
			bPixelFog ? D3DRS_FOGTABLEMODE : D3DRS_FOGVERTEXMODE,
			bRangeFog ? D3DFOG_LINEAR : D3DFOG_EXP); 
}


RDEVICESTATUS RDeviceD3D::QueryStatus()
{
	RPFC_THISFUNC;

	if(!m_pd3dDevice) return RDEVICE_LOST;

	HRESULT hr;
    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
    {
        // If the device was lost, do not render until we get it back
        if( D3DERR_DEVICELOST == hr )
            return RDEVICE_LOST;

        // Check if the device needs to be resized.
        if( D3DERR_DEVICENOTRESET == hr )
			return RDEVICE_RESTORED;
	}
	return RDEVICE_OK;
}

bool RDeviceD3D::ResetDevice()
{
	OnLostDevice();

	InitPresentParameters();

	mlog("RDeviceD3D::ResetDevice\n");

	HRESULT hr = m_pd3dDevice->Reset(&m_d3dpp);
	if(hr != D3D_OK)
	{
		_ASSERT(!"Failed to reset device.");
		return false;
	}
	OnRestoreDevice();
	InitDeviceDefault();

	return true;
}

void RDeviceD3D::OnLostDevice()
{
	SAFE_RELEASE(m_pFrameBuffer);
	SAFE_RELEASE(m_pDepthStencilBuffer);

	m_pTextureManager->Invalidate();

	const list<int>& vbHandles = m_VertexBuffers.GetUsedHandleList();
	for(list<int>::const_iterator i=vbHandles.begin(); i!=vbHandles.end();++i) {
		if(m_VertexBuffers[*i].pool==D3DPOOL_DEFAULT)
			m_VertexBuffers[*i].Invalidate();
	}
	const list<int>& ibHandles = m_IndexBuffers.GetUsedHandleList();
	for(list<int>::const_iterator i=ibHandles.begin(); i!=ibHandles.end();++i) {
		if(m_IndexBuffers[*i].pool==D3DPOOL_DEFAULT)
			m_IndexBuffers[*i].Invalidate();
	}

	for(list<RFont*>::iterator i=m_fonts.begin();i!=m_fonts.end();++i) {
		(*i)->OnLostDevice();
	}

	for(vector<RShaderFX*>::iterator i = m_vShaderFXs.begin(); i!=m_vShaderFXs.end();++i) {
		(*i)->OnLostDevice();
	}

	m_QueryMgr.LostDevice();
	
	
	REngine::BroadcastEvent(RE_DEVICE_LOST);
}

void RDeviceD3D::OnRestoreDevice()
{
	m_pTextureManager->Restore();

	const list<int>& vbHandles = m_VertexBuffers.GetUsedHandleList();
	for(list<int>::const_iterator i=vbHandles.begin(); i!=vbHandles.end();++i) {
		if(m_VertexBuffers[*i].pool==D3DPOOL_DEFAULT)
			m_VertexBuffers[*i].Restore(m_pd3dDevice);
	}
	const list<int>& ibHandles = m_IndexBuffers.GetUsedHandleList();
	for(list<int>::const_iterator i=ibHandles.begin(); i!=ibHandles.end();++i) {
		if(m_IndexBuffers[*i].pool==D3DPOOL_DEFAULT)
			m_IndexBuffers[*i].Restore(m_pd3dDevice);
	}

	for(list<RFont*>::iterator i=m_fonts.begin();i!=m_fonts.end();++i) {
		(*i)->OnResetDevice();
	}

	for(vector<RShaderFX*>::iterator i = m_vShaderFXs.begin(); i!=m_vShaderFXs.end();++i) {
		(*i)->OnResetDevice();
	}

	m_pd3dDevice->GetRenderTarget(0,&m_pFrameBuffer);
	m_pd3dDevice->GetDepthStencilSurface(&m_pDepthStencilBuffer);
	SetViewport(0, 0, GetScreenWidth(), GetScreenHeight());	// ViewPort 갱신

	m_QueryMgr.RestoreDevice();

	REngine::BroadcastEvent(RE_DEVICE_RESTORE);
}

void RDeviceD3D::RegisterFont(RFont* pFont)	// lost 관리를 위해 등록한다
{
	m_fonts.push_back(pFont);
}

void RDeviceD3D::UnregisterFont(RFont* pFont)
{
	list<RFont*>::iterator i=m_fonts.begin();
	while(i!=m_fonts.end()) {
		if(*i==pFont)
			i=m_fonts.erase(i);
		else
			++i;
	}
}

// 
// RShaderFX* RDeviceD3D::CreateShaderFXFromString(const string& strShader, const vector<string>& vShaderDefines /* = vector<string> */, RShaderFXPool *pShaderPool /* = NULL */)
// {
// 	RShaderFX* ret = new RShaderFX(m_pd3dDevice);
// 	ret->SetFromString( strShader, vShaderDefines, pShaderPool );
// 	ret->BuildAll();
// 
// 	if(ret->GetState()==RRS_WRONG)
// 	{
// 		delete ret;
// 		return NULL;
// 	}
// 
// 	// 성공
// 	m_vShaderFXs.push_back(ret);
// 	return ret;
// }

RShaderFX* RDeviceD3D::CreateShaderFX()
{
	RShaderFX* ret = new RShaderFX(m_pd3dDevice);
	m_vShaderFXs.push_back(ret);
	return ret;
}

RShaderFX* RDeviceD3D::CreateShaderFX(const string& strFilename, const vector<string>& vShaderDefines /* = vector<string> */, RShaderFXPool *pShaderPool /* = NULL */)
{

	RShaderFX* ret = new RShaderFX(m_pd3dDevice);
	ret->SetFromFile(strFilename, vShaderDefines, pShaderPool);
	ret->BuildAll();

	if(ret->GetState()==RRS_WRONG)
	{
		delete ret;
		return NULL;
	}

	m_vShaderFXs.push_back(ret);

	return ret;
}

void RDeviceD3D::DeleteShaderFX(RShaderFX* shader)
{
	SAFE_RELEASE(shader->m_pEffect);
	
	vector<RShaderFX*>::iterator i = find(m_vShaderFXs.begin(), m_vShaderFXs.end(), shader);
	_ASSERT(i != m_vShaderFXs.end());
	m_vShaderFXs.erase(i);

	delete shader;
}


RTexture* RDeviceD3D::CreateNormalMapFromHeightMap(RTexture* hHeightMap, float fAttitude)
{
	if ( hHeightMap == R_NONE )		return R_NONE;

	const RTextureInfo *tInfo = GetTextureInfo(hHeightMap);

	RTexture* ret = CreateTexture(tInfo->nTextureHeight, tInfo->nTextureWidth, RFMT_ARGB32);
	if (ret == R_NONE) return ret;

	if (SUCCEEDED(D3DXComputeNormalMap((LPDIRECT3DTEXTURE9)GetD3DTexture(ret), (LPDIRECT3DTEXTURE9)GetD3DTexture(hHeightMap), NULL, 0, D3DX_CHANNEL_RED, fAttitude)))
		return ret;
	else
	{
		RDevice::DeleteTexture(ret);
		return R_NONE;
	}
}

HRESULT RDeviceD3D::CheckResourceFormat(D3DFORMAT fmt, D3DRESOURCETYPE resType, DWORD dwUsage)
{
	HRESULT hr = S_OK;
	IDirect3D9* tempD3D = NULL;
	m_pd3dDevice->GetDirect3D(&tempD3D);
	D3DCAPS9 devCaps;
	m_pd3dDevice->GetDeviceCaps(&devCaps);

	D3DDISPLAYMODE displayMode;
	tempD3D->GetAdapterDisplayMode(devCaps.AdapterOrdinal, &displayMode);

	hr = tempD3D->CheckDeviceFormat(devCaps.AdapterOrdinal, devCaps.DeviceType, displayMode.Format, dwUsage, resType, fmt);

	tempD3D->Release(), tempD3D = NULL;

	return hr;
}

// bool SaveMemoryBmp(int x,int y,void *data,void **retmemory,int *nsize)
// {
// 	unsigned char *memory=NULL,*dest=NULL;
// 
// 	if(!data) return false;
// 
// 	int nBytesPerLine=(x*3+3)/4*4;		// 4 byte align
// 	int nMemoryNeed=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nBytesPerLine*y;
// 	memory=new unsigned char[nMemoryNeed];
// 	if(!memory) return false;
// 
// 	*nsize=nMemoryNeed;
// 	*retmemory=memory;
// 
// 
// 	dest=memory;
// 	BITMAPFILEHEADER *pbmfh=(BITMAPFILEHEADER*)dest;
// 	dest+=sizeof(BITMAPFILEHEADER);
// 	BITMAPINFOHEADER *pbmih=(BITMAPINFOHEADER*)dest;
// 	dest+=sizeof(BITMAPINFOHEADER);
// 
// 	// SET FILE HEADER : 14 byte
// 	pbmfh->bfType = 0x4D42;
// 	pbmfh->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + x*y*3;
// 	pbmfh->bfReserved1 = 0;
// 	pbmfh->bfReserved2 = 0;
// 	pbmfh->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
// 
// 	// SET INFO HEADER : 40 byte
// 	ZeroMemory( pbmih, sizeof(BITMAPINFOHEADER) );
// 	pbmih->biSize = sizeof(BITMAPINFOHEADER);
// 	pbmih->biWidth = x;
// 	pbmih->biHeight = y;
// 	pbmih->biPlanes = 1;
// 	pbmih->biBitCount = 24;
// 	pbmih->biCompression = BI_RGB;
// 	pbmih->biSizeImage = 0;
// 	pbmih->biXPelsPerMeter = 0;
// 	pbmih->biYPelsPerMeter = 0;
// 	pbmih->biClrUsed = 0;
// 	pbmih->biClrImportant = 0;
// 
// 	int i,j;
// 
// 	//	unsigned char *p=(unsigned char*)data+(y-1)*(x*3);
// 	DWORD *p=(DWORD*)data+(y-1)*x;
// 
// 	for(i=y-1;i>=0;i--)
// 	{
// 		for(j=0;j<x;j++)
// 		{
// 			*dest++=*((BYTE*)p+j*4+0);
// 			*dest++=*((BYTE*)p+j*4+1);
// 			*dest++=*((BYTE*)p+j*4+2);
// 		}
// 		if( x*3 % 4 != 0)
// 		{
// 			unsigned char zero[]={0,0,0,0};
// 			memcpy(dest,zero,4 - x*3 % 4);
// 			dest+=4 - x*3 % 4;
// 		}
// 		p-=x;
// 	}
// 
// 	return true;
// }
// 
// bool RSaveAsBmp(int x,int y,void *data,const char *szFilename)
// {
// 	void *memory;
// 	int nSize;
// 
// 	if(!SaveMemoryBmp(x,y,data,&memory,&nSize))
// 		return false;
// 
// 	FILE *file;
// 	file=fopen(szFilename,"wb+");
// 	if(!file) return false;
// 
// 	fwrite( memory, nSize, 1, file );
// 	fclose( file );
// 
// 	delete memory;
// 
// 	return true;
// }

bool RDeviceD3D::SaveScreenShot( const char* szFileName )
{
	if(!szFileName || strlen(szFileName)==0) return false;

	RImage* pImage = GetSaveScreenImage();
	if ( pImage == NULL ) return false;

	bool bSuccess = pImage->Save( szFileName );

	delete pImage;

	return bSuccess;
}

RImage* RDeviceD3D::GetSaveScreenImage()
{
	LPBYTE data = NULL;

	LPDIRECT3DSURFACE9 backbuffer=NULL;
	
	HRESULT hr;

	hr = m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer); 
	if (hr != D3D_OK) return NULL;

	LPDIRECT3DSURFACE9 copybuffer=NULL;

	hr = m_pd3dDevice->CreateOffscreenPlainSurface( m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight ,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&copybuffer ,NULL);
	if (hr != D3D_OK) 
	{
		backbuffer->Release();
		return NULL;
	}

	hr = D3DXLoadSurfaceFromSurface( copybuffer,NULL, NULL, backbuffer, NULL, NULL,  D3DX_FILTER_NONE , 0xff000000 );
	backbuffer->Release();

	if (hr != D3D_OK) return NULL;

	RImageD3D* pImage = new RImageD3D;
	pImage->m_pSurface = copybuffer;

	return pImage;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// From G3D (License : BSD )
//
/**
@file DXCaps.cpp

@created 2006-04-06
@edited  2006-05-06

@maintainer Corey Taylor

Copyright 2000-2006, Morgan McGuire.
All rights reserved.
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// IDirectDraw2 interface pieces required for GetVideoMemorySize()
DEFINE_GUID( CLSID_DirectDraw,                  0xD7B70EE0,0x4340,0x11CF,0xB0,0x63,0x00,0x20,0xAF,0xC2,0xCD,0x35 );
DEFINE_GUID( IID_IDirectDraw2,                  0xB3A6F3E0,0x2B43,0x11CF,0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56 );

/*
* DDSCAPS
*/
typedef struct _DDSCAPS
{
	DWORD       dwCaps;         // capabilities of surface wanted
} DDSCAPS;

typedef DDSCAPS FAR* MYLPDDSCAPS;

#undef INTERFACE
#define INTERFACE IDirectDraw2
DECLARE_INTERFACE_( IDirectDraw2, IUnknown )
{
	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;
	/*** IDirectDraw methods ***/
	STDMETHOD(Compact)(THIS) PURE;
	STDMETHOD(CreateClipper)(THIS_ DWORD, LPVOID FAR*, IUnknown FAR * ) PURE;
	STDMETHOD(CreatePalette)(THIS_ DWORD, LPVOID, LPVOID FAR*, IUnknown FAR * ) PURE;
	STDMETHOD(CreateSurface)(THIS_  LPVOID, LPVOID FAR *, IUnknown FAR *) PURE;
	STDMETHOD(DuplicateSurface)( THIS_ LPVOID, LPVOID FAR * ) PURE;
	STDMETHOD(EnumDisplayModes)( THIS_ DWORD, LPVOID, LPVOID, LPVOID ) PURE;
	STDMETHOD(EnumSurfaces)(THIS_ DWORD, LPVOID, LPVOID,LPVOID ) PURE;
	STDMETHOD(FlipToGDISurface)(THIS) PURE;
	STDMETHOD(GetCaps)( THIS_ LPVOID, LPVOID) PURE;
	STDMETHOD(GetDisplayMode)( THIS_ LPVOID) PURE;
	STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD, LPDWORD ) PURE;
	STDMETHOD(GetGDISurface)(THIS_ LPVOID FAR *) PURE;
	STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD) PURE;
	STDMETHOD(GetScanLine)(THIS_ LPDWORD) PURE;
	STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL ) PURE;
	STDMETHOD(Initialize)(THIS_ GUID FAR *) PURE;
	STDMETHOD(RestoreDisplayMode)(THIS) PURE;
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD) PURE;
	STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD,DWORD, DWORD, DWORD) PURE;
	STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE ) PURE;
	/*** Added in the v2 interface ***/
	STDMETHOD(GetAvailableVidMem)(THIS_ MYLPDDSCAPS, LPDWORD, LPDWORD) PURE;
};

/*
* Indicates that this surface exists in video memory.
*/
#define DDSCAPS_VIDEOMEMORY                     0x00004000l

/*
* Indicates that a video memory surface is resident in true, local video
* memory rather than non-local video memory. If this flag is specified then
* so must DDSCAPS_VIDEOMEMORY. This flag is mutually exclusive with
* DDSCAPS_NONLOCALVIDMEM.
*/
#define DDSCAPS_LOCALVIDMEM                     0x10000000l

DWORD RDeviceD3D::GetVideoMemorySize() {

	static DWORD totalVidSize = 0;
	static bool init = false;

	if (init) {
		return totalVidSize;
	}

	CoInitialize(NULL);
	IDirectDraw2* ddraw = NULL;

	DWORD freeVidSize  = 0;

	if ( !FAILED(CoCreateInstance( CLSID_DirectDraw, NULL, CLSCTX_INPROC_SERVER, IID_IDirectDraw2, reinterpret_cast<LPVOID*>(&ddraw))) ) {

		ddraw->Initialize(0);

		DDSCAPS ddsCaps;
		memset(&ddsCaps, 0, sizeof(DDSCAPS));

		ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;

		if ( FAILED(ddraw->GetAvailableVidMem(&ddsCaps, &totalVidSize, &freeVidSize)) ) {
			totalVidSize = 0;
			freeVidSize  = 0;
		}

		ddraw->Release();
	}

	CoUninitialize();
	init = true;

	return totalVidSize;
}

bool RDeviceD3D::IsCurrentDisplayModeSupportFullScreenMode()
{
	// 현재 엔진은 D3DADAPTER_DEFAULT 디바이스만 사용한다.

	_ASSERT( m_pd3d );

	unsigned int adapterModeCount = m_pd3d->GetAdapterModeCount( D3DADAPTER_DEFAULT, m_d3dpp.BackBufferFormat );
	_ASSERT( adapterModeCount > 0 );

	for( unsigned int i = 0; i < adapterModeCount; ++i )
	{
		D3DDISPLAYMODE enumedDiplayMode;
		m_pd3d->EnumAdapterModes( D3DADAPTER_DEFAULT, m_d3dpp.BackBufferFormat, i, &enumedDiplayMode );

		if( m_d3dpp.BackBufferWidth == enumedDiplayMode.Width &&
			m_d3dpp.BackBufferHeight == enumedDiplayMode.Height  )
		{
			return true;
		}
	}

	return false;
}

unsigned int RDeviceD3D::GetVertexBufferMemoryUsed()
{
	int nBytes = 0;
	const list<int>& vbHandles = m_VertexBuffers.GetUsedHandleList();
	for(list<int>::const_iterator i=vbHandles.begin(); i!=vbHandles.end();++i) {
		nBytes+= m_VertexBuffers[*i].size;
	}
	return nBytes;
}

void RDeviceD3D::OnShowCursor(bool bShow)
{
	m_pd3dDevice->ShowCursor(bShow ? TRUE : FALSE);
}

RImage* RDeviceD3D::CreateImage(int nWidth, int nHeight, RFORMAT format, void* pSrcPixels, int nSourcePitch, int nSrcX, int nSrcY)
{
	if(format!=RFMT_A8R8G8B8)
	{
		// 현재 argb 32 에 대해서만 구현
		_ASSERT(FALSE);
		return NULL;
	}

	RImageD3D* pImage = new RImageD3D;
	pImage->nWidth = nWidth;
	pImage->nHeight = nHeight;
	D3DFORMAT d3dFormat = d3dFormatTable[format];

	LPDIRECT3DSURFACE9 pSurface = NULL;
	m_pd3dDevice->CreateOffscreenPlainSurface(nWidth,nHeight,d3dFormat,D3DPOOL_SYSTEMMEM,&pSurface,NULL);
	pImage->m_pSurface = pSurface;

	int nDestX = 0;
	int nDestY = 0;

	D3DLOCKED_RECT d3dlr;
	HRESULT hr = pSurface->LockRect(&d3dlr,NULL,NULL);
	if(hr != D3D_OK) return false;

	BYTE *pDest = (BYTE*)d3dlr.pBits;
	DWORD *pDestTemp = NULL;
	DWORD *pSrcTemp = NULL;
	for(int i=0;i<nHeight; ++i)
	{
		pDestTemp = (DWORD*)(pDest+((i+nDestY)*d3dlr.Pitch)) + nDestX;
		pSrcTemp = (DWORD*)((BYTE*)pSrcPixels+((i+nSrcY)*nSourcePitch)) + nSrcX;
		for(int j=0;j<nWidth; ++j)
		{
			pDestTemp[j] = pSrcTemp[j];
		}
	}

	hr = pSurface->UnlockRect();
	_ASSERT(hr==D3D_OK);

	return pImage;
}

bool RDeviceD3D::SetCursorProperties(int XHotSpot, int YHotSpot, RImage* pImage)
{
	RImageD3D* pImageD3D = (RImageD3D*)pImage;
	
	HRESULT hr = m_pd3dDevice->SetCursorProperties( XHotSpot, YHotSpot, pImageD3D->m_pSurface);

	ShowCursor(true);
	return hr==D3D_OK;
}

ROcclusionQuery* RDeviceD3D::CreateOcclusionQuery( void )
{
	return (ROcclusionQuery*)m_QueryMgr.CreateQuery(QUERY_OCCLUSION);
}

void RDeviceD3D::ReleaseOcclusionQuery( ROcclusionQuery* pQuery )
{
	m_QueryMgr.ReleaseQuery(pQuery);
}

REventQuery* RDeviceD3D::CreateEventQuery( void )
{
	return (REventQuery*)m_QueryMgr.CreateQuery(QUERY_EVENT);
}

void RDeviceD3D::ReleaseEventQuery( REventQuery* pEventQuery )
{
	m_QueryMgr.ReleaseQuery(pEventQuery);
}

// Profile Marker
void RDeviceD3D::SetProfileMarker( LPCWSTR wszName)
{
#ifdef _NVPERFHUD
	D3DPERF_SetMarker( 0, wszName);
#endif
}

void RDeviceD3D::BeginProfileMarker( LPCWSTR wszName)
{
#ifdef _NVPERFHUD
	D3DPERF_BeginEvent( 0, wszName);
#endif
}

void RDeviceD3D::EndProfileMarker()
{
#ifdef _NVPERFHUD
	D3DPERF_EndEvent();
#endif
}

}
