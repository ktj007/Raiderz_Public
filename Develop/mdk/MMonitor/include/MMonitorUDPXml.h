#ifndef _MMONITORUDPXML
#define _MMONITORUDPXML



#include "MMonitorMemPool.h"
#include "MMonitorNetInfo.h"



#include <deque>
#include <string>


using std::string;
using std::deque;

namespace mmonitor {

class MMonitorUDPXml : public MMonitorMemPool< MMonitorUDPXml >
{
private :
	MMonitorNetInfo m_NetInfo;
	string			m_strXml;
	
public :
	MMonitorUDPXml() {}
	~MMonitorUDPXml() {}

	const DWORD		GetIP()	const						{ return m_NetInfo.GetIP(); } 
	const WORD		GetPort() const						{ return m_NetInfo.GetPort(); }
	const string&	GetXml() const						{ return m_strXml; }

	void SetIP( const DWORD dwIP )						{ m_NetInfo.SetIP( dwIP ); }
	void SetPortNtoH( const WORD wPort )				{ m_NetInfo.SetPortNtoH( wPort ); }
	void SetXml( const char* pXml, const DWORD dwSize );
};


class MMonitorUDPXmlManager
{
private :
	CRITICAL_SECTION m_CriSection;

	deque< MMonitorUDPXml* > m_UDPXmlList;

public :
	MMonitorUDPXmlManager();
	~MMonitorUDPXmlManager();

	void Push( MMonitorUDPXml* pUDPXml  );
	void SafePush( MMonitorUDPXml* pUDPXml );
	
	MMonitorUDPXml* Pop();
	MMonitorUDPXml* SafePop();

	DWORD GetSize() { return static_cast< DWORD >( m_UDPXmlList.size() ); }

	bool Init();
	void Release();

	void Lock()		{ EnterCriticalSection( &m_CriSection ); }
	void Unlock()	{ LeaveCriticalSection( &m_CriSection ); }
};

}

#endif