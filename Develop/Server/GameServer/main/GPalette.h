#ifndef _G_PALETTE_H_
#define _G_PALETTE_H_

#include "CSPalette.h"
#include "GPaletteItem.h" // for VEC_PALETTE_ITEM

class GEntityPlayer;
class GPaletteItem;


typedef vector<GPaletteItem> VEC_PALETTE_ITEM;

class GPalette : public MTestMemPool<GPalette>
{
private:
	GEntityPlayer*		m_pOwner;
	VEC_PALETTE_ITEM	m_vecPaletteItem;
	PALETTE_NUM			m_nSelectedPaletteNum;		///< 현제 선택된 팔레트
	PALETTE_NUM			m_nPrimarySetPaletteNum;	///< 주장비에 바인딩된 팔레트
	PALETTE_NUM			m_nSecondarySetPaletteNum;	///< 보조장비에 바인딩된 팔레트
	bool				m_bLock;	///< 리스트 및 아이템 변경 불가

public:
	GPalette(GEntityPlayer* pOwner);
	~GPalette();

	void Lock();
	void Unlock();
	bool IsLocked() const { return m_bLock; }

	void SelectPalette(PALETTE_NUM nNum) { m_nSelectedPaletteNum = nNum; }	///< 팔레트 선택
	PALETTE_NUM GetSelectedPalette() { return m_nSelectedPaletteNum; }

	void SetPrimary(PALETTE_NUM nNum);	///< 주장비에 팔레트 세팅
	PALETTE_NUM GetPrimary() { return m_nPrimarySetPaletteNum; }

	void SetSecondary(PALETTE_NUM nNum);///< 보조장비에 팔레트 세팅
	PALETTE_NUM GetSecondary() { return m_nSecondarySetPaletteNum; }

	void Set(PALETTE_NUM nNum, PALETTE_SLOT nSlot, PALETTE_ITEM_TYPE nType, int nItemIDorTalentID);
	bool Reset(PALETTE_NUM nNum, PALETTE_SLOT nSlot);
	void ResetTalentSlot();
	void Change(PALETTE_NUM nNum1, PALETTE_SLOT nSlot1, PALETTE_NUM nNum2, PALETTE_SLOT nSlot2);

	GPaletteItem* GetPaletteItem(PALETTE_NUM nNum, PALETTE_SLOT nSlot);
	void GetNumAndSlotByTalentID(int nTalentID, vector<pair<PALETTE_NUM, PALETTE_SLOT>>& outvecPaletteNumAndSlot);
	void GetNumAndSlotByItemID(int nItemID, vector<pair<PALETTE_NUM, PALETTE_SLOT>>& outvecPaletteNumAndSlot);
	bool IsEmpty(PALETTE_NUM nNum, PALETTE_SLOT nSlot);
	int	GetTalentCountOnPalette();

	VEC_PALETTE_ITEM&	GetContainer()		{ return m_vecPaletteItem;		}
};

#endif
