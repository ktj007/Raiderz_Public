#ifndef _GBINDING_POS_INFO_MGR_H_
#define _GBINDING_POS_INFO_MGR_H_


#include "CSTypes.h"

class GSoulBindingInfo;

#define SOULBINDING_XML_TAG_MAIET			"maiet"
#define SOULBINDING_XML_TAG_SOULBINDING		"SOULBINDING"
#define SOULBINDING_XML_ATTR_ID				"id"
#define SOULBINDING_XML_ATTR_FIELD			"field"
#define SOULBINDING_XML_ATTR_MARKER			"marker"
#define SOULBINDING_XML_ATTR_DIALOG			"dialog"
#define SOULBINDING_XML_ATTR_FACTION		"faction"

typedef map<SBID, GSoulBindingInfo*>		MAP_SOULBINDINGINFO;
typedef map<int, vector<GSoulBindingInfo*>>	MAP_SOULBINDINFIELD;


class GSoulBindingInfoMgr : public MTestMemPool<GSoulBindingInfoMgr>
{
public:
	GSoulBindingInfoMgr();
	~GSoulBindingInfoMgr();

	void Clear();

	bool Load(const wchar_t* szFileName);
	void Insert(GSoulBindingInfo* pSoulBindingInfo);
	GSoulBindingInfo* Get(SBID nID);
	bool IsSoulBindingID(SBID nID);
	bool IsSoulBindingField(int nFieldID);

	void Cooking();

	MAP_SOULBINDINGINFO&	GetContainer();
	vector<GSoulBindingInfo*> CollectSoulBindingInfo(int nFieldID);

private:
	void ParserSoulBinding(MXmlElement* pElement, MXml* pXml);

private:
	MAP_SOULBINDINGINFO		m_mapSoulBindingInfo;
	MAP_SOULBINDINFIELD		m_mapSoulBindingField;
};

#endif//_GBINDING_POS_MGR_H_
