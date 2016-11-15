#ifndef _G_PALETTE_ITEM_H_
#define _G_PALETTE_ITEM_H_

#include "CSPalette.h"

class GPaletteItem
{
public:
	PALETTE_ITEM_TYPE	m_nType;
	int					m_nItemIDorTalentID; // ItemID or TalentID

public:
	GPaletteItem();
	~GPaletteItem();

	bool IsEmpty();
};

#endif
