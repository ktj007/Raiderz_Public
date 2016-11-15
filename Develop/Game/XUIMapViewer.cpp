#include "stdafx.h"
#include "XUIMapViewer.h"
#include "XGame.h"
#include "XWorld.h"
#include "XMyPlayer.h"
#include "XFieldInfo.h"
#include "RDevice.h"


#define SIZE_CURSOR_ME			50
#define SIZE_CURSOR_ME_SMALL	40
#define SIZE_CURSOR_PARTY		30
#define SIZE_CURSOR_QUESTOBJ	30
#define SIZE_COMPASS			80
#define SIZE_FLAG				40

#define MARGIN_MINIMAP			20

#define SCALE_ZOOM1				0.02f
#define SCALE_ZOOM2				0.04f
#define SCALE_MINIMAP			0.0015f

#define COLOR_MINIMAP_ME		MCOLOR( 255, 0, 0)
#define COLOR_MINIMAP_MEMO		MCOLOR( 255, 0, 255)
#define COLOR_MINIMAP_PARTY		MCOLOR( 0, 255, 255)
#define COLOR_MINIMAP_QUESTOBJ	MCOLOR( 255, 255, 0)
#define COLOR_MEMO1				MCOLOR( 255, 255, 0)
#define COLOR_MEMO2				MCOLOR( 80, 80, 255)


// 디버그용
struct TestMemo
{
	int num;
	int type;
	MPOINT pos;
	wchar_t name[ 64];
	wchar_t desc[ 256];

	int GetNumber()			{ return num;	}
	int GetType()			{ return type;	}
	MPOINT GetPos()			{ return pos;	}
	const wchar_t* GetName()	{ return name;	}
	const wchar_t* GetDesc()	{ return desc;	}
};

TestMemo* GetTestMemo( int nNum)
{
	static bool bInit = false;
	static TestMemo memo[ 10];

	if ( bInit == false)
	{
		for ( int i = 0;  i < 10;  i++)
		{
			memo[ i].num = i;
			memo[ i].type = ((i % 2) == 0) ? 0 : 1;
			memo[ i].pos = MPOINT( (rand() * 10) % 60000, (rand() * 10) % 60000);
		}

		wcscpy( memo[ 0].name, L"111");
		wcscpy( memo[ 0].desc, L"222");
		wcscpy( memo[ 1].name, L"333");
		wcscpy( memo[ 1].desc, L"444");
		wcscpy( memo[ 2].name, L"555");
		wcscpy( memo[ 2].desc, L"666");
		wcscpy( memo[ 3].name, L"777");
		wcscpy( memo[ 3].desc, L"888");
		wcscpy( memo[ 4].name, L"999");
		wcscpy( memo[ 4].desc, L"000");

		bInit = true;
	}
	return &memo[ nNum];
}

class TestParty
{
public:
	vec3 pos;
	vec3 dir;
	wchar_t name[ 32];

	vec3 GetPos()			{ return pos;	}
	vec3 GetDir()			{ return dir;	}
	const wchar_t* GetName()	{ return name;	}
};

TestParty* GetTestParty( int nNum)
{
	static bool bInit = false;
	static TestParty party[ 10];

	if ( bInit == false)
	{
		for ( int i = 0;  i < 10;  i++)
		{
			party[ i].pos = vec3( (rand() * 10) % 60000, (rand() * 10) % 60000, 0);
			party[ i].dir = vec3( 0, 1, 0);
			swprintf_s( party[ i].name, L"%s%d", L"테스트파티원", i);
		}
		bInit = true;
	}
	return &party[ nNum];
}

class TestQuestObj
{
public:
	vec3 pos;
	wchar_t name[ 32];

	vec3 GetPos()			{ return pos;	}
	const wchar_t* GetName()	{ return name;	}
};

TestQuestObj* GetTestQuestObj( int nNum)
{
	static bool bInit = false;
	static TestQuestObj QuestObj[ 10];

	if ( bInit == false)
	{
		for ( int i = 0;  i < 10;  i++)
		{
			QuestObj[ i].pos = vec3( (rand() * 10) % 60000, (rand() * 10) % 60000, 0);
			swprintf_s( QuestObj[ i].name, L"%s%d", L"테스트퀘스트", i);
		}
		bInit = true;
	}
	return &QuestObj[ nNum];
}




XUIMapViewer::XUIMapViewer( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	m_pFieldMap = NULL;
	m_pWorldMap = NULL;
	m_ptCurrView = MPOINT( 0, 0);
	m_ptTargetView = MPOINT( 0, 0);
	m_ptPlayer = MPOINT( 0, 0);
	m_bShowWorldMap = false;
	m_bShowGrid = true;
	m_nZoom = 1;
	m_nSelMemo = -1;
	m_bMoveView = false;
	m_fMinX = m_fMinY = m_fMaxX = m_fMaxY = 0.0f;
}

XUIMapViewer::~XUIMapViewer()
{
	if ( m_pFieldMap)
	{
		m_pFieldMap->Destroy();
		delete m_pFieldMap;
		m_pFieldMap = NULL;
	}


	if ( m_pWorldMap)
	{
		m_pWorldMap->Destroy();
		delete m_pWorldMap;
		m_pWorldMap = NULL;
	}
}

void XUIMapViewer::ShowWorldMap( bool show)
{
	m_bShowWorldMap = show;
}

void XUIMapViewer::ShowGrid( bool show)
{
	m_bShowGrid = show;
}

void XUIMapViewer::SetZoom( int zoom)
{
	if ( (zoom < 0) || (zoom > 2))
		zoom = 0;

	m_nZoom = zoom;

	if ( m_nZoom == 0)
		m_bMoveView = false;
}

float XUIMapViewer::GetZoomScale()
{
	static float scale_damp = -1.0;

	float scale = 0.01f;


	switch ( m_nZoom)
	{
	case 1 :
		scale = SCALE_ZOOM1;
		break;

	case 2 :
		scale = SCALE_ZOOM2;
		break;

	default :
		{
			MRECT rect = GetClientRect();

			XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
			if ( pFieldInfo == NULL)
				break;

			scale = (float)(rect.w - 40) / abs( pFieldInfo->m_fMaxX - pFieldInfo->m_fMinX);
			break;
		}
	}


	static bool bInit = false;
	if ( bInit == false)
	{
		scale_damp = scale;
		bInit = true;
	}

	if ( m_bUpdateZoomScale == false)
		return scale_damp;

	m_bUpdateZoomScale = false;

	static DWORD time = timeGetTime();
	DWORD curr = timeGetTime();
	double delta_t = (double)( curr - time) * 0.008f;
	time = curr;

	float delta_s = (scale - scale_damp) * (float)delta_t;
	if ( abs( delta_s) > abs( (float)(scale - scale_damp)))
		scale_damp = scale;
	else
		scale_damp += delta_s;

	return scale_damp;
}

void XUIMapViewer::OnDraw( MDrawContext* pDC)
{
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if ( pMyPlayer == NULL)
		return;

	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pFieldInfo == NULL)
		return;

	m_fMinX = pFieldInfo->m_fMinX;
	m_fMinY = pFieldInfo->m_fMinY;
	m_fMaxX = pFieldInfo->m_fMaxX;
	m_fMaxY = pFieldInfo->m_fMaxY;

	vec3 pos = pMyPlayer->GetPosition();
	float dir = pMyPlayer->GetDirection().AngleToXY( vec3( 0 ,1 ,0));


	static bool bInit = false;
	if ( bInit == false)
	{
		m_ptCurrView.x = m_ptTargetView.x = m_ptPlayer.x = (int)pos.x;
		m_ptCurrView.y = m_ptTargetView.y = m_ptPlayer.y = (int)pos.y;

		bInit = true;
	}


	if ( m_nZoom == 0)
	{
		m_ptTargetView.x = m_ptPlayer.x = (int)( (pFieldInfo->m_fMaxX - pFieldInfo->m_fMinX) / 2.0f + 0.5f);
		m_ptTargetView.y = m_ptPlayer.y = (int)( (pFieldInfo->m_fMaxY - pFieldInfo->m_fMinY) / 2.0f + 0.5f);;
	}
	else if ( (m_ptPlayer.x != (int)pos.x) || (m_ptPlayer.y != (int)pos.y))
	{
		m_ptTargetView.x = m_ptPlayer.x = (int)pos.x;
		m_ptTargetView.y = m_ptPlayer.y = (int)pos.y;
	}

	static DWORD time = timeGetTime();
	DWORD curr = timeGetTime();
	double delta_t = (double)(curr - time) * 0.008f;
	time = curr;

	int dx = m_ptTargetView.x - m_ptCurrView.x;
	int dy = m_ptTargetView.y - m_ptCurrView.y;
	int rx = (int)( (double)dx * delta_t + 0.5f);
	int ry = (int)( (double)dy * delta_t + 0.5f);
	if ( abs(rx) > abs(dx))	rx = dx;
	if ( abs(ry) > abs(dy))	ry = dy;

	m_ptCurrView.x += rx;
	m_ptCurrView.y += ry;


	m_bUpdateZoomScale = true;


	wchar_t szFieldName[ 32];
	swprintf_s( szFieldName, L"%05d", pFieldInfo->m_nFieldID);

	DrawFieldMap( pDC, szFieldName);
	DrawGrid( pDC);

	// 테스트용 - 가상으로 메모 표시
#ifdef _DEBUG
	for ( int i = 0;  i < 5;  i++)
	{
		if ( i == m_nSelMemo)			// 선택된 메모는 나중에 출력함
			continue;

		TestMemo* pMemo = GetTestMemo( i);

		DrawMemo( pDC, pMemo->GetPos(), pMemo->GetNumber(), pMemo->GetType());
	}
#endif

	// 파티원 표시
	for (XObjectMap::iterator itor = gg.currentgamestate->GetObjectManager()->BeginItor(); itor != gg.currentgamestate->GetObjectManager()->EndItor(); ++itor)
	{
		XObject* pObj = (*itor).second;
		if ( !pObj->IsModuleEntity())
			continue;

		if ( pObj->GetEntityType() != ETID_PLAYER)
			continue;

		if ( pObj->GetUID() == XGetMyUID())		// 내 캐릭터
			continue;

		XParty& pParty = gvar.MyInfo.Party;
		if ( pParty.IsJoined() && pParty.IsPartyMember( pObj->GetUID()))
			DrawParty( pDC, MPOINT( pObj->GetPosition().x, pObj->GetPosition().y), pParty.GetPartyMemberName( pObj->GetUID()));
	}

	// 테스트용 - 가상으로 파티원 표시
#ifdef _DEBUG
	for ( int i = 0;  i < 3;  i++)
	{
		TestParty* pParty = GetTestParty( i);

		DrawParty( pDC, MPOINT( pParty->GetPos().x, pParty->GetPos().y), pParty->GetName());
	}
#endif

	// 테스트용 - 가상으로 현재 선택된 메모 표시
#ifdef _DEBUG
	if ( m_nSelMemo >= 0)
	{
		TestMemo* pMemo = GetTestMemo( m_nSelMemo);

		DrawMemo( pDC, pMemo->GetPos(), pMemo->GetNumber(), pMemo->GetType(), pMemo->GetName(), pMemo->GetDesc());
	}
#endif

	// 퀘스트 오브젝트를 표시함
	for each ( pair<int, XPlayerQuest*> pairPlayerQuest in gvar.MyInfo.Quests.m_mapPlayerQuest)
	{
		XPlayerQuest* pPlayerQuest = pairPlayerQuest.second;

		vector<XQObjectiveInfo*> vecQObjectiveInfo = pPlayerQuest->m_pQuestInfo->GetAllQObjectiveInfo();

		for each (XQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
		{
			if (pFieldInfo->m_nFieldID != pQObjectiveInfo->nFieldID)
				continue;

			DrawQuestObject( pDC, MPOINT(pQObjectiveInfo->GetPos().x, pQObjectiveInfo->GetPos().y), L"?");
		}
	}

	// 테스트용 - 가상으로 퀘스트 오브젝트를 표시함
#ifdef _DEBUG
	for ( int i = 0;  i < 3;  i++)
	{
		TestQuestObj* pQuestObj = GetTestQuestObj( i);

		DrawQuestObject( pDC, MPOINT( pQuestObj->GetPos().x, pQuestObj->GetPos().y), pQuestObj->GetName());
	}
#endif

	DrawMyCursor( pDC, MPOINT( pos.x, pos.y), dir);
	DrawCompass( pDC);
	DrawMapInfo( pDC, pFieldInfo->GetName().c_str());
	DrawMiniMap( pDC, pos, pFieldInfo->m_nFieldID);
	DrawWorldMap( pDC);

#ifdef _DEBUG
	pDC->SetColor( 0, 0, 0);
	pDC->Rectangle( GetClientRect());

	char szBuff[ 100];
	sprintf( szBuff, "scale = %f", GetZoomScale());
	pDC->Text( 2, 1, szBuff);
#endif
}

bool XUIMapViewer::LoadFieldMap( const wchar_t* szFileName)
{
	wchar_t szBuff[ MAX_PATH];
	swprintf_s( szBuff, PATH_UI L"minimap/%s.jpg", szFileName);

	if ( m_pFieldMap != NULL)
	{
		if ( m_strFieldMapName == wstring( szBuff))
			return true;

		m_pFieldMap->Destroy();
		delete m_pFieldMap;
		m_pFieldMap = NULL;
	}

	// 임시로 이렇게 사용합니다. 이미지는 사이즈와 용량이 매우 크기때문에 동적 할당방식을 사용하도록 합니다.
	if ( m_pFieldMap == NULL)
	{
		m_pFieldMap = new MBitmapR3();
		m_pFieldMap->Create( REngine::GetDevicePtr(), "FiledMiniMap", MLocale::ConvUTF16ToAnsi(szBuff).c_str());
		m_strFieldMapName = szBuff;
	}


	if ( m_pFieldMap == NULL)
		return false;

	return true;
}

bool XUIMapViewer::LoadWorldMap( const wchar_t* szFileName)
{
	wchar_t szBuff[ MAX_PATH];
	swprintf_s( szBuff, PATH_UI L"minimap/%s", szFileName);


	// 임시로 이렇게 사용합니다. 이미지는 사이즈와 용량이 매우 크기때문에 동적 할당방식을 사용하도록 합니다.
	if ( m_pWorldMap == NULL)
	{
		m_pWorldMap = new MBitmapR3();
		m_pWorldMap->Create( REngine::GetDevicePtr(), "FiledMiniMap", MLocale::ConvUTF16ToAnsi(szBuff).c_str());
	}


	if ( m_pWorldMap == NULL)
		return false;

	return true;
}

void XUIMapViewer::DrawFieldMap( MDrawContext* pDC, const wchar_t* szFieldName)
{
	if ( LoadFieldMap( szFieldName) == false)
		return;

	pDC->SetBitmap( m_pFieldMap);


	float scale = GetZoomScale();


	// 필드 맵 표시
	MRECT r;
	r.h = (int)( abs( m_fMaxY - m_fMinY) * scale + 0.5f);
	r.w = (int)( abs( m_fMaxX - m_fMinX) * scale + 0.5f);

	MPOINT pt = GetPosFromFieldMap( MPOINT( m_fMinX, m_fMaxY));
	r.x = pt.x;
	r.y = pt.y;

	pDC->Draw( r);
}

void XUIMapViewer::DrawGrid( MDrawContext* pDC)
{
	if ( m_bShowGrid == false)
		return;

	MRECT rect = GetClientRect();

	pDC->SetColor( 0, 0, 0, 32);

	for ( float x = m_fMinX;  x <= m_fMaxX;  x += 5000.0f)
	{
		MPOINT pt_s = GetPosFromFieldMap( MPOINT( x, m_fMinY));

		if ( (pt_s.x < rect.x) || (pt_s.x > rect.w))
			continue;

		MPOINT pt_e = GetPosFromFieldMap( MPOINT( x, m_fMaxY));

		pt_s.y = min( pt_s.y, (float)rect.h);
		pt_e.y = max( pt_e.y, 0.0f);

		pDC->Line( pt_s.x, pt_s.y, pt_e.x, pt_e.y);
	}

	for ( float y = m_fMinY;  y <= m_fMaxY;  y += 5000.0f)
	{
		MPOINT pt_s = GetPosFromFieldMap( MPOINT( m_fMinX, y));

		if ( (pt_s.y < rect.y) || (pt_s.y > rect.h))
			continue;

		MPOINT pt_e = GetPosFromFieldMap( MPOINT( m_fMaxX, y));

		pt_s.x = min( pt_s.x, (float)rect.w);
		pt_e.x = max( pt_e.x, 0.0f);

		pDC->Line( pt_s.x, pt_s.y, pt_e.x, pt_e.y);
	}
}

void XUIMapViewer::DrawMiniMap( MDrawContext* pDC, vec3& pos, int nFieldID)
{
	MRECT rect = GetClientRect();
	MRECT r;
	r.w = (int)( abs(m_fMaxX - m_fMinX) * SCALE_MINIMAP + 0.5f);
	r.h = (int)( abs(m_fMaxY - m_fMinY) * SCALE_MINIMAP + 0.5f);
	r.x = rect.w - r.w - MARGIN_MINIMAP;
	r.y = rect.h - r.h - MARGIN_MINIMAP;


	// 그림자 표시
	MBitmap* pBitmap = MBitmapManager::Get( "bmp_mapshadow");
	if ( pBitmap)
	{
		int size = (int)( (float)abs(m_fMaxX - m_fMinX) * 0.0003f);
		pDC->SetBitmap( pBitmap);
		pDC->Draw( r.x - size, r.y - size, r.w + size + size, r.h + size + size);
	}


	// 미니 필드 맵 표시
	pDC->SetBitmap( m_pFieldMap);
	pDC->Draw( r);


	// 화면 영역 표시
	if ( m_nZoom > 0)
	{
		float scale = GetZoomScale();
		MRECT rs;
		rs.w = (int)( (float)rect.w / scale * SCALE_MINIMAP + 0.5f);
		rs.h = (int)( (float)rect.h / scale * SCALE_MINIMAP + 0.5f);

		MPOINT pt = GetPosFromMiniMap( r, m_ptCurrView);
		rs.x = pt.x - (rs.w >> 1);
		rs.y = pt.y - (rs.h >> 1);

		if ( rs.x < r.x)						{ rs.w -= r.x - rs.x;		rs.x = r.x;	}
		else if ( (rs.x + rs.w) > (r.x + r.w))	{ rs.w -= (rs.x + rs.w) - (r.x + r.w);	}
		if ( rs.y < r.y)						{ rs.h -= r.y - rs.y;		rs.y = r.y;	}
		else if ( (rs.y + rs.h) > (r.y + r.h))	{ rs.h -= (rs.y + rs.h) - (r.y + r.h);	}

		pDC->SetColor( 255, 255, 255, 150);
		pDC->Rectangle( rs);
	}


#ifdef _DEBUG
	// 메모 표시
	if ( ((timeGetTime() + 1000) % 4000) > 300)
	{
		for ( int i = 0;  i < 5;  i++)
		{
			TestMemo* pMemo = GetTestMemo( i);

			MPOINT pt = GetPosFromMiniMap( r, pMemo->GetPos());

			if ( r.InPoint( pt) == false)
				continue;

			pDC->SetColor( 0, 0, 0, 64);
			pDC->Rectangle( pt.x - 2, pt.y - 2, 4, 4);
			pDC->SetColor( COLOR_MINIMAP_MEMO);
			pDC->FillRectangle( pt.x - 1, pt.y - 1, 3, 3);
		}
	}
#endif

	// 파티원 표시
#ifdef _DEBUG
	// 테스트 - 파티원 표시
	if ( ((timeGetTime() + 2000) % 4000) > 300)
	{
		for (XObjectMap::iterator itor = gg.currentgamestate->GetObjectManager()->BeginItor(); itor != gg.currentgamestate->GetObjectManager()->EndItor(); ++itor)
		{
			XObject* pObj = (*itor).second;
			if ( !pObj->IsModuleEntity())
				continue;

			if ( pObj->GetEntityType() != ETID_PLAYER)
				continue;

			if ( pObj->GetUID() == XGetMyUID())		// 내 캐릭터
				continue;

			XParty& pParty = gvar.MyInfo.Party;
			if ( !pParty.IsJoined() || !pParty.IsPartyMember( pObj->GetUID()))
				continue;

			MPOINT pt = GetPosFromMiniMap( r, MPOINT( pObj->GetPosition().x, pObj->GetPosition().y));

			if ( r.InPoint( pt) == false)
				continue;

			pDC->SetColor( 0, 0, 0, 64);
			pDC->Rectangle( pt.x - 2, pt.y - 2, 4, 4);
			pDC->SetColor( COLOR_MINIMAP_PARTY);
			pDC->FillRectangle( pt.x - 1, pt.y - 1, 3, 3);
		}
	}
#endif

	// 테스트용 - 가상으로 파티원을 표시함
#ifdef _DEBUG
	if ( ((timeGetTime() + 2000) % 4000) > 300)
	{
		for ( int i = 0;  i < 3;  i++)
		{
			TestParty* pParty = GetTestParty( i);

			MPOINT pt = GetPosFromMiniMap( r, MPOINT( pParty->GetPos().x, pParty->GetPos().y));

			if ( r.InPoint( pt) == false)
				continue;

			pDC->SetColor( 0, 0, 0, 64);
			pDC->Rectangle( pt.x - 2, pt.y - 2, 4, 4);
			pDC->SetColor( COLOR_MINIMAP_PARTY);
			pDC->FillRectangle( pt.x - 1, pt.y - 1, 3, 3);
		}
	}
#endif

	// 퀘스트 오브젝트를 표시함
	if ( ((timeGetTime() + 3000) % 4000) > 300)
	{
		for each ( pair<int, XPlayerQuest*> pairPlayerQuest in gvar.MyInfo.Quests.m_mapPlayerQuest)
		{
			XPlayerQuest* pPlayerQuest = pairPlayerQuest.second;

			vector<XQObjectiveInfo*> vecQObjectiveInfo = pPlayerQuest->m_pQuestInfo->GetAllQObjectiveInfo();
			for each (XQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
			{			
				if ( nFieldID != pQObjectiveInfo->nFieldID)
					continue;

				DrawQuestObject( pDC, MPOINT(pQObjectiveInfo->GetPos().x, pQObjectiveInfo->GetPos().y), L"?");
			}
		}
	}

	// 테스트용 - 가상으로 퀘스트 오브젝트를 표시함
#ifdef _DEBUG
	if ( ((timeGetTime() + 3000) % 4000) > 300)
	{
		for ( int i = 0;  i < 3;  i++)
		{
			TestQuestObj* pQuestObj = GetTestQuestObj( i);

			MPOINT pt = GetPosFromMiniMap( r, MPOINT( pQuestObj->GetPos().x, pQuestObj->GetPos().y));

			if ( r.InPoint( pt) == false)
				continue;

			pDC->SetColor( 0, 0, 0, 64);
			pDC->Rectangle( pt.x - 2, pt.y - 2, 4, 4);
			pDC->SetColor( COLOR_MINIMAP_QUESTOBJ);
			pDC->FillRectangle( pt.x - 1, pt.y - 1, 3, 3);
		}
	}
#endif

	// 내 위치 표시
	if ( (timeGetTime() % 400) > 150)
	{
		MPOINT pt = GetPosFromMiniMap( r, MPOINT( pos.x, pos.y));

		if ( r.InPoint( pt))
		{
			pDC->SetColor( 0, 0, 0, 80);
			pDC->Rectangle( pt.x - 3, pt.y - 3, 6, 6);
			pDC->SetColor( COLOR_MINIMAP_ME);
			pDC->FillRectangle( pt.x - 2, pt.y - 2, 5, 5);
		}
	}
}

void XUIMapViewer::DrawWorldMap( MDrawContext* pDC)
{
	if ( m_bShowWorldMap == false)
		return;


	if ( LoadWorldMap( L"worldmap.jpg") == false)
		return;

	pDC->SetBitmap( m_pWorldMap);


	MRECT r;
	r.x = 10;
	r.y = 10;
	r.w = m_pWorldMap->GetWidth() >> 1;
	r.h = m_pWorldMap->GetHeight() >> 1;

	if ( m_pWorldMap != NULL)
	{
		pDC->SetBitmap( m_pWorldMap);
		pDC->Draw( r);
	}


	// 테두리 표시
	pDC->SetColor( 255, 255, 255);
	pDC->Rectangle( r);
}

void XUIMapViewer::DrawMemo( MDrawContext* pDC, MPOINT& pos, int number, int type, const wchar_t* name, const wchar_t* desc)
{
	MPOINT _pos = GetPosFromFieldMap( pos);

	MRECT inner = GetClientRect();
	inner.x -= 100;
	inner.y -= 100;
	inner.w += 200;
	inner.h += 200;
	if ( inner.InPoint( _pos) == false)
		return;

	pDC->SetBitmap( (type == 0) ? MBitmapManager::Get( "bmp_mapflag_r") : MBitmapManager::Get( "bmp_mapflag_b"));
	pDC->Draw( _pos.x, _pos.y, SIZE_FLAG, SIZE_FLAG);

	MRECT r;
	r.x = _pos.x + 5;
	r.y = _pos.y + 6;
	r.w = 30;
	r.h = 30;
	char szBuff[ 512];
	itoa( number, szBuff, 10);
	pDC->SetColor( 0, 0, 0);
	pDC->Text( r, szBuff, MAM_HCENTER | MAM_VCENTER);
	//	pDC->Rectangle( r);					// 텍스트 출력 위치 조정


	if ( (name[ 0] == 0) || (desc[ 0] == 0))
		return;

	wsprintfA( szBuff, "%S  (X:%d Y:%d)", name, pos.x, pos.y);

	r.x = _pos.x + SIZE_FLAG - 15;
	r.y = _pos.y + SIZE_FLAG - 15;
	r.w = max( pDC->GetFont()->GetWidth( szBuff) + 30,  200);
	r.h = pDC->GetFont()->GetHeight() * 3 + 20;

	if ( (r.x + r.w + 20) > m_Rect.w)
		r.x = _pos.x - r.w - 10;
	if ( (r.y + r.h + 20) > m_Rect.h)
		r.y = _pos.y - r.h - 10;

	pDC->SetColor( 0, 0, 0, 128);
	pDC->FillRectangle( r);

	r.x += 5;
	r.y += 5;
	r.w -= 10;
	r.h = pDC->GetFont()->GetHeight();
	pDC->SetColor( 255, 255, 255);
	pDC->Text( r, szBuff, MAM_LEFT | MAM_TOP);

	r.y += pDC->GetFont()->GetHeight() + 8;
	r.h = pDC->GetFont()->GetHeight() * 2;
	pDC->SetColor( 180, 180, 180);
//	pDC->TextMultiLine( r, desc, 0, true);
}

void XUIMapViewer::DrawParty( MDrawContext* pDC, MPOINT& pos, const wchar_t* name)
{
	MPOINT _pos = GetPosFromFieldMap( pos);

	MRECT inner = GetClientRect();
	inner.x += (SIZE_CURSOR_PARTY >> 1) + 5;
	inner.y += (SIZE_CURSOR_PARTY >> 1) + 5;
	inner.w -= SIZE_CURSOR_PARTY + 10;
	inner.h -= SIZE_CURSOR_PARTY + 10;

	float angle;
	bool bOut = GetPosDirByRect( inner, _pos, angle);


	if ( bOut == true)
	{
		MBitmap* pBitmapArrow = MBitmapManager::Get( "bmp_mapcursor_arrow_b");
		if ( pBitmapArrow)
		{
			pDC->SetBitmap( pBitmapArrow);
			pDC->DrawRotate( _pos.x, _pos.y, SIZE_CURSOR_PARTY, SIZE_CURSOR_PARTY,
				(SIZE_CURSOR_PARTY >> 1), (SIZE_CURSOR_PARTY >> 1), angle,
				0, 0, pBitmapArrow->GetWidth(), pBitmapArrow->GetHeight());
		}
	}
	else
	{
		pDC->SetBitmap( MBitmapManager::Get( "bmp_mapcursor_dot_b"));
		pDC->Draw( _pos.x - (SIZE_CURSOR_PARTY >> 1), _pos.y - (SIZE_CURSOR_PARTY >> 1), SIZE_CURSOR_PARTY, SIZE_CURSOR_PARTY);
	}


	if ( name[ 0] == 0)
		return;

	MRECT r;
	r.x = _pos.x + 10;
	r.y = _pos.y + 10;
	r.w = pDC->GetFont()->GetWidth( MLocale::ConvUTF16ToAnsi(name).c_str()) + 10;
	r.h = 20;

	if ( (r.x + r.w + 20) > m_Rect.w)
		r.x = _pos.x - r.w - 10;
	if ( (r.y + r.h + 20) > m_Rect.h)
		r.y = _pos.y - r.h - 10;

	pDC->SetColor( 0, 0, 0, 128);
	pDC->FillRectangle( r);
	pDC->SetColor( 255, 255, 255);
	pDC->Text( r, MLocale::ConvUTF16ToAnsi(name).c_str(), MAM_HCENTER | MAM_VCENTER);
}

void XUIMapViewer::DrawQuestObject( MDrawContext* pDC, MPOINT& pos, const wchar_t* name)
{	
	return;

	MPOINT _pos = GetPosFromFieldMap( pos);

	MRECT inner = GetClientRect();
	inner.x += (SIZE_CURSOR_QUESTOBJ >> 1) + 10;
	inner.y += (SIZE_CURSOR_QUESTOBJ >> 1) + 10;
	inner.w -= SIZE_CURSOR_QUESTOBJ + 20;
	inner.h -= SIZE_CURSOR_QUESTOBJ + 20;

	float angle;
	bool bOut = GetPosDirByRect( inner, _pos, angle);


	if ( bOut == true)
	{
		MBitmap* pBitmapArrow = MBitmapManager::Get( "bmp_mapcursor_arrow_y");
		if ( pBitmapArrow)
		{
			pDC->SetBitmap( pBitmapArrow);
			pDC->DrawRotate( _pos.x, _pos.y, SIZE_CURSOR_QUESTOBJ, SIZE_CURSOR_QUESTOBJ,
				(SIZE_CURSOR_QUESTOBJ >> 1), (SIZE_CURSOR_QUESTOBJ >> 1), angle,
				0, 0, pBitmapArrow->GetWidth(), pBitmapArrow->GetHeight());
		}
	}
	else
	{
		pDC->SetBitmap( MBitmapManager::Get( "bmp_mapcursor_dot_y"));
		pDC->Draw( _pos.x - (SIZE_CURSOR_QUESTOBJ >> 1), _pos.y - (SIZE_CURSOR_QUESTOBJ >> 1), SIZE_CURSOR_QUESTOBJ, SIZE_CURSOR_QUESTOBJ);
	}


	if ( name[ 0] == 0)
		return;

	MRECT r;
	r.x = _pos.x + 10;
	r.y = _pos.y + 10;
	r.w = pDC->GetFont()->GetWidth( MLocale::ConvUTF16ToAnsi(name).c_str()) + 10;
	r.h = 20;

	if ( (r.x + r.w + 20) > m_Rect.w)
		r.x = _pos.x - r.w - 10;
	if ( (r.y + r.h + 20) > m_Rect.h)
		r.y = _pos.y - r.h - 10;

	pDC->SetColor( 0, 0, 0, 128);
	pDC->FillRectangle( r);
	pDC->SetColor( 255, 255, 255);
	pDC->Text( r, MLocale::ConvUTF16ToAnsi(name).c_str(), MAM_LEFT | MAM_VCENTER);
}

void XUIMapViewer::DrawCompass( MDrawContext* pDC)
{
	MBitmap* pBitmap = MBitmapManager::Get( "bmp_mapcompass");
	if ( pBitmap == NULL)
		return;

	pDC->SetBitmap( pBitmap);


	// Compass
	pDC->Draw( GetClientRect().w - SIZE_COMPASS - 10, 10, SIZE_COMPASS, SIZE_COMPASS);
}

void XUIMapViewer::DrawMyCursor( MDrawContext* pDC, MPOINT& pos, float dir)
{
	MBitmap* pBitmap = MBitmapManager::Get( "bmp_mapcursor_arrow_r");
	if ( pBitmap == NULL)
		return;

	pDC->SetBitmap( pBitmap);


	MPOINT _pos = GetPosFromFieldMap( pos);

	MRECT inner = GetClientRect();
	inner.x += (SIZE_CURSOR_ME_SMALL >> 1);
	inner.y += (SIZE_CURSOR_ME_SMALL >> 1);
	inner.w -= SIZE_CURSOR_ME_SMALL;
	inner.h -= SIZE_CURSOR_ME_SMALL;

	float angle;
	bool bOut = GetPosDirByRect( inner, _pos, angle);

	if ( bOut == true)
	{
		pDC->DrawRotate( _pos.x, _pos.y, SIZE_CURSOR_ME_SMALL, SIZE_CURSOR_ME_SMALL,
			(SIZE_CURSOR_ME_SMALL >> 1), (SIZE_CURSOR_ME_SMALL >> 1), angle,
			0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
	}
	else
	{
		float delta_size = 10.0f * sin( (double)timeGetTime() * 0.004f);
		if ( delta_size < 0.0f)
			delta_size = 0.0f;

		// 위치 좌표 표시
		char szBuff[ 64];
		sprintf_s( szBuff, "X:%d , Y:%d", (int)pos.x, (int)pos.y);
		MRECT r = MRECT( _pos.x + (SIZE_CURSOR_ME >> 1) - 10, _pos.y + (SIZE_CURSOR_ME >> 1) - 10,
			pDC->GetFont()->GetWidth( szBuff) + 10, pDC->GetFont()->GetHeight() + 6);

		pDC->SetColor( 0, 0, 0, 128);
		pDC->FillRectangle( r);

		pDC->SetColor( 255, 255, 255);
		pDC->Text( r, szBuff, MAM_HCENTER | MAM_VCENTER);


		// Shadow
		MCOLOR color = pDC->GetBitmapColor();
		pDC->SetBitmapColor( 0, 0, 0);
		int opacity = pDC->GetOpacity();
		pDC->SetOpacity( 0.25f);
		int size = SIZE_CURSOR_ME + 5 + delta_size;
		pDC->DrawRotate( _pos.x + 3, _pos.y + 3, size, size, size >> 1, size >> 1, dir, 0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());

		// Cursor
		pDC->SetBitmapColor( 255, 255, 255);
		pDC->SetOpacity( 1.0f);
		size = SIZE_CURSOR_ME + delta_size;
		pDC->DrawRotate( _pos.x, _pos.y, size, size, size >> 1, size >> 1, dir, 0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());

		pDC->SetBitmapColor( color);
		pDC->SetOpacity( opacity);
	}
}

void XUIMapViewer::DrawMapInfo( MDrawContext* pDC, const wchar_t* szMapName)
{
	MRECT rect = GetClientRect();
	int width = pDC->GetFont()->GetWidth( MLocale::ConvUTF16ToAnsi(szMapName).c_str());

	MRECT r;
	r.x = ((rect.w - width) >> 1) - 30;
	r.y = 30;
	r.w = width + 60;
	r.h = pDC->GetFont()->GetHeight() + 10;

	// 맵 이름 표시
	pDC->SetColor( 0, 0, 0, 128);
	pDC->FillRectangle( r);

	pDC->SetColor( 255, 255, 255);
	pDC->Text( r, MLocale::ConvUTF16ToAnsi(szMapName).c_str(), MAM_HCENTER | MAM_VCENTER);
}

MPOINT XUIMapViewer::GetPosFromFieldMap( MPOINT& pos)
{
	MRECT rect = GetClientRect();
	float scale = GetZoomScale();

	MPOINT pt;
	pt.x = (rect.w >> 1) - (int)((float)(m_ptCurrView.x - pos.x + m_fMinX) * scale + 0.5f);
	pt.y = (rect.h >> 1) + (int)((float)(m_ptCurrView.y - pos.y + m_fMinY) * scale + 0.5f);

	return pt;
}

MPOINT XUIMapViewer::GetPosFromMiniMap( MRECT& rect, MPOINT& pos)
{
	float scale = SCALE_MINIMAP;

	MPOINT pt;
	pt.x = rect.x + (int)( ((float)pos.x - m_fMinX) * scale + 0.5f);
	pt.y = rect.y + rect.h - (int)( ((float)pos.y - m_fMinY) * scale + 0.5f);

	return pt;
}

bool XUIMapViewer::GetPosDirByRect( MRECT& inner, MPOINT& point, float& angle)
{
	angle = 0.0f;

	MRECT rect = GetClientRect();

	bool bOut = false;

	// x축에 대한 리사이징 시도
	if ( (point.x < inner.x) || ( point.x > (inner.x + inner.w)))
	{
		int d1 = inner.w >> 1;
		int d2 = point.x - (rect.w >> 1);
		float ratio = abs( (float)d1 / (float)d2);
		point.x = (int)( (float)(point.x - (rect.w >> 1)) * ratio + (float)(rect.w >> 1) + 0.5f);
		point.y = (int)( (float)(point.y - (rect.h >> 1)) * ratio + (float)(rect.h >> 1) + 0.5f);

		bOut = true;
	}


	// y축에 대한 리사이징 시도
	if ( (point.y < inner.y) || ( point.y > (inner.y + inner.h)))
	{
		int d1 = inner.h >> 1;
		int d2 = point.y - (rect.h >> 1);
		float ratio = abs( (float)d1 / (float)d2);
		point.x = (int)( (float)(point.x - (rect.w >> 1)) * ratio + (float)(rect.w >> 1) + 0.5f);
		point.y = (int)( (float)(point.y - (rect.h >> 1)) * ratio + (float)(rect.h >> 1) + 0.5f);

		bOut = true;
	}


	if ( bOut == true)
	{
		angle = (float)atan2( (double)(point.y - (rect.h >> 1)), (double)(point.x - (rect.w >> 1))) + 3.14f * 0.5f;

		return true;
	}

	return false;
}

bool XUIMapViewer::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)
		return true;


	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONUP :
		{
			if ( m_bMoveView == false)
				break;

			m_bMoveView = false;

			ReleaseCapture();

			return true;
		}

	case MWM_LBUTTONDOWN :
	case MWM_MOUSEMOVE :
		{
			XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
			if ( pFieldInfo == NULL)
				break;


			MPOINT pt = pEvent->Pos;
			MRECT rectClient = GetClientRect();
			MRECT rectWindow = GetInitialClientRect();
			MRECT r;
			r.w = (int)( abs(pFieldInfo->m_fMaxX - pFieldInfo->m_fMinX) * SCALE_MINIMAP);
			r.h = (int)( abs(pFieldInfo->m_fMaxY - pFieldInfo->m_fMinY) * SCALE_MINIMAP);
			r.x = rectWindow.x + rectClient.w - r.w - MARGIN_MINIMAP;
			r.y = rectWindow.y + rectClient.h - r.h - MARGIN_MINIMAP;


			// 현재 선택한 메모장이 있는지 확인
			if ( pEvent->nMessage == MWM_MOUSEMOVE)
			{
				if ( (m_bLButtonDown == false) || (m_bMoveView == false))
				{
					m_nSelMemo = -1;

#ifdef _DEBUG
					// 테스트용
					for ( int i = 0;  i < 5;  i++)
					{
						TestMemo* pMemo = GetTestMemo( i);

						MPOINT _pos = GetPosFromFieldMap( pMemo->GetPos());
						MRECT inner;
						inner.x = _pos.x;
						inner.y = _pos.y;
						inner.w = SIZE_FLAG;
						inner.h = SIZE_FLAG;
						if ( inner.InPoint( pt) == true)
						{
							m_nSelMemo = i;
							break;
						}
					}
#endif
					break;
				}
			}

			if ( m_nZoom == 0)
				break;

			if ( pEvent->nMessage == MWM_MOUSEMOVE)
			{
				if ( pt.x < r.x)				pt.x = r.x;
				else if ( pt.x > (r.x + r.w))	pt.x = r.x + r.w;
				if ( pt.y < r.y)				pt.y = r.y;
				else if ( pt.y > (r.y + r.w))	pt.y = r.y + r.w;
			}
			else
			{
				if ( r.InPoint( pt) == false)
					break;

				m_bMoveView = true;
				SetCapture();
			}

			m_ptTargetView.x = (int)( (float)(       pt.x - r.x) / SCALE_MINIMAP + 0.5f);
			m_ptTargetView.y = (int)( (float)( r.h - pt.y + r.y) / SCALE_MINIMAP + 0.5f);

			return true;
		}
	}

	return false;
}

void XUIMapViewer::glueShowWorldMap()
{
	ShowWorldMap( (m_bShowWorldMap == true) ? false : true);
}

void XUIMapViewer::glueZoom()
{
	SetZoom( m_nZoom + 1);
}

void XUIMapViewer::glueShowGrid()
{
	ShowGrid( (m_bShowGrid == true) ? false : true);
}
