#ifndef _G_SOUL_BINDING_INFO_H_
#define _G_SOUL_BINDING_INFO_H_

#include "CSTypes.h"

class GFieldInfo;
struct MARKER_INFO;

class GSoulBindingInfo : public MTestMemPool<GSoulBindingInfo>
{
public:
	SBID			m_nID;
	const MARKER_INFO*	m_pMarkerInfo;
	int				m_nDialogID;
	int				m_nFactionID;
	
public:
	GSoulBindingInfo();
	~GSoulBindingInfo();
};

#endif//_G_SOUL_BINDING_INFO_H_