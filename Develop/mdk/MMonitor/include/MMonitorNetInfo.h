#ifndef _MMONITORNETINFO
#define _MMONITORNETINFO


#include <string>

using std::string;

namespace mmonitor {

class MMonitorNetInfo
{
private :
	string	m_strIP;
    DWORD	m_dwIP;

	WORD	m_nPort;
	WORD	m_nPortHtoN;

public :
	MMonitorNetInfo() {}
	~MMonitorNetInfo() {}

	void SetIPStr( const string& strIP );					// DWORD형 ip도 자동으로 셋팅해준다.
	void SetIP( const DWORD dwIP )							{ m_dwIP = dwIP; }
	void SetIP( const DWORD dwIP, const string& strIP )		{ m_dwIP = dwIP; m_strIP = strIP; }

	void SetPort( const WORD nPort );						// NtoS형 Port도 자동으로 셋팅해준다. 
	void SetPortNtoH( const WORD nNetPort );
	
	const string&	GetIPStr() const						{ return m_strIP; }
	const DWORD		GetIP() const							{ return m_dwIP; }
	const WORD		GetPort() const							{ return m_nPort; }
	const WORD		GetPortHtoN() const						{ return m_nPortHtoN; }
	const string	GetPortStr() const;
};

}
#endif