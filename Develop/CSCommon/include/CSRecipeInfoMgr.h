#ifndef _CS_RECIPE_INFO_MGR_H
#define _CS_RECIPE_INFO_MGR_H

#include <map>
#include <vector>
using namespace std;

#include "MXml.h"
#include "CSDef.h"
#include "CSCommonLib.h"

class CSRecipeInfo;
enum CRAFT_TYPE;


#define  RECIPE_XML_TAG_MAIET			"maiet"
#define  RECIPE_XML_TAG_RECIPE			"recipe"

#define  RECIPE_XML_ATTR_ID				"id"
#define  RECIPE_XML_ATTR_ITEM			"item"
#define  RECIPE_XML_ATTR_AMOUNT			"amount"

#define  RECIPE_XML_ATTR_MAT			"mat"
#define  RECIPE_XML_ATTR_AMOUNT			"amount"

#define  RECIPE_XML_ATTR_PRICE			"price"

#define  RECIPE_XML_ATTR_CONDITION		"condition"

typedef map<int, CSRecipeInfo*> MAP_CSRECIPEINFO;

class CSCOMMON_API CSRecipeInfoMgr
{
private:
	MAP_CSRECIPEINFO			m_mapRecipeInfo;
	set<int>					m_setRecipeMaterialItemID;	// 재료 아이템

protected:
	virtual bool ParseRecipe(MXmlElement* pElement, MXml* pXml, CSRecipeInfo* pInfo);
	CSRecipeInfo* Get(int nID);

public:
	CSRecipeInfoMgr();
	virtual ~CSRecipeInfoMgr();

	void Clear();

	bool Load(const TCHAR* szFileName);
	virtual CSRecipeInfo* New() = 0;
	void Cooking();

	void Insert(CSRecipeInfo* pInfo);
	MAP_CSRECIPEINFO& GetContainer()	{ return m_mapRecipeInfo; }

	bool IsMaterialItemID(int nItemID);
};

#endif//_CS_RECIPE_INFO_MGR_H
