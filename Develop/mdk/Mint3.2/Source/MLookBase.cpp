#include "stdafx.h"
#include "MLookBase.h"
#include "MColorTable.h"
#include "Mint.h"
#include "MScriptManager.h"


namespace mint3
{

// MLookStateInfo
MLookStateInfo::MLookStateInfo()
{
	m_pAssign = NULL;
	m_pTiled = NULL;
	m_pFontColor = NULL;
}

MLookStateInfo::~MLookStateInfo()
{
	if ( m_pAssign)
	{
		delete [] m_pAssign;
		m_pAssign = NULL;
	}

	if ( m_pTiled)
	{
		delete [] m_pTiled;
		m_pTiled = NULL;
	}

	if ( m_pFontColor)
	{
		delete m_pFontColor;
		m_pFontColor = NULL;
	}
}

void MLookStateInfo::SetAssignRect( int index, MRECT& r, bool bTile)
{
	if ( index < 0  ||  index >= 9)		return;

	if ( m_pAssign == NULL)
	{
		m_pAssign = new MRECT[ 9];
		memset( m_pAssign, 0, sizeof( MRECT) * 9);
	}

	if ( m_pTiled == NULL)
	{
		m_pTiled = new bool[ 9];
		memset( m_pAssign, false, sizeof( bool) * 9);
	}

	m_pAssign[ index] = r;
	m_pTiled[ index] = bTile;
}

MRECT* MLookStateInfo::GetAssignRect()
{
	if ( m_pAssign == NULL)
	{
		_ASSERT( 0);
		return NULL;
	}
	return m_pAssign;
}

void MLookStateInfo::SetFontColor( MCOLOR& color)
{
	m_pFontColor = new MCOLOR( color);
}

MCOLOR MLookStateInfo::GetFontColor() const
{
	if ( m_pFontColor == NULL)
	{
		_ASSERT( 0);
		return MCOLOR( 255, 255, 255);
	}
	return *m_pFontColor;
}





// MLookPartsInfo
MLookPartsInfo::MLookPartsInfo()
{
}

MLookPartsInfo::~MLookPartsInfo()
{
	if ( m_StateInfo.empty() == false)
	{
		for ( map< string, MLookStateInfo*>::iterator itr = m_StateInfo.begin();  itr != m_StateInfo.end();  itr++)
			delete (*itr).second;

		m_StateInfo.clear();
	}
}

MLookStateInfo* MLookPartsInfo::FindState( const string& strState)
{
	map< string, MLookStateInfo*>::iterator itr = m_StateInfo.find( strState);
	if ( itr == m_StateInfo.end())		return NULL;
	return (*itr).second;
}

void MLookPartsInfo::SetAssignRect( const string& strState, int index, MRECT& r, bool bTile)
{
	MLookStateInfo* pInfo = FindState( strState);
	if ( pInfo == NULL)
	{
		pInfo = new MLookStateInfo;
		m_StateInfo.insert( map< string, MLookStateInfo*>::value_type( strState, pInfo));
	}
	pInfo->SetAssignRect( index, r, bTile);
}

MRECT* MLookPartsInfo::GetAssignRect( const string& strState)
{
	MLookStateInfo* pInfo = FindState( strState);
	if ( pInfo == NULL)
	{
		if ( strState == string( MWS_NORMAL))	return NULL;

		pInfo = FindState( MWS_NORMAL);
		if ( pInfo == NULL)						return NULL;
	}
	return pInfo->GetAssignRect();
}

void MLookPartsInfo::SetFontColor( const string& strState, MCOLOR& color)
{
	MLookStateInfo* pInfo = FindState( strState);
	if ( pInfo == NULL)
	{
		pInfo = new MLookStateInfo;
		m_StateInfo.insert( map< string, MLookStateInfo*>::value_type( strState, pInfo));
	}
	pInfo->SetFontColor( color);
}

MCOLOR MLookPartsInfo::GetFontColor( const string& strState)
{
	MLookStateInfo* pInfo = FindState( strState);
	if ( pInfo == NULL)
	{
		if ( strState == string( MWS_NORMAL))	return MCOLOR( 255, 255, 255);

		pInfo = FindState( MWS_NORMAL);
		if ( pInfo == NULL)						return MCOLOR( 255, 255, 255);
	}
	return pInfo->GetFontColor();
}

bool MLookPartsInfo::IsValidAssign( const string& strState)
{
	MLookStateInfo* pInfo = FindState( strState);
	if ( pInfo == NULL)		return false;
	return pInfo->IsValidAssign();
}

bool MLookPartsInfo::IsValidFontColor( const string& strState)
{
	MLookStateInfo* pInfo = FindState( strState);
	if ( pInfo == NULL)		return false;
	return pInfo->IsValidFontColor();
}







// MLookBase
MLookBase::MLookBase()
{
	m_pBitmap = NULL;
	m_BitmapStyle = DRAWBITMAP_STRETCH;
	m_pFont = NULL;
	m_nTextAlign = MAM_LEFT | MAM_TOP;
	m_WorkRectOffset = MRECT( 0, 0, 0, 0);
	m_BkgrndRectOffset = MRECT( 0, 0, 0, 0);
}


// ~MLookBase
MLookBase::~MLookBase()
{
	if ( m_PartsInfo.empty() == false)
	{
		for ( map< string, MLookPartsInfo*>::iterator itr = m_PartsInfo.begin();  itr != m_PartsInfo.end();  itr++)
			delete (*itr).second;

		m_PartsInfo.clear();
	}
}


// FindParts
MLookPartsInfo* MLookBase::FindPart( const string& strPart)
{
	map< string, MLookPartsInfo*>::iterator itr = m_PartsInfo.find( strPart);
	if ( itr == m_PartsInfo.end())		return NULL;
	return (*itr).second;
}


// SetAssignRect
void MLookBase::SetAssignRect( const string& strPart, const string& strState, int index, MRECT& r, bool bTile)
{
	MLookPartsInfo* pInfo = FindPart( strPart);
	if ( pInfo == NULL)
	{
		pInfo = new MLookPartsInfo;
		m_PartsInfo.insert( map< string, MLookPartsInfo*>::value_type( strPart, pInfo));
	}
	pInfo->SetAssignRect( strState, index, r, bTile);
}


// GetAssignRect
MRECT* MLookBase::GetAssignRect( const string& strPart, const string& strState)
{
	MLookPartsInfo* pInfo = FindPart( strPart);
	if ( pInfo == NULL)		return NULL;
	return pInfo->GetAssignRect( strState);
}


// SetFontColor
void MLookBase::SetFontColor( const string& strPart, const string& strState, MCOLOR& color)
{
	MLookPartsInfo* pInfo = FindPart( strPart);
	if ( pInfo == NULL)
	{
		pInfo = new MLookPartsInfo;
		m_PartsInfo.insert( map< string, MLookPartsInfo*>::value_type( strPart, pInfo));
	}
	pInfo->SetFontColor( strState, color);
}


// GetFontColor
MCOLOR MLookBase::GetFontColor( const string& strPart, const string& strState, MCOLOR& _default)
{
	if ( IsValidFontColor( strPart, strState) == false)		return _default;

	MLookPartsInfo* pInfo = FindPart( strPart);
	return pInfo->GetFontColor( strState);
}


// IsValidAssign
bool MLookBase::IsValidAssign( const string& strPart, const string& strState)
{
	if ( m_pBitmap == NULL)		return false;

	MLookPartsInfo* pInfo = FindPart( strPart);
	if ( pInfo == NULL)		return false;

	return pInfo->IsValidAssign( strState);
}


// IsValidFontColor
bool MLookBase::IsValidFontColor( const string& strPart, const string& strState)
{
	MLookPartsInfo* pInfo = FindPart( strPart);
	if ( pInfo == NULL)		return false;
	return pInfo->IsValidFontColor( strState);
}


// GetWorkRect
MRECT MLookBase::GetWorkRect( MWidget* pWidget) const
{
	MRECT r = pWidget->GetRect();
	r.x = m_WorkRectOffset.x;
	r.y = m_WorkRectOffset.y;
	r.w += m_WorkRectOffset.w;
	r.h += m_WorkRectOffset.h;
	return r;
}


// GetWindowRect
MRECT MLookBase::GetBkgrndRect( MWidget* pWidget) const
{
	MRECT r = pWidget->GetRect();
	r.x = m_BkgrndRectOffset.x;
	r.y = m_BkgrndRectOffset.y;
	r.w += m_BkgrndRectOffset.w;
	r.h += m_BkgrndRectOffset.h;
	return r;
}


// BindScriptEvent
void MLookBase::BindScriptEvent( string& strEventName, string& strScript)
{
	if ( m_strName.empty() == true)			return;

	MEventMsg nMsg = FindEventMsgFromName( strEventName.c_str());
	if ( nMsg != MEVENT_UNKNOWN)	m_EventHandler.SetFlagScriptEvents( nMsg);

	MGetMint()->GetScriptManager()->AddScriptEvent( m_strName, strEventName, strScript);
}


// Draw
bool MLookBase::Draw( MDrawContext* pDC, MRECT& rect, const string& strPart, const string& strState)
{
	if ( IsValidAssign( strPart, strState) == false)
	{
		if ( strState == string( MWS_NORMAL))				return false;
		if ( IsValidAssign( strPart, MWS_NORMAL) == false)	return false;

		DrawBitmapByStyle( pDC, rect, GetBitmap(), GetAssignRect( strPart, MWS_NORMAL), GetBitmapStyle(), false);
		return true;
	}
		
	DrawBitmapByStyle( pDC, rect, GetBitmap(), GetAssignRect( strPart, strState), GetBitmapStyle(), false);
	return true;
}


// OnEvent
bool MLookBase::OnEvent( MWidget* pWidget, MEventMsg nEvent)
{
	if ( pWidget == NULL)		return false;

	MWidget::m_EventArgs.SetFromEvent( pWidget, NULL);
	return m_EventHandler.OnScriptEvent( m_strName, nEvent);
}


// OnDrawBorder
void MLookBase::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MRECT r = GetBkgrndRect( pWidget);
	if ( Draw( pDC, r, MWP_BORDER, pWidget->GetState()) == false)
	{
		pDC->SetColor( MColorTable::BKGRND);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pWidget->GetStateNum()]);
		pDC->Rectangle( r);
	}
}


// OnDraw
void MLookBase::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	// Insert your code
}


} // namespace mint3
