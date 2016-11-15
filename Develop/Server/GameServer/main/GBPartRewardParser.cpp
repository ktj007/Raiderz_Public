#include "StdAfx.h"
#include "GBPartRewardParser.h"
#include "GValidateLogger.h"
#include "GGlobal.h"
#include "GNPCInfoMgr.h"


#define BPARTREWARD_XML_TAG_MAIET			"maiet"
#define BPARTREWARD_XML_TAG_NPC				"NPC"
#define BPARTREWARD_XML_TAG_BPART			"BPART"

#define BPARTREWARD_XML_ATTR_ID				"id"
#define BPARTREWARD_XML_ATTR_FRAG			"frag"
#define BPARTREWARD_XML_ATTR_LOOT			"loot"
#define BPARTREWARD_XML_ATTR_RATE			"rate"
#define BPARTREWARD_XML_ATTR_DIRECTION		"dir"


GBPartRewardParser::GBPartRewardParser(void)
{
}

GBPartRewardParser::~GBPartRewardParser(void)
{
}

bool GBPartRewardParser::Parse( const wchar_t* szFileName )
{
	_VLI(FILENAME_BPART_REWARD);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) 
		return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BPARTREWARD_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BPARTREWARD_XML_TAG_NPC))
		{
			ParseNPC(pElement, &xml);
		}
	}

	return true;
}


void GBPartRewardParser::ParseNPC( MXmlElement* pElement, MXml* pXml )
{
	int id = INVALID_ID;
	_Attribute(&id,									pElement, BPARTREWARD_XML_ATTR_ID);

	if (id <= INVALID_ID)
		return;

	GNPCInfo* pNPCInfo = gmgr.pNPCInfoMgr->Find(id);
	if (!pNPCInfo)
	{
		mlog("%s - invalid npc id (%d)\n", __FUNCTION__, id);
		return;
	}

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), BPARTREWARD_XML_TAG_BPART))
		{
			BPART_REWARD_INFO infoBPartReward;
			ParseBPart(infoBPartReward, pChild, pXml);

			BPartRewardMap::iterator it = 
				pNPCInfo->m_mapBPartRewards.find(infoBPartReward.nID);
			if (it == pNPCInfo->m_mapBPartRewards.end())
			{
				pNPCInfo->m_mapBPartRewards.insert(make_pair(infoBPartReward.nID, vector<BPART_REWARD_INFO>()));
			}

			pNPCInfo->m_mapBPartRewards[infoBPartReward.nID].push_back(infoBPartReward);
		}
	}
}

void GBPartRewardParser::ParseBPart( BPART_REWARD_INFO& infoBPartReward, MXmlElement* pElement, MXml* pXml )
{
	_Attribute(&infoBPartReward.nID,									pElement, BPARTREWARD_XML_ATTR_ID);
	_Attribute(&infoBPartReward.nFlag,									pElement, BPARTREWARD_XML_ATTR_FRAG);
	_Attribute(&infoBPartReward.nLootID,							pElement, BPARTREWARD_XML_ATTR_LOOT);
	_Attribute(&infoBPartReward.fRate,							pElement, BPARTREWARD_XML_ATTR_RATE);

	string strAttr;
	infoBPartReward.nDir = UBD_FRONT; // 기본값은 전방
	if (_Attribute(strAttr,							pElement, BPARTREWARD_XML_ATTR_DIRECTION))
	{
		for (int i=0; i<UBD_MAX; ++i)
		{
			if (strAttr == USABLE_BPART_DIRECTION_STR[i])
			{
				infoBPartReward.nDir = (USABLE_BPART_DIRECTION)i;
			}
		}
	}
}

