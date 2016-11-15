#ifndef _MOCK_SERVER_H_
#define _MOCK_SERVER_H_

#include "GServer.h"
#include "GConst.h"
#include "GStrings.h"

class MockServer : public GServer
{
private:

protected:	
	virtual bool	InitInfo();
	virtual void	UpdateForDebug(float fDelta) {}
	virtual void	UpdateServerStatus( const float fDelta ) {}
	virtual bool CreateNetwork();
public:
	MockServer();
	virtual ~MockServer();;
};


#endif //_MOCK_SERVER_H_