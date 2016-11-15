#pragma once

#include "MWidget.h"

using namespace mint3;



class XUIMapViewer : public MWidget
{
protected:
	MBitmapR3*	m_pFieldMap;
	MBitmapR3*	m_pWorldMap;

	MPOINT		m_ptCurrView;
	MPOINT		m_ptTargetView;
	MPOINT		m_ptPlayer;

	wstring		m_strFieldMapName;

	bool		m_bShowWorldMap;
	bool		m_bShowGrid;

	int			m_nZoom;
	bool		m_bUpdateZoomScale;

	int			m_nSelMemo;

	bool		m_bMoveView;

	float		m_fMinX, m_fMinY, m_fMaxX, m_fMaxY;


public:
	XUIMapViewer( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XUIMapViewer();

	void ShowWorldMap( bool show);
	void ShowGrid( bool show);
	void SetZoom( int zoom);

	void glueShowWorldMap();
	void glueZoom();
	void glueShowGrid();


protected:
	float GetZoomScale();

	bool LoadFieldMap( const wchar_t* szFileName);
	bool LoadWorldMap( const wchar_t* szFileName);

	void DrawFieldMap( MDrawContext* pDC, const wchar_t* szFieldName);
	void DrawGrid( MDrawContext* pDC);
 	void DrawMiniMap( MDrawContext* pDC, vec3& pos, int nFieldID);
 	void DrawWorldMap( MDrawContext* pDC);
 
 	void DrawMemo( MDrawContext* pDC, MPOINT& pos, int number, int type, const wchar_t* name = L"", const wchar_t* desc = L"");
 	void DrawParty( MDrawContext* pDC, MPOINT& pos, const wchar_t* name = L"");
	void DrawQuestObject( MDrawContext* pDC, MPOINT& pos, const wchar_t* name = L"");
 
 	void DrawCompass( MDrawContext* pDC);
 	void DrawMyCursor( MDrawContext* pDC, MPOINT& pos, float dir);
 	void DrawMapInfo( MDrawContext* pDC, const wchar_t* szMapName);

	MPOINT GetPosFromFieldMap( MPOINT& pos);
	MPOINT GetPosFromMiniMap( MRECT& rect, MPOINT& pos);
	bool GetPosDirByRect( MRECT& inner, MPOINT& point, float& angle);

protected:
 	virtual void OnDraw( MDrawContext* pDC);
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);


#define MINT_MAPVIEWER		"MapViewer"
	virtual const char* GetClassName(void)		{ return MINT_MAPVIEWER; }
};