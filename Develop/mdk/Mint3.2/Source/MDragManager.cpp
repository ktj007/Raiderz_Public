#include "stdafx.h"
#include "MDragManager.h"
#include "Mint.h"
#include "MMainFrame.h"
#include "MResourceManager.h"


namespace mint3
{



// MDragData
MDragData::MDragData()
{
	m_ImageSize = MSIZE( 16, 16);
	m_pFont = MFontManager::Get( NULL);
	m_nColor = MCOLOR( 255, 255, 255);
	m_ptHotSpot = MPOINT( 0, 0);	
	m_fOpacity = 0.75f;
	m_DrawEffect = MDE_NORMAL;
}

MDragData::~MDragData()
{
	Clear();
}

bool MDragData::AddObject( MDragObject* pObject)
{
	if ( pObject == NULL)		return false;
	m_DragObjects.push_back( pObject);
	return true;
}

void MDragData::Clear()
{
	if ( m_DragObjects.empty() == false)
	{
		size_t _size = m_DragObjects.size();
		for ( size_t i = 0;  i < _size;  i++)		delete m_DragObjects[ i];
		m_DragObjects.clear();
	}
}






// MDragManager
MDragManager::MDragManager()
{
	m_pOwner = NULL;
	m_pDragData = NULL;
	m_ptCursor = MPOINT( 0, 0);
	m_bDragging = false;
	m_bDropped = false;
	m_bQueryDragBegin = false;
	m_bDropWhenBtnDown = false;
}

MDragManager::~MDragManager()
{
	Clear();
}

bool MDragManager::Clear()
{
	bool bRetVal = m_bDragging;

	if ( m_bDragging == true  &&  m_pOwner != NULL)
		m_pOwner->EventDragEnd( m_pDragData, m_ptCursor, m_bDropped);

	if ( m_pDragData != NULL)
		delete m_pDragData;

	m_pOwner = NULL;
	m_pDragData = NULL;
	m_bDragging = false;
	m_bDropped = false;
	m_bQueryDragBegin = false;
	m_bDropWhenBtnDown = false;

	return bRetVal;
}

bool MDragManager::PostProcessDragEvent( MWidget* pMainFrame, MEvent* pEvent)
{
	// Restore mouse position
	if ( pEvent->nMessage == MWM_MOUSEMOVE)
		m_ptCursor = pEvent->Pos;



	// Extern added process
	if ( m_bDropWhenBtnDown == true)
	{
		switch ( pEvent->nMessage)
		{
		case MWM_LBUTTONDOWN :
			{
				if ( m_bDragging == true)
				{
					MWidget* pWidget = FindWindowFromPoint( pMainFrame, pEvent->Pos);
					if ( pWidget != NULL  &&  pWidget->IsEnableDragAndDrop())
						m_bDropped = pWidget->EventDrop( m_pOwner, m_pDragData, pEvent->Pos);
				}

				return Clear();
			}

		case MWM_RBUTTONDOWN :
			{
				return Clear();
			}
		}
	}

	// Normal process
	else
	{
		switch ( pEvent->nMessage)
		{
		case MWM_LBUTTONDOWN :
			{
				m_ptLBtnDown = pEvent->Pos;

				Clear();

				MWidget* pOwner = FindWindowFromPoint( pMainFrame, pEvent->Pos);
				if ( pOwner != NULL  &&  pOwner->IsEnableDragAndDrop())
				{
					m_pOwner = pOwner;
					m_bQueryDragBegin = true;
				}
				break;
			}

		case MWM_LBUTTONUP :
			{
				if ( m_bDragging == true)
				{
					MWidget* pWidget = FindWindowFromPoint( pMainFrame, pEvent->Pos);
					if ( pWidget != NULL  &&  pWidget->IsEnableDragAndDrop())
						m_bDropped = pWidget->EventDrop( m_pOwner, m_pDragData, pEvent->Pos);
				}

				Clear();
				break;
			}

		case MWM_RBUTTONDOWN :
			{
				return Clear();
			}

		case MWM_MOUSEMOVE :
			{
				if ( m_bDragging == false  &&  m_bQueryDragBegin == true)
				{
					MPOINT _diff( pEvent->Pos.x - m_ptLBtnDown.x, pEvent->Pos.y - m_ptLBtnDown.y);
					int _dist  = _diff.x * _diff.x + _diff.y * _diff.y;
					if ( _dist > 500)
					{
						m_pDragData = new MDragData;
						m_pOwner->EventDragBegin( m_pDragData, pEvent->Pos);
						if ( m_pDragData->IsEmpty() == false)
						{
							m_bDragging = true;
							m_bQueryDragBegin = false;

							MGetMint()->GetMainFrame()->ReleaseCapture();
						}
						else
							Clear();
					}
				}

				break;
			}
		}
	}

	// Press ESC key
	if ( pEvent->nMessage == MWM_KEYDOWN  &&  pEvent->nKey == VK_ESCAPE)
		return Clear();


	return false;
}


MWidget* MDragManager::FindWindowFromPoint( MWidget* pMainFrame, MPOINT& pt)
{
	MWidget* pWidget = pMainFrame->WindowFromPoint( pt, true);

	// 없으면 메인 프레임에 속한 위젯 중에서 드래그를 받을 수 있는 레이어를 찾는다
	if ( pWidget == NULL)
	{
		size_t _size = pMainFrame->GetChildCount();
		for ( size_t i = 0;  i < _size;  i++)
		{
			MWidget* pChild = pMainFrame->GetChild( i);
			if ( pChild == NULL  ||  pChild->IsLayered() == false  ||  pChild->IsEnableDragAndDrop() == false)	continue;

			pWidget = pChild;
			break;
		}
	}

	return pWidget;
}


void MDragManager::AddDragData( MWidget* pOwner, MDragObject* pDragObject)
{
	if ( m_bDragging == true)
	{
		m_pDragData->AddObject( pDragObject);
		m_bDropWhenBtnDown = true;
	}

	else
	{
		m_pOwner = pOwner;
		m_bQueryDragBegin = false;
		m_bDropWhenBtnDown = true;

		m_pDragData = new MDragData;
		m_pDragData->AddObject( pDragObject);
		m_pOwner->EventDragBegin( m_pDragData, m_ptCursor);

		if ( m_pDragData->IsEmpty() == false)
			m_bDragging = true;
		else
			Clear();
	}

	MGetMint()->GetMainFrame()->ReleaseCapture();
	return;
}


void MDragManager::DrawDragImage( MDrawContext* pDC, MPOINT& pt)
{
	if ( IsEmpty())		return;

	MDrawEffect _old_effect = pDC->SetEffect( m_pDragData->GetEffect());
	MFont* pFont = m_pDragData->GetFont();
	MSIZE _image_size = m_pDragData->GetImageSize();
	int _h = max( pFont->GetHeight() + 2, _image_size.h);

	size_t _size = min( 10, m_pDragData->GetCount());

	MPOINT _hotspot = m_pDragData->GetHotSpot();
	MPOINT _pos = pt;
	_pos.x -= _hotspot.x;
	_pos.y -= _hotspot.y;


	MFont* _old_font = pDC->SetFont( m_pDragData->GetFont());
	float _old_opacity = pDC->SetOpacity( m_pDragData->GetOpacity());
	MCOLOR _old_color = pDC->SetColor( m_pDragData->GetColor());

	for ( size_t i = 0;  i < _size;  i++)
	{
		if ( m_pDragData->GetItemImage( i) != NULL)
		{
			pDC->SetBitmap( m_pDragData->GetItemImage( i));
			pDC->Draw( _pos.x, _pos.y + ((_h - _image_size.h) >> 1), _image_size.w, _image_size.h);
			pDC->Text( _pos.x + _image_size.w, _pos.y + ((_h - pFont->GetHeight()) >> 1), m_pDragData->GetItemText( i));
		}
		else
			pDC->Text( _pos.x, _pos.y + ((_h - pFont->GetHeight()) >> 1), m_pDragData->GetItemText( i));

		_pos.y += _h;
	}

	pDC->SetFont( _old_font);
	pDC->SetOpacity( _old_opacity);
	pDC->SetColor( _old_color);
	pDC->SetEffect( _old_effect);
}



} // namespace mint3
