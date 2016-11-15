#include "stdafx.h"
#include "MTabCtrl.h"
#include "MButton.h"



namespace mint3
{


// MTabItem
MTabCtrl::MTabCtrl( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
}


// ~MTabCtrl
MTabCtrl::~MTabCtrl()
{
	RemoveAll();
}


// Add
void MTabCtrl::Add( MButton *pButton, MPanel *pPanel)
{
	MTabItem *pItem = new MTabItem( pButton, pPanel, pButton->GetListener());
	pButton->SetListener( this);
	pButton->SetButtonGroup( &m_ButtonGroup);
	m_TabList.push_back( pItem);

	SetCurSel( 0);
}


// RemoveAll
void MTabCtrl::RemoveAll()
{
	while( m_TabList.size() > 0)
	{
		MTabItem* pItem = *m_TabList.begin();
		delete pItem;
		m_TabList.pop_front();
	}
}


// SetCurSel
int MTabCtrl::SetCurSel( int index)
{
	MTabItem *pSelItem = NULL;
	int nCurIndex = 0;

	for ( MTabList::iterator i = m_TabList.begin(); i != m_TabList.end();  i++)
	{
		MTabItem *pItem = *i;
		if ( index == nCurIndex) 
		{
			pSelItem = pItem;
			m_nCurSel = nCurIndex;
		}
		else 
		{
			pItem->m_pButton->SetCheck( false);
			if ( pItem->m_pPanel)	pItem->m_pPanel->Show( false);
		}
		nCurIndex++;
	}

	if ( pSelItem)
	{
		pSelItem->m_pButton->SetCheck( true); 
		if ( pSelItem->m_pPanel)	pSelItem->m_pPanel->Show( true);
	}

	return index;
}


// OnCommand
bool MTabCtrl::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( nMsg == MEVENT_CLICK) 
	{
		int nIndex = 0;
		for ( MTabList::iterator i=m_TabList.begin();  i != m_TabList.end();  i++)
		{
			MTabItem *pItem = *i;
			if ( pItem->m_pButton == pWidget) 
			{
				SetCurSel( nIndex);

				OnSelChanged();
//				if ( pItem->m_pListener)		return pItem->m_pListener->OnCommand( pWidget,nMsg);

				return true;
			}

			nIndex++;
		}
	}

	return false;
}


// UpdateListeners
void MTabCtrl::UpdateListeners()
{
	int index = 0;
	for ( MTabList::iterator i = m_TabList.begin();  i!=m_TabList.end();  i++)
	{
		MTabItem *pItem = *i;
		if ( pItem->m_pButton->GetListener() != this)
		{
			pItem->m_pListener = pItem->m_pButton->GetListener();
			pItem->m_pButton->SetListener( this);
		}
	}
}


int MTabCtrl::glueGetCount()
{
	return GetCount();
}

int MTabCtrl::glueGetSelIndex()
{
	return GetCurSel();
}

int MTabCtrl::glueSetSelIndex( int index)
{
	return SetCurSel( index);
}

bool MTabCtrl::glueSetSelTabName( const char* szTabName)
{
	if ( szTabName == NULL)		return false;

	MTabItem *pSelItem = NULL;
	int nCurIndex = 0;
	for ( MTabList::iterator i=m_TabList.begin();  i!=m_TabList.end();  i++)
	{
		MTabItem *pItem = *i;
		if ( !strcmp( pItem->m_pButton->GetName().c_str(), szTabName))
		{
			pSelItem = pItem;
			m_nCurSel = nCurIndex;
		}
		else 
		{
			pItem->m_pButton->SetCheck( false);
			if(pItem->m_pPanel)		pItem->m_pPanel->Show( false);
		}
		nCurIndex++;
	}

	if ( pSelItem)
	{
		//юс╫ц
		pSelItem->m_pButton->SetCheck( false);
		if  (pSelItem->m_pPanel)	pSelItem->m_pPanel->Show( true);
	}

	return true;
}

bool MTabCtrl::glueSetTabName( int index, const char* szTabName)
{
	if ( szTabName == NULL)		return false;

	MTabItem *pSelItem = NULL;
	int nCurIndex = 0;

	MTabList::iterator itr = m_TabList.begin();
	for ( int i = 0;  i < index;  i++, itr++)
	{
		if ( itr == m_TabList.end())
			return false;
	}

	(*itr)->m_pButton->SetText( szTabName);

	return true;
}

} // namespace mint3