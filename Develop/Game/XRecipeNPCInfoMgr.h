#pragma once

class XRecipeNPCInfo;

typedef map<int, XRecipeNPCInfo*> MAP_RECIPENPCINFO;

#define  RECIPENPC_XML_TAG_MAIET		"maiet"
#define  RECIPENPC_XML_TAG_RECIPENPC	"RECIPENPC"

#define  RECIPENPC_XML_ATTR_RECIPE		"recipe"
#define  RECIPENPC_XML_ATTR_FIELD		"field"
#define  RECIPENPC_XML_ATTR_NPC			"NPC"

class XRecipeNPCInfoMgr
{
public:
	XRecipeNPCInfoMgr(void);
	~XRecipeNPCInfoMgr(void);

	void Clear();
	bool Load(const TCHAR* szFileName);
	int GetNPCCount(int nRecipeID);
	const TCHAR* GetNPCName(int nRecipeID, int nIndex);	
	int GetNPCFieldID(int nRecipeID, int nIndex);	
	XRecipeNPCInfo* Get(int nRecipeID);	

private:
	void Insert(XRecipeNPCInfo* pInfo);
	bool ParseRecipe(MXmlElement* pElement, MXml* pXml, XRecipeNPCInfo* pInfo);
private:
	MAP_RECIPENPCINFO	m_mapInfo;
};
