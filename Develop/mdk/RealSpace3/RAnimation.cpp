#include "StdAfx.h"
#include "RAnimation.h"
#include "RResourceID.h"
#include "RAnimationNode.h"

namespace rs3 {

RAnimation::RAnimation(string& strName, RAnimationResource* pAnimation, int nID)
: m_strAliasName(strName)
, m_pResource(pAnimation)
, m_nID(nID)
, m_AniLoopType(RAniLoopType_Loop)
, m_fSpeed(1.0f)
{
}

RAnimation::~RAnimation()
{
	ClearEvent();
}

void RAnimation::LoadEvent(MXmlElement* pElement)
{
	char szNodeName[256];
	char szEventName[256];
	int nFrame;

	MXmlElement* pChild = pElement->FirstChildElement();

	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement()) {

		strcpy(szNodeName, pChild->Value());

		if (strcmp(szNodeName, "EVENT")==0) {
			_Attribute(szEventName, pChild,	 "name");
			_Attribute(&nFrame, pChild, "frame");

			RAnimationEvent *pEvent = new RAnimationEvent;
			pEvent->m_nFrame = nFrame;
			pEvent->m_strEvent = szEventName;
			pEvent->m_bUnconditional = false;
			_Attribute(pEvent->m_strParam1, pChild, "param1");
			_Attribute(pEvent->m_strParam2, pChild, "param2");
			_Attribute(pEvent->m_strParam3, pChild, "param3");
			_Attribute(&(pEvent->m_bUnconditional), pChild, "unconditional");
			m_animationEvents.push_back(pEvent);
		}
	}	
}

void RAnimation::SaveEvent(MXmlElement* pElement)
{
	for(int i=0;i<(int)m_animationEvents.size();i++)
	{
		MXmlElement* pEvent = new MXmlElement("EVENT");
		_SetAttribute(pEvent,"name",m_animationEvents[i]->m_strEvent);
		_SetAttribute(pEvent,"frame",(int)m_animationEvents[i]->m_nFrame);
		_SetAttribute(pEvent,"param1",m_animationEvents[i]->m_strParam1);
		_SetAttribute(pEvent,"param2",m_animationEvents[i]->m_strParam2);
		_SetAttribute(pEvent,"param3",m_animationEvents[i]->m_strParam3);
		_SetAttribute(pEvent,"unconditional",m_animationEvents[i]->m_bUnconditional);
		pElement->LinkEndChild(pEvent);
	}
}

void RAnimation::ClearEvent()
{
	for(int i=0;i<(int)m_animationEvents.size();i++)
		delete m_animationEvents[i];
	m_animationEvents.clear();
}


}
