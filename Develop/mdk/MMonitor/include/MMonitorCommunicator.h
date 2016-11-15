#ifndef _MMONITORCOMMUNICATOR
#define _MMONITORCOMMUNICATOR


#include <string>

using std::string;


namespace mmonitor {

class MMonitorCommunicator
{
public :
	MMonitorCommunicator() {}
	virtual ~MMonitorCommunicator() {}
	
	virtual bool	Init()		{ return true; }
	virtual void	Release()	{}

	virtual bool	Send( const DWORD dwIP, const USHORT nPort, const string& strMonitorCommand ) = 0;
};

}

#endif