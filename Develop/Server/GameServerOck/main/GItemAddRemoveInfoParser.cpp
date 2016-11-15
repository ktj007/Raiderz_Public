#include "StdAfx.h"
#include "GItemAddRemoveInfoParser.h"
#include "GItemAddRemoveInfo.h"

void GItemAddRemoveInfoParser::ParseItem(MXmlElement* pElement, MXml* pXml, GItemAddRemoveInfo* pInfo)
{
	VALID(pElement);
	VALID(pXml);
	VALID(pInfo);	

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), ITEM_TAG_FILL))
			ParseFill(pChild, pXml, pInfo);
		else if (!_stricmp(pChild->Value(), ITEM_TAG_ADD))
			ParseAdd(pChild, pXml, pInfo);
		else if (!_stricmp(pChild->Value(), ITEM_TAG_REMOVE))
			ParseRemove(pChild, pXml, pInfo);
		else _ASSERT(0);
	}
}

void GItemAddRemoveInfoParser::ParseFill( MXmlElement* pElement, MXml* pXml, GItemAddRemoveInfo* pInfo )
{
	int nID;
	int nAmount;
	_Attribute(&nID, pElement, ITEM_ATTR_ID);
	_Attribute(&nAmount, pElement, ITEM_ATTR_AMOUNT);

	pInfo->InsertItemFillInfo(nID, nAmount);
}

void GItemAddRemoveInfoParser::ParseAdd( MXmlElement* pElement, MXml* pXml, GItemAddRemoveInfo* pInfo )
{
	int nID;
	int nAmount;
	_Attribute(&nID, pElement, ITEM_ATTR_ID);
	_Attribute(&nAmount, pElement, ITEM_ATTR_AMOUNT);

	pInfo->InsertItemAddInfo(nID, nAmount);
}

void GItemAddRemoveInfoParser::ParseRemove( MXmlElement* pElement, MXml* pXml, GItemAddRemoveInfo* pInfo )
{
	int nID;
	string strAmount;

	_Attribute(&nID, pElement, ITEM_ATTR_ID);
	if (true == _Attribute(strAmount, pElement, ITEM_ATTR_AMOUNT, pXml))
	{
		if (ITEM_VALUE_ALL == strAmount)
		{
			pInfo->InsertItemRemoveAllInfo(nID);
		}
		else
		{
			int nAmount = atoi(strAmount.c_str());
			pInfo->InsertItemRemoveInfo(nID, nAmount);
		}
	}
}