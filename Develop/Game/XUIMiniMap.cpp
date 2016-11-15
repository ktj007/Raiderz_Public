#include "stdafx.h"
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



#define DEFAULT_SCALE					1.0f
#define DEFAULT_SCALEMIN				0.25f
#define DEFAULT_SCALEMAX				1.0f
#define DEFAULT_FIELDMAPNAME			"__fieldmapname"


static bool g_bExistMissionTarget = false;
static int  g_nMissionTargetNpcID = 0;
static vec3 g_MissionTargetPos = vec3( 0.0f, 0.0f, 0.0f);



// XUIMiniMap
XUIMiniMap::XUIMiniMap( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( XMiniMapLook);

	m_bEnableFocus = true;

	m_nFieldID = 0;
	m_nCurFloor = 0;
	m_pMapImage = NULL;
	m_fScaleBegin = m_fScaleEnd = DEFAULT_SCALE;
	m_bRotate = false;
	m_tZoom.SetArrivalTime( 500);
	m_dwUpdateTimer = 0;
	m_bDynamicField = false;
	m_nArtisanFieldID = 0;
	m_nArtisanID = 0;
}


// ~XUIMiniMap
XUIMiniMap::~XUIMiniMap()
{
	m_nFieldID = 0;
	m_bDynamicField = false;

	if ( m_pMapImage != NULL)
	{
		m_pMapImage->Destroy();
		delete m_pMapImage;
		m_pMapImage = NULL;
	}
}


// SetScale
void XUIMiniMap::SetScale( float fScale)
{
	if ( fScale < DEFAULT_SCALEMIN)			fScale = DEFAULT_SCALEMIN;
	else if ( fScale > DEFAULT_SCALEMAX)	fScale = DEFAULT_SCALEMAX;

	if ( fScale == m_fScaleEnd)		return;

	m_fScaleBegin = GetScale();
	m_fScaleEnd = fScale;

	m_tZoom.Start();
}


// GetScale
float XUIMiniMap::GetScale()
{
	return ( m_fScaleBegin + (m_fScaleEnd - m_fScaleBegin) * m_tZoom.GetRatio());
}


// ZoomIn
void XUIMiniMap::ZoomIn()
{
	if ( m_tZoom.IsArrival() == false)			return;

	SetScale( min( DEFAULT_SCALEMAX, m_fScaleEnd * 2.0f));
}


// ZoomOut
void XUIMiniMap::ZoomOut()
{
	if ( m_tZoom.IsArrival() == false)			return;

	SetScale( max( DEFAULT_SCALEMIN, m_fScaleEnd * 0.5f));
}


// RotateMap
void XUIMiniMap::RotateMap( bool bRotate)
{
	m_bRotate = bRotate;
}


// IsRotateMap
bool XUIMiniMap::IsRotateMap()
{
	return m_bRotate;
}


// GetMapSize
void XUIMiniMap::GetMapSize( float& minx, float& miny, float& maxx, float& maxy) const
{
	minx = m_fMinX;
	miny = m_fMinY;
	maxx = m_fMaxX;
	maxy = m_fMaxY;
}


// GetBoundaryBoxRect
MRectF XUIMiniMap::GetBoundaryBoxRect() const
{
	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pFieldInfo == NULL)		return MRectF( 0.0f, 0.0f, 0.0f, 0.0f);
	return pFieldInfo->m_rtBoundary;
}


// UpdateInfo
bool XUIMiniMap::UpdateInfo()
{
	g_bExistMissionTarget = false;
	g_nMissionTargetNpcID = 0;


	if ( gg.currentgamestate == NULL  ||
		 gg.currentgamestate->GetWorld() == NULL ||
		 gg.currentgamestate->GetWorld()->GetInfo() == NULL)
	{
		m_nFieldID = 0;
		m_strFieldName.clear();
		m_nCurFloor = 0;
		m_bDynamicField = false;

		if ( m_pMapImage != NULL)
		{
			m_pMapImage->Destroy();
			delete m_pMapImage;
			m_pMapImage = NULL;
		}

		 return false;
	}


	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pFieldInfo == NULL)					return false;
	if ( m_nFieldID == pFieldInfo->m_nFieldID)	return false;


	// Set map info
	m_nFieldID = pFieldInfo->m_nFieldID;
	m_strFieldName = pFieldInfo->GetName();
	m_nCurFloor = 0;
	m_fMinX = pFieldInfo->m_fMinX;
	m_fMinY = pFieldInfo->m_fMinY;
	m_fMaxX = pFieldInfo->m_fMaxX;
	m_fMaxY = pFieldInfo->m_fMaxY;

	// IsDynamicField
	CSFieldFileInfo* pFieldFileInfo = info.field->GetFieldList().Find( m_nFieldID);
	if ( pFieldFileInfo)	m_bDynamicField = pFieldFileInfo->bDynamic;

	wchar_t szBuff[ MAX_PATH];
	swprintf_s( szBuff, L"%smap%05d.jpg", PATH_UI_MINIMAP, pFieldInfo->m_nZoneID);

	if ( pFieldInfo->m_vFloors.empty() == false)
	{
		XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
		if ( pMyPlayer == NULL)			return false;

		int nHeight = pMyPlayer->GetPosition().z;

		size_t _size = pFieldInfo->m_vFloors.size();
		for ( size_t i = 0;  i < _size;  i++)
		{
			if ( nHeight < pFieldInfo->m_vFloors[ i])
			{
				m_nCurFloor = i;
				swprintf_s( szBuff, L"%smap%05d_%d.jpg", PATH_UI_MINIMAP, pFieldInfo->m_nZoneID, (i + 1) );
				break;
			}
		}
	}

	SetMapImage(szBuff);

	return true;
}


// SetMissionTarget
void XUIMiniMap::SetMissionTarget( int nNpcID)
{
	if ( nNpcID == 0)
	{
		g_bExistMissionTarget = false;
		g_nMissionTargetNpcID = 0;
	}
	else
	{
		g_bExistMissionTarget = true;
		g_nMissionTargetNpcID = nNpcID;

		if ( gg.omgr)
		{
			XNonPlayer* pNpc = gg.omgr->FindNPC_ID( nNpcID);
			if ( pNpc != NULL)	g_MissionTargetPos = pNpc->GetPosition();
		}
	}
}


// SetMissionTargetPos
void XUIMiniMap::SetMissionTargetPos( vec3* vPos)
{
	if ( vPos == NULL)
	{
		g_bExistMissionTarget = false;
		g_nMissionTargetNpcID = 0;
	}
	else
	{
		g_bExistMissionTarget = true;
		g_MissionTargetPos = *vPos;
	}
}


// GetMissionTarget
bool XUIMiniMap::GetMissionTarget( string* strName, vec3* vPos)
{
	if ( g_bExistMissionTarget == true)
	{
		if ( info.npc == NULL  ||  g_nMissionTargetNpcID == 0)
		{
			g_bExistMissionTarget = false;
			return false;
		}


		XNPCInfo* pNpcInfo = info.npc->Get( g_nMissionTargetNpcID);
		if ( pNpcInfo == NULL)
		{
			g_bExistMissionTarget = false;
			return false;
		}

		if ( strName != NULL)
			*strName = MLocale::ConvUTF16ToAnsi( pNpcInfo->GetName());

		if ( vPos != NULL)
			*vPos = g_MissionTargetPos;
	}

	return g_bExistMissionTarget;
}


// AddMarkerArtisan
void XUIMiniMap::AddMarkerArtisan( int nRecipeID, int nIndex)
{
	if ( nRecipeID <= 0  ||  nIndex < 0)
	{
		m_nArtisanFieldID = 0;
		m_nArtisanID = 0;
		return;
	}


	XRecipeNPCInfo* _info = info.recipenpc->Get( nRecipeID);
	if ( _info == NULL)
	{
		m_nArtisanFieldID = 0;
		m_nArtisanID = 0;
		return;
	}

	m_nArtisanFieldID = info.recipenpc->GetNPCFieldID( nRecipeID, nIndex);;
	m_nArtisanID = _info->m_vecFieldNPC[ nIndex].second;

	m_dwUpdateTimer -= 3000;
}


// GetMarkerArtisanPos
bool XUIMiniMap::GetMarkerArtisanPos( vec3& vPos) const
{
	if ( m_nFieldID != m_nArtisanFieldID)
		return false;

	XFieldInfo* pWorldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pWorldInfo == NULL)
		return false;

	vPos = gg.currentgamestate->GetWorld()->GetInfo()->GetNPCPos( m_nArtisanID);
	return true;
}


// OnEvent
bool XUIMiniMap::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;

	switch ( pEvent->nMessage)
	{
	case MWM_MOUSEWHEEL :
		if ( pEvent->nDelta > 0)			ZoomIn();
		else if ( pEvent->nDelta < 0)		ZoomOut();
		return true;

	case MWM_LBUTTONDBLCLK :
		RotateMap( !IsRotateMap());
		return true;

	case MWM_MOUSEMOVE :
		m_ptLastCursorPos = pEvent->Pos;
		break;
	}

	return false;
}


void XUIMiniMap::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	// Update timer
	if ( dwCurrTime < (m_dwUpdateTimer + 3000))		return;
	m_dwUpdateTimer = dwCurrTime;


	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pFieldInfo == NULL)						return;

	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if ( pMyPlayer == NULL)							return;


	// Update floor
	if ( pFieldInfo->m_vFloors.empty() == false)
	{
		// Change floor
		int nHeight = pMyPlayer->GetPosition().z;
		size_t _size = pFieldInfo->m_vFloors.size();
		for ( size_t i = 0;  i < _size;  i++)
		{
			if ( nHeight < pFieldInfo->m_vFloors[ i])
			{
				if ( m_nCurFloor != i)
				{
					m_nCurFloor = i;

					wchar_t szBuff[ MAX_PATH];
					swprintf_s( szBuff, L"%smap%05d_%d.jpg", PATH_UI_MINIMAP, pFieldInfo->m_nZoneID, (i + 1) );

					SetMapImage(szBuff);
				}

				break;
			}
		}
	}


	// Update marker
	if ( m_nArtisanFieldID == m_nFieldID)
	{
		vec3 _pos = gg.currentgamestate->GetWorld()->GetInfo()->GetNPCPos( m_nArtisanID);
		vec3 _diff = _pos - pMyPlayer->GetPosition();
		float _distance = _diff.x * _diff.x  +  _diff.y * _diff.y;
		if ( _distance < 1000000 /*10m*/)
		{
			m_nArtisanFieldID = 0;
			m_nArtisanID = 0;
		}
	}



	// Update mission target
	if ( g_bExistMissionTarget == true)
	{
		if ( gg.omgr  &&  g_nMissionTargetNpcID > 0)
		{
			XNonPlayer* pNpc = gg.omgr->FindNPC_ID( g_nMissionTargetNpcID);
			if ( pNpc != NULL)
				g_MissionTargetPos = pNpc->GetPosition();
		}
	}
}

void XUIMiniMap::SetMapImage( wchar_t * szBuff )
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














// XMiniMapLook
XMiniMapLook::XMiniMapLook()
{
	m_pSmallFont = NULL;
	m_pMaskImage = NULL;
	m_nTextAlign = MAM_HCENTER | MAM_VCENTER;
}


void XMiniMapLook::SetSmallFont(const char* szFont)
{
	m_pSmallFont = MFontManager::Get( szFont);
}


void XMiniMapLook::SetMaskImage(const char* szImage)
{
	m_pMaskImage = MBitmapManager::Get( szImage);
}


struct _mmapitem
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
	bool		out_rgn;
	string		out_bitmap;
	string		out_state;
	bool		toinner;

	_mmapitem( int _priority, MPOINT _pos, string _name, string _bitmap, string _state, float _scale)
		: priority( _priority), pos( _pos), distance( 0.0f), name( _name), bitmap( _bitmap), state( _state), scale( _scale), opacity( 1.0f), rotate( false), angle( 0.0f),
		  indicate( 0), highlight( false), out_rgn( false), toinner( false)
	{}

	_mmapitem( int _priority, vec3 _pos, string _name, string _bitmap, string _state, float _scale)
		: priority( _priority), distance( 0.0f), name( _name), bitmap( _bitmap), state( _state), scale( _scale), opacity( 1.0f), rotate( false), angle( 0.0f),
		  indicate( 0), highlight( false), out_rgn( false), toinner( false)
	{ pos.x = _pos.x;  pos.y = _pos.y;	}

	_mmapitem& operator = (const _mmapitem& other)
	{
		priority = other.priority;  pos = other.pos;  distance = other.distance;  name = other.name;  bitmap = other.bitmap;  state = other.state;
		scale = other.scale;  opacity = other.opacity;  rotate = other.rotate;  angle = other.angle;  indicate = other.indicate;  highlight = other.highlight;
		out_rgn = other.out_rgn;  out_bitmap = other.out_bitmap;  out_state = other.out_state;  toinner = other.toinner;
		return *this;
	}
};

void XMiniMapLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	XUIMiniMap* pMiniMap = (XUIMiniMap*)pWidget;
	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();

	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if ( pMyPlayer == NULL)		return;


	// Get variables
	MRECT r = pMiniMap->GetClientRect();
	MPOINT _cp = MPOINT( r.w >> 1, r.h >> 1);
	float _rot = pMyPlayer->GetDirection().AngleToXY( vec3( 0 ,1 ,0));
	float _scale = pMiniMap->GetScale();
	bool _rotate = pMiniMap->IsRotateMap();
	float _minx, _miny, _maxx, _maxy;
	pMiniMap->GetMapSize( _minx, _miny, _maxx, _maxy);

	MPOINT _pos = MPOINT( pMyPlayer->GetPosition().x, pMyPlayer->GetPosition().y);
	MPOINT _cursor = pMiniMap->GetLastCursorPos();
	MBitmap* pBitmap = pMiniMap->GetMapImage();
	float _rw = pBitmap ? pBitmap->GetWidth()  / (_maxx - _minx) : 512;
	float _rh = pBitmap ? pBitmap->GetHeight() / (_maxy - _miny) : 512;
	if ( pBitmap != NULL  &&  pBitmap->GetWidth() <= 512.0f  &&  pBitmap->GetHeight() <= 512.0f)		_scale *= 2.0f;
	float _srw = _rw * _scale;
	float _srh = _rh * _scale;
	float _rad = (float)min( _cp.x, _cp.y) - 10.0f;
	MRectF _bdrect = pMiniMap->GetBoundaryBoxRect();
	bool _boundary = false;
	if ( _bdrect.left != 0.0f  ||  _bdrect.top != 0.0f  ||  _bdrect.right != 0.0f  ||  _bdrect.bottom != 0.0f)
		_boundary = true;
	bool bDynamicField = pMiniMap->IsDynamicField();
	float _scaleratio = min( 1.0f,  ( (_scale - DEFAULT_SCALEMIN) / ( DEFAULT_SCALEMAX - DEFAULT_SCALEMIN) ) * 0.7f + 0.5f);

	XJOURNALTYPE _journaltype; int _journalid;
	XUIJournalIndicator::GetFocusedTypeID( &_journaltype, &_journalid);

	string _missiontarname;
	vec3 _missiontarpos;
	bool _missiontar = pMiniMap->GetMissionTarget( &_missiontarname, &_missiontarpos);

	XCameraBackViewController* pCameraController = global.camera->GetCameraController<XCameraBackViewController>();
	if ( pCameraController != NULL  &&  pCameraController->IsFreeLook() == false)
	{
		RCameraSceneNode* pCamera = global.camera->GetPrimaryCamera();
		if ( pCamera != NULL)	_rot = pCamera->GetDirection().AngleToXY( vec3( 0 ,1 ,0));
	}


	// Draw map
	if ( pBitmap != NULL  &&  m_pMaskImage != NULL)
	{
		// Set mask image
		pDC->SetMaskBitmap( m_pMaskImage);
		pDC->SetMaskRect( MRECT( 0, 0, r.w, r.h));

		// Draw map
		MRECT _r;
		_r.w = r.w / _scale;
		_r.h = r.h / _scale;
		_r.x =  (_pos.x - _minx) * _rw - (_r.w >> 1);
		_r.y = -(_pos.y - _miny) * _rh - (_r.h >> 1) + pBitmap->GetHeight();

		pDC->SetBitmap( pBitmap);
		if ( _boundary == true && bDynamicField == true)
		{
			MCOLOR _col = pDC->SetBitmapColor( 128, 128, 128);

			if ( _rotate == true)
			{
				pDC->DrawRotate( r.x, r.y, r.w, r.h,   (r.w >> 1), (r.h >> 1), -_rot,   _r.x, _r.y, _r.w, _r.h);

				_r.w =  ( _bdrect.right - _bdrect.left) * _rw;
				_r.h =  ( _bdrect.bottom - _bdrect.top) * _rh;
				_r.x =  ( _bdrect.left - _minx) * _rw;
				_r.y = -( _bdrect.top - _miny)  * _rh + pBitmap->GetHeight() - _r.h;

				MRECT _pr;
				_pr.w = _r.w * _scale;
				_pr.h = _r.h * _scale;
				_pr.x = _cp.x + (_bdrect.left - _pos.x) * _srw;
				_pr.y = _cp.y - (_bdrect.top - _pos.y) * _srh - _pr.h;

				pDC->SetBitmapColor( _col);
				pDC->DrawRotate( _pr.x, _pr.y, _pr.w, _pr.h,  (_cp.x - _pr.x), (_cp.y - _pr.y), -_rot,   _r.x, _r.y, _r.w, _r.h);
			}
			else
			{
				pDC->Draw( r.x, r.y, r.w, r.h,   _r.x, _r.y, _r.w, _r.h);

				_r.w =  ( _bdrect.right - _bdrect.left) * _rw;
				_r.h =  ( _bdrect.bottom - _bdrect.top) * _rh;
				_r.x =  ( _bdrect.left - _minx) * _rw;
				_r.y = -( _bdrect.top - _miny)  * _rh + pBitmap->GetHeight() - _r.h;

				MRECT _pr;
				_pr.w = _r.w * _scale;
				_pr.h = _r.h * _scale;
				_pr.x = _cp.x + (_bdrect.left - _pos.x) * _srw;
				_pr.y = _cp.y - (_bdrect.top - _pos.y) * _srh - _pr.h;

				pDC->SetBitmapColor( _col);
				pDC->Draw( _pr.x, _pr.y, _pr.w, _pr.h,   _r.x, _r.y, _r.w, _r.h);
			}
		}
		else
		{
			if ( _rotate == true)	pDC->DrawRotate( r.x, r.y, r.w, r.h,   (r.w >> 1), (r.h >> 1), -_rot,   _r.x, _r.y, _r.w, _r.h);
			else					pDC->Draw( r.x, r.y, r.w, r.h,   _r.x, _r.y, _r.w, _r.h);
		}

		// Restore state
		pDC->SetMaskBitmap( NULL);
	}



	// Draw border
	MLookBase::OnDrawBorder( pWidget, pDC);
	pDC->SetBitmap( GetBitmap());



	

	/******************************
	 Get item info
	*******************************/

	vector< _mmapitem> vItems;

	// Add me
	if ( true)
	{
		_mmapitem _item( 0, pMyPlayer->GetPosition(), MLocale::ConvUTF16ToAnsi( pMyPlayer->GetName()), "player", MWS_NORMAL, 1.0f);
		if ( _rotate == false)
		{
			_item.rotate = true;
			_item.angle = _rot;
		}
		vItems.push_back( _item);
	}


	// Add quest object icons
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
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
				if ( pQuest->m_pQuestInfo->nRewardFieldID == pMiniMap->GetFieldID() && pQuest->m_pQuestInfo->vRewardPos != vec3::ZERO)
				{
					_mmapitem _item( 2, pQuest->m_pQuestInfo->vRewardPos, MLocale::ConvUTF16ToAnsi( pQuest->m_pQuestInfo->GetTitle()), "quest", MWS_FOCUS, 1.0f);
					if ( _journaltype == XJT_QUEST  &&  _journalid == pQuest->m_pQuestInfo->nID)
					{
						_item.priority = 1;
						_item.indicate = (_missiontar == false) ? 1 : 0;
					}
					_item.out_bitmap = "questobj";
					_item.out_state = MWS_NORMAL;
					_item.toinner = true;
					vItems.push_back( _item);
				}
				continue;
			}

			vector<XQObjectiveInfo*> vecQObjectiveInfo = pQuest->m_pQuestInfo->GetAllQObjectiveInfo();
			for each ( XQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
			{
				XPlayerQObjective* pQObjective = pQuest->Get( pQObjectiveInfo->nID);
				if ( pQObjective == NULL  ||  pQObjective->bComplete == true  ||  pQObjectiveInfo->nFieldID != pMiniMap->GetFieldID()  ||  pQObjectiveInfo->nMarkerID <= 0)
					continue;

				_mmapitem _item( 2, pQObjectiveInfo->GetPos(), MLocale::ConvUTF16ToAnsi( pQuest->m_pQuestInfo->GetTitle()), "questobj", MWS_FOCUS, 1.0f);
				if ( _journaltype == XJT_QUEST  &&  _journalid == pQuest->m_pQuestInfo->nID)
				{
					_item.priority = 1;
					_item.indicate = ( _missiontar == false) ? 1 : 0;
				}
				_item.out_state = MWS_NORMAL;
				_item.toinner = true;
				vItems.push_back( _item);
				break;
			}
		}
	}


	// Add party members
	if ( gvar.MyInfo.Party.IsJoined())
	{
		for ( XObjectMap::iterator itr = gg.currentgamestate->GetObjectManager()->BeginItor();  itr != gg.currentgamestate->GetObjectManager()->EndItor();  itr++)
		{
			XObject* pObj = (*itr).second;
			if ( pObj->IsModuleEntity() == false ||  pObj->GetEntityType() != ETID_PLAYER  ||  gvar.MyInfo.Party.IsPartyMember( pObj->GetUID()) == false  ||  pObj->GetUID() == gvar.MyInfo.MyUID)
				continue;

			_mmapitem _item( 3, pObj->GetPosition(), MLocale::ConvUTF16ToAnsi( pObj->GetName()), "party", MWS_FOCUS, 1.0f);
			_item.out_state = MWS_NORMAL;
			_item.toinner = true;
			vItems.push_back( _item);
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

			_mmapitem _item( (_scaleratio < 0.9f) ? 4 : 6, _box.GetCenter(), MLocale::ConvUTF16ToAnsi( _area.GetName()), "", "", 1.0f);
			_item.opacity = (1.0f - _scaleratio) + 0.5f;
			_item.highlight = true;
			vItems.push_back( _item);
		}
	}


	// Add NPC icons
	map< pair< int, int>, _mmapitem> vNpcIcons;
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

			int nCountFloors = pFieldInfo->m_vFloors.size();
			if (nCountFloors != 0)
			{
				int nCurFloor = pMiniMap->GetCurrentFloor();
				if (nCurFloor != 0 && (*itr).vPos.z < pFieldInfo->m_vFloors[nCurFloor-1]) continue;
				if ((*itr).vPos.z > pFieldInfo->m_vFloors[nCurFloor]) continue;
			}

			MPOINT _p( (*itr).vPos.x, (*itr).vPos.y);
			_mmapitem _item( 5, _p, MLocale::ConvUTF16ToAnsi( info.npc->GetName( (*itr).nNPCID)), _bitmap, _state, _scaleratio);
			vNpcIcons.insert( map< pair< int, int>, _mmapitem>::value_type( pair< int, int>( _p.x, _p.y), _item));
		}
	}

	for (std::map<UIID, NPC_ICON_INFO>::iterator itr = gvar.Game.vecNPCIcon.begin(); itr != gvar.Game.vecNPCIcon.end();itr++)
	{
		NPC_ICON_INFO tdNPCIcon = (*itr).second;
		XNonPlayer* pNPC = gg.omgr->FindNPC_UIID((*itr).first);
		if (pNPC == NULL) continue;
		if ( pNPC->IsModuleEntity() == false  ||  pNPC->GetEntityType() != ETID_NPC)		continue;

		string _bitmap, _state;

		switch ( pNPC->GetIConState())
		{
		case NIS_QUEST_BEGIN:		_bitmap = "quest";		_state = MWS_NORMAL;			break;
		case NIS_QUEST_END	:		_bitmap = "quest";		_state = MWS_FOCUS;				break;
		case NIS_SHOP		:		_bitmap = "merchant";	_state = XFieldInfoHelper::GetNPCIconStateString( pFieldInfo, NPC_UIMAP_INFO::TYPE_SHOP, pNPC->GetNPCID());		break;
		case NIS_CRAFT		:		_bitmap = "artisan";	_state = XFieldInfoHelper::GetNPCIconStateString( pFieldInfo, NPC_UIMAP_INFO::TYPE_CRAFT, pNPC->GetNPCID());	break;
		default				:		continue;
		}

		if ( _bitmap.empty() == true  ||  _state.empty() == true)							continue;
		if      ( _bitmap == string( "quest")  &&  XGETCFG_GAME_SHOWQUESTICON == false)		continue;
		else if ( _bitmap == string( "merchant")  &&  XGETCFG_GAME_SHOWSHOPICON == false)	continue;
		else if ( _bitmap == string( "artisan")  &&  XGETCFG_GAME_SHOWCRAFTICON == false)	continue;

		int nCountFloors = pFieldInfo->m_vFloors.size();
		if (nCountFloors != 0)
		{
			int nCurFloor = pMiniMap->GetCurrentFloor();
			if (nCurFloor != 0 && pNPC->GetPosition().z < pFieldInfo->m_vFloors[nCurFloor-1]) continue;
			if (pNPC->GetPosition().z > pFieldInfo->m_vFloors[nCurFloor]) continue;
		}

		MPOINT _p( pNPC->GetPosition().x, pNPC->GetPosition().y);
		map< pair< int, int>, _mmapitem>::iterator _find = vNpcIcons.find( pair< int, int>( _p.x, _p.y));
		if ( _find != vNpcIcons.end())
		{
			(*_find).second.bitmap = _bitmap;
			(*_find).second.state = _state;
		}
		else
		{
			_mmapitem _item( 5, _p, MLocale::ConvUTF16ToAnsi( pNPC->GetName()), _bitmap, _state, _scaleratio);
			vNpcIcons.insert( map< pair< int, int>, _mmapitem>::value_type( pair< int, int>( _p.x, _p.y), _item));
		}
	}

	for ( map< pair< int, int>, _mmapitem>::iterator itr = vNpcIcons.begin();  itr != vNpcIcons.end();  itr++)
		vItems.push_back( (*itr).second);


	// Add mission target
	if ( _missiontar == true)
	{
		_mmapitem _item( 5, _missiontarpos, _missiontarname, "", "", 1.0f);
		_item.indicate = 2;
		_item.toinner = true;
		vItems.push_back( _item);
	}


	// Add artisan marker
	vec3 _artisanpos;
	if ( pMiniMap->GetMarkerArtisanPos( _artisanpos) == true)
	{
		_mmapitem _item( 5, _artisanpos, "", "", "", 1.0f);
		_item.indicate = 2;
		_item.toinner = true;
		vItems.push_back( _item);
	}





	/******************************
	 Post processing
	*******************************/

	if ( vItems.empty() == false)
	{
		size_t _size = vItems.size();
		float _rad_sqr = _rad * _rad;
		float _cos = cos( -_rot), _sin = sin( -_rot);
		for ( size_t i = 0;  i < _size;  i++)
		{
			MPOINT _bp( _cp.x + ( pMyPlayer->GetPosition().x - _pos.x) * _srw, _cp.y - ( pMyPlayer->GetPosition().y - _pos.y) * _srh);
			vItems[ i].pos = MPOINT( _cp.x + ( vItems[ i].pos.x - _pos.x) * _srw, _cp.y - ( vItems[ i].pos.y - _pos.y) * _srh);

			vec3 _diff( vItems[ i].pos.x - _cp.x, vItems[ i].pos.y - _cp.y, 0.0f);
			vItems[ i].distance = _diff.x * _diff.x  +  _diff.y * _diff.y;

			if ( vItems[ i].distance > _rad_sqr)
			{
				if ( vItems[ i].toinner == true)
				{
					_diff.Normalize();
					vItems[ i].pos = MPOINT( _cp.x + _diff.x * _rad, _cp.y + _diff.y * _rad);

					vItems[ i].rotate = true;
					vItems[ i].angle = _rotate ? -(_diff.AngleToXY( vec3( 0.0f, -1.0f, 0.0f)) + _rot) : -_diff.AngleToXY( vec3( 0.0f, -1.0f, 0.0f));

					if ( vItems[ i].out_bitmap.empty() == false)
						vItems[ i].bitmap = vItems[ i].out_bitmap;

					if ( vItems[ i].out_state.empty() == false)
						vItems[ i].state = vItems[ i].out_state;
				}
				else
					vItems[ i].out_rgn = true;
			}

			if ( vItems[ i].out_rgn == false  &&  _rotate == true)
			{
				float _x = vItems[ i].pos.x - _cp.x, _y = vItems[ i].pos.y - _cp.y;
				vItems[ i].pos.x = _cp.x + ( _x * _cos) - ( _y * _sin);
				vItems[ i].pos.y = _cp.y + ( _x * _sin) + ( _y * _cos);
			}
		}

		for ( size_t i = 0;  i < _size;  i++)
		{
			for ( size_t j = i;  j < _size;  j++)
			{
				if ( vItems[ i].priority > vItems[ j].priority  ||  
					( vItems[ i].priority == vItems[ j].priority  &&  vItems[ i].distance > vItems[ j].distance) )
				{
					_mmapitem _tmp = vItems[ i];
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
	MCOLOR _color = GetFontColor( MWP_BORDER, MWS_NORMAL, MCOLOR( 255, 255, 255));
	pDC->SetFont( GetFont());
	pDC->SetColor( _color);


	// Draw icons
	for ( vector< _mmapitem>::reverse_iterator itr = vItems.rbegin();  itr != vItems.rend();  itr++)
	{
		_mmapitem& _item = (*itr);
		if ( _item.out_rgn == true)		continue;

		MRECT* _r = GetAssignRect( _item.bitmap, _item.state);
		if ( _r == NULL)		continue;

		MRECT _tr( _item.pos.x, _item.pos.y, _r->w * _item.scale, _r->h * _item.scale);
		_tr.x -= _tr.w >> 1;
		_tr.y -= _tr.h >> 1;

		pDC->SetOpacity( (*itr).opacity);

		if ( (*itr).rotate == false)
			pDC->Draw( _tr.x, _tr.y, _tr.w, _tr.h, _r->x, _r->y, _r->w, _r->h);
		else
			pDC->DrawRotate( _tr.x, _tr.y, _tr.w, _tr.h, (_tr.w >> 1), (_tr.h >> 1), _item.angle, _r->x, _r->y, _r->w, _r->h);
	}


	// Draw tooltip
	pDC->SetOpacity( 1.0f);
	if ( vItems.empty() == false  &&  global.ui->IsMouseVisible() == true  &&  pMiniMap->IsMouseOver() == true)
	{
		int _w = 0;
		int _h = pDC->GetFont()->GetHeight();
		list< _mmapitem> _tooltip;
		for ( vector< _mmapitem>::reverse_iterator itr = vItems.rbegin();  itr != vItems.rend();  itr++)
		{
			if ( (*itr).name.empty() == true  ||  MRECT( (*itr).pos.x - 16, (*itr).pos.y - 16, 32, 32).InPoint( _cursor) == false)
				continue;

			if ( (*itr).highlight == true)		_tooltip.push_front( (*itr));
			else								_tooltip.push_back( (*itr));
			_w = max( _w, pDC->GetFont()->GetWidth( (*itr).name.c_str()));
		}

		int _x = _cursor.x - _w - ( _h + 6) - 20;
		int _y = _cursor.y;

		if ( _tooltip.empty() == false)
		{
			pDC->SetColor( 180, 180, 180, 128);
			pDC->FillRectangle( _x, _y, _w + _h + 6 + 10, ( _h + 1) * _tooltip.size() + 6);
			pDC->SetColor( _color);

			_x += 3;
			_y += 3;

			for ( list< _mmapitem>::iterator itr = _tooltip.begin();  itr != _tooltip.end();  itr++)
			{
				MRECT* _r = GetAssignRect( (*itr).bitmap, (*itr).state);
				if ( _r != NULL)	pDC->Draw( _x, _y, _h, _h, _r->x, _r->y, _r->w, _r->h);

				MCOLOR _col = pDC->GetColor();
				if ( (*itr).highlight == true)		pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MCOLOR( 255, 0, 0)) );
				pDC->Text( _x + _h, _y, (*itr).name.c_str());
				pDC->SetColor( _col);

				_y += _h + 1;
			}
		}
	}


	// Draw indicator
	for ( vector< _mmapitem>::iterator itr = vItems.begin();  itr != vItems.end();  itr++)
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


	// Draw north marker
	if ( IsValidAssign( "north", MWS_NORMAL))
	{
		MRECT _r = GetAssignRect( "north", MWS_NORMAL)[ 0];
		MPOINT _pr( 0, -min( _cp.x, _cp.y) - 4);
		if ( _rotate == true)
		{
			float x = _pr.x;			float y = _pr.y;
			float _cos = cos( -_rot);	float _sin = sin( -_rot);
			_pr.x = (x * _cos) - (y * _sin);
			_pr.y = (x * _sin) + (y * _cos);
			pDC->DrawRotate( _cp.x + _pr.x - (_r.w >> 1), _cp.y + _pr.y - (_r.h >> 1), _r.w, _r.h, (_r.w >> 1), (_r.h >> 1), -_rot, _r.x, _r.y, _r.w, _r.h);
		}
		else
			pDC->Draw( _cp.x + _pr.x - (_r.w >> 1), _cp.y + _pr.y - (_r.h >> 1), _r.w, _r.h, _r.x, _r.y, _r.w, _r.h);
	}
}


void XMiniMapLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
}
