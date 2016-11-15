#include "stdafx.h"
#include "MXml.h"
#include "LightList.h"
#include "RToken.h"

namespace rs3 {

#define XML_FORMAT_FLOAT	"%.7f"

char *Format(char *buffer,RVector3 &v)
{
	sprintf(buffer,XML_FORMAT_FLOAT" "XML_FORMAT_FLOAT" "XML_FORMAT_FLOAT,v.x,v.y,v.z);
	return buffer;
}

char *Format(char *buffer,float f)
{
	sprintf(buffer,XML_FORMAT_FLOAT,f);
	return buffer;
}

char *Format(char *buffer,DWORD dw)
{
	sprintf(buffer,"%x",dw);
	return buffer;
}

void MCLIGHT::CalcName2Type(INode* node)
{
	TSTR userPropBuffer;
	node->GetUserPropBuffer(userPropBuffer);

	istringstream iStringBuffer(userPropBuffer.data());
	string aLine;
	string aValue;
	while(getline(iStringBuffer,aLine))
	{
		// trim right
		int n = aLine.find_last_not_of(" \n\r");
		if(n==string::npos) continue;
		string buf = aLine.substr(0,n+1);

		stringstream::pos_type spos;
		stringstream sstream(buf); // string을 stream에 넣는다
		sstream >> aLine;

		if(strcmp(aLine.c_str(),"set_visible_at_night")==0)
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(),"true")==0)
				m_userPropertyString.push_back(buf);
		}
	}
}

void MCLIGHT::ExportUserProperty(MXmlElement *pElement)
{
	for(list<string>::iterator ip = m_userPropertyString.begin(); ip != m_userPropertyString.end(); ip++)
	{
		MXmlElement* pUserPropertyElement = new MXmlElement("USERPROPERTY");
		pElement->LinkEndChild(pUserPropertyElement);
		pUserPropertyElement->SetAttribute("property",ip->c_str());
	}

}

LightList::~LightList()
{
	for(iterator i=begin();i!=end();i++)
		delete *i;
}

bool LightList::Save(MXmlElement *pElement, bool bStaticMesh)
{
	for(LightList::iterator i=begin();i!=end();i++)
	{
		MCLIGHT *plight=*i;

		if( plight->bStatic != bStaticMesh) continue;

		char buffer[256];

		MXmlElement* pChild=NULL;

		MXmlElement* pLightElement;
		if( plight->lightType == DIR_LIGHT )
			pLightElement = new MXmlElement("DIRLIGHT");
		else if( plight->lightType == FSPOT_LIGHT || plight->lightType == TSPOT_LIGHT)
			pLightElement = new MXmlElement("SPOTLIGHT");
		else
			pLightElement = new MXmlElement(RTOK_LIGHT);
		pElement->LinkEndChild(pLightElement);

		///// common tag
		MXmlElement* pCommonElement = new MXmlElement("COMMON");
		pLightElement->LinkEndChild(pCommonElement);
		_SetAttribute(pCommonElement, RTOK_NAME, plight->Name.c_str());

		pChild = new MXmlElement(RTOK_POSITION);
		pCommonElement->LinkEndChild(pChild);
		_SetContents(pChild, Format(buffer,plight->Position));

		if( plight->lightType == DIR_LIGHT || plight->lightType == FSPOT_LIGHT || plight->lightType == TSPOT_LIGHT)
		{
			MXmlElement* pDirectionElement = new MXmlElement(RTOK_DIRECTION);
			_SetContents(pDirectionElement, Format(buffer,plight->Direction));
			pCommonElement->LinkEndChild(pDirectionElement);

			MXmlElement* pUpElement = new MXmlElement("UP");
			_SetContents(pUpElement, Format(buffer,plight->Up));
			pCommonElement->LinkEndChild(pUpElement);
		}

		//// SceneNode Controller tag
		if( !plight->ParentName.empty() )
		{
			MXmlElement* pControllersElement = new MXmlElement("CONTROLLERS");
			pLightElement->LinkEndChild(pControllersElement);

			MXmlElement *pControllerElement = new MXmlElement("ACTORNODECONSTRAINTCONTROLLER");
			pControllersElement->LinkEndChild(pControllerElement);
			
			MXmlElement *pElement = new MXmlElement("TARGET");
			_SetContents(pElement, plight->ParentName);
			pControllerElement->LinkEndChild(pElement);
		}

		//// property tag
		MXmlElement* pPropertyElement = new MXmlElement("PROPERTY");
		pLightElement->LinkEndChild(pPropertyElement);

		pChild = new MXmlElement(RTOK_COLOR);
		pPropertyElement->LinkEndChild(pChild);
		_SetContents(pChild, Format(buffer,plight->Color));

		pChild = new MXmlElement(RTOK_INTENSITY);
		pPropertyElement->LinkEndChild(pChild);
		_SetContents(pChild, Format(buffer,plight->fIntensity));

		if( plight->lightType != DIR_LIGHT )
		{
			pChild = new MXmlElement(RTOK_ATTNSTART);
			pPropertyElement->LinkEndChild(pChild);
			_SetContents(pChild, Format(buffer,plight->fAttnStart));

			pChild = new MXmlElement(RTOK_ATTNEND);
			pPropertyElement->LinkEndChild(pChild);
			_SetContents(pChild, Format(buffer,plight->fAttnEnd));
		}

		if((plight->dwFlags & RL_FLAG_CASTSHADOW) !=0)
		{
			MXmlElement* pChild = new MXmlElement(RTOK_CASTSHADOW);
			pPropertyElement->LinkEndChild(pChild);
		}

		if( plight->lightType != FSPOT_LIGHT || plight->lightType != TSPOT_LIGHT)
		{
			pChild = new MXmlElement("FOV");
			pPropertyElement->LinkEndChild(pChild);
			_SetContents(pChild, Format(buffer,plight->fFalloff));
		}

		MXmlElement* pUserPropertyElement = new MXmlElement("USERPROPERTYLIST");
		pPropertyElement->LinkEndChild(pUserPropertyElement);
		plight->ExportUserProperty(pUserPropertyElement);
	}

	return true;
}

}
