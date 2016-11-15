#include "stdafx.h"
#include "XUIFieldMap.h"
#include "XUIMiniMap.h"
#include "XGame.h"
#include "XWorld.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XFieldInfo.h"
#include "XCameraManager.h"
#include "XUIJournalIndicator.h"
#include "RDevice.h"
#include "XFieldInfoHelper.h"
#include "XRecipeNPCInfo.h"
#include "XRecipeNPCInfoMgr.h"
#include "XPost_GM.h"
#include "XInteractionHelper.h"

#define DEFAULT_SCALE					1.0f
#define DEFAULT_SCALEMIN				0.5f
#define DEFAULT_SCALEMAX				2.0f
#define DEFAULT_FIELDMAPNAME			"__fieldmapname"


enum
{
	CLICKRGN_NONE				= -1,
	CLICKRGN_FLOOR				= 0,
	CLICKRGN_FULLSCALE,
	CLICKRGN_SCALE,
	CLICKRGN_MAP
};


// XUIFieldMap
XUIFieldMap::XUIFieldMap( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( XFieldMapLook);

	m_pFieldInfo = NULL;
	m_nFieldID = 0;
	m_nCurFloor = 0;
	m_nShowFloor = 0;
	m_nSelFloor = -1;
	m_pMapImage = NULL;
	m_fScaleBegin = DEFAULT_SCALE;
	m_fScaleEnd = DEFAULT_SCALE;
	m_fFullScale = DEFAULT_SCALE;
	m_nClickRgn = CLICKRGN_NONE;
	m_ptDragOffset[ 0] = 0.0f;
	m_ptDragOffset[ 1] = 0.0f;
	m_bFullScale = false;
	m_tZoom.SetArrivalTime( 500);
	m_dwUpdateTimer = 0;
	m_bDynamicField = false;
	m_nArtisanFieldID = 0;
	m_nArtisanID = 0;
	m_vArtisanPos = vec3::ZERO;
	m_bUpdateDragOffsetToArtisan = true;

	SetEnableFocus( true);
}


// ~XUIFieldMap
XUIFieldMap::~XUIFieldMap()
{
	m_nFieldID = 0;
	m_bDynamicField = false;

	if ( m_pFieldInfo != NULL)
	{
		//delete m_pFieldInfo;
		m_pFieldInfo = NULL;
	}

	if ( m_pMapImage != NULL)
	{
		m_pMapImage->Destroy();
		delete m_pMapImage;
		m_pMapImage = NULL;
	}
}


// GetMapSize
void XUIFieldMap::GetMapSize( float& minx, float& miny, float& maxx, float& maxy) const
{
	minx = m_fMinX;
	miny = m_fMinY;
	maxx = m_fMaxX;
	maxy = m_fMaxY;
}


// GetBoundaryBoxRect
MRectF XUIFieldMap::GetBoundaryBoxRect() const
{
	if ( m_pFieldInfo == NULL)		return MRectF( 0.0f, 0.0f, 0.0f, 0.0f);
	return m_pFieldInfo->m_rtBoundary;
}


// GetDragOffset
MPOINT XUIFieldMap::GetDragOffset()
{
	if ( m_bFullScale == true)
		return MPOINT( 0, 0);

	float fScale = m_fScaleBegin + (m_fScaleEnd - m_fScaleBegin) * m_tZoom.GetRatio();
	return MPOINT( (int)m_ptDragOffset[ 0] * fScale, (int)m_ptDragOffset[ 1] * fScale);
}


// ResetDragOffset
void XUIFieldMap::ResetDragOffset()
{
	m_ptDragOffset[ 0] = 0.0f;
	m_ptDragOffset[ 1] = 0.0f;
}


// SetScale
void XUIFieldMap::SetScale( float fScale)
{
	if ( m_bFullScale == true)		return;

	if ( fScale < DEFAULT_SCALEMIN)		fScale = DEFAULT_SCALEMIN;
	if ( fScale > DEFAULT_SCALEMAX)		fScale = DEFAULT_SCALEMAX;
	if ( fScale == m_fScaleEnd)		return;

	m_fScaleBegin = GetScale();
	m_fScaleEnd = fScale;

	m_tZoom.Start();
}


// SetFullScale
void XUIFieldMap::SetFullScale( bool bFullScale)
{
	if ( m_bFullScale == bFullScale)	return;
	m_bFullScale = bFullScale;


	float _scale = DEFAULT_SCALE;
	if ( m_bFullScale == true)
	{
		if ( m_pMapImage != NULL)
		{
			MRECT r = GetClientRect();

			// 먼저 width에 대해서 스케일링을 해보고 높이가 벗어나면 높이에 대한 스케일을 구한다
			_scale = (float)r.w / (float)m_pMapImage->GetWidth();

			if ( (m_pMapImage->GetHeight() * _scale) > r.h)
				_scale = (float)r.h / (float)m_pMapImage->GetHeight();

			m_fFullScale = _scale;
		}
	}
}


// GetScale
float XUIFieldMap::GetScale()
{
	if ( m_bFullScale == true)
		return m_fFullScale;

	return ( m_fScaleBegin + (m_fScaleEnd - m_fScaleBegin) * m_tZoom.GetRatio());
}


// ZoomIn
void XUIFieldMap::ZoomIn()
{
	if ( m_bFullScale == true)		return;

	SetScale( min( DEFAULT_SCALEMAX, m_fScaleEnd * 1.2f));
}


// ZoomOut
void XUIFieldMap::ZoomOut()
{
	if ( m_bFullScale == true)		return;

	SetScale( max( DEFAULT_SCALEMIN, m_fScaleEnd * 0.8f));
}


// UpdateInfo
bool XUIFieldMap::UpdateInfo( int nFieldID)
{
	if ( gg.currentgamestate == NULL  ||
		gg.currentgamestate->GetWorld() == NULL ||
		gg.currentgamestate->GetWorld()->GetInfo() == NULL)
	{
		m_nFieldID = 0;
		m_strFieldName.clear();
		m_nCurFloor = 0;
		m_nShowFloor = 0;
		m_nSelFloor = -1;
		m_nClickRgn = CLICKRGN_NONE;
		m_bDynamicField = false;

		if ( m_pFieldInfo != NULL)
		{
			//delete m_pFieldInfo;
			m_pFieldInfo = NULL;
		}

		if ( m_pMapImage != NULL)
		{
			m_pMapImage->Destroy();
			delete m_pMapImage;
			m_pMapImage = NULL;
		}

		return false;
	}


	if ( nFieldID <= 0  || (nFieldID == m_nFieldID && m_nCurFloor == m_nShowFloor))		return false;


	// Field info
	m_nFieldID = 0;
	m_strFieldName.clear();
	m_nCurFloor = 0;
	m_nShowFloor = 0;
	m_nSelFloor = -1;
	m_fMinX = 0.0f;
	m_fMinY = 0.0f;
	m_fMaxX = 0.0f;
	m_fMaxY = 0.0f;
	m_ptDragOffset[ 0] = 0.0f;
	m_ptDragOffset[ 1] = 0.0f;
	m_nClickRgn = CLICKRGN_NONE;
	m_bDynamicField = false;

	if ( m_pFieldInfo != NULL)
	{
		//delete m_pFieldInfo;
		m_pFieldInfo = NULL;
	}

	m_pFieldInfo = info.field->GetSimpleFieldInfo( nFieldID);
	if ( m_pFieldInfo == NULL)		return false;


	// Set map info
	m_nFieldID = m_pFieldInfo->m_nFieldID;
	m_strFieldName = m_pFieldInfo->GetName();
	m_fMinX = m_pFieldInfo->m_fMinX;
	m_fMinY = m_pFieldInfo->m_fMinY;
	m_fMaxX = m_pFieldInfo->m_fMaxX;
	m_fMaxY = m_pFieldInfo->m_fMaxY;

	// IsDynamicField
	CSFieldFileInfo* pFieldFileInfo = info.field->GetFieldList().Find( m_nFieldID);
	if ( pFieldFileInfo)	m_bDynamicField = pFieldFileInfo->bDynamic;	

	wchar_t szBuff[ MAX_PATH];
	swprintf_s( szBuff, L"%smap%05d.jpg", PATH_UI_MINIMAP, m_pFieldInfo->m_nZoneID);

	// 층별 정보가 있는지 확인...
	if ( m_pFieldInfo->m_vFloors.empty() == false)
	{
		// 현재 로드된 필드와 내 캐릭터가 있는 필드가 같은지 확인해서 현재 위치한 층을 구함
		if ( gg.currentgamestate  &&  gg.currentgamestate->GetWorld()  &&  gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID == m_pFieldInfo->m_nFieldID)
		{
			XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
			if ( pMyPlayer != NULL)
			{
				int nHeight = pMyPlayer->GetPosition().z;

				size_t _size = m_pFieldInfo->m_vFloors.size();
				for ( size_t i = 0;  i < _size;  i++)
				{
					if ( nHeight < m_pFieldInfo->m_vFloors[ i])
					{
						m_nCurFloor = i;
						m_nShowFloor = i;
						swprintf_s( szBuff, L"%smap%05d_%d.jpg", PATH_UI_MINIMAP, m_pFieldInfo->m_nZoneID, (i + 1) );
						break;
					}
				}
			}
		}

		// 아니면 1층...
		else
		{
			swprintf_s( szBuff, L"%smap%05d_%d.jpg", PATH_UI_MINIMAP, m_pFieldInfo->m_nZoneID, (m_nCurFloor + 1) );
		}
	}
	
	SetMapImage(szBuff);


	// Set scale
	if ( m_bFullScale == true)
	{
		float _scale = DEFAULT_SCALE;
		if ( m_bFullScale == true  &&  m_pMapImage != NULL)
		{
			MRECT r = GetClientRect();

			// 먼저 width에 대해서 스케일링을 해보고 높이가 벗어나면 높이에 대한 스케일링을 시도한다
			_scale = (float)r.w / (float)m_pMapImage->GetWidth();

			if ( (m_pMapImage->GetHeight() * _scale) > r.h)
				_scale = (float)r.h / (float)m_pMapImage->GetHeight();
		}

		m_fFullScale = _scale;
	}

	return true;
}


// GetFloors
int XUIFieldMap::GetFloors() const
{
	return ( m_pFieldInfo  ?  m_pFieldInfo->m_vFloors.size()  :  1);
}


// AddMarkerArtisan
void XUIFieldMap::AddMarkerArtisan( int nRecipeID, int nIndex)
{
	if ( nRecipeID <= 0  ||  nIndex < 0)
	{
		m_nArtisanFieldID = 0;
		m_nArtisanID = 0;
		m_vArtisanPos = vec3::ZERO;
		return;
	}


	XRecipeNPCInfo* _info = info.recipenpc->Get( nRecipeID);
	if ( _info == NULL)
	{
		m_nArtisanFieldID = 0;
		m_nArtisanID = 0;
		m_vArtisanPos = vec3::ZERO;
		return;
	}

	m_nArtisanFieldID = info.recipenpc->GetNPCFieldID( nRecipeID, nIndex);;
	m_nArtisanID = _info->m_vecFieldNPC[ nIndex].second;
	m_vArtisanPos = vec3::ZERO;

	m_bUpdateDragOffsetToArtisan = true;
	m_dwUpdateTimer -= 3000;
}


// GetMarkerArtisanPos
bool XUIFieldMap::GetMarkerArtisanPos( vec3& vPos) const
{
	if ( m_nFieldID != m_nArtisanFieldID)
		return false;

	vPos = m_vArtisanPos;
	return true;
}


// FloorFromPoint
int XUIFieldMap::FloorFromPoint( const MPOINT& pt)
{
	if ( m_pFieldInfo == NULL)		return -1;

	size_t nFloors = m_pFieldInfo->m_vFloors.size();
	if ( nFloors == 0)				return -1;

	int y = 3;
	for ( size_t i = 0;  i < nFloors;  i++)
	{
		char szBuff[ 256];
		sprintf_s( szBuff, "%dF", i);

		if ( MRECT( 3, y, 32, 24).InPoint( (MPOINT)pt) == true)
			return i;

		y += 27;
	}

	return -1;
}


// OnEvent
bool XUIFieldMap::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)	return true;

	// Translate message
	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN :
		{
			if (MEvent::GetAltState())
			{
				MoveMyPlayerForTest(pEvent->Pos);
				return true;
			}

			int nFloor = FloorFromPoint( pEvent->Pos);
			if ( nFloor >= 0)
			{
				m_nClickRgn = CLICKRGN_FLOOR;
				m_nSelFloor = nFloor;

				if ( nFloor != m_nShowFloor)
				{
					m_nShowFloor = nFloor;

					wchar_t szBuff[ MAX_PATH];
					swprintf_s( szBuff, L"%smap%05d_%d.jpg", PATH_UI_MINIMAP, m_pFieldInfo->m_nZoneID, (m_nShowFloor + 1) );

					SetMapImage(szBuff);
				}
			}

			else if ( MRECT( m_Rect.w - 20, 3, 16, 16).InPoint( (MPOINT)pEvent->Pos) == true)
			{
				m_nClickRgn = CLICKRGN_FULLSCALE;

				SetFullScale( !m_bFullScale);
			}

			else if ( MRECT( m_Rect.w - 20, 25, 16, 32).InPoint( (MPOINT)pEvent->Pos) == true)
			{
				m_nClickRgn = CLICKRGN_SCALE;

				float fRatio = 1.0f - (float)min( 32, max( 0, pEvent->Pos.y - 25)) / 32.0f;
				float fScale = DEFAULT_SCALEMIN + (DEFAULT_SCALEMAX - DEFAULT_SCALEMIN) * fRatio;
				m_fScaleBegin = fScale;
				m_fScaleEnd = fScale;
			}

			else if ( m_bFullScale == false)
			{
				m_nClickRgn = CLICKRGN_MAP;

				m_ptLButtonPos = pEvent->Pos;
			}

			else
				m_nClickRgn = CLICKRGN_NONE;

			break;
		}

	case MWM_LBUTTONUP :
		{
			m_nClickRgn = CLICKRGN_NONE;
			break;
		}

	case MWM_LBUTTONDBLCLK :
		{
			m_nClickRgn = CLICKRGN_NONE;
			m_ptDragOffset[ 0] = 0.0f;
			m_ptDragOffset[ 1] = 0.0f;

			break;
		}

	case MWM_MOUSEMOVE :
		{
			m_ptLastCursorPos = pEvent->Pos;

			if ( m_bLButtonDown == true)
			{
				if ( m_nClickRgn == CLICKRGN_FLOOR)
				{
					m_nSelFloor = FloorFromPoint( pEvent->Pos);
				}

				if ( m_nClickRgn == CLICKRGN_SCALE)
				{
					float fRatio = 1.0f - (float)min( 32, max( 0, pEvent->Pos.y - 25)) / 32.0f;
					float fScale = DEFAULT_SCALEMIN + (DEFAULT_SCALEMAX - DEFAULT_SCALEMIN) * fRatio;
					m_fScaleBegin = fScale;
					m_fScaleEnd = fScale;
				}

				else if ( m_nClickRgn == CLICKRGN_MAP)
				{
					m_ptDragOffset[ 0] += (float)( pEvent->Pos.x - m_ptLButtonPos.x) / m_fScaleEnd;
					m_ptDragOffset[ 1] += (float)( pEvent->Pos.y - m_ptLButtonPos.y) / m_fScaleEnd;

					m_ptLButtonPos = pEvent->Pos;
				}
			}

			else
			{
				int nFloor = FloorFromPoint( pEvent->Pos);
				m_nSelFloor = nFloor;
			}
			break;
		}

	case MWM_MOUSEWHEEL :
		{
			if ( pEvent->nDelta > 0)			ZoomIn();
			else if ( pEvent->nDelta < 0)		ZoomOut();
			return true;
		}
	}

	return false;
}


// OnRun
void XUIFieldMap::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	// Update timer
	if ( dwCurrTime < (m_dwUpdateTimer + 3000))		return;
	m_dwUpdateTimer = dwCurrTime;


	if ( m_pFieldInfo == NULL)						return;

	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if ( pMyPlayer == NULL)							return;


	// Update floor
	if ( m_pFieldInfo->m_vFloors.empty() == false  &&
		gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID == m_pFieldInfo->m_nFieldID)
	{
		int nHeight = pMyPlayer->GetPosition().z;
		size_t _size = m_pFieldInfo->m_vFloors.size();
		for ( size_t i = 0;  i < _size;  i++)
		{
			if ( nHeight < m_pFieldInfo->m_vFloors[ i])
			{
				if ( m_nCurFloor != i)
				{
					m_nCurFloor = i;
					m_nShowFloor = i;

					wchar_t szBuff[ MAX_PATH];
					swprintf_s( szBuff, L"%smap%05d_%d.jpg", PATH_UI_MINIMAP, m_pFieldInfo->m_nZoneID, (i + 1) );
					
					SetMapImage(szBuff);
				}

				break;
			}
		}
	}


	// Update artisan
	if ( m_bUpdateDragOffsetToArtisan == true)
	{
		if ( m_pFieldInfo->m_nFieldID != m_nArtisanFieldID)
			UpdateInfo( m_nArtisanFieldID);

		m_vArtisanPos = m_pFieldInfo->GetNPCPos( m_nArtisanID);
		if ( m_vArtisanPos == vec3::ZERO)
		{
			ResetDragOffset();
		}
		else
		{
			bool _located = false;		// 현재 이 맵에 위치해있는지 확인
			XFieldInfo* pWorldInfo = gg.currentgamestate->GetWorld()->GetInfo();
			if ( pWorldInfo != NULL  &&  pWorldInfo->m_nFieldID == m_nFieldID)	_located = true;

			MPOINT _pos = ( m_bFullScale || _located == false) ?	MPOINT( m_fMinX + (m_fMaxX - m_fMinX) * 0.5f, m_fMinY + (m_fMaxY - m_fMinY) * 0.5f) :
																	MPOINT( pMyPlayer->GetPosition().x, pMyPlayer->GetPosition().y);
			float _rw = m_pMapImage ? m_pMapImage->GetWidth()  / (m_fMaxX - m_fMinX) : 512 / (m_fMaxX - m_fMinX);
			float _rh = m_pMapImage ? m_pMapImage->GetHeight() / (m_fMaxY - m_fMinY) : 512 / (m_fMaxY - m_fMinY);

			m_ptDragOffset[ 0] = (_pos.x - m_vArtisanPos.x) * _rw;
			m_ptDragOffset[ 1] = (m_vArtisanPos.y - _pos.y) * _rh;
		}

		m_bUpdateDragOffsetToArtisan = false;
	}


	// Update marker
	if ( m_nArtisanID > 0  &&  m_nArtisanFieldID == m_nFieldID)
	{
		vec3 _diff = m_vArtisanPos - pMyPlayer->GetPosition();
		float _distance = _diff.x * _diff.x  +  _diff.y * _diff.y;
		if ( _distance < 1000000 /*10m*/)
		{
			m_nArtisanFieldID = 0;
			m_nArtisanID = 0;
			m_vArtisanPos = vec3::ZERO;
		}
	}
}

MPOINT XUIFieldMap::ConvertToWorldPos( const MPOINT ptEventPos )
{
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if ( pMyPlayer == NULL) return MPOINT(0,0);

	bool _located = false;		// 현재 이 맵에 위치해있는지 확인
	XFieldInfo* pWorldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pWorldInfo != NULL  &&  pWorldInfo->m_nFieldID == GetFieldID())
		_located = true;

	// Get variables
	MRECT r = GetClientRect();
	MPOINT _cp = MPOINT( r.w >> 1, r.h >> 1);
	float _scale = GetScale();
	float _minx, _miny, _maxx, _maxy;
	GetMapSize( _minx, _miny, _maxx, _maxy);

	MBitmap* pBitmap = GetMapImage();
	MPOINT _drag = GetDragOffset();

	MPOINT _pos = (IsFullScale()  ||  _located == false) ?  MPOINT( _minx + (_maxx - _minx) * 0.5f, _miny + (_maxy - _miny) * 0.5f) : MPOINT( pMyPlayer->GetPosition().x, pMyPlayer->GetPosition().y);

	float _rw = pBitmap ? pBitmap->GetWidth()  / (_maxx - _minx) : 512 / (_maxx - _minx);
	float _rh = pBitmap ? pBitmap->GetHeight() / (_maxy - _miny) : 512 / (_maxy - _miny);
	float _srw = _rw * _scale;
	float _srh = _rh * _scale;


	MPOINT ptExpectationPos;

	ptExpectationPos.x = (ptEventPos.x - _drag.x - _cp.x) / _srw + _pos.x;
	ptExpectationPos.y = -(ptEventPos.y - _drag.y - _cp.y) / _srh + _pos.y;

	return ptExpectationPos;
}

void XUIFieldMap::MoveMyPlayerForTest( const MPOINT ptPos )
{
	if (gvar.MyInfo.ChrInfo.nPlayerGrade < PLAYER_GRADE_GM) return;

	MPOINT ptExpectationPos = ConvertToWorldPos(ptPos);

	vec3 pos_to = vec3(ptExpectationPos.x, ptExpectationPos.y, 25000.0f);
	if (m_nFieldID == gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID)
	{
		pos_to.z = gg.currentgamestate->GetWorld()->GetGroundZ(pos_to) + 5.0f;
	}

	XPostGM_Move(m_nFieldID, pos_to);
}

void XUIFieldMap::SetMapImage( wchar_t * szBuff )
{
	// Set map image
	if ( m_pMapImage != NULL)
	{
		m_pMapImage->Destroy();
		delete m_pMapImage;
		m_pMapImage = NULL;
	}

	m_pMapImage = new MBitmapR3();
	if ( m_pMapImage->Create( REngine::GetDevicePtr(), DEFAULT_FIELDMAPNAME, MLocale::ConvUTF16ToAnsi(szBuff).c_str()) == false)
	{
		if ( m_pMapImage != NULL)
		{
			m_pMapImage->Destroy();
			delete m_pMapImage;
		}

		m_pMapImage = NULL;
	}
	// 맵이미지가 없는 테스트맵등을 위해
	else
	{
		if (m_pMapImage->CreateTexture() == false)
		{
			m_pMapImage->Destroy();
			delete m_pMapImage;
			m_pMapImage = NULL;
		}
	}
}













// XFieldMapLook
XFieldMapLook::XFieldMapLook()
{
	m_pLargeFont = NULL;
	m_nTextAlign = MAM_HCENTER | MAM_VCENTER;
}


void XFieldMapLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
}

void XFieldMapLook::SetLargeFont(const char* szFont)
{
	m_pLargeFont = MFontManager::Get( szFont);
}


struct _fmapitem
{
	int			priority;
	MPOINT		pos;
	float		distance;
	string		name;
	string		bitmap;
	string		state;
	float		scale;
	float		opacity;
	bool		rotate;
	float		angle;
	int			indicate;
	bool		highlight;

	_fmapitem( int _priority, MPOINT _pos, string _name, string _bitmap, string _state, float _scale)
		: priority( _priority), pos( _pos), distance( 0.0f), name( _name), bitmap( _bitmap), state( _state), scale( _scale), opacity( 1.0f), rotate( false), angle( 0.0f), indicate( 0), highlight( false)
		{}
	_fmapitem( int _priority, vec3 _pos, string _name, string _bitmap, string _state, float _scale)
		: priority( _priority), distance( 0.0f), name( _name), bitmap( _bitmap), state( _state), scale( _scale), opacity( 1.0f), rotate( false), angle( 0.0f), indicate( 0), highlight( false)
		{ pos.x = _pos.x;  pos.y = _pos.y;	}
	_fmapitem& operator = (const _fmapitem& other)
		{	priority = other.priority;  pos = other.pos;  distance = other.distance;  name = other.name;  bitmap = other.bitmap;  state = other.state;  scale = other.scale;
			opacity = other.opacity;  rotate = other.rotate;  angle = other.angle;  indicate = other.indicate;  highlight = other.highlight;		return *this; }
};


void XFieldMapLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	XUIFieldMap* pFieldMap = (XUIFieldMap*)pWidget;
	XFieldInfo* pFieldInfo = pFieldMap->GetFieldInfo();

	if ( pFieldMap == NULL  ||  pFieldInfo == NULL)
		return;

	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if ( pMyPlayer == NULL)		return;

	bool _located = false;		// 현재 이 맵에 위치해있는지 확인
	XFieldInfo* pWorldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pWorldInfo != NULL  &&  pWorldInfo->m_nFieldID == pFieldMap->GetFieldID())
		_located = true;

	bool _on_floor = false;
	if ( pFieldMap->GetCurFloor() == pFieldMap->GetShowFloor())
		_on_floor = true;


	// Get variables
	MRECT r = pFieldMap->GetClientRect();
	MPOINT _cp = MPOINT( r.w >> 1, r.h >> 1);
	float _rot = pMyPlayer->GetDirection().AngleToXY( vec3( 0 ,1 ,0));
	float _scale = pFieldMap->GetScale();
	float _minx, _miny, _maxx, _maxy;
	pFieldMap->GetMapSize( _minx, _miny, _maxx, _maxy);
	MPOINT _pos = (pFieldMap->IsFullScale()  ||  _located == false) ?  MPOINT( _minx + (_maxx - _minx) * 0.5f, _miny + (_maxy - _miny) * 0.5f) : MPOINT( pMyPlayer->GetPosition().x, pMyPlayer->GetPosition().y);
	MPOINT _cursor = pFieldMap->GetLastCursorPos();
	MPOINT _drag = pFieldMap->GetDragOffset();
	MBitmap* pBitmap = pFieldMap->GetMapImage();
	float _rw = pBitmap ? pBitmap->GetWidth()  / (_maxx - _minx) : 512 / (_maxx - _minx);
	float _rh = pBitmap ? pBitmap->GetHeight() / (_maxy - _miny) : 512 / (_maxy - _miny);
	float _srw = _rw * _scale;
	float _srh = _rh * _scale;
	float _rad = (float)min( _cp.x, _cp.y) - 10.0f;

	MRectF _bdrect = pFieldMap->GetBoundaryBoxRect();
	bool _boundary = false;
	if ( _bdrect.left != 0.0f  ||  _bdrect.top != 0.0f  ||  _bdrect.right != 0.0f  ||  _bdrect.bottom != 0.0f)
		_boundary = true;
	bool bDynamicField = pFieldMap->IsDynamicField();
	float _scaleratio = min( 1.0f,  ( (_scale - DEFAULT_SCALEMIN) / ( DEFAULT_SCALEMAX - DEFAULT_SCALEMIN) ) * 0.7f + 0.5f);

	XJOURNALTYPE _journaltype; int _journalid;
	XUIJournalIndicator::GetFocusedTypeID( &_journaltype, &_journalid);

	vec3 _missiontarpos;
	string _missiontarname;
	bool _missiontar = XUIMiniMap::GetMissionTarget( &_missiontarname, &_missiontarpos);



	/******************************
	 Get item info
	*******************************/

	vector< _fmapitem> vItems;

	// Add me
	if ( _located == true)
	{
		bool bDrawMe = true;
		int nCountFloors = pFieldMap->GetFloors();
		if (nCountFloors != 0)
		{
			int nShowFloor = pFieldMap->GetShowFloor();
			if (nShowFloor != 0 && pMyPlayer->GetPosition().z < pWorldInfo->m_vFloors[nShowFloor-1])	bDrawMe = false;
			if (pMyPlayer->GetPosition().z > pWorldInfo->m_vFloors[nShowFloor])	bDrawMe = false;
		}

		if (bDrawMe)
		{
			_fmapitem _item( 0, pMyPlayer->GetPosition(), MLocale::ConvUTF16ToAnsi( pMyPlayer->GetName()), "player", MWS_NORMAL, 1.0f);
			if ( (MGetMint()->GetUpdateTime() % 1000) < 100)	_item.opacity = 0.25f;
			_item.rotate = true;
			_item.angle = _rot;
			vItems.push_back( _item);
		}
	}


	// Add quest object icons
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount != NULL  &&  _located == true)
	{
		vector< pair< int, int>>& _journal = pAccount->m_JournalIndicator;
		for ( vector< pair< int, int>>::iterator itr = _journal.begin();  itr != _journal.end();  itr++)
		{
			if ( (*itr).first != XJT_QUEST)				continue;

			XPlayerQuest* pQuest = gvar.MyInfo.Quests.Get( (*itr).second);
			if ( pQuest == NULL)						continue;
			if ( pQuest->IsComplete() == true)
			{
				if ( pQuest->m_pQuestInfo->nRewardFieldID == pFieldMap->GetFieldID() && pQuest->m_pQuestInfo->vRewardPos != vec3::ZERO)
				{
					int nCountFloors = pFieldMap->GetFloors();
					if (nCountFloors != 0)
					{
						int nShowFloor = pFieldMap->GetShowFloor();
						if (nShowFloor != 0 && pQuest->m_pQuestInfo->vRewardPos.z < pFieldInfo->m_vFloors[nShowFloor-1]) continue;
						if (pQuest->m_pQuestInfo->vRewardPos.z > pFieldInfo->m_vFloors[nShowFloor]) continue;
					}

					_fmapitem _item( 2, pQuest->m_pQuestInfo->vRewardPos, MLocale::ConvUTF16ToAnsi( pQuest->m_pQuestInfo->GetTitle()), "quest", MWS_FOCUS, 1.0f);
					if ( _journaltype == XJT_QUEST  &&  _journalid == pQuest->m_pQuestInfo->nID)
					{
						_item.priority = 1;
						_item.indicate = 1;
					}
					vItems.push_back( _item);
				}
				continue;
			}

			vector<XQObjectiveInfo*> vecQObjectiveInfo = pQuest->m_pQuestInfo->GetAllQObjectiveInfo();
			for each ( XQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
			{
				XPlayerQObjective* pQObjective = pQuest->Get( pQObjectiveInfo->nID);
				if ( pQObjective == NULL  ||  pQObjective->bComplete == true)	continue;

				if (pQObjectiveInfo->nMarkerID > 0 && pQObjectiveInfo->nFieldID == pFieldMap->GetFieldID())
				{
					int nCountFloors = pFieldMap->GetFloors();
					if (nCountFloors != 0)
					{
						int nShowFloor = pFieldMap->GetShowFloor();
						if (nShowFloor != 0 && pQObjectiveInfo->GetPos().z < pFieldInfo->m_vFloors[nShowFloor-1]) continue;
						if (pQObjectiveInfo->GetPos().z > pFieldInfo->m_vFloors[nShowFloor]) continue;
					}

					_fmapitem _item( 2, pQObjectiveInfo->GetPos(), MLocale::ConvUTF16ToAnsi( pQuest->m_pQuestInfo->GetTitle()), "questobj", MWS_FOCUS, 1.0f);
					if ( _journaltype == XJT_QUEST  &&  _journalid == pQuest->m_pQuestInfo->nID)
					{
						_item.priority = 1;
						_item.indicate = 1;
					}
					vItems.push_back( _item);
				}

				if (pQObjectiveInfo->nMarkerID2 > 0 && pQObjectiveInfo->nFieldID2 == pFieldMap->GetFieldID())
				{
					int nCountFloors = pFieldMap->GetFloors();
					if (nCountFloors != 0)
					{
						int nShowFloor = pFieldMap->GetShowFloor();
						if (nShowFloor != 0 && pQObjectiveInfo->GetPos(2).z < pFieldInfo->m_vFloors[nShowFloor-1]) continue;
						if (pQObjectiveInfo->GetPos(2).z > pFieldInfo->m_vFloors[nShowFloor]) continue;
					}

					_fmapitem _item( 2, pQObjectiveInfo->GetPos(2), MLocale::ConvUTF16ToAnsi( pQuest->m_pQuestInfo->GetTitle()), "questobj", MWS_FOCUS, 1.0f);
					if ( _journaltype == XJT_QUEST  &&  _journalid == pQuest->m_pQuestInfo->nID)
					{
						_item.priority = 1;
						_item.indicate = 1;
					}
					vItems.push_back( _item);
				}

				break;
			}
		}
	}


	// Add party members
	if ( _located == true  &&  gvar.MyInfo.Party.IsJoined())
	{
		for ( XObjectMap::iterator itr = gg.currentgamestate->GetObjectManager()->BeginItor();  itr != gg.currentgamestate->GetObjectManager()->EndItor();  itr++)
		{
			XObject* pObj = (*itr).second;
			if ( pObj->IsModuleEntity() == false ||  pObj->GetEntityType() != ETID_PLAYER  ||  gvar.MyInfo.Party.IsPartyMember( pObj->GetUID()) == false  ||  pObj->GetUID() == gvar.MyInfo.MyUID)
				continue;

			vItems.push_back( _fmapitem( 3, pObj->GetPosition(), MLocale::ConvUTF16ToAnsi( pObj->GetName()), "party", MWS_FOCUS, 1.0f));
		}
	}


	// Add map sign
	if ( XGETCFG_GAME_SHOWAREAICON == true)
	{
		AREA_INFO_VECTOR& vecArea = pFieldInfo->m_Areas;
		for ( AREA_INFO_VECTOR::iterator itr = vecArea.begin();  itr != vecArea.end();  itr++)
		{
			AREA_INFO& _area = (*itr);
			vector<XCylinder>&	_cylinder = _area.veccylinder;
			if ( _cylinder.size() == 0)		continue;

			RBoundingBox _box;
			_box.Initialize();
			for ( vector<XCylinder>::iterator itr2 = _cylinder.begin();  itr2 != _cylinder.end();  itr2++)
			{
				XCylinder cylinder;
				cylinder = (*itr2);
				cylinder.radius += AREA_BUFFER_RADIUS;
				_box.Add( cylinder.GetAABB());
			}

			int nCountFloors = pFieldMap->GetFloors();
			if (nCountFloors != 0)
			{
				int nShowFloor = pFieldMap->GetShowFloor();
				if (nShowFloor != 0 && _box.GetCenter().z < pFieldInfo->m_vFloors[nShowFloor-1]) continue;
				if (_box.GetCenter().z > pFieldInfo->m_vFloors[nShowFloor]) continue;
			}

			if (_area.bPath == false)
			{
				_fmapitem _item( (_scaleratio < 0.9f) ? 4 : 6, _box.GetCenter(), MLocale::ConvUTF16ToAnsi( _area.GetName()), "", MWS_NORMAL, 1.0f);
				_item.opacity = (1.0f - _scaleratio) * 0.6f + 0.7f;
				_item.highlight = true;
				vItems.push_back( _item);
			}
			else
			{
				_fmapitem _item( (_scaleratio < 0.9f) ? 4 : 6, _box.GetCenter(), MLocale::ConvUTF16ToAnsi( _area.GetName()), "", MWS_FOCUS, 1.0f);
				_item.opacity = (1.0f - _scaleratio) * 0.6f + 0.7f;
				_item.priority = 0.9f;
				_item.highlight = true;
				vItems.push_back( _item);
			}
		}
	}


	// Add NPC icons
	if ( _located == true)
	{
		map< pair< int, int>, _fmapitem> vNpcIcons;
		for ( int i = 0;  i < NPC_UIMAP_INFO::TYPE_CNT;  i++)
		{
			NPC_UIMAP_INFO_VECTOR* pNpcInfo = NULL;
			if ( (i == NPC_UIMAP_INFO::TYPE_CRAFT && XGETCFG_GAME_SHOWCRAFTICON)  ||  (i == NPC_UIMAP_INFO::TYPE_SHOP && XGETCFG_GAME_SHOWSHOPICON) )
				pNpcInfo = &pFieldInfo->m_NpcUiInfo[ i];

			if ( pNpcInfo == NULL)		continue;
			for ( NPC_UIMAP_INFO_VECTOR::iterator itr = pNpcInfo->begin();  itr != pNpcInfo->end();  itr++)
			{
				string _bitmap = (*itr).strLookPart, _state = (*itr).strLookState;
				if ( _bitmap.empty() == true  ||  _state.empty() == true)							continue;
				if      ( _bitmap == string( "quest")  &&  XGETCFG_GAME_SHOWQUESTICON == false)		continue;
				else if ( _bitmap == string( "merchant")  &&  XGETCFG_GAME_SHOWSHOPICON == false)	continue;
				else if ( _bitmap == string( "artisan")  &&  XGETCFG_GAME_SHOWCRAFTICON == false)	continue;

				int nCountFloors = pFieldMap->GetFloors();
				if (nCountFloors != 0)
				{
					int nShowFloor = pFieldMap->GetShowFloor();
					if (nShowFloor != 0 && (*itr).vPos.z < pWorldInfo->m_vFloors[nShowFloor-1]) continue;
					if ((*itr).vPos.z > pWorldInfo->m_vFloors[nShowFloor]) continue;
				}

				MPOINT _p( (*itr).vPos.x, (*itr).vPos.y);
				_fmapitem _item( 5, _p, MLocale::ConvUTF16ToAnsi( info.npc->GetName( (*itr).nNPCID)), _bitmap, _state, _scaleratio);
				vNpcIcons.insert( map< pair< int, int>, _fmapitem>::value_type( pair< int, int>( _p.x, _p.y), _item));
			}
		}

		NPC_ICON_STATE nIConState = NIS_NONE;

		for (std::map<UIID, NPC_ICON_INFO>::iterator itr = gvar.Game.vecNPCIcon.begin(); itr != gvar.Game.vecNPCIcon.end();itr++)
		{
			NPC_ICON_INFO tdNPCIcon = (*itr).second;
			nIConState = XInteractionHelper::ConvertNPCIConTypeToIconState(tdNPCIcon.m_nIcon);
			vec3& vNPCPos = tdNPCIcon.m_vPos;

			wstring pszString = info.npc->GetName(tdNPCIcon.m_nNPCID);

			XNonPlayer* pNPC = gg.omgr->FindNPC_UIID((*itr).first);
			if ( pNPC)
			{
				if ( pNPC->IsModuleEntity() == false  ||  pNPC->GetEntityType() != ETID_NPC)		continue;

				vNPCPos = pNPC->GetPosition();
			}

			int nCountFloors = pFieldMap->GetFloors();
			if (nCountFloors != 0)
			{
				int nShowFloor = pFieldMap->GetShowFloor();
				if (nShowFloor != 0 && vNPCPos.z < pWorldInfo->m_vFloors[nShowFloor-1]) continue;
				if (vNPCPos.z > pWorldInfo->m_vFloors[nShowFloor]) continue;
			}

			string _bitmap, _state;
			switch ( nIConState)
			{
			case NIS_QUEST_BEGIN:		_bitmap = "quest";		_state = MWS_NORMAL;			break;
			case NIS_QUEST_END	:		_bitmap = "quest";		_state = MWS_FOCUS;				break;
			case NIS_SHOP		:		_bitmap = "merchant";	_state = XFieldInfoHelper::GetNPCIconStateString( pFieldInfo, NPC_UIMAP_INFO::TYPE_SHOP, tdNPCIcon.m_nNPCID);		break;
			case NIS_CRAFT		:		_bitmap = "artisan";	_state = XFieldInfoHelper::GetNPCIconStateString( pFieldInfo, NPC_UIMAP_INFO::TYPE_CRAFT,tdNPCIcon.m_nNPCID);	break;
			default				:		continue;
			}

			if ( _bitmap.empty() == true  ||  _state.empty() == true)							continue;
			if      ( _bitmap == string( "quest")  &&  XGETCFG_GAME_SHOWQUESTICON == false)		continue;
			else if ( _bitmap == string( "merchant")  &&  XGETCFG_GAME_SHOWSHOPICON == false)	continue;
			else if ( _bitmap == string( "artisan")  &&  XGETCFG_GAME_SHOWCRAFTICON == false)	continue;

			MPOINT _p( vNPCPos.x, vNPCPos.y);
			map< pair< int, int>, _fmapitem>::iterator _find = vNpcIcons.find( pair< int, int>( _p.x, _p.y));
			if ( _find != vNpcIcons.end())
			{
				(*_find).second.bitmap = _bitmap;
				(*_find).second.state = _state;
			}
			else
			{
				_fmapitem _item( 5, _p, MLocale::ConvUTF16ToAnsi(pszString), _bitmap, _state, _scaleratio);
				vNpcIcons.insert( map< pair< int, int>, _fmapitem>::value_type( pair< int, int>( _p.x, _p.y), _item));
			}
		}

		for ( map< pair< int, int>, _fmapitem>::iterator itr = vNpcIcons.begin();  itr != vNpcIcons.end();  itr++)
			vItems.push_back( (*itr).second);
	}

	
	// Add mission target
	if ( _located == true  &&  _missiontar == true)
	{
		_fmapitem _item( 5, _missiontarpos, _missiontarname, "", "", 1.0f);
		_item.indicate = 2;
		vItems.push_back( _item);
	}


	// Add artisan marker
	vec3 _artisanpos;
	if ( pFieldMap->GetMarkerArtisanPos( _artisanpos) == true)
	{
		_fmapitem _item( 5, _artisanpos, "", "", "", 1.0f);
		_item.indicate = 2;
		vItems.push_back( _item);
	}





	/******************************
	 Post processing
	*******************************/

	if ( vItems.empty() == false)
	{
		MPOINT _bp = _cursor;
		if ( global.ui->IsMouseVisible() == false  ||  pFieldMap->IsMouseOver() == false)
			_bp = MPOINT( _cp.x + ( pMyPlayer->GetPosition().x - _pos.x) * _srw + _drag.x, _cp.y - ( pMyPlayer->GetPosition().y - _pos.y) * _srh + _drag.y);

		size_t _size = vItems.size();
		for ( size_t i = 0;  i < _size;  i++)
		{
			vItems[ i].pos = MPOINT( _cp.x + ( vItems[ i].pos.x - _pos.x) * _srw + _drag.x, _cp.y - ( vItems[ i].pos.y - _pos.y) * _srh + _drag.y);
			vItems[ i].distance = (vItems[ i].pos.x - _bp.x) * (vItems[ i].pos.x - _bp.x) + (vItems[ i].pos.y - _bp.y) * (vItems[ i].pos.y - _bp.y);
		}

		for ( size_t i = 0;  i < _size;  i++)
		{
			for ( size_t j = i;  j < _size;  j++)
			{
				if ( vItems[ i].priority > vItems[ j].priority  ||  
					( vItems[ i].priority == vItems[ j].priority  &&  vItems[ i].distance > vItems[ j].distance) )
				{
					_fmapitem _tmp = vItems[ i];
					vItems[ i] = vItems[ j];
					vItems[ j] = _tmp;
				}
			}
		}
	}




	/******************************
	 Draw
	*******************************/

	// Set DC
	//MCOLOR _color = GetFontColor( MWP_BORDER, MWS_NORMAL, MCOLOR( 255, 255, 255));
	pDC->SetFont( GetFont());
	//pDC->SetColor( _color);


	// Draw map
	DrawMap(pBitmap, pDC, r, _scale, _pos, _minx, _rw, _drag, _miny, _rh, _cp, _srw, _srh, _boundary, bDynamicField, _bdrect);	


	// Draw icons
	pDC->SetBitmap( GetBitmap());
	float _opacity = pDC->GetOpacity();
	for ( vector< _fmapitem>::reverse_iterator itr = vItems.rbegin();  itr != vItems.rend();  itr++)
	{
		_fmapitem& _item = (*itr);
		pDC->SetOpacity(_opacity * (*itr).opacity);

		MRECT* _r = GetAssignRect( _item.bitmap, _item.state);
		if ( _r != NULL)
		{
			MRECT _tr( _item.pos.x, _item.pos.y, _r->w * _item.scale, _r->h * _item.scale);
			_tr.x -= _tr.w >> 1;
			_tr.y -= _tr.h >> 1;

			if ( (*itr).rotate == false)
				pDC->Draw( _tr.x, _tr.y, _tr.w, _tr.h, _r->x, _r->y, _r->w, _r->h);
			else
				pDC->DrawRotate( _tr.x, _tr.y, _tr.w, _tr.h, (_tr.w >> 1), (_tr.h >> 1), _item.angle, _r->x, _r->y, _r->w, _r->h);
		}
		else if ( _item.name.empty() == false)
		{
			MCOLOR _color = GetFontColor( MWP_BORDER, _item.state, MCOLOR( 255, 255, 255));

			pDC->SetColor( _color.r, _color.g, _color.b,  (_color.a * min( 1.0f, (*itr).opacity)));
			pDC->Text( _item.pos.x - 100, _item.pos.y - 100, 200, 200, _item.name.c_str(), MAM_HCENTER | MAM_VCENTER);
		}
	}

	pDC->SetOpacity( _opacity);


	// Draw tooltip
	if ( vItems.empty() == false  &&  global.ui->IsMouseVisible() == true  &&  pFieldMap->IsMouseOver() == true)
	{
		int _y = _cursor.y - pDC->GetFont()->GetHeight() - 3;
		int _w = 0;
		int _h = pDC->GetFont()->GetHeight();
		list< _fmapitem> _tooltip;
		for ( vector< _fmapitem>::reverse_iterator itr = vItems.rbegin();  itr != vItems.rend();  itr++)
		{
			if ( (*itr).name.empty() == true  ||  MRECT( (*itr).pos.x - 16, (*itr).pos.y - 16, 32, 32).InPoint( _cursor) == false)
				continue;

			if ( (*itr).highlight == true)		_tooltip.push_front( (*itr));
			else								_tooltip.push_back( (*itr));
			_w = max( _w, pDC->GetFont()->GetWidth( (*itr).name.c_str()));
		}

		if ( _tooltip.empty() == false)
		{
			pDC->SetColor( 180, 180, 180, 128);
			pDC->FillRectangle( _cursor.x - 2, _cursor.y, _w + _h + 8, -( _h + 1) * _tooltip.size() - 4);
			
			MCOLOR _color = GetFontColor( MWP_BORDER, MWS_NORMAL, MCOLOR( 255, 255, 255));
			pDC->SetColor( _color);

			for ( list< _fmapitem>::iterator itr = _tooltip.begin();  itr != _tooltip.end();  itr++)
			{
				MRECT* _r = GetAssignRect( (*itr).bitmap, (*itr).state);
				if ( _r != NULL)	pDC->Draw( _cursor.x, _y, _h + 2, _h + 2, _r->x, _r->y, _r->w, _r->h);

				MCOLOR _col = pDC->GetColor();
				if ( (*itr).highlight == true)		pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MCOLOR( 255, 0, 0)) );
				pDC->Text( _cursor.x + _h, _y, (*itr).name.c_str());
				pDC->SetColor( _col);

				_y -= _h + 1;
			}
		}
	}


	// Draw indicator
	for ( vector< _fmapitem>::iterator itr = vItems.begin();  itr != vItems.end();  itr++)
	{
		if ( (*itr).indicate == 0)		continue;

		char _buff[ 32];
		sprintf_s( _buff, "indicator%d", (*itr).indicate);
		MRECT* _r = GetAssignRect( _buff, MWS_NORMAL);
		if ( _r != NULL)
		{
			int _delta = (int)( (double)(MGetMint()->GetUpdateTime() % 1000) * 0.001 * 10.0);
			pDC->Draw( (*itr).pos.x - (_r->w >> 1) + _delta, (*itr).pos.y - (_r->h >> 1) + _delta, _r->w - (_delta << 1), _r->h - (_delta << 1), _r->x, _r->y, _r->w, _r->h);
		}
	}


	// Draw scaler
	DrawScaler( pDC, pFieldMap, _scale);


	// Draw floor buttons
	DrawFloorButtons( pDC, pFieldMap);
}

void XFieldMapLook::DrawMap( MBitmap* pBitmap, MDrawContext* pDC, MRECT &r, float _scale, MPOINT &_pos, float _minx, float _rw, MPOINT &_drag, float _miny, float _rh, MPOINT _cp, float _srw, float _srh, bool _boundary, bool bDynamicField, MRectF _bdrect)
{
	if ( pBitmap != NULL)
	{
		pDC->SetBitmap( pBitmap);

		// Draw map
		MRECT _r;
		_r.w = r.w / _scale;
		_r.h = r.h / _scale;
		_r.x =  (_pos.x - _minx) * _rw - (_r.w >> 1) - (_drag.x / _scale);
		_r.y = -(_pos.y - _miny) * _rh - (_r.h >> 1) - (_drag.y / _scale) + pBitmap->GetHeight();

		if ( _boundary == true && bDynamicField == true)
		{
			MCOLOR _col = pDC->SetBitmapColor( 128, 128, 128);
			pDC->Draw( r.x, r.y, r.w, r.h,   _r.x, _r.y, _r.w, _r.h);

			_r.w =  ( _bdrect.right - _bdrect.left) * _rw;
			_r.h =  ( _bdrect.bottom - _bdrect.top) * _rh;
			_r.x =  ( _bdrect.left - _minx) * _rw;
			_r.y = -( _bdrect.top - _miny)  * _rh + pBitmap->GetHeight() - _r.h;

			MRECT _pr;
			_pr.w = _r.w * _scale;
			_pr.h = _r.h * _scale;
			_pr.x = _cp.x + (_bdrect.left - _pos.x) * _srw + _drag.x;
			_pr.y = _cp.y - (_bdrect.top - _pos.y)  * _srh + _drag.y - _pr.h;

			pDC->SetBitmapColor( _col);
			pDC->Draw( _pr.x, _pr.y, _pr.w, _pr.h,   _r.x, _r.y, _r.w, _r.h);
		}
		else
			pDC->Draw( r.x, r.y, r.w, r.h,   _r.x, _r.y, _r.w, _r.h);
	}
}

void XFieldMapLook::DrawScaler( MDrawContext* pDC, XUIFieldMap* pFieldMap, float _scale)
{
	if ( IsValidAssign( "fullscale", MWS_NORMAL) == false  ||  IsValidAssign( "scaletrack", MWS_NORMAL) == false  ||  IsValidAssign( "scalethumb", MWS_NORMAL) == false)
		return;

	MRECT _wr = pFieldMap->GetRect();
	MRECT* _r = GetAssignRect( "fullscale", (pFieldMap->IsFullScale()  ?  MWS_DOWN : MWS_NORMAL) );
	if ( _r != NULL)	pDC->Draw( _wr.w - 20, 3, 16, 16, _r->x, _r->y, _r->w, _r->h);

	_r = GetAssignRect( "scaletrack", MWS_NORMAL);
	if ( _r != NULL)	pDC->Draw( _wr.w - 20, 25, 16, 32, _r->x, _r->y, _r->w, _r->h);

	int h = 25;
	if ( pFieldMap->IsFullScale() == false)
		h += (32.0 - 32.0 * (_scale - (float)DEFAULT_SCALEMIN) / (float)(DEFAULT_SCALEMAX - DEFAULT_SCALEMIN) ) - 3;

	_r = GetAssignRect( "scalethumb", MWS_NORMAL);
	if ( _r != NULL)	pDC->Draw( _wr.w - 20, h, 16, 6, _r->x, _r->y, _r->w, _r->h);
}

void XFieldMapLook::DrawFloorButtons( MDrawContext* pDC, XUIFieldMap* pFieldMap)
{
	int nFloors = pFieldMap->GetFloors();
	if ( nFloors <= 0  ||  IsValidAssign( "floor", MWS_NORMAL) == false)
		return;

	pDC->SetFont( GetLargeFont());

	int y = 3;
	for ( int i = 0;  i < nFloors;  i++)
	{
		MRECT* _r = NULL;
		MCOLOR _color;
		if ( i == pFieldMap->GetShowFloor())
		{
			_r = GetAssignRect( "floor", MWS_DOWN);
			_color = GetFontColor( "floor", MWS_DOWN, MCOLOR( 255, 255, 255, 100));
		}
		else if ( i == pFieldMap->GetSelFloor())
		{
			_r = GetAssignRect( "floor", MWS_FOCUS);
			_color = GetFontColor( "floor", MWS_FOCUS, MCOLOR( 255, 255, 255));
		}
		else
		{
			_r = GetAssignRect( "floor", MWS_NORMAL);
			_color = GetFontColor( "floor", MWS_NORMAL, MCOLOR( 255, 255, 0));
		}

		if ( _r == NULL)	continue;

		pDC->Draw( 3, y, 32, 24, _r->x, _r->y, _r->w, _r->h);

		char szBuff[ 256];
		sprintf_s( szBuff, "%dF", i + 1);
		pDC->SetColor( _color);
		pDC->Text( MRECT( 3, y, 32, 24), szBuff, MAM_HCENTER | MAM_VCENTER);

		y += 27;
	}
}










/*

void XFieldMapLook::DrawAreaName( XFieldInfo* pFieldInfo, MPOINT &_pos, float _srw, MPOINT &_drag, float _srh, MPOINT &_cp, MDrawContext* pDC )
{
	if (XGETCFG_GAME_SHOWAREAICON)
	{
		AREA_INFO_VECTOR& vecArea = pFieldInfo->m_Areas;
		for (AREA_INFO_VECTOR::iterator itor = vecArea.begin();
			itor != vecArea.end(); ++itor)
		{
			AREA_INFO& info = (*itor);
			vector<XCylinder>&	veccylinder = info.veccylinder;

			if(veccylinder.size() == 0) continue;

			RBoundingBox bbox;
			bbox.Initialize();
			for (vector<XCylinder>::iterator itor2 = veccylinder.begin();
				itor2 != veccylinder.end(); ++itor2)
			{
				XCylinder& Cylionderinfo = (*itor2);

				XCylinder cylinder;
				cylinder = Cylionderinfo;
				cylinder.radius += AREA_BUFFER_RADIUS;

				bbox.Add(cylinder.GetAABB());
			}

			vec3 vPos = bbox.GetCenter();

			MRECT* pAssignRect = GetAssignRect( "questobj", MWS_FOCUS);
			if (!pAssignRect) continue;

			MRECT _r = *pAssignRect;
			MPOINT _p( (vPos.x - _pos.x) * _srw + _drag.x,  (vPos.y - _pos.y) * _srh - _drag.y);
			MRECT _tr( _cp.x + _p.x - (_r.w >> 1), _cp.y - _p.y, _r.w, _r.h);
			//pDC->Draw( _tr.x, _tr.y, _tr.w, _tr.h, _r.x, _r.y, _r.w, _r.h);



			const wchar_t* name = info.strName.c_str();

			MSIZE _box( pDC->GetFont()->GetWidth( MLocale::ConvUTF16ToAnsi(name).c_str()) + 10,  pDC->GetFont()->GetHeight() + 5);
			_tr.x = _tr.x + (_tr.w >> 1) - (_box.w >> 1);
			//_tr.y += _tr.h;
			_tr.w = _box.w;
			_tr.h = _box.h;

			pDC->SetColor( 230, 230, 230);
			pDC->Text( _tr, MLocale::ConvUTF16ToAnsi(name).c_str(), MAM_HCENTER | MAM_VCENTER);
		}
	}
}

void XFieldMapLook::DrawMe( MDrawContext* pDC, vec3& _my_pos, MPOINT &_pos, float _srw, float _srh, MPOINT& _cp, float _rot, MPOINT &_drag, bool _fullscale)
{
	MRECT* _r = GetAssignRect( "player", MWS_NORMAL);
	if ( _r == NULL)		return;

	if ( (MGetMint()->GetUpdateTime() % 1000) < 100)
		return;

	if ( _fullscale == true)
	{
		MPOINT _p( (_my_pos.x - _pos.x) * _srw + _drag.x,  (_my_pos.y - _pos.y) * _srh - _drag.y);
		pDC->DrawRotate( _cp.x + _p.x - (_r->w >> 1), _cp.y - _p.y - (_r->h >> 1), _r->w, _r->h, (_r->w >> 1), (_r->h >> 1), _rot, _r->x, _r->y, _r->w, _r->h);
	}

	else
		pDC->DrawRotate( _cp.x + _drag.x - (_r->w >> 1), _cp.y + _drag.y - (_r->h >> 1), _r->w, _r->h, (_r->w >> 1), (_r->h >> 1), _rot, _r->x, _r->y, _r->w, _r->h);
}

void XFieldMapLook::ExtractStaticNPCRect( XFieldInfo* pFieldInfo, MPOINT &_pos, float _srw, MPOINT &_drag, float _srh, MPOINT &_cp, MDrawContext* pDC, vector<NPCRECT>& vecNPCRect)
{
	for(int i = NPC_UIMAP_INFO::TYPE_CNT-1; i >=0 ; --i)
	{
		NPC_UIMAP_INFO_VECTOR* pVecNpcUiInfo = NULL;
		if ((i == NPC_UIMAP_INFO::TYPE_CRAFT && XGETCFG_GAME_SHOWCRAFTICON) ||
			(i == NPC_UIMAP_INFO::TYPE_SHOP && XGETCFG_GAME_SHOWSHOPICON) )
		{
			pVecNpcUiInfo = &pFieldInfo->m_NpcUiInfo[i];
		}

		if (NULL == pVecNpcUiInfo)
			continue;

		for (NPC_UIMAP_INFO_VECTOR:: iterator itr = pVecNpcUiInfo->begin(); itr != pVecNpcUiInfo->end(); ++itr)
		{
			NPC_UIMAP_INFO& npcUIMapInfo = (*itr);
			MRECT* pRect = GetAssignRect( npcUIMapInfo.strLookPart, npcUIMapInfo.strLookState);
			if (NULL == pRect)
				continue;

			if (gg.currentgamestate->GetObjectManager()->FindNPC_ID(npcUIMapInfo.nNpcId)) continue;

			MRECT& _r = pRect[0];
			MPOINT _p( (npcUIMapInfo.x - _pos.x) * _srw + _drag.x,  (npcUIMapInfo.y - _pos.y) * _srh - _drag.y);
			MRECT _tr( _cp.x + _p.x - (_r.w >> 1), _cp.y - _p.y - (_r.h >> 1), _r.w, _r.h);

			vecNPCRect.push_back(NPCRECT(npcUIMapInfo.nNpcId, _r, _tr));
		}
	}
}

void XFieldMapLook::ExtractDynamicNPCRect( XFieldInfo* pFieldInfo, MPOINT &_pos, float _srw, MPOINT &_drag, float _srh, MPOINT &_cp, MDrawContext* pDC, vector<NPCRECT>& vecNPCRect, bool _located)
{
	if (false == _located) return;

	for ( XObjectMap::iterator itor = gg.currentgamestate->GetObjectManager()->BeginItor();  itor != gg.currentgamestate->GetObjectManager()->EndItor();  itor++)
	{
		XObject* pObj = (*itor).second;
		if ( pObj->IsModuleEntity() == false)		continue;
		if ( pObj->GetEntityType() != ETID_NPC)		continue;

		XNonPlayer* pNPC = AsNPC( pObj);
		MRECT _r;
		if (false == GetDynamicNPCIconAssignRect(pNPC, pFieldInfo, _r)) continue;

		MPOINT _p( (pObj->GetPosition().x - _pos.x) * _srw + _drag.x,  (pObj->GetPosition().y - _pos.y) * _srh - _drag.y);
		MRECT _tr( _cp.x + _p.x - (_r.w >> 1), _cp.y - _p.y - (_r.h >> 1), _r.w, _r.h);

		vecNPCRect.push_back(NPCRECT(pNPC->GetNPCID(), _r, _tr));
	}
}

bool XFieldMapLook::GetDynamicNPCIconAssignRect( XNonPlayer* pNPC, XFieldInfo* pFieldInfo, MRECT& _r )
{
	string strState;

	MRECT* pAssignRect = NULL;

	switch ( pNPC->GetIConState())
	{
	case NIS_QUEST_BEGIN:
		{
			if (XGETCFG_GAME_SHOWQUESTICON == true)
			{
				strState = MWS_NORMAL;
				pAssignRect = GetAssignRect( "quest", strState);
				if (NULL == pAssignRect) return false;
			}		
		}
		break;

	case NIS_QUEST_END:
		{
			if (XGETCFG_GAME_SHOWQUESTICON == true)
			{
				strState = MWS_FOCUS;
				pAssignRect = GetAssignRect( "quest", strState);
				if (NULL == pAssignRect) return false;
			}			
		}
		break;

	case NIS_SHOP :
		{
			if (XGETCFG_GAME_SHOWSHOPICON == true)
			{
				strState = XFieldInfoHelper::GetNPCIconStateString(pFieldInfo, NPC_UIMAP_INFO::TYPE_SHOP, pNPC->GetNPCID());
				pAssignRect = GetAssignRect( "merchant", strState);
				if (NULL == pAssignRect) return false;
			}			
		}
		break;

	case NIS_CRAFT :
		{
			if (XGETCFG_GAME_SHOWCRAFTICON == true)
			{
				strState = XFieldInfoHelper::GetNPCIconStateString(pFieldInfo, NPC_UIMAP_INFO::TYPE_CRAFT, pNPC->GetNPCID());
				pAssignRect = GetAssignRect( "artisan", strState);
				if (NULL == pAssignRect) return false;
			}			
		}
		break;

	default :
		{
			return false;
		}
	}
	if (NULL == pAssignRect) return false;

	_r = *pAssignRect;

	return true;
}

void XFieldMapLook::DrawNPCICon( const vector<NPCRECT>& vecNPCRect, MDrawContext* pDC)
{
	for each ( const NPCRECT& val in vecNPCRect)
	{
		const MRECT& _tr = val._tr;
		const MRECT& _r = val._r;

		pDC->Draw( _tr.x, _tr.y, _tr.w, _tr.h, _r.x, _r.y, _r.w, _r.h);
	}
}

void XFieldMapLook::DrawQuest( bool _located, XUIFieldMap* pFieldMap, MPOINT _pos, float _srw, MPOINT _drag, float _srh, MPOINT _cp, MDrawContext* pDC, MPOINT _cursor )
{
	if (false == _located) return;

	int nFocusedQuestID = XUIJournalIndicator::GetFocusedQuestID();
	if ( IsValidAssign( "questobj", MWS_NORMAL))
	{
		XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo( gvar.MyInfo.ChrInfo.szName);
		if ( pAccount != NULL)
		{
			vector< pair< int, int>>& _journal = pAccount->m_JournalIndicator;
			for ( vector< pair< int, int>>::iterator itr = _journal.begin();  itr != _journal.end();  itr++)
			{
				if ( (*itr).first != XJT_QUEST)				continue;

				XPlayerQuest* pQuest = gvar.MyInfo.Quests.Get( (*itr).second);
				if ( pQuest == NULL)						continue;
				if ( pQuest->IsComplete() == true)
				{
					if ( pQuest->m_pQuestInfo->nRewardFieldID == pFieldMap->GetFieldID())
					{
						vec3 vPos = pQuest->m_pQuestInfo->vRewardPos;
						DrawQuestComplete(vPos, _pos, _srw, _drag, _srh, _cp, pDC, pQuest, nFocusedQuestID, _cursor);
					}
					continue;
				}

				vector<XQObjectiveInfo*> vecQObjectiveInfo = pQuest->m_pQuestInfo->GetAllQObjectiveInfo();
				for each (XQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
				{
					XPlayerQObjective* pQObjective = pQuest->Get(pQObjectiveInfo->nID);
					if ( pQObjective == NULL)				continue;
					if ( pQObjective->bComplete == true)	continue;

					if ( pQObjectiveInfo->nFieldID != pFieldMap->GetFieldID())		continue;
					if ( pQObjectiveInfo->nMarkerID <= 0)							continue;


					vec3 vPos = pQObjectiveInfo->GetPos();
					DrawQuestObject(vPos, _pos, _srw, _drag, _srh, _cp, pDC, pQuest, nFocusedQuestID, _cursor);

					break;
				}
			}
		}
	}
}

void XFieldMapLook::DrawQuestObject( vec3 &vPos, MPOINT &_pos, float _srw, MPOINT &_drag, float _srh, MPOINT &_cp, MDrawContext* pDC, XPlayerQuest* pQuest, int nFocusedQuestID, MPOINT _cursor )
{
	MRECT* pAssignRect = GetAssignRect( "questobj", MWS_FOCUS);
	if (!pAssignRect) return;

	MRECT _r = *pAssignRect;
	MPOINT _p( (vPos.x - _pos.x) * _srw + _drag.x,  (vPos.y - _pos.y) * _srh - _drag.y);
	MRECT _tr( _cp.x + _p.x - (_r.w >> 1), _cp.y - _p.y - (_r.h >> 1), _r.w, _r.h);
	pDC->Draw( _tr.x, _tr.y, _tr.w, _tr.h, _r.x, _r.y, _r.w, _r.h);

	if ( pQuest->m_pQuestInfo->nID == nFocusedQuestID)
	{
		DrawIndicator(vPos, _pos, _srw, _drag, _srh, pDC, _cp);
	}

	if ( _tr.InPoint( _cursor) == true)
	{
		MSIZE _box( pDC->GetFont()->GetWidth( MLocale::ConvUTF16ToAnsi(pQuest->m_pQuestInfo->GetTitle()).c_str()) + 10,  pDC->GetFont()->GetHeight() + 5);
		_tr.x = _tr.x + (_tr.w >> 1) - (_box.w >> 1);
		_tr.y += _tr.h;
		_tr.w = _box.w;
		_tr.h = _box.h;

		pDC->SetColor( 0, 0, 0, 160);
		pDC->FillRectangle( _tr);

		pDC->SetColor( 180, 180, 180);
		pDC->Text( _tr, MLocale::ConvUTF16ToAnsi(pQuest->m_pQuestInfo->GetTitle()).c_str(), MAM_HCENTER | MAM_VCENTER);
	}
}

void XFieldMapLook::DrawQuestComplete( vec3 &vPos, MPOINT &_pos, float _srw, MPOINT &_drag, float _srh, MPOINT &_cp, MDrawContext* pDC, XPlayerQuest* pQuest, int nFocusedQuestID, MPOINT _cursor )
{
	MRECT* pAssignRect = GetAssignRect( "quest", MWS_FOCUS);
	if (!pAssignRect) return;
	MRECT _r = *pAssignRect;
	MPOINT _p( (vPos.x - _pos.x) * _srw + _drag.x,  (vPos.y - _pos.y) * _srh - _drag.y);
	MRECT _tr( _cp.x + _p.x - (_r.w >> 1), _cp.y - _p.y - (_r.h >> 1), _r.w, _r.h);
	pDC->Draw( _tr.x, _tr.y, _tr.w, _tr.h, _r.x, _r.y, _r.w, _r.h);

	if ( pQuest->m_pQuestInfo->nID == nFocusedQuestID)
	{
		DrawIndicator(vPos, _pos, _srw, _drag, _srh, pDC, _cp);
	}

	if ( _tr.InPoint( _cursor) == true)
	{
		MSIZE _box( pDC->GetFont()->GetWidth( MLocale::ConvUTF16ToAnsi(pQuest->m_pQuestInfo->GetTitle()).c_str()) + 10,  pDC->GetFont()->GetHeight() + 5);
		_tr.x = _tr.x + (_tr.w >> 1) - (_box.w >> 1);
		_tr.y += _tr.h;
		_tr.w = _box.w;
		_tr.h = _box.h;

		pDC->SetColor( 0, 0, 0, 160);
		pDC->FillRectangle( _tr);

		pDC->SetColor( 180, 180, 180);
		pDC->Text( _tr, MLocale::ConvUTF16ToAnsi(pQuest->m_pQuestInfo->GetTitle()).c_str(), MAM_HCENTER | MAM_VCENTER);
	}
}

void XFieldMapLook::DrawParty( bool _located, MPOINT &_pos, float _srw, MPOINT &_drag, float _srh, MPOINT &_cp, MDrawContext* pDC, MPOINT _cursor )
{
	if (false == _located) return;

	if ( gvar.MyInfo.Party.IsJoined()  &&  IsValidAssign( "party", MWS_NORMAL))
	{
		for ( XObjectMap::iterator itor = gg.currentgamestate->GetObjectManager()->BeginItor();  itor != gg.currentgamestate->GetObjectManager()->EndItor();  itor++)
		{
			XObject* pObj = (*itor).second;
			if ( pObj->IsModuleEntity() == false)		continue;
			if ( pObj->GetEntityType() != ETID_PLAYER)	continue;
			if ( gvar.MyInfo.Party.IsPartyMember( pObj->GetUID()) == false)		continue;
			if ( pObj->GetUID() == gvar.MyInfo.MyUID)	continue;

			MRECT* pAssignRect = GetAssignRect( "party", MWS_FOCUS);
			if (!pAssignRect) continue;

			MRECT _r = *pAssignRect;
			MPOINT _p( (pObj->GetPosition().x - _pos.x) * _srw + _drag.x,  (pObj->GetPosition().y - _pos.y) * _srh - _drag.y);
			MRECT _tr( _cp.x + _p.x - (_r.w >> 1), _cp.y - _p.y - (_r.h >> 1), _r.w, _r.h);
			pDC->Draw( _tr.x, _tr.y, _tr.w, _tr.h, _r.x, _r.y, _r.w, _r.h);

			if ( _tr.InPoint( _cursor) == true)
			{
				MSIZE _box( pDC->GetFont()->GetWidth( MLocale::ConvUTF16ToAnsi(pObj->GetName()).c_str()) + 10,  pDC->GetFont()->GetHeight() + 5);
				_tr.x = _tr.x + (_tr.w >> 1) - (_box.w >> 1);
				_tr.y += _tr.h;
				_tr.w = _box.w;
				_tr.h = _box.h;

				pDC->SetColor( 0, 0, 0, 160);
				pDC->FillRectangle( _tr);

				pDC->SetColor( 180, 180, 180);
				pDC->Text( _tr, MLocale::ConvUTF16ToAnsi(pObj->GetName()).c_str(), MAM_HCENTER | MAM_VCENTER);
			}
		}
	}
}

void XFieldMapLook::DrawMe( MDrawContext* pDC, vec3& _my_pos, MPOINT &_pos, float _srw, float _srh, MPOINT& _cp, float _rot, MPOINT &_drag, bool _fullscale)
{
	MRECT* _r = GetAssignRect( "player", MWS_NORMAL);
	if ( _r == NULL)		return;

	if ( (MGetMint()->GetUpdateTime() % 1000) < 100)
		return;

	if ( _fullscale == true)
	{
		MPOINT _p( (_my_pos.x - _pos.x) * _srw + _drag.x,  (_my_pos.y - _pos.y) * _srh - _drag.y);
		pDC->DrawRotate( _cp.x + _p.x - (_r->w >> 1), _cp.y - _p.y - (_r->h >> 1), _r->w, _r->h, (_r->w >> 1), (_r->h >> 1), _rot, _r->x, _r->y, _r->w, _r->h);
	}

	else
		pDC->DrawRotate( _cp.x + _drag.x - (_r->w >> 1), _cp.y + _drag.y - (_r->h >> 1), _r->w, _r->h, (_r->w >> 1), (_r->h >> 1), _rot, _r->x, _r->y, _r->w, _r->h);
}

void XFieldMapLook::DrawRecipeNPCIndicator(XUIFieldMap* pFieldMap, MPOINT &_pos, float _srw, MPOINT &_drag, float _srh, MPOINT &_cp, MDrawContext* pDC)
{
	if (pFieldMap->GetRecipeNPCFieldID() != pFieldMap->GetFieldID()) return;

	vec3 vNPCPos = pFieldMap->GetRecipeNPCPos();
	if (vec3::ZERO == vNPCPos)	return;

	DrawIndicator(vNPCPos, _pos, _srw, _drag, _srh, pDC, _cp);
}

void XFieldMapLook::DrawNPCIConTooltip( const vector<NPCRECT>& vecNPCRect, MDrawContext* pDC, MPOINT& _cursor)
{
	int nMaxWidth = 0;
	int nMaxHeight = 0;
	set<int> setNPCID;

	for each ( const NPCRECT& val in vecNPCRect)
	{
		int nNPCID = val.nNPCID;
		MRECT _tr = val._tr;

		const TCHAR* pNPCName = info.npc->GetName(nNPCID);
		if (NULL == pNPCName) continue;
		
		if ( InTollerPoint(_tr, _cursor, 10) == true)
		{
			MSIZE _box( pDC->GetFont()->GetWidth( MLocale::ConvUTF16ToAnsi(pNPCName).c_str()) + 10,  pDC->GetFont()->GetHeight() + 5);
			_tr.x = _tr.x + (_tr.w >> 1) - (_box.w >> 1);
			_tr.y += _tr.h;
			_tr.w = _box.w;
			_tr.h = _box.h;

			nMaxWidth = (nMaxWidth < _tr.w) ? _tr.w : nMaxWidth;
			nMaxHeight = (nMaxHeight < _tr.h) ? _tr.h : nMaxHeight;

			setNPCID.insert(nNPCID);
		}
	}

	if (setNPCID.empty()) return;

	MRECT rectangleR = MRECT(_cursor.x, _cursor.y + 30, nMaxWidth, nMaxHeight * setNPCID.size());	
	pDC->SetColor( 0, 0, 0, 160);
	pDC->FillRectangle(rectangleR);

	MRECT npcNameR = rectangleR;
	npcNameR.x += 5;
	npcNameR.h = nMaxHeight;
	for each (int nNPCID in setNPCID)
	{
		const TCHAR* pNPCName = info.npc->GetName(nNPCID);
		if (NULL == pNPCName) continue;

		pDC->SetColor( 180, 180, 180);
		pDC->Text(npcNameR.x, npcNameR.y, npcNameR.w, npcNameR.h, MLocale::ConvUTF16ToAnsi(pNPCName).c_str(), MAM_LEFT | MAM_VCENTER);

		npcNameR.y += nMaxHeight;
	}
}

void XFieldMapLook::DrawIndicator( vec3 &vPos, MPOINT &_pos, float _srw, MPOINT &_drag, float _srh, MDrawContext* pDC, MPOINT &_cp )
{
	MPOINT _p( (vPos.x - _pos.x) * _srw + _drag.x,  (vPos.y - _pos.y) * _srh - _drag.y);
	
	if ( IsValidAssign( "indicator", MWS_NORMAL) == true)
	{
		MRECT* pAssignRect = GetAssignRect( "indicator", MWS_NORMAL);
		if (pAssignRect)
		{
			MRECT _sr = *pAssignRect;
			int _delta = (int)( (double)(MGetMint()->GetUpdateTime() % 1000) * 0.001 * 10.0);
			pDC->Draw( _cp.x + _p.x - (_sr.w >> 1) + _delta, _cp.y - _p.y - (_sr.h >> 1) + _delta, _sr.w - (_delta << 1), _sr.h - (_delta << 1), _sr.x, _sr.y, _sr.w, _sr.h);
		}
	}
}



bool XFieldMapLook::InTollerPoint( MRECT& _tr, MPOINT& p, int nToler )
{
	if(p.x>=_tr.x-nToler && p.x<=_tr.x+_tr.w+nToler && p.y>=_tr.y-nToler && p.y<=_tr.y+_tr.h+nToler) return true;

	return false;
}
*/
