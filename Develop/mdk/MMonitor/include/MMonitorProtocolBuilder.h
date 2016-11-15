#ifndef _MMONITORPROTOCOLBUILDER
#define _MMONITORPROTOCOLBUILDER


#include "MMonitorProtocolManager.h"

namespace mmonitor {

class MMonitorCommand;
class MMonitorRecvXml;
class MMonitorUDPXml;
class MMonitorXmlDocument;


class MMonitorProtocolBuilder
{
private :
	string						m_strXmlPreTag;
	string						m_strXmlTailTag;

//	MMonitorXmlDocument*		m_pXmlDocument;

	MMonitorProtocolManager		m_ProtocolManager;

public :
	MMonitorProtocolBuilder();
	~MMonitorProtocolBuilder();

	bool				Init(  );
	void				Release();

	bool				BuildXml( const MMonitorCommand* pCmd, string& strOutXml );
	MMonitorCommand*	BuildMonitorCommand( const MMonitorUDPXml* pMonitorUDPvXml );
};

}

#endif