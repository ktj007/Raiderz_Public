#ifndef _MMONITORCOMMAND
#define _MMONITORCOMMAND



#include "MMonitorMemPool.h"



#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

namespace mmonitor {

class MMonitorCommandParam : public MMonitorMemPool< MMonitorCommandParam >
{
private :
	string	m_strName;
	string	m_strType;
	string	m_strData;

public :
	const string& GetName() const			{ return m_strName; }
	const string& GetType() const			{ return m_strType; }
	const string& GetData() const			{ return m_strData; }

	void SetName( const string& strName )	{ m_strName = strName; }
	void SetType( const string& strType )	{ m_strType = strType; }
	void SetData( const string& strData )	{ m_strData = strData; }
};


class MMonitorCommandElement : public MMonitorMemPool< MMonitorCommandElement >
{
private :
	string m_strProtocolType;				// "RESPONSE" or "REQUEST"
	string m_strProtocolID;
	
	map< string, MMonitorCommandParam* >	m_ParamList;
	vector< MMonitorCommandParam* >			m_ParamListVec;

public :
	MMonitorCommandElement();
	~MMonitorCommandElement();

	

	const string&					GetProtocolType() const						{ return m_strProtocolType; }
	const string&					GetProtocolID() const						{ return m_strProtocolID; }
	const DWORD						GetParamCount() const						{ return static_cast< DWORD >( m_ParamList.size() ); }
	
	void							SetProtocolType( const string& strType )	{ m_strProtocolType = strType; }
	void							SetProtocolID( const string& strID )		{ m_strProtocolID = strID; }
	
	const bool						IsRequest() const;
	const bool						IsResponse() const;
	void							ThisIsRequest();
	void							ThisisResponse();

	bool							AddParam( const string& strName, const string& strValue );
	bool							SetParamDataType( const string& strName, const string& strType );
	
	const bool						GetParamValue( const string& strName, string& strOutValue ) const;
	const MMonitorCommandParam*		GetParam( const string& strName ) const;
	MMonitorCommandParam*			GetParam( const string& strName );
	const MMonitorCommandParam*		GetParam( const DWORD dwIndex ) const;

	void							Release();

public :
	// helper function.
	const string&					GetTaskName() const;
	const string&					GetTarget() const;

	void							SetTarget( const string& strTarget );
	void							SetTaskName( const string& strTaskName );
	// end helper function.
};


class MMonitorCommand : public MMonitorMemPool< MMonitorCommand >
{
private :
	string								m_strVersion;
	DWORD								m_dwIP;
	USHORT								m_nPort;
	
	vector< MMonitorCommandElement* >	m_CommandElementList;
	vector< MMonitorCommandElement* >	m_RequestCommandElementList;
	vector< MMonitorCommandElement* >	m_ResponseCommandElementList;

public :
	MMonitorCommand();
	~MMonitorCommand();

	const bool						AddCommandElement( MMonitorCommandElement* pCommandElement );

	const DWORD						GetElementCount() const					{ return static_cast< DWORD >( m_CommandElementList.size() ); }
	const DWORD						GetRequestElementCount() const			{ return static_cast< DWORD >( m_RequestCommandElementList.size() ); }
	const DWORD						GetResponseElementCount() const			{ return static_cast< DWORD >( m_ResponseCommandElementList.size() ); }

	const DWORD						GetIP() const							{ return m_dwIP; }
	const USHORT					GetPort() const							{ return m_nPort; }

	void							SetVertion( const string& strVersion )	{ m_strVersion = strVersion; }
	void							SetIP( const DWORD dwIP )				{ m_dwIP = dwIP; }
	void							SetPort( const unsigned short nPort )	{ m_nPort = nPort; }

	const MMonitorCommandElement*	GetElement( const DWORD dwIndex ) const ;
	const MMonitorCommandElement*	GetRequestElement( const DWORD dwIndex ) const;
	const MMonitorCommandElement*	GetResponseElement( const DWORD dwIndex ) const;

	void Release();
};


void DeleteMonitorCommand( MMonitorCommand*& pCmd );
void DeleteMonitorCommandElement( MMonitorCommandElement*& pCmdElement );
}

#endif