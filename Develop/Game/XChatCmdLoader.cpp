#include "stdafx.h"
#include "XChatCmdLoader.h"

#define XCMD_XML_TAG_MAIET		"maiet"
#define XCMD_XML_TAG_CHAT_CMD	"CHAT_CMD"
#define XCMD_XML_TAG_CMD		"CMD"
#define XCMD_XML_TAG_ALIAS		"ALIAS"
#define XCMD_XML_ATTR_ID		"id"
#define XCMD_XML_ATTR_NAME		"name"
#define XCMD_XML_ATTR_USAGE		"usage"
#define XCMD_XML_ATTR_HELP		"help"

XChatCmdLoader::_CmdStr* XChatCmdLoader::GetStr(int nID)
{
	map<int, _CmdStr*>::iterator itor = m_CmdMap.find(nID);
	if (itor != m_CmdMap.end())
	{
		return (*itor).second;
	}

	return NULL;
}

bool XChatCmdLoader::Load()
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(FILENAME_CHAT_CMDS).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(XCMD_XML_TAG_MAIET).FirstChildElement().Element();
	MXmlElement* pCmdElement = pElement->FirstChildElement();

	for( pCmdElement; pCmdElement != NULL; pCmdElement=pCmdElement->NextSiblingElement() )
	{
		if (!_stricmp(pCmdElement->Value(), XCMD_XML_TAG_CMD))
		{
			ParseCmd(pCmdElement, xml);
		}
	}


	return true;

}

void XChatCmdLoader::ParseCmd( MXmlElement* pElement, MXml& xml )
{
	_CmdStr* new_cmd = new _CmdStr();

	wstring strValue;
	int nID = -1;

	_Attribute(&nID, pElement, XCMD_XML_ATTR_ID);
	_Attribute(strValue, pElement, XCMD_XML_ATTR_NAME, &xml);
	wcsncpy(new_cmd->szName, strValue.c_str(), XSTRCMD_TEXSIZE);

	_Attribute(strValue, pElement, XCMD_XML_ATTR_USAGE, &xml);
	wcsncpy(new_cmd->szUsage, strValue.c_str(), XSTRCMD_TEXSIZE);

	_Attribute(strValue, pElement, XCMD_XML_ATTR_HELP, &xml);
	wcsncpy(new_cmd->szHelp, strValue.c_str(), XSTRCMD_TEXSIZE);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), XCMD_XML_TAG_ALIAS))
		{
			if (_Attribute(strValue, pChild, XCMD_XML_ATTR_NAME, &xml))
			{
				new_cmd->vecAliases.push_back(strValue);
			}
		}
	}

	if (nID != -1)
	{
		m_CmdMap.insert(map<int, _CmdStr*>::value_type(nID, new_cmd));
	}
	else
	{
		SAFE_DELETE(new_cmd);
	}

}

XChatCmdLoader::~XChatCmdLoader()
{
	while (!m_CmdMap.empty())
	{
		map<int, _CmdStr*>::iterator itorBegin = m_CmdMap.begin();
		delete (*itorBegin).second;
		m_CmdMap.erase(itorBegin);
	}
}





