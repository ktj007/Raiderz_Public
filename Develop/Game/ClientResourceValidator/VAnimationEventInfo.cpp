#include "stdafx.h"
#include "VAnimationEventInfo.h"


VAnimationEventInfo::VAnimationEventInfo()
{

}

VAnimationEventInfo::~VAnimationEventInfo()
{

}

bool VAnimationEventInfo::LoadInfo( string& strMeshFileName )
{
	m_strFileName = strMeshFileName + ANIMATION_EVENT_INFO_EXT;

	return VAnimationInfoLoader::GetAnimationEventInfoInXml(m_vecAnimationEvents, m_strFileName);
}

void VAnimationEventInfo::DeleteInfo()
{
	for(vector<ANIMATION_EVENT_INFO>::iterator it = m_vecAnimationEvents.begin(); it != m_vecAnimationEvents.end(); ++it)
	{
		it->DeleteInfo();
	}
}