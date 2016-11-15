#ifndef _MMONITORTASK
#define _MMONITORTASK


#include "MMonitorDefine.h"

#include <map>
#include <string>

using std::map;
using std::string;

namespace mmonitor {

class MTask
{
protected :
	map< string, string > m_AttrList;

	DWORD			m_nIndex;
	bool			m_IsUse;
	DWORD			m_dwInterval;
	DWORD			m_dwLastUpdateTime;

public :
	MTask();
	~MTask();

	bool					AddAttribute( const string& strAttrName, const string& strAttrValue );
	const string&			GetValueStr( const string& strName ) const;
	const bool				GetValueStr( const string& strName, string& strOutValue ) const;
	
	const DWORD				GetInterval()															{ return m_dwInterval; }
	const DWORD				GetLastUpdateTime()														{ return m_dwLastUpdateTime; }
	const DWORD				GetAttrCount()															{ return static_cast< DWORD >( m_AttrList.size() ); }
	const DWORD				GetIndex() const														{ return m_nIndex; }

	const bool				IsUse() const															{ return m_IsUse; }
	void					SetUse()																{ m_IsUse = true; }
	void					SetDontUse()															{ m_IsUse = false; }
	
	void					SetEnvirontment( const DWORD dwInterval, const bool IsUse = true );
	void					SetLastUpdatedTime( const DWORD dwCurTime )								{ m_dwLastUpdateTime = dwCurTime; }
	void					SetIndex( const DWORD dwIndex )											{ m_nIndex = dwIndex; }

public : 
	// helper function.
	const int				GetValueInt( const string& strName );
	const unsigned int		GetValueUInt( const string& strName );
	const short				GetValueShort( const string& strName );
	const unsigned short	GetValueUShort( const string& strName );

	const string&			GetName() const															{ return GetValueStr(TASK_NAME); }
	const string&			GetTarget()	const 														{ return GetValueStr(TASK_TARGET); }
	const string&			GetRequest() const														{ return GetValueStr(TASK_REQUEST); }
	const string&			GetIPStr() const														{ return GetValueStr(TASK_IP); }
	const string&			GetPortStr() const														{ return GetValueStr(TASK_PORT); }
	const unsigned short	GetPort()																{ return GetValueUShort(TASK_PORT); }
	// end helper function.
};

}

#endif