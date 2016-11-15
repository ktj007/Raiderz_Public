#include "stdafx.h"
#include "MLookBase.h"
#include "MLookManager.h"


namespace mint3
{


// MLookManager
MLookManager::MLookManager()
{
}


// ~MLookManager
MLookManager::~MLookManager()
{
	Clear();
}


// Add
bool MLookManager::Add( const string& strName, MLookBase* pLook)
{
	map< string, MLookBase*>::iterator itr = m_Look.find( strName);
	if ( itr != m_Look.end())
	{
//		delete (*itr).second;
//		(*itr).second = pLook;
		return false;
	}

	m_Look.insert( map< string, MLookBase*>::value_type( strName, pLook));
	return true;
}


// Get
MLookBase* MLookManager::Get( const string& strName)
{
	map< string, MLookBase*>::iterator itr = m_Look.find( strName);
	if ( itr == m_Look.end())		return NULL;
	return (*itr).second;
}


// Clear
void MLookManager::Clear()
{
	if ( m_Look.empty() == false)
	{
		for ( map< string, MLookBase*>::iterator itr = m_Look.begin();  itr != m_Look.end();  itr++)
			delete (*itr).second;

		m_Look.clear();
	}
}


} // namespace mint3