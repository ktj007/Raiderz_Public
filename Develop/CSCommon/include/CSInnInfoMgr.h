#ifndef _CS_INN_INFO_MGR_H_
#define _CS_INN_INFO_MGR_H_

class CSInnInfo;

#define INN_XML_TAG_MAIET			"maiet"
#define INN_XML_TAG_INN				"INN"
#define INN_XML_ATTR_ID				"id"

typedef map<uint8, CSInnInfo*> MAP_INNINFO;

class CSInnInfoMgr
{	
private:
	MAP_INNINFO m_mapInnInfo;

protected:
	virtual bool ParseInn(MXmlElement* pElement, MXml* pXml, CSInnInfo* pInfo);
	CSInnInfo* Get(uint8 nID);

public:
	CSInnInfoMgr();
	virtual ~CSInnInfoMgr();

	void Clear();

	bool Load(const TCHAR* szFileName);
	virtual CSInnInfo* NewInfo() = 0;
	void Insert(CSInnInfo* pInfo);		
};

#endif

