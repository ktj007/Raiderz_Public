#ifndef MINT4R3_H
#define MINT4R3_H

#include "Mint.h"
#include "MDrawContext.h"
#include "MCursor.h"
#include "RDevice.h"
#include "RDeviceD3D.h"
#include "RTypes.h"
#include "RTexture.h"
#include "RFont.h"
#include <D3dx9.h>
using namespace mint3;
using namespace rs3;




template<typename VERTEX_FORMAT>
class MSharedVertexBuffer;//전방선언

struct CUSTOMVERTEX;
struct MASKVERTEX;
struct PIXEL2DVERTEX;

class MDrawContextR3 : public MDrawContext
{
protected:
	static rs3::RDevice*	m_pDevice;
	static MDrawState		m_eCurrentState;
	static MDrawEffect		m_eCurrentEffect;
	static MBitmap*			m_pCurrentMaskBitmap;
	bool					m_bInDraw;


public:
	MDrawContextR3( rs3::RDevice* pDevice);
	virtual ~MDrawContextR3(void);

	static rs3::RDevice* GetDevice()		{ return m_pDevice;				}

public:
	MSharedVertexBuffer<CUSTOMVERTEX>*	GetSharedCustomVertexBuffer(void)	{	return m_pSharedCustomVertexBuffer; }
	MSharedVertexBuffer<MASKVERTEX>*	GetSharedMaskVertexBuffer(void)		{	return m_pSharedMaskVertexBuffer; }
	MSharedVertexBuffer<PIXEL2DVERTEX>*	GetSharedPixel2dVertexBuffer(void)	{	return m_pSharedPixel2dVertexBuffer; }


/// Override functions
protected:
	// Basic Drawing Functions
	virtual void overrideBeginDraw();
	virtual void overrideEndDraw();
	virtual void overrideSetState( MDrawState eState, MDrawEffect eEffect);
	virtual void overrideSetPixel( int x, int y);
	virtual void overrideLine( int sx, int sy, int ex, int ey);
	virtual void overrideRectangle( int x, int y, int w, int h);
	virtual void overrideFillRectangle( int x, int y, int w, int h);
	virtual void overrideClipRect( int x, int y, int w, int h);
	virtual void overrideDraw( int tx, int ty, int tw, int th, int sx, int sy, int sw, int sh);
	virtual void overrideDrawRotate( int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh);
	virtual void overrideText( int x, int y, const char* szText);
	virtual void overrideFlushFont();

private:
	MSharedVertexBuffer<CUSTOMVERTEX>*	m_pSharedCustomVertexBuffer;
	MSharedVertexBuffer<MASKVERTEX>*	m_pSharedMaskVertexBuffer;
	MSharedVertexBuffer<PIXEL2DVERTEX>*	m_pSharedPixel2dVertexBuffer;
};




/// class : MBitmapR3
class MBitmapR3 : public MBitmap
{
private:
	rs3::RDevice*		m_pDevice;
	rs3::RTexture*		m_hTexture;
	int					m_nWidth;
	int					m_nHeight;
	float				m_fTexCoord[ 4];


public:
	MBitmapR3();
	virtual ~MBitmapR3();

	bool Create( rs3::RDevice* pDevice, rs3::RTexture* pTexture);
	virtual bool Create( rs3::RDevice* pDevice, const char* szAliasName, const char* szFileName);

	virtual bool CreateTexture();
	virtual bool Destroy();

	rs3::RDevice* GetDevice()								{ return m_pDevice;				}

	rs3::RTexture* GetTexture()
	{ 
		if (m_hTexture == NULL)
		{
			if (m_bCaching && CreateTexture() == false)
			{
				m_bCaching = false;
			}
		}
		else					ResetTime();

		return m_hTexture;
	}

	virtual int GetWidth()
	{ 
		ResetTime();
		return m_nWidth;
	}
	virtual int GetHeight()
	{
		ResetTime();
		return m_nHeight;
	}

	virtual unsigned int GetDrawMode()
	{
		ResetTime();
		return m_DrawMode;
	}

	virtual void SetTexCoord( float x1, float y1, float x2, float y2);
	virtual void GetTexCoord( float* pTexCoord);


protected:
	void BeginState( MDrawEffect effect);
	void EndState();

	void CalcSizeFromTexCoord();
	void ResetTime();
};







/// class : MFontR3
class MFontR3 : public MFont
{
protected:
	rs3::RFont*		m_pFont;


public:
	MFontR3();
	virtual ~MFontR3();

	virtual bool Create( rs3::RDevice* pDevice, const char* szName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f);
	virtual bool Destroy();

	virtual int GetWidth( const char* szText, int nLen =-1);
	virtual int GetHeight();

	rs3::RFont* GetFont()				{ return m_pFont;			}
};







/// class : MCursorR3
class MCursorR3 : public MCursor
{
protected:
	rs3::RDeviceD3D*		m_pDevice;
	vector< RImage*>		m_cursorImages;
	int						m_nCurrnetImage;


public:
	MCursorR3( RDeviceD3D* pDevice, const char* szName);
	virtual ~MCursorR3();

	virtual bool Release();


protected:
	virtual void OnDraw( MDrawContext* pDC, int x, int y);
	virtual void OnSet();
	virtual void OnSetBitmap( MBitmap* pBitmap);
	virtual void OnSetAnimationTemplate( MAnimationTemplate* pAnimationTemplate);
};


template<typename VERTEX_FORMAT>
class MSharedVertexBuffer
{
public:
	MSharedVertexBuffer(void);
	~MSharedVertexBuffer(void);

public:
	int		Render(RPRIMITIVETYPE primitiveType, unsigned int nPrimitiveCount,void* pVertex, unsigned int VertexFormatSize,int TheNumberOfVertexData);

private:
	bool	AddVertex(VERTEX_FORMAT* pVertex,int TheNumberOfVBData);

private:
	rs3::HVERTEXBUFFER		m_SharedVB;
	static const int		m_VBSizeLimit = 512;

	int						m_NonusedVBDataOffset;		//D3DLOCK_NOOVERWRITE 사용시에 gpu가 사용하고 있지 않는 버퍼의 메모리영역 시작주소
	int						m_CurrentUsingVBOffset;	//현재 락을 걸어서 버텍스데이타를 카피한 주소. 렌더링시에 사용됨
	int						m_CurrentVertexWriteCount;
};



//////////////////////////////////////////////////////////////////////////
// MSharedVertexBuffer

template<typename VERTEX_FORMAT>
MSharedVertexBuffer<VERTEX_FORMAT>::MSharedVertexBuffer(void)
:m_SharedVB(R_NONE),m_CurrentUsingVBOffset(0),m_NonusedVBDataOffset(0),m_CurrentVertexWriteCount(0)
{
	m_SharedVB = REngine::GetDevice().CreateVertexBuffer( m_VBSizeLimit,sizeof(VERTEX_FORMAT), NULL, (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY));
}

template<typename VERTEX_FORMAT>
MSharedVertexBuffer<VERTEX_FORMAT>::~MSharedVertexBuffer( void)
{
	if( m_SharedVB != R_NONE)
	{
		REngine::GetDevice().DeleteVertexBuffer( m_SharedVB);
		m_SharedVB = R_NONE;
	}
}
template<typename VERTEX_FORMAT>
int MSharedVertexBuffer<VERTEX_FORMAT>::Render( RPRIMITIVETYPE primitiveType, unsigned int nPrimitiveCount,void* pVertex, unsigned int VertexFormatSize,int TheNumberOfVertexData )
{
	if( AddVertex((VERTEX_FORMAT*)pVertex,TheNumberOfVertexData) )
	{
		REngine::GetDevice().SetVertexBuffer( m_SharedVB, 0, 0);
		return REngine::GetDevice().DrawPrimitive( primitiveType, m_CurrentUsingVBOffset / sizeof(VERTEX_FORMAT) , nPrimitiveCount);
	}
	return 0;
}




template<typename VERTEX_FORMAT>
bool MSharedVertexBuffer<VERTEX_FORMAT>::AddVertex( VERTEX_FORMAT* pVertex,int TheNumberOfVBData )
{
	int TotalSizeOfData = TheNumberOfVBData * sizeof(VERTEX_FORMAT);	//데이타 크기
	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;

	if( m_NonusedVBDataOffset + TotalSizeOfData >= m_VBSizeLimit)
	{
		// No space remains. Start over from the beginning 
		//   of the vertex buffer.
		dwLockFlags = D3DLOCK_DISCARD;
		m_NonusedVBDataOffset = 0;
	}

	VERTEX_FORMAT *pVideoVertex = (VERTEX_FORMAT*)REngine::GetDevice().LockVertexBuffer( m_SharedVB, dwLockFlags, m_NonusedVBDataOffset / sizeof(VERTEX_FORMAT), TheNumberOfVBData);
	if( pVideoVertex)
	{
		memcpy( pVideoVertex, pVertex, TotalSizeOfData );
		REngine::GetDevice().UnlockVertexBuffer(m_SharedVB);
		m_CurrentVertexWriteCount = TheNumberOfVBData;

		m_CurrentUsingVBOffset = m_NonusedVBDataOffset;			
		m_NonusedVBDataOffset+=TotalSizeOfData;					
		return true;
	}

	m_CurrentVertexWriteCount = 0;
	return false;
}


/// class : Mint4R3
class Mint4R3 : public Mint
{
public:
	Mint4R3()		{}

	virtual MBitmap* NewBitmap( const char* szName, const char* szFileName);
	virtual MFont* NewFont( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f);
	virtual MFont* NewFontMemory( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f);
	virtual MCursor* NewCursor( const char* pszAliasName);
};

#endif