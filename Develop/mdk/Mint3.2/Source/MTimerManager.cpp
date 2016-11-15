#include "stdafx.h"
#include "MTimerManager.h"
#include "Mint.h"


namespace mint3
{


// MTimerManager
MTimerManager::MTimerManager()
{
}


// ~MTimerManager
MTimerManager::~MTimerManager()
{
	if ( m_vTimer.empty() == false)
	{
		for ( list< MTimerNode*>::iterator itr = m_vTimer.begin();  itr != m_vTimer.end();  itr++)
			delete (*itr);

		m_vTimer.clear();
	}
}


// Add
bool MTimerManager::Add( MWidget* pWidget, int nElapse, DWORD dwEventData)
{
	if ( pWidget == NULL)		return false;
	if ( nElapse < 0)			nElapse = 1000;


	for ( list< MTimerNode*>::iterator itr = m_vTimer.begin();  itr != m_vTimer.end();  itr++)
	{
		if ( (*itr)->pOwner == pWidget)
		{
			(*itr)->nElapse = nElapse;
			(*itr)->dwEventData = dwEventData;
			(*itr)->dwNextEventTime = MGetMint()->GetUpdateTime() + nElapse;
			(*itr)->bDelete = false;

			return true;
		}
	}


	MTimerNode* pNode = new MTimerNode();
	pNode->pOwner = pWidget;
	pNode->nElapse = nElapse;
	pNode->dwEventData = dwEventData;
	pNode->dwNextEventTime = MGetMint()->GetUpdateTime() + nElapse;
	pNode->bDelete = false;
	m_vTimer.push_back( pNode);

	return true;
}


// Remove
bool MTimerManager::Remove( MWidget* pWidget)
{
	if ( pWidget == NULL)		return false;

	for ( list< MTimerNode*>::iterator itr = m_vTimer.begin();  itr != m_vTimer.end();  itr++)
	{
		if ( (*itr)->pOwner == pWidget)
		{
			(*itr)->bDelete = true;
			return true;
		}
	}

	return false;
}


// Clear
void MTimerManager::Clear()
{
	for ( list< MTimerNode*>::iterator itr = m_vTimer.begin();  itr != m_vTimer.end();  itr++)
		(*itr)->bDelete = true;
}


// Update
void MTimerManager::Update( DWORD dwCurrTime)
{
	for ( list< MTimerNode*>::iterator itr = m_vTimer.begin();  itr != m_vTimer.end();  )
	{
		if ( (*itr)->bDelete == true)
		{
			delete (*itr);
			itr = m_vTimer.erase( itr);
			continue;
		}


		if ( dwCurrTime >= (*itr)->dwNextEventTime)
		{
			MWidget* pWidget = (*itr)->pOwner;
			pWidget->EventTimer( (*itr)->dwEventData);

			(*itr)->dwNextEventTime = dwCurrTime + (*itr)->nElapse;
		}

		itr++;
	}
}


} // namespace mint3
