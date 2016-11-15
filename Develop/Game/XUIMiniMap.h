#pragma once

#include "MWidget.h"


using namespace std;
using namespace mint3;


/// class : XMiniMapLook
class XMiniMapLook : public MLookBase
{
/// Member variables
protected:
	MFont*		m_pSmallFont;
	MBitmap*	m_pMaskImage;


/// Member functions
public:
	XMiniMapLook();

	void SetSmallFont( const char* szFont);
	MFont* GetSmallFont()							{ return ( m_pSmallFont ? m_pSmallFont : GetFont());	}

	void SetMaskImage( const char* szImage);
	MBitmap* GetMaskImage()							{ return m_pMaskImage;									}

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;
};






/// class : XUIMiniMap
class XUIMiniMap : public MWidget
{
protected:
	int				m_nFieldID;
	wstring			m_strFieldName;
	int				m_nCurFloor;
	MBitmapR3*		m_pMapImage;
	MPOINT			m_ptLastCursorPos;
	float			m_fMinX, m_fMinY, m_fMaxX, m_fMaxY;
	float			m_fScaleBegin, m_fScaleEnd;
	bool			m_bRotate;
	MTimer			m_tZoom;
	DWORD			m_dwUpdateTimer;
	bool			m_bDynamicField;
	int				m_nArtisanFieldID;
	int				m_nArtisanID;


public:
	XUIMiniMap( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XUIMiniMap();

	int GetFieldID() const								{ return m_nFieldID;				}
	const wchar_t* GetFieldName() const					{ return m_strFieldName.c_str();	}
	int GetCurrentFloor() const							{ return m_nCurFloor;				}
	MBitmap* GetMapImage() const						{ return m_pMapImage;				}
	void GetMapSize( float& minx, float& miny, float& maxx, float& maxy) const;
	MRectF GetBoundaryBoxRect() const;

	MPOINT GetLastCursorPos()							{ return m_ptLastCursorPos;			}

	void SetScale( float fScale);
	float GetScale();
	void ZoomIn();
	void ZoomOut();

	void RotateMap( bool bRotate);
	bool IsRotateMap();
	bool IsDynamicField()	{ return m_bDynamicField; }

	bool UpdateInfo();

	void SetMapImage( wchar_t * szBuff );

	static void SetMissionTarget( int nNpcID);
	static void SetMissionTargetPos( vec3* vPos);
	static bool GetMissionTarget( string* strName, vec3* vPos);

	void AddMarkerArtisan( int nRecipeID, int nIndex);
	bool GetMarkerArtisanPos( vec3& vPos) const;


protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;


public:
	const wchar_t* glueGetFieldName()					{ return GetFieldName();			}
	void glueSetScale( float fScale)					{ SetScale( fScale);				}
	float glueGetScale()								{ return GetScale();				}
	void glueZoomIn()									{ ZoomIn();							}
	void glueZoomOut()									{ ZoomOut();						}
	void glueRotateMap( bool bRotate)					{ RotateMap( bRotate);				}
	bool glueIsRotateMap()								{ return IsRotateMap();				}
	void glueUpdateInfo()								{ UpdateInfo();						}
	void glueAddMarkerArtisan( int nRecipeID, int nIndex)	{ AddMarkerArtisan( nRecipeID, nIndex);		}



#define MINT_MINIMAP		"MiniMap"
	virtual const char* GetClassName() override			{ return MINT_MINIMAP;				}
};