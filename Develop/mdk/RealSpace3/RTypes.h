#ifndef __RTYPES_H
#define __RTYPES_H

// 대부분의 파일이 이 파일을 include 하므로, 되도록 의존성을 만들지 말것.

#include "MTypes.h"
#include "MMath.h"
#include "RLibTypes.h"

//using namespace std;

namespace rs3 {

class RViewport
{
public:
    DWORD       x;
    DWORD       y;            /* Viewport Top left */
    DWORD       nWidth;
    DWORD       nHeight;       /* Viewport Dimensions */
    float       fMinZ;         /* Min/max of clip Volume */
    float       fMaxZ;

	RViewport()
	{
		x			= 0;
		y			= 0;      
		nWidth		= 0;
		nHeight		= 0;
		fMinZ		= 0.f;  
		fMaxZ		= 0.f;
	}
	RViewport(int _x, int _y, int _nWidth, int _nHeight, float _fMinZ = 0, float _fMaxZ = 1.f)
		: x(_x), y(_y), nWidth(_nWidth), nHeight(_nHeight), fMinZ(_fMinZ), fMaxZ(_fMaxZ) { }
};

enum RDEVICESTATUS {
	RDEVICE_OK,
	RDEVICE_LOST,
	RDEVICE_RESTORED,
};
// rdeviced3d 에 정의되어있는 포맷들에 대응. D3DFORMAT과 순서가 맞아야 합니다.
enum RFORMAT {
	RFMT_NONE = 0,
    RFMT_RGB16,
	RFMT_RGB24,
	RFMT_XRGB32,
	RFMT_ARGB32,

	RFMT_A8,
	RFMT_A8R8G8B8,

	RFMT_A2R10G10B10,
	RFMT_G16R16,

	// 실수형 포맷
	RFMT_R16F,
	RFMT_G16R16F,
	RFMT_G32R32F,
	RFMT_R32F,
	RFMT_A16B16G16R16F,
	RFMT_A32B32G32R32F,

	// depth stencil buffer 용
	RFMT_D24S8,
	RFMT_D24X8,
	RFMT_D16,
	RFMT_D24FS8,
	RFMT_D32F_LOCKABLE,
	RFMT_D32,

	RFMT_L8,		// 8-bit luminance only.

	// index buffer 포맷
	RFMT_INDEX16,
	RFMT_INDEX32,

	// 범프맵 포맷
	RFMT_Q8W8V8U8,
	RFMT_V16U16,
	RFMT_V8U8,

	// 압축텍스쳐 포맷
	RFMT_DXT1,
	RFMT_DXT3,
	RFMT_DXT5,

	//TODO: HDR을 위한 포맷추가
	RFMT_END,
};

enum RQUERYFEATURETYPE {
	RQF_HARDWARETNL=0,
	RQF_USERCLIPPLANE,
	RQF_WFOG,
	RQF_VS11,
	RQF_VS20,
	RQF_PS10,
	RQF_PS11,
	RQF_PS20,
	RQF_PS30,
	RQF_R32F,
	RQF_A32B32G32R32F,
	RQF_A16B16G16R16F,
	RQF_R16F,
	RQF_RGB16,
	RQF_G16R16F,
	RQF_G32R32F,
	RQF_VERTEXTEXTURE,				// VS30의 Vertex Texture 기능이 지원 되는가
	RQF_HWSHADOWMAP,				// 새도우맵에서 하드웨어 PCF (Percentage Closer Filtering) 샘플링 지원
	RQF_MRTINDEPENDENTBITDEPTHS,	// 일부 카드에서 R32F 포멧에 대해 POST PIXEL SHADER BLENDING(디더링, 알파 테스트, 포그(안개:fog), 블렌드, 래스터 처리, 마스킹)이 안먹힘. 그에대한 지원 유무 http://telnet.or.kr/sec_directx/index.html?init_mode=api_contents_read&api_no=60
	// 텍스쳐 필터 지원 여부
	// To check if a format supports texture filter types other than D3DTEXF_POINT (which is always supported), call IDirect3D9::CheckDeviceFormat with D3DUSAGE_QUERY_FILTER.
	RQF_RGB16_RTF,
	RQF_R32F_RTF,
	RQF_A8R8G8B8_RTF,					// RFMT_A8R8G8B8
	RQF_A32B32G32R32F_RTF,
	RQF_A16B16G16R16F_RTF,
	RQF_R16F_RTF,
	RQF_G32R32F_RTF,
	RQF_MRTBLEND_R32F,
	RQF_MRTBLEND_G16R16F,
	RQF_MRTBLEND_A8R8G8B8,
};

enum RTEXTUREFILTERTYPE {
	// Single image filters
	RTF_POINT = 0,
	RTF_LINEAR,
	// Mipmapped image filters
	RTF_BILINEAR,
	RTF_TRILINEAR,
	// Mipmapped image filters with anisotropic
	RTF_BILINEAR_ANISO,
	RTF_TRILINEAR_ANISO,
	// min - aniso, mag - linear  for RFont
	RTF_BILINEAR_ANISO_LINEAR_FOR_FONT,
	// attribute means count of filter type for ignore saving value of filter
	RTF_COUNT,
};

enum RSETTRANSFORMTYPE {
	RST_WORLD = 0,
	RST_VIEW,
	RST_PROJECTION,
	RST_TEXTURE0,
	RST_TEXTURE1,
	RST_TEXTURE2,
	RST_TEXTURE3,

	RST_MAX
};


////////////////////////////////////////////
// 매크로들 & RELEASE & DELETE 매크로 ( from dxutil.h )
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#define TOLER 0.001f

#define IS_ZERO(a) ((fabs(a) < TOLER))

#define IS_VALID(a) ((a) != R_NONE)


/*
typedef int HTEXTURE;
typedef int HVERTEXFORMAT;
typedef int HVERTEXBUFFER;
typedef int HINDEXBUFFER;
typedef int HSHADER;

struct RTexture;
struct RVertexFormat;
struct RVertexBuffer;
struct RIndexBuffer;
class  RShader;
*/

/**
    Vertex attribute enumeration.  These values are used as input to
    constructing vertex or stream attributes; they map 1:1 with, for example,
    D3DDECLUSAGE_<USAGE> in Direct3D and gl<ARRAY>Pointer() in OpenGL.
*/
enum AttribType {
	TYPE_GENERIC  = 0,
	TYPE_VERTEX   = 1,  /**< POSITION */
	TYPE_NORMAL   = 2,  /**< NORMAL */
	TYPE_TEXCOORD = 3,  /**< TEXCOORD<i>n</i> */
	TYPE_COLOR    = 4,  /**< COLOR<i>n</i> */
	TYPE_TANGENT  = 5,  /**< TANGENT */
	TYPE_BINORMAL = 6,  /**< BINORMAL */
	TYPE_POSITIONT = 7,  /**< POSITION x,y,z,rhw */
	TYPE_BLENDINDICES = 8,
	TYPE_BLENDWEIGHT  = 9,
};

/**
    Vertex attribute format enumeration.  These values are used as input to
    constructing vertex or stream attributes; they map 1:1 with, for example,
    D3DDECLTYPE_<FORMAT> in Direct3D and the 'type' parameter to gl<ARRAY>Pointer()
	in OpenGL.
*/
enum AttribFormat {
	FORMAT_FLOAT    = 0, /**< FLOAT<i>n</i> */
	FORMAT_UBYTE    = 1, /**< UBYTE */
	FORMAT_UBYTEUN  = 2, /**< UBYTEUN */
	FORMAT_SHORT    = 3, /**< SHORT */
	FORMAT_USHORT   = 4, /**< USHORT */
	FORMAT_SHORTUN  = 5, /**< SHORTUN */
	FORMAT_HALF     = 6, /**< HALF */
	FORMAT_DEC      = 7, /**< DEC */
	FORMAT_UDECUN   = 8, /**< UDECUN */
	FORMAT_D3DCOLOR = 9, /**< D3DCOLOR */
};

struct RVertexAttribute {
	AttribType type;        /**< Specifies the vertex type. */
	AttribFormat format;    /**< Specifies the vertex format (float, bool, int, etc). */
	int size;               /**< Specifies the vertex format size (3 for (type)3, for example, where type is float and size is 3, the attribute is a float3). */
};

struct RVertexAttributeEx {
	RVertexAttributeEx()
		: stream(0), type(TYPE_GENERIC), usage(0), format(FORMAT_FLOAT), size(0) { }
	RVertexAttributeEx( int s, AttribType t, int u, AttribFormat f, int z)
		: stream(s), type(t), usage(u), format(f), size(z) { }
	int stream;				// stream
	AttribType type;        /**< Specifies the vertex type. */
	int usage;               // usage
	AttribFormat format;    /**< Specifies the vertex format (float, bool, int, etc). */
	int size;               /**< Specifies the vertex format size (3 for (type)3, for example, where type is float and size is 3, the attribute is a float3). */
};

// 극좌표계 좌표
struct RS_API RPolarCoord
{
	float fRadius;
	float fTheta;
	float fPhi;
};

struct RCULLDESC
{
	MPlane* pPlanes;
	int		nPlanes;

	void SetDesc(MPlane* pPlaneList, int nPlaneNum)
	{
		pPlanes =pPlaneList;
		nPlanes =nPlaneNum;
	}
};

//////////////////////////////////////////////////
// 기본 타입들

class RTexture;

typedef MPlane			RPlane;
typedef MQuat			RQuaternion;
typedef MDQuat			RDQuaternion;
typedef MMatrix			RMatrix;
typedef MMatrixA16		RMatrixA16;
typedef MVector2		RVector2;
typedef MVector3		RVector3,	RVector;
typedef MVector4		RVector4;
typedef MBox			RBox,		RBoundingBox	,RAABB;
typedef MOBox			ROBB;
typedef MSphere			RSphere,	RBoundingSphere;
typedef MCapsule		RCapsule;
typedef MLine			RLine;


#define	ALIGNED16_MALLOC(p)		{ (p) = (RMatrixA16*)_aligned_malloc(sizeof(RMatrixA16), 16); }
#define	ALIGNED16_FREE(p)		{ if(p) { _aligned_free(p); (p)=NULL; } }

typedef int RHANDLE;
//typedef RHANDLE HTEXTURE;
typedef RHANDLE HVERTEXFORMAT;
typedef RHANDLE HVERTEXBUFFER;
typedef RHANDLE HINDEXBUFFER;
typedef RHANDLE HSHADER;
//typedef int RBACKGOUNDWORKTICKET;


//typedef RTexture* HTEXTURE;	//  HTEXTURE 컴파일안되는것이 있으면 이것을 써보세요

const int	R_NONE = 0;				// RS3 에서 관리하는 리소스들의 핸들이 없을때.
extern RTexture*	RT_FRAMEBUFFER;	// RenderTarget 기본 프레임버퍼
const int	RT_DEFAULT = -2;
const int	R_SAMPLER_INDEX_INVALID = -1;
const int	R_UNKNOWN	= -1;

#define MAX_PIXELSAMPLER	16
#define MAX_VERTEXSAMPLER	4			// VS 3.0에서 사용하는 버텍스 샘플러 최대 수
#define MAX_IMAGEUNIT		MAX_PIXELSAMPLER+MAX_VERTEXSAMPLER			// 최대 텍스쳐 유닛 수 + VERTEXSAMPLER 수
#define MAX_VERTEXSTREAM	4			// 최대 버텍스 스트림 수

// 클래스 미리 선언

//class RBoundingBox;
class RDevice;
class RSceneManager;
class RResource;
class RFont;
class RCollision;
class RMaterial;

// device 클래스들 미리선언
class RTexture;
struct RVertexFormat;
struct RVertexBuffer;
struct RIndexBuffer;
class  RShader;

};



// 전역 클래스 미리선언
class MFile;
class MFileSystem;

class TiXmlElement;
typedef TiXmlElement		MXmlElement;


// 속성플래그를 가지는 템플릿 클래스
template < class _TYPE > 
	class RHaveAttributes 
	{
	protected:
		DWORD m_attributes;
	public:
		RHaveAttributes() : m_attributes(0) {}
		void AddAttribute(_TYPE _eAttribute)	{	m_attributes |= _eAttribute;	}
		void RemoveAttribute(_TYPE _eAttribute)	{	m_attributes &= ~_eAttribute;	}
		bool QueryAttribute(_TYPE _eAttribute) const {
			if ( (m_attributes & _eAttribute) == _eAttribute )
				return true;
			return false;
		}
	};

#endif
