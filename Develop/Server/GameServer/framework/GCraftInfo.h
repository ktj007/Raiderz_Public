#ifndef _G_CRAFT_INFO_H_
#define _G_CRAFT_INFO_H_

#include "GFrameworkLib.h"
enum CRAFT_TYPE;

class GFRAMEWORK_API GCraftInfo : public MTestMemPool<GCraftInfo>
{
public:
	int			m_nID;
	int			m_nDialogID;	///< 출력될 대사
	float		m_fBaseMakeMod;	///< 기본 요율
	int			m_nFactionID;	///< 장인이 속해있는 팩션(요율에 영향)
	set<int>	m_setRecipeID;	///< 생산 가능한 레시피 목록 	

	bool IsHaveRecipeID(int nRecipeID);

public:
	GCraftInfo();
	~GCraftInfo();
};

#endif//_G_CRAFT_INFO_H_
