#include "stdafx.h"
#include "MTask.h"

#include <utility>

using std::pair;

namespace mmonitor {

MTask::MTask()
{
	m_dwInterval		= 0;
	m_dwLastUpdateTime	= 0;
	m_nIndex			= 0;
}


MTask::~MTask()
{
}


bool MTask::AddAttribute( const string& strAttrName, const string& strAttrValue )
{
	map< string, string >::iterator itFind = m_AttrList.find( strAttrName );
	if( m_AttrList.end() != itFind )
		return false;

	m_AttrList.insert( pair<string, string>(strAttrName, strAttrValue) );
	return true;
}


const string& MTask::GetValueStr( const string& strName ) const
{
	map< string, string >::const_iterator itFind = m_AttrList.find( strName );
	if( m_AttrList.end() == itFind )
	{
		static string strNULL;
		return strNULL;
	}

	return itFind->second;
}

 
const bool MTask::GetValueStr( const string& strName, string& strOutValue ) const
{
	map< string, string >::const_iterator itFind = m_AttrList.find( strName );
	if( m_AttrList.end() == itFind )
		return false;

	strOutValue = itFind->second;
	return true;
}


const int MTask::GetValueInt( const string& strName )
{
	const string& strValue = GetValueStr( strName );
	if( strValue.empty() )
		return 0;

	return atoi( strValue.c_str() );
}


const unsigned int MTask::GetValueUInt( const string& strName )
{
	return static_cast< unsigned int >( GetValueInt(strName) );
}


const short MTask::GetValueShort( const string& strName )
{
	return static_cast< short >( GetValueInt(strName) );
}


const unsigned short MTask::GetValueUShort( const string& strName )
{
	return static_cast< unsigned short >( GetValueInt(strName) );
}


void MTask::SetEnvirontment( const DWORD dwInterval, const bool IsUse )
{
	m_dwInterval	= dwInterval;
	m_IsUse			= IsUse;
}

}