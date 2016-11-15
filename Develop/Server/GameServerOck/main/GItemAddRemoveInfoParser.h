#pragma once

#define ITEM_TAG_FILL				"FILL"
#define ITEM_TAG_ADD				"ADD"
#define ITEM_TAG_REMOVE				"REMOVE"

#define ITEM_ATTR_ID				"id"
#define ITEM_ATTR_AMOUNT			"amount"

#define ITEM_VALUE_ALL				"all"


class GItemAddRemoveInfo;

class GItemAddRemoveInfoParser
{
public:
	void ParseItem(MXmlElement* pElement, MXml* pXml, GItemAddRemoveInfo* pInfo);

private:
	void ParseFill(MXmlElement* pElement, MXml* pXml, GItemAddRemoveInfo* pInfo);
	void ParseAdd(MXmlElement* pElement, MXml* pXml, GItemAddRemoveInfo* pInfo);
	void ParseRemove(MXmlElement* pElement, MXml* pXml, GItemAddRemoveInfo* pInfo);
};
