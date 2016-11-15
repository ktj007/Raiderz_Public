#ifndef _MMONITORPROTOCOL
#define _MMONITORPROTOCOL


#include "MMonitorMemPool.h"
#include "MMonitorDefine.h"


#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

class TiXmlElement;

namespace mmonitor {

//class MXmlElement;
typedef TiXmlElement		MXmlElement;


class MMonitorProtocolParam
{
private :
	friend class MMonitorProtocol;
	friend class MMonitorProtocolManager;
	friend class MMonitorProtocolBuilder;
	
	string m_strType;
	string m_strData;
	string m_strDescription;
};


class MMonitorProtocol
{
private :
	string								m_strType;
	string								m_strID;
	string								m_strDescription;
	vector< MMonitorProtocolParam* >	m_ParamList;

public :
	friend class MMonitorProtocolManager;

	MMonitorProtocol();
	~MMonitorProtocol();

	bool AddParam( MMonitorProtocolParam* pParam );

	const string&					GetType()	const		{ return m_strType; }
	const string&					GetID()	const			{ return m_strID; }
	const string&					GetDescription() const	{ return m_strDescription; }
	const DWORD						GetParamCount() const	{ return static_cast< DWORD >( m_ParamList.size() ); }

	const bool						IsRequsetType() const;
	const bool						IsResponseType() const;

	void							Release();

	const MMonitorProtocolParam*	GetParam( const DWORD dwIndex ) const;
	bool							GetParamName( const DWORD dwIndex, string& strOutParamName ) const;
};


void DeleteMonitorProtocol( MMonitorProtocol*& pProtocol );

}

#endif