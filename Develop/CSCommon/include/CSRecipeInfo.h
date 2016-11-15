#ifndef _CS_RECIPE_INFO_H
#define _CS_RECIPE_INFO_H

#include "CSCommonLib.h"

#define MAX_RECIPE_MATERIAL_TYPE 5

struct RecipeMaterial
{
	RecipeMaterial(int nItemID, int nAmount)
	: m_nItemID(nItemID), m_nAmount(nAmount)
	{

	}

	int m_nItemID;
	int m_nAmount;
};

class CSCOMMON_API CSRecipeInfo
{
public:
	int			m_nID;
	int			m_nProductItemID;			///< 생산 아이템
	int			m_nAmount;					///< 생산량
	vector<RecipeMaterial> m_vecMaterial;	///< 재료 아이템
	int			m_nPrice;					///< 생산에 필요한 돈의 양. 단위는 실버
	int			m_nConditionID;				///< 이아이템을 제작하기 위한 조건

	set<int>	m_setMaterialItemID;		///< 특정 아이템이 재료 아이템인지 확인할때 사용

public:
	CSRecipeInfo();
	~CSRecipeInfo();

	bool	IsMaterialItemID(int nItemID);
	int		GetMaterialItemAmount(int nItemID);
};


#endif//_CS_RECIPE_INFO_H