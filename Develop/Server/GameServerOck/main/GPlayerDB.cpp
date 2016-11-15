#include "stdafx.h"
#include "GPlayerDB.h"
#include "GPlayerObject.h"
#include "MXml.h"
#include "MLocale.h"
#include "GCalculator.h"
#include "GItemManager.h"
#include "GField.h"
#include "GItemContainer.h"
#include "GEquipmentSlot.h"
#include "GItemSystem.h"
#include "GConst.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GGlobal.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GPlayerQObjective.h"
#include "GPlayerField.h"
#include "GPlayerSystem.h"


#define PATH_USERS		"./users/"

string GPlayerDB::GetFullFileName(string strUserID)
{
	string filename = PATH_USERS;
	filename.append(strUserID);
	filename.append(string(".xml"));

	return filename;
}


bool GPlayerDB::Save(GPlayerObject* pPlayerObject)
{
//#ifdef _DEBUG
//	return true;		// test
//#endif

	GEntityPlayer* pEntityPlayer = pPlayerObject->GetEntity();
	if (pEntityPlayer == NULL) return false;

	string strUserID = pEntityPlayer->GetPlayerInfo()->szName;
	string filename = GetFullFileName(strUserID);

	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);
	
	MXmlElement* pElement1 = new MXmlElement( "maiet" );
	xml.Doc()->LinkEndChild(pElement1);


	xml.Doc()->SaveFile(filename.c_str());

	return true;
}


bool GPlayerDB::Load(GPlayerObject* poutPlayerObject)
{
	if (poutPlayerObject->GetEntity() == NULL) return false;

	string strUserID = poutPlayerObject->GetEntity()->GetPlayerInfo()->szName;
	string filename = GetFullFileName(strUserID);

	int level=0, xp=0, bp=0, hp=0, en=0;


	MXml xml;
	if (!xml.LoadFile(filename.c_str())) 
	{
		return false;
	}
	
	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild("maiet").FirstChildElement().Element();

	return true;
}

bool GPlayerDB::LoadForAccountCharList(GAccountCharList& outCharList)
{
	for (int i=0; i<outCharList.m_nAccountCharCount; ++i)
	{
		TD_AccountCharInfo& charInfo = outCharList.m_TransData[i];

		string strUserID = charInfo.szName;
		string filename = GetFullFileName(strUserID);

		MXml xml;
		if (!xml.LoadFile(filename.c_str())) continue;

		MXmlHandle docHandle = xml.DocHandle();
		MXmlElement* pElement = docHandle.FirstChild("maiet").FirstChildElement().Element();
		for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
		{
			// pos
			if (!_stricmp(pElement->Value(), "pos"))
			{
				int map_id;
				_Attribute(&map_id,	pElement, "mapid");
				charInfo.nFieldID = map_id;
			}
		}
	}	

	return true;
}

bool GPlayerDB::Delete(const char* szName)
{
	if (NULL == szName) return false;

	string filename = GetFullFileName(szName);

	MXml xml;
	xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->SaveFile(filename.c_str());

	return true;
}