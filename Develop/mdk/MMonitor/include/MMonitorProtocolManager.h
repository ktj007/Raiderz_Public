#ifndef _MMONITORPROTOCOLMANAGER
#define _MMONITORPROTOCOLMANAGER


#include <string>
#include <map>


using std::string;
using std::map;

class TiXmlElement;

namespace mmonitor {

typedef TiXmlElement		MMonitorXmlElement;
class MMonitorProtocol;
class MMonitorProtocolParam;

class MMonitorProtocolManager
{
private :
	// protocol id, protocol description
	// 전체 리스트를 가지고 있을 필요가 있을까?
	map< string, MMonitorProtocol* > m_RequestProtocolList;
	map< string, MMonitorProtocol* > m_ResponseProtocolList;

private :
	bool						LoadProtocolXmlTest( /*MMonitorXmlDocument* pXmlDocument*/ );
	bool						LoadProtocolTest( MMonitorXmlElement* pProtocolElement );
	MMonitorProtocolParam*		LoadParamTest( MMonitorXmlElement* pParamElement );

	bool AddMonitorDefaultProtocol();

	bool AddRequestPingProtocol();
	bool AddResponsePingProtocol();

	MMonitorProtocolParam* MakeProtocolParam( const string& strType, const string& strDesc, const string& strData );

public :
	MMonitorProtocolManager();
	~MMonitorProtocolManager();

	bool						Init( /*MMonitorXmlDocument* pXmlDocument*/ );

	bool						AddProtocol( MMonitorProtocol* pProtocol );

	const MMonitorProtocol*		GetProtocol( const string& strProtocolType, const string& strProtocolID );
	const MMonitorProtocol*		GetRequestProtocol( const string& strProtocolID);
	const MMonitorProtocol*		GetResponseProtocol( const string& strProtocolID );

	void						Release();
	void						ReleaseRequestProtocolList();
	void						ReleaseResponseProtocolList();
};




}

#endif