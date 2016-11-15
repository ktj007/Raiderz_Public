#pragma once

#include "MResource.h"


namespace mint3
{

class MBitmap;
class MAnimationTemplate;
class MDrawContext;
class MCursorSystem;



/// Type of cursor
enum MCURSORTYPE
{
	MCT_NONE,
	MCT_BITMAP,
	MCT_ANIBITMAP
};




/// class : MCursor
class MCursor : public MResource
{
protected:
	friend class MCursorSystem;


protected:
	MBitmap*				m_pBitmap;
	MAnimationTemplate*		m_pAnimationTemplate;
	int						m_nSceneNum;
	MPOINT					m_ptHotSpot;
	DWORD					m_dwTimer;


public:
	MCursor( const char* szName);
	virtual ~MCursor()											{ Release();				}

	virtual bool Release()										{ return true;				}

	void	SetBitmap( MBitmap* pBitmap);
	void	SetAnimationTemplate( MAnimationTemplate* pAnimationTemplate);

	void	SetHotSpot( const MPOINT& hotspot)					{ m_ptHotSpot = hotspot;		}
	MPOINT	GetHotSpot() const									{ return m_ptHotSpot;		}

protected:
	void Draw( MDrawContext* pDC, int x, int y);


protected:
	virtual void OnDraw( MDrawContext* pDC, int x, int y)		{}
	virtual void OnSet()										{}
	virtual void OnSetBitmap( MBitmap* pBitmap)					{}
	virtual void OnSetAnimationTemplate( MAnimationTemplate* pAnimationTemplate)	{}
};





/// class : MDefaultCursor
class MDefaultCursor : public MCursor
{
public:
	MDefaultCursor( const char* szName);

protected:
	virtual void OnDraw( MDrawContext* pDC, int x, int y);
};


}
