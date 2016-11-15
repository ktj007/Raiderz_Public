#include "StdAfx.h"
#include "TEquipment.h"
//#include "TNaming.h"

#using <System.Xml.dll>

#define	EQUIPMENT_FILENAME "EquipItems.xml"

const char* EquipSlotStr[] =
{
	"hat_item",
	"face",
	"hands_item",
	"feet_item",
	"chest_item",
	"legs_item",
	"lfinger",
	"rfinger",
	"neck",
	"hair",

	"lweapon",
	"rweapon",
	"lweapon2",
	"rweapon2",

	"look_head",
	"look_hands",
	"look_feet",
	"look_body",
	"look_leg",

	"inn_crafting",
	"inn_bed",
	"inn_steward",
	"inn_carpet",
	"inn_fireplace",
	"inn_mural",
	"inn_table",
	"inn_funiture",

	"none"
};

TEquipment::TEquipment(void)
{
}

TEquipment::~TEquipment(void)
{
}

bool TEquipment::LoadEquipment()
{
	MXml xml;
	bool bOk = xml.LoadFile(EQUIPMENT_FILENAME);
	if(!bOk) return false;

	MXmlElement* pRootNode = xml.Doc()->FirstChildElement();
	if (pRootNode == NULL) return false;

	MXmlElement* pChild = pRootNode->FirstChildElement();

	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		char NodeName[256];
		strcpy(NodeName, pChild->Value());
		if(strcmp(NodeName, "human_female")==0)
		{
			MXmlElement* pSubChild = pChild->FirstChildElement();
			for (pSubChild; pSubChild != NULL; pSubChild = pSubChild->NextSiblingElement())
			{
				char PartName[256];
				strcpy(PartName, pSubChild->Value());
				for(int i =0; i < ITEMSLOT_MAX; ++i)
				{
					if(strcmp(PartName, EquipSlotStr[i])==0
						|| strcmp(PartName, GetRemoveUnderBarlast(EquipSlotStr[i]).c_str())==0)
					{
						char Name[256];
						_Contents(Name, pSubChild);
						m_hf[i] = Name;
						break;
					}
				}
			}
		}
		else if (strcmp(NodeName, "human_male")==0)
		{
			MXmlElement* pSubChild = pChild->FirstChildElement();
			for (pSubChild; pSubChild != NULL; pSubChild = pSubChild->NextSiblingElement())
			{
				char PartName[256];
				strcpy(PartName, pSubChild->Value());
				for(int i =0; i < ITEMSLOT_MAX; ++i)
				{
					if(strcmp(PartName, EquipSlotStr[i])==0
						|| strcmp(PartName, GetRemoveUnderBarlast(EquipSlotStr[i]).c_str())==0)
					{
						char Name[256];
						_Contents(Name, pSubChild);
						m_hm[i] = Name;
						break;
					}
				}
			}
		}
	}

	return true;
}

bool TEquipment::SaveEquipment()
{
	System::Text::Encoding^ encoding = System::Text::Encoding::UTF8;

	System::Xml::XmlTextWriter^ tw = gcnew System::Xml::XmlTextWriter(EQUIPMENT_FILENAME, encoding);
	tw->Formatting = System::Xml::Formatting::Indented;
	tw->WriteStartDocument();
	tw->WriteStartElement("XML");                

	tw->WriteStartElement("human_female");
	for (int i =0; i < ITEMSLOT_MAX; ++i)
	{
		if(!m_hf[i].empty())
		{
			tw->WriteStartElement(gcnew String(EquipSlotStr[i]));
			tw->WriteString(gcnew String(m_hf[i].c_str()));
			tw->WriteEndElement();
		} 
	}
	tw->WriteEndElement();

	tw->WriteStartElement("human_male");
	for (int i =0; i < ITEMSLOT_MAX; ++i)
	{
		if(!m_hm[i].empty())
		{
			tw->WriteStartElement(gcnew String(EquipSlotStr[i]));
			tw->WriteString(gcnew String(m_hm[i].c_str()));
			tw->WriteEndElement();
		}
	}
	tw->WriteEndElement();

	tw->WriteEndElement();
	tw->Flush();
	tw->Close();

	return true;
}

bool TEquipment::IsEquip(SH_ITEM_SLOT islot, PLAYER_TYPE playertype)
{
	if(playertype == PLAYER_TYPE_FEMALE)
	{
		if(!m_hf[islot].empty())
		{
			return true;
		}
	}
	else if (playertype == PLAYER_TYPE_MALE)
	{
		if(!m_hm[islot].empty())
		{
			return true;
		}
	}

	return false;
}

// Equip정보로 장비를 착용한다.
void TEquipment::DefaultItemEquip(PLAYER_TYPE playertype)
{
	if(playertype == PLAYER_TYPE_FEMALE)
	{
		for( int i = 0;i < ITEMSLOT_MAX; ++i)
		{
			if(!m_hf[i].empty())
			{
				g_pMainApp->m_EquipItems.EquipReviewItems(m_hf[i].c_str(), EquipSlotStr[i], ITEMTYPE_ARMOR);
			}
			else
			{
				g_pMainApp->GetActor()->UnequipItem(i, true);
			}
		}
	}
	else if (playertype == PLAYER_TYPE_MALE)
	{
		for( int i = 0;i < ITEMSLOT_MAX; ++i)
		{
			if(!m_hm[i].empty())
			{
				g_pMainApp->m_EquipItems.EquipReviewItems(m_hm[i].c_str(), EquipSlotStr[i], ITEMTYPE_ARMOR);
			}
			else
			{
				g_pMainApp->GetActor()->UnequipItem(i, true);
			}
		}
	}
}

// Equip정보를 갱신한다.
void TEquipment::SetEquipInfo(tstring MeshName, const char* NodeName, PLAYER_TYPE playertype)
{
	if(playertype == PLAYER_TYPE_FEMALE)
	{
		for( int i = 0;i < ITEMSLOT_MAX; ++i)
		{
			if(strcmp(NodeName, EquipSlotStr[i])==0)
			{
				m_hf[i] = MeshName;
				return;
			}
		}
	}
	else if (playertype == PLAYER_TYPE_MALE)
	{
		for( int i = 0;i < ITEMSLOT_MAX; ++i)
		{
			if(strcmp(NodeName, EquipSlotStr[i])==0)
			{
				m_hm[i] = MeshName;
				return;
			}
		}
	}
}

void TEquipment::SetUnEquipInfo(int islot, PLAYER_TYPE playertype)
{
	if(playertype == PLAYER_TYPE_FEMALE)
	{
		m_hf[islot] = "";
		return;
	}
	else if (playertype == PLAYER_TYPE_MALE)
	{
		m_hm[islot] = "";
		return;
	}
}

void TEquipment::SetItemEquipInfo( const CSItemData* pItem, int nSlot )
{
	const char* slotstr = pItem->m_strName.c_str();

	if (pItem->m_ItemType == ITEMTYPE_WEAPON)
	{
		slotstr = CSItemSlotStr[pItem->m_nItemSlot];

		// 왼쪽이면...
		if((SH_ITEM_SLOT)nSlot == ITEMSLOT_LWEAPON)
		{
			slotstr = CSItemSlotStr[pItem->m_nSubSlot];
		}
	}

	SetEquipInfo(pItem->m_strMeshName, slotstr, g_pMainApp->m_TabModel.GetPlayerType());
}

bool TEquipment::EquipItemName(string name)
{
	CSItemData* pItem = g_pMainApp->GetItemMgr()->GetItemData(name);
	return EquipItem(pItem, pItem->m_nItemSlot);
}

bool TEquipment::EquipItemID(int itemid, int nSlot)
{
	CSItemData* pItem = g_pMainApp->GetItemMgr()->GetItemData(itemid);
	return EquipItem(pItem, nSlot);
}

bool TEquipment::EquipItemSlotIndex(int index)
{
	const CSItemData* pItem = g_pMainApp->GetItemData(index);
	return EquipItem(pItem, pItem->m_nItemSlot);
}

bool TEquipment::EquipItem(const CSItemData* pItem, int nSlot)
{
	TCharacter* pActor = g_pMainApp->GetActor();
	if(pActor == NULL) return false;

	if(pItem == NULL)
	{
		pActor->UnequipItem(g_pMainApp->m_TabItem.GetSelSlot(), true);
		SetUnEquipInfo(g_pMainApp->m_TabItem.GetSelSlot()-1, g_pMainApp->m_TabModel.GetPlayerType());
		mlog("정보 : 장비를 벗습니다. \r");

		g_pMainApp->GetMotion()->SetWeaponType(WEAPON_NONE);
	}
	else
	{
		//int nSlot = pItem->m_nItemSlot;
		if( pActor->GetEquipItems()[nSlot].pItemData != NULL)
		{
			int nID = pActor->GetEquipItems()[nSlot].pItemData->m_nID;
			mlog("정보 : %s를 벗습니다. \r", pActor->GetEquipItems()[nSlot].pItemData->m_strName.c_str());

			pActor->UnequipItem(nSlot, true);
			SetUnEquipInfo(nSlot, g_pMainApp->m_TabModel.GetPlayerType());

			g_pMainApp->GetMotion()->SetWeaponType(WEAPON_NONE);

			if(pItem->m_nID == nID) return false;
		}

		if(pActor->EquipItem(*pItem, (SH_ITEM_SLOT)nSlot))
		{
			SetItemEquipInfo(pItem, nSlot);
			mlog("정보 : %s를 착용합니다. \r", pItem->m_strName.c_str());

			g_pMainApp->GetMotion()->SetWeaponType(g_pMainApp->GetActor()->GetCurrentWeaponType());
			return true;
		}
		else
		{
			mlog3("에러 : %s를 착용하는데 실패했습니다. \r", pItem->m_strName.c_str());
		}
	}

	return false;
}

ARMOR_TYPE TEquipment::GetArmorType(SH_ITEM_SLOT iSlot)
{
	TCharacter* pActor = g_pMainApp->GetActor();
	if(pActor == NULL) return ARMOR_NONE;

	const CSItemData* pItemData = pActor->GetEquipItems()[iSlot].pItemData;
	if(pItemData == NULL) return ARMOR_NONE;

	return pItemData->m_nArmorType;
}