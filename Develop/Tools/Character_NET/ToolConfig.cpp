#include "StdAfx.h"
#include "ToolConfig.h"
#include "RConfiguration.h"
//#include "RXMLTemplate.h"

//int		CToolConfig::CAMERA_FOV				= 70;

CToolConfig::CToolConfig(void)
{
	CAMERA_FOV		= 70.f;
	start_X			= 0;
	start_Y			= 0;
	maximize		= false;
	WinForm_width	= 1092;
	WinForm_height	= 864;
	split1			= 210;
	split2			= 676;
}

CToolConfig::~CToolConfig(void)
{
}

bool CToolConfig::LoadConfig()
{
	MXml xml;
	if (xml.LoadFile("config.xml") == false)
	{
		if(xml.LoadFile("config_rel.xml") == false)
			return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pRootElement = docHandle.FirstChild("XML").Element();

	MXmlElement* pWinForm = pRootElement->FirstChildElement("WINFORM");
	if(pWinForm)
	{
		_Attribute(start_X, pWinForm, "startX");
		_Attribute(start_Y, pWinForm, "startY");
		_Attribute(&maximize, pWinForm, "maximize");
		_Attribute(WinForm_width, pWinForm, "width");
		_Attribute(WinForm_height, pWinForm, "height");
		_Attribute(split1, pWinForm, "split1");
		_Attribute(split2, pWinForm, "split2");
	}

	if(WinForm_width <= 0)
		WinForm_width = 800;
	if(WinForm_height <= 0)
		WinForm_height = 600;
	if(start_X <= 0)
		start_X = 0;
	if(start_Y <= 0)
		start_Y = 0;

	return true;
}

bool CToolConfig::SaveConfig()
{
	MXml xml;
	if (xml.LoadFile("config.xml") == false)
	{
		if (xml.LoadFile("config_rel.xml") == false)
			return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pRootElement = docHandle.FirstChild("XML").Element();
	MXmlElement* pWinForm = pRootElement->FirstChildElement("WINFORM");
	if(!pWinForm)
	{
		pWinForm = new MXmlElement("WINFORM");
		pRootElement->LinkEndChild(pWinForm);
	}

	_SetAttribute(pWinForm, "startX",	start_X);
	_SetAttribute(pWinForm, "startY",	start_Y);
	_SetAttribute(pWinForm, "maximize",	maximize);
	_SetAttribute(pWinForm, "width", WinForm_width);
	_SetAttribute(pWinForm, "height", WinForm_height);
	_SetAttribute(pWinForm, "split1", split1);
	_SetAttribute(pWinForm, "split2", split2);

	return xml.SaveFile("config.xml");
}

#define _SET_XMLVALUE_NUM(_variable, _tagname, _parentElement)						\
{																					\
	MXmlElement* _pDataElement = _parentElement->FirstChildElement(_tagname);		\
	if (_pDataElement) _Attribute(&_variable, _pDataElement, "value");				\
}	

bool CToolConfig::LoadSysConfig()
{
	MXml xml;
	if (xml.LoadFile("../../Data/system/const.xml") == false) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild("maiet").FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!stricmp(pElement->Value(), "CONST"))
		{
			_SET_XMLVALUE_NUM(CAMERA_FOV,				"CAMERA_FOV",				pElement);
			//_SET_XMLVALUE_NUM(XCONST::CAMERA_NEAR_Z,	"CAMERA_NEAR_Z",			pElement);
			//_SET_XMLVALUE_NUM(XCONST::CAMERA_FAR_Z,	"CAMERA_FAR_Z",				pElement);
			//_SET_XMLVALUE_NUM(XCONST::TREE_LOD_NEAR,	"TREE_LOD_NEAR",			pElement);
			//_SET_XMLVALUE_NUM(XCONST::TREE_LOD_FAR,	"TREE_LOD_FAR",				pElement);
			//_SET_XMLVALUE_NUM(XCONST::GRASS_LOD_NEAR,	"GRASS_LOD_NEAR",			pElement);
			//_SET_XMLVALUE_NUM(XCONST::GRASS_LOD_FAR,	"GRASS_LOD_FAR",			pElement);
		}
	}

	return true;
}