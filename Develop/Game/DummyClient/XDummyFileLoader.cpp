#include "stdafx.h"
#include "XDummyFileLoader.h"
#include "XDummyMaster.h"
#include "XDummyPattern.h"
#include "XDummyHelper.h"
#include "XDummyCollision.h"

#define DUMMYCLIENT_XML_TAG_MAIET					"maiet"
#define DUMMYCLIENT_XML_TAG_PATTERNS				"Patterns"
#define DUMMYCLIENT_XML_TAG_PATTERN					"Pattern"
#define DUMMYCLIENT_XML_TAG_AGENTS					"Agents"
#define DUMMYCLIENT_XML_TAG_AGENT					"Agent"
#define DUMMYCLIENT_XML_TAG_CONFIG					"Config"

#define DUMMYCLIENT_CONFIG_TAG_GAME_SERVER			"GameServer"
#define DUMMYCLIENT_CONFIG_TAG_LOGIN_SERVER			"LoginServer"
#define DUMMYCLIENT_CONFIG_TAG_ID					"Id"
#define DUMMYCLIENT_CONFIG_TAG_STARTINGID			"startingid"
#define DUMMYCLIENT_CONFIG_TAG_RUNNING_TIME			"RunningTime"
#define DUMMYCLIENT_CONFIG_TAG_DIRECT_LOGIN_TO_GAME_SERVER		"DirectLoginToGameServer"
#define DUMMYCLIENT_AGENT_TAG_PATTERN				"Pattern"
#define DUMMYCLIENT_AGENT_TAG_VALUE					"Value"

#define DUMMYCLIENT_AGENT_ATTR_NAME					"name"
#define DUMMYCLIENT_AGENT_ATTR_COUNT				"count"
#define DUMMYCLIENT_AGENT_ATTR_TYPE					"type"


bool XDummyFileLoader::LoadFromStream( const wchar_t* szStream )
{
	USES_CONVERSION_EX;

	if(!m_Xml.LoadStream(W2A_EX(szStream, 0)))
	{
		return false;
	}

	MXmlElement* pElement = m_Xml.Doc()->FirstChildElement(DUMMYCLIENT_XML_TAG_MAIET);
	if (pElement)
	{
		ParseRoot(pElement);
	}

	return true;
}

bool XDummyFileLoader::Load( const wchar_t* szFileName )
{
	USES_CONVERSION_EX;

	m_Xml.LoadFile(W2A_EX(szFileName, 0));

	MXmlElement* pElement = m_Xml.Doc()->FirstChildElement(DUMMYCLIENT_XML_TAG_MAIET);
	if (pElement)
	{
		ParseRoot(pElement);
	}

	return true;
}

void XDummyFileLoader::ParseRoot( MXmlElement* pRootElement )
{
	for (MXmlElement* pChild = pRootElement->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (IsSameTag(pChild, DUMMYCLIENT_XML_TAG_AGENTS))
		{
			ParseAgents(pChild);			
		}
		if (IsSameTag(pChild, DUMMYCLIENT_XML_TAG_PATTERNS))
		{
			ParsePatterns(pChild);
		}
		else if (IsSameTag(pChild, DUMMYCLIENT_XML_TAG_CONFIG))
		{
			ParseConfig(pChild);
		}
	}
}

bool XDummyFileLoader::IsSameTag( MXmlElement* pElement, const char* szTagName )
{
	if (!_stricmp(pElement->Value(), szTagName)) return true;
	return false;
}

void XDummyFileLoader::ParsePatterns( MXmlElement* pElement )
{
	for (MXmlElement* pChild = pElement->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (IsSameTag(pChild, DUMMYCLIENT_XML_TAG_PATTERN))
		{
			ParsePattern(pChild);
		}
	}
}

void XDummyFileLoader::ParseAgents( MXmlElement* pElement )
{
	for (MXmlElement* pChild = pElement->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (IsSameTag(pChild, DUMMYCLIENT_XML_TAG_AGENT))
		{
			ParseAgent(pChild);
		}
	}
}

void XDummyFileLoader::ParseConfig( MXmlElement* pElement )
{
	USES_CONVERSION_EX;

	for (MXmlElement* pChild = pElement->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (IsSameTag(pChild, DUMMYCLIENT_CONFIG_TAG_GAME_SERVER))
		{
			string strGameServer;
			_Contents(strGameServer, pChild);
			m_pData->m_Config.strGameServer = A2W_EX(strGameServer.c_str(), 0);
		}
		else if (IsSameTag(pChild, DUMMYCLIENT_CONFIG_TAG_LOGIN_SERVER))
		{
			string strLoginServer;
			_Contents(strLoginServer, pChild);
			m_pData->m_Config.strLoginServer = A2W_EX(strLoginServer.c_str(), 0);
		}
		else if (IsSameTag(pChild, DUMMYCLIENT_CONFIG_TAG_ID))
		{
			string strID;
			_Contents(strID, pChild);
			m_pData->m_Config.strID = A2W_EX(strID.c_str(), 0);
		}
		else if (IsSameTag(pChild, DUMMYCLIENT_CONFIG_TAG_STARTINGID))
		{
			_Contents(&(m_pData->m_Config.nStartingID), pChild);
		}
		else if (IsSameTag(pChild, DUMMYCLIENT_CONFIG_TAG_RUNNING_TIME))
		{
			_Contents(&(m_pData->m_Config.nRunningTime), pChild);
		}
		else if (IsSameTag(pChild, DUMMYCLIENT_CONFIG_TAG_DIRECT_LOGIN_TO_GAME_SERVER))
		{
			_Contents(&(m_pData->m_Config.bDirectLoginToGameServer), pChild);
			gvar.Network.bLoginToGameServer = m_pData->m_Config.bDirectLoginToGameServer;
		}
	}

	if (m_pData->m_Config.strID.empty())
	{
		m_pData->m_Config.bAutoID = true;
	}

	if (m_pData->m_Config.bAutoID)
	{
		m_pData->m_Config.strID = XDummyHelper::MakeAutoID();
	}
}

void XDummyFileLoader::ParsePattern( MXmlElement* pElement )
{
	XDummyPattern* pDummyPattern = new XDummyPattern();

	pDummyPattern->ParseDummyClient(pElement, &m_Xml);

	m_pData->m_DummyPatternMgr.AddPattern(pDummyPattern->GetName(), pDummyPattern);
}

void XDummyFileLoader::ParseAgent( MXmlElement* pElement )
{
	USES_CONVERSION_EX;

	XDummyAgentInfo		agentInfo;

	_Attribute(agentInfo.strName, pElement, DUMMYCLIENT_AGENT_ATTR_NAME);
	_Attribute(&agentInfo.nCount, pElement, DUMMYCLIENT_AGENT_ATTR_COUNT);

	for (MXmlElement* pChild = pElement->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		string strPatternName;

		if (IsSameTag(pChild, DUMMYCLIENT_AGENT_TAG_PATTERN))
		{
			_Contents(strPatternName, pChild);

			wstring strPatternType;
			_Attribute(strPatternType, pChild, DUMMYCLIENT_AGENT_ATTR_TYPE);

			if (!wcsicmp(strPatternType.c_str(), L"glue"))
			{
				agentInfo.strGluePatterns.push_back(A2W_EX(strPatternName.c_str(), 0));
			}
			else if (!wcsicmp(strPatternType.c_str(), L"xml"))
			{
				agentInfo.strXmlPatterns.push_back(A2W_EX(strPatternName.c_str(), 0));
			}
			else if (!wcsicmp(strPatternType.c_str(), L"bot"))
			{
				agentInfo.strBotPatterns.push_back(A2W_EX(strPatternName.c_str(), 0));
			}
		}
		else if (IsSameTag(pChild, DUMMYCLIENT_AGENT_TAG_VALUE))
		{
			string strKey, strValue;

			_Contents(strValue, pChild);
			_Attribute(strKey, pChild, DUMMYCLIENT_AGENT_ATTR_NAME);

			if (!strKey.empty())
			{
				agentInfo.mapValues.insert(multimap<wstring,wstring>::value_type(A2W_EX(strKey.c_str(), 0), A2W_EX(strValue.c_str(), 0)));
			}
		}
	}

	wstring strField = agentInfo.GetValue(L"field");
	agentInfo.nFieldID = _wtoi(strField.c_str());

	wstring strCenter = agentInfo.GetValue(L"center");
	swscanf_s(strCenter.c_str(), L"%f %f %f", &agentInfo.vCenter.x, &agentInfo.vCenter.y, &agentInfo.vCenter.z);

	wstring strRadius = agentInfo.GetValue(L"radius");
	agentInfo.fRadius = (float)_wtoi(strRadius.c_str());

	wstring strCollisionFileName = agentInfo.GetValue(L"collision_file");
	if (!strCollisionFileName.empty())
	{
		XDummyCollisionMgr::GetInstance()->Load(agentInfo.nFieldID, W2A_EX(strCollisionFileName.c_str(), 0));
		agentInfo.pCollision = XDummyCollisionMgr::GetInstance()->Get(agentInfo.nFieldID);
	}


	m_pData->m_AgentInfos.push_back(agentInfo);
}