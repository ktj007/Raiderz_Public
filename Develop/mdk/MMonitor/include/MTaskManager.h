#ifndef _MMONITORTASKMANAGER
#define _MMONITORTASKMANAGER


#include "MMonitorDefine.h"
#include "MMonitorNetInfo.h"


#include <string>
#include <vector>


using std::string;
using std::vector;

namespace mmonitor {


class MTask;



class MTaskManager
{
protected :
	string				m_strName;
	bool				m_IsUse;

	// TaskManager는 Type, IP, Port로 구분을 한다.
	const string		m_strTaskType;	
	MMonitorNetInfo		m_NetInfo;

	DWORD				m_dwLastPingUpdateTime;

	vector< MTask* >	m_TaskList;

private :
	MTaskManager();

	virtual void	OnRelease() {}

public :
	MTaskManager( const string& strTaskType );
	~MTaskManager();

	bool			AddTask( MTask* pTask );
	MTask*			GetTask( const DWORD dwIndex );
	MTask*			GetTask( const string& strTaskName );

	inline DWORD	GetTaskCount()									{ return static_cast<DWORD>(m_TaskList.size()); }
    const string&	GetTaskType()									{ return m_strTaskType; }
	const string&	GetName()										{ return m_strName; }
	const string&	GetIPStr()										{ return m_NetInfo.GetIPStr(); }
	const DWORD		GetIP()											{ return m_NetInfo.GetIP(); }
	const USHORT	GetPort()										{ return m_NetInfo.GetPort(); }
	const DWORD		GetLastPingUpdateTime()	const					{ return m_dwLastPingUpdateTime; }						
	
	void			SetName( const string& strName )				{ m_strName = strName; }
	void			SetIPStr( const string& strIP )					{ m_NetInfo.SetIPStr( strIP ); }
	void			SetPort( const unsigned short nPort )			{ m_NetInfo.SetPort( nPort ); }
	void			SetUse( const bool IsUse )						{ m_IsUse = IsUse; }
	void			SetLastPingUpdateTime( const DWORD dwTime )		{ m_dwLastPingUpdateTime; }

	bool			IsUse()											{ return m_IsUse; }
	bool			MakePingTask( const DWORD dwIndex );
	void			Release();
};


void DeleteMonitorTaskManager( MTaskManager*& pTaskManager );

}

#endif