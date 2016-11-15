#include "stdafx.h"
#include "VAnimationInfoLoader.h"

VAnimationInfoLoader::VAnimationInfoLoader()
{

}

VAnimationInfoLoader::~VAnimationInfoLoader()
{

}

bool VAnimationInfoLoader::GetAnimationInfoInXml( vector<ANIMATION_INFO>& vecOutData, string strAnimationXMLFileName, string* strFilter )
{
	MXml xml;
	if(!xml.LoadFile(strAnimationXMLFileName.c_str()))
	{
		return false;
	}

	string strPath = MGetPurePath(strAnimationXMLFileName);

	MXmlElement* pElement = xml.DocHandle().FirstChild("XML").FirstChildElement().Element();
	if(pElement)
	{
		for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
		{
			if (!_stricmp( pElement->Value(), "CopyFromXml"))
			{
				string strCopyPath, strFilter;
				_Attribute(strCopyPath, pElement, "filename");
				_Attribute(strFilter, pElement, "filter");

				string path = MGetPathFileNameFromRelativePath( strAnimationXMLFileName, strCopyPath);

				GetAnimationInfoInXml(vecOutData, path, &strFilter);
			}
			else if (!_stricmp(pElement->Value(), "AddAnimation"))
			{
				string strTempAnimationName;
				_Attribute(strTempAnimationName, pElement , "name");

				if(strFilter && strFilter->empty() == false)
				{
					int nFilterLen = strFilter->size();
					if ( strncmp( strTempAnimationName.c_str(), strFilter->c_str(), nFilterLen) != 0)
						continue;
				}

				ANIMATION_INFO stAnimation;
				stAnimation.strAnimationName = strTempAnimationName;

				char FileName[256] = {0,};
				_Attribute(FileName, pElement , "filename");
				for(size_t i=0; i<strlen(FileName); i++)
					if(FileName[i]=='\\') FileName[i]='/';

				_Attribute(&stAnimation.fAnimationSpeed , pElement, "speed");

				string strLoopType;
				_Attribute(strLoopType, pElement, "motion_loop_type");
				if(strLoopType == "loop") 
				{
					stAnimation.loopType = RAniLoopType_Loop;
				}
				else if(strLoopType == "lastframe") 
				{
					stAnimation.loopType = RAniLoopType_HoldLastFrame;
				}

				stAnimation.strAnimationFileFullName = MGetPathFileNameFromRelativePath( strPath, FileName);

				vecOutData.push_back(stAnimation);
			}
		}
	}

	return true;
}

bool VAnimationInfoLoader::GetAnimationEventInfoInXml( vector<ANIMATION_EVENT_INFO>& vecOutData, string strAnimationXMLFileName )
{
	MXml xml;
	if(!xml.LoadFile(strAnimationXMLFileName.c_str()))
	{
		return false;
	}

	MXmlElement* pRootNode = xml.Doc()->FirstChildElement();
	MXmlElement* pChild = pRootNode->FirstChildElement();

	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement()) 
	{
		string strNodeName = pChild->Value();
		if(strNodeName == "Animation")
		{
			ANIMATION_EVENT_INFO animationEventInfo;

			_Attribute(animationEventInfo.strAnimationName, pChild, "name");

			char szNodeName[256] = {0, };

			MXmlElement* pChild2 = pChild->FirstChildElement();

			for (pChild2; pChild2 != NULL; pChild2 = pChild2->NextSiblingElement()) 
			{
				strcpy(szNodeName, pChild2->Value());

				if (strcmp(szNodeName, "EVENT")==0) 
				{
					RAnimationEvent *pEvent = new RAnimationEvent;

					_Attribute(pEvent->m_strEvent, pChild2,	 "name");
					_Attribute(&pEvent->m_nFrame, pChild2, "frame");

					pEvent->m_bUnconditional = false;
					_Attribute(pEvent->m_strParam1, pChild2, "param1");
					_Attribute(pEvent->m_strParam2, pChild2, "param2");
					_Attribute(pEvent->m_strParam3, pChild2, "param3");
					_Attribute(&(pEvent->m_bUnconditional), pChild2, "unconditional");
					animationEventInfo.vecAnimationEvent.push_back(pEvent);
				}
			}

			vecOutData.push_back(animationEventInfo);
		}
	}

	return true;
}