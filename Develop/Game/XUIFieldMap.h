#pragma once

#include "MWidget.h"


using namespace std;
using namespace mint3;


class XUIFieldMap;


/// class : XFieldMapLook
class XFieldMapLook : public MLookBase
{
/// Member variables
protected:
	MFont*		m_pLargeFont;


/// Member functions
public:
	XFieldMapLook();

	void SetLargeFont( const char* szFont);
	MFont* GetLargeFont()							{ return ( m_pLargeFont ? m_pLargeFont : GetFont());	}

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;

private:

	void DrawMap( MBitmap* pBitmap, MDrawContext* pDC, MRECT &r, float _scale, MPOINT &_pos, float _minx, float _rw, MPOINT &_drag, float _miny, float _rh, MPOINT _cp, float _srw, float _srh, bool _boundary, bool bDynamicField, MRectF _bdrect);
	void DrawScaler( MDrawContext* pDC, XUIFieldMap* pFieldMap, float _scale );	
	void DrawFloorButtons( MDrawContext* pDC, XUIFieldMap* pFieldMap);
};






/// class : XUIFieldMap
class XUIFieldMap : public MWidget
{
protected:
	XFieldInfo*		m_pFieldInfo;
	int				m_nFieldID;
	wstring			m_strFieldName;
	int				m_nCurFloor;
	int				m_nShowFloor;
	int				m_nSelFloor;
	MBitmapR3*		m_pMapImage;
	float			m_fMinX, m_fMinY, m_fMaxX, m_fMaxY;
	int				m_nClickRgn;
	MPOINT			m_ptLastCursorPos;
	float			m_ptDragOffset[ 2];
	MPOINT			m_ptLButtonPos;
	bool			m_bFullScale;
	float			m_fScaleBegin, m_fScaleEnd;
	float			m_fFullScale;
	MTimer			m_tZoom;
	DWORD			m_dwUpdateTimer;
	bool			m_bDynamicField;
	int				m_nArtisanFieldID;
	int				m_nArtisanID;
	vec3			m_vArtisanPos;
	bool			m_bUpdateDragOffsetToArtisan;


public:
	XUIFieldMap( const char* szName =NULL, MWidget* pParent =NULL, MListener* zpListener =NULL);
	virtual ~XUIFieldMap();

	XFieldInfo* GetFieldInfo()							{ return m_pFieldInfo;				}
	int GetFieldID() const								{ return m_nFieldID;				}
	const wchar_t* GetFieldName() const					{ return m_strFieldName.c_str();	}
	MBitmap* GetMapImage() const						{ return m_pMapImage;				}
	void GetMapSize( float& minx, float& miny, float& maxx, float& maxy) const;
	MRectF GetBoundaryBoxRect() const;

	MPOINT GetLastCursorPos()							{ return m_ptLastCursorPos;			}
	MPOINT GetDragOffset();
	void ResetDragOffset();
	
	void SetScale( float fScale);
	float GetScale();
	void SetFullScale( bool bFullScale);
	bool IsFullScale() const							{ return m_bFullScale;				}
	void ZoomIn();
	void ZoomOut();

	bool UpdateInfo( int nFieldID);

	void SetMapImage( wchar_t * szBuff );

	int GetFloors() const;
	int GetCurFloor() const								{ return m_nCurFloor;				}
	int GetShowFloor() const							{ return m_nShowFloor;				}
	int GetSelFloor() const								{ return m_nSelFloor;				}

	bool IsDynamicField()								{ return m_bDynamicField;			}

	void AddMarkerArtisan( int nRecipeID, int nIndex);
	bool GetMarkerArtisanPos( vec3& vPos) const;

protected:
	int FloorFromPoint( const MPOINT& pt);
	void MoveMyPlayerForTest( const MPOINT ptPos);
	MPOINT ConvertToWorldPos( const MPOINT ptEventPos );

protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;


public:
	const wchar_t* glueGetFieldName()					{ return GetFieldName();			}
	void glueSetScale( float fScale)					{ SetScale( fScale);				}
	float glueGetScale()								{ return GetScale();				}
	void glueFullScale( bool bFullScale)				{ SetFullScale( bFullScale);		}
	bool glueIsFullScale()								{ return IsFullScale();				}
	void glueZoomIn()									{ ZoomIn();							}
	void glueZoomOut()									{ ZoomOut();						}
	void glueUpdateInfo( int nFieldID)					{ UpdateInfo( nFieldID);			}
	void glueResetDragOffset()							{ ResetDragOffset();				}
	void glueAddMarkerArtisan( int nRecipeID, int nIndex)	{ AddMarkerArtisan( nRecipeID, nIndex);		}



#define MINT_FIELDMAP		"FieldMap"
	virtual const char* GetClassName() override			{ return MINT_FIELDMAP;				}	
};