#include "StdAfx.h"
#include "XMyRecipeInfo.h"
#include "XRecipeInfo.h"
#include "XRecipeInfoMgr.h"


void XMyRecipeInfo::push_back( int nRecipeID )
{
	m_vecRecipe.push_back(nRecipeID);
}

void XMyRecipeInfo::erase( int nRecipeID )
{
	vector<int>::iterator itor = m_vecRecipe.begin();
	for( ; itor != m_vecRecipe.end(); itor++)
	{
		if (nRecipeID == (*itor))
		{
			m_vecRecipe.erase(itor);
			break;
		}
	}
}

MWLua::table XMyRecipeInfo::GetRecipeTable()
{
	MWLua::table t(global.script->GetLua()->GetState());
	if( m_vecRecipe.size() <= 0) return t;

	char buffer[32];
	int i =1;
	vector<int>::iterator itor = m_vecRecipe.begin();
	for (; itor != m_vecRecipe.end(); itor++, i++)
	{
		sprintf_s(buffer,"%d", i);
		t.set(buffer, (*itor));
	}

	return t;
}


bool XMyRecipeInfo::IsExist( int nRecipeID)
{
	for ( vector<int>::iterator itr = m_vecRecipe.begin();  itr != m_vecRecipe.end();  itr++)
	{
		if ( (*itr) == nRecipeID)
			return true;
	}

	return false;
}

void XMyRecipeInfo::CollectJustCraftableRecipeID(vector<int>& vecJustMakableRecipeID, int nItemID, int nAddAmount)
{	
	for each (int nRecipeID in m_vecRecipe)
	{
		XRecipeInfo* pRecipeInfo = info.recipe->Get(nRecipeID);
		if (NULL == pRecipeInfo) continue;
		if (false == pRecipeInfo->IsMaterialItemID(nItemID)) continue;

		bool bJustMakable = true;
		for each (const RecipeMaterial& material in pRecipeInfo->m_vecMaterial)
		{
			int nRequireAmount = pRecipeInfo->GetMaterialItemAmount(material.m_nItemID);
			int nHaveAmount = gvar.MyInfo.Inventory.GetItemAmountByID(material.m_nItemID) +
				gvar.MyInfo.EquipmentSlot.GetItemQuantityByID(material.m_nItemID);

			if (material.m_nItemID == nItemID)
			{
				if (nRequireAmount <= nHaveAmount || nRequireAmount > nHaveAmount + nAddAmount)
				{
					bJustMakable = false;
					break;
				}				
			}
			else
			{
				if (nRequireAmount > nHaveAmount)
				{
					bJustMakable = false;
					break;
				}
			}			
		}

		if (true == bJustMakable)
		{
			vecJustMakableRecipeID.push_back(nRecipeID);
		}		
	}
}

int XMyRecipeInfo::GetCraftableRecipeCount()
{
	int nCount = 0;

	for each (int nRecipeID in m_vecRecipe)
	{
		if (false == info.recipe->IsCraftableRecipe(nRecipeID)) continue;

		nCount++;
	}

	return nCount;
}