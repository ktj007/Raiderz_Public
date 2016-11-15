#include "StdAfx.h"
#include "GPalette.h"
#include "GPaletteItem.h"
#include "GEntityPlayer.h"
#include "GPaletteSystem.h"
#include "GGlobal.h"

GPalette::GPalette(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_nSelectedPaletteNum(PALETTENUM_1)
, m_nPrimarySetPaletteNum(PALETTENUM_MAX)
, m_nSecondarySetPaletteNum(PALETTENUM_MAX)
, m_bLock(false)
{
	m_vecPaletteItem.resize(PALETTESLOT_MAX * PALETTENUM_MAX, GPaletteItem());
	
}

GPalette::~GPalette()
{
}

void GPalette::Set(PALETTE_NUM nNum, PALETTE_SLOT nSlot, PALETTE_ITEM_TYPE nType, int nItemIDorTalentID)
{
	if (m_bLock)
		return;

	if (false == gsys.pPaletteSystem->IsValidNumAndSlot(nNum, nSlot)) return;

	uint8 nIndex = gsys.pPaletteSystem->NumAndSlotToIndex(nNum, nSlot);

	m_vecPaletteItem[nIndex].m_nType = nType;
	m_vecPaletteItem[nIndex].m_nItemIDorTalentID = nItemIDorTalentID;
}

bool GPalette::Reset(PALETTE_NUM nNum, PALETTE_SLOT nSlot)
{
	if (m_bLock)
		return false;

	if (false == gsys.pPaletteSystem->IsValidNumAndSlot(nNum, nSlot)) 
		return false;

	uint8 nIndex = gsys.pPaletteSystem->NumAndSlotToIndex(nNum, nSlot);

	m_vecPaletteItem[nIndex].m_nType = PIT_NONE;
	m_vecPaletteItem[nIndex].m_nItemIDorTalentID = 0;

	return true;
}


void GPalette::Change(PALETTE_NUM nNum1, PALETTE_SLOT nSlot1, PALETTE_NUM nNum2, PALETTE_SLOT nSlot2)
{
	if (m_bLock)
		return;

	if (false == gsys.pPaletteSystem->IsValidNumAndSlot(nNum1, nSlot1)) return;
	if (false == gsys.pPaletteSystem->IsValidNumAndSlot(nNum2, nSlot2)) return;

	uint8 nIndex1 = gsys.pPaletteSystem->NumAndSlotToIndex(nNum1, nSlot1);
	uint8 nIndex2 = gsys.pPaletteSystem->NumAndSlotToIndex(nNum2, nSlot2);

	PALETTE_ITEM_TYPE nSlot1Type = m_vecPaletteItem[nIndex1].m_nType;
	int nSlot1ItemIDorTalentID = m_vecPaletteItem[nIndex1].m_nItemIDorTalentID;
	
	PALETTE_ITEM_TYPE nSlot2Type = m_vecPaletteItem[nIndex2].m_nType;
	int nSlot2ItemIDorTalentID = m_vecPaletteItem[nIndex2].m_nItemIDorTalentID;

	m_vecPaletteItem[nIndex1].m_nType = nSlot2Type;
	m_vecPaletteItem[nIndex1].m_nItemIDorTalentID = nSlot2ItemIDorTalentID;

	m_vecPaletteItem[nIndex2].m_nType = nSlot1Type;
	m_vecPaletteItem[nIndex2].m_nItemIDorTalentID = nSlot1ItemIDorTalentID;
}

void GPalette::ResetTalentSlot()
{
	for (int i=0; i<PALETTESLOT_MAX*PALETTENUM_MAX; ++i)
	{
		if (PIT_TALENT != m_vecPaletteItem[i].m_nType) continue;

		PALETTE_NUM nNum;
		PALETTE_SLOT nSlot;
		gsys.pPaletteSystem->IndexToNumAndSlot(i, nNum, nSlot);

		Reset(nNum, nSlot);
	}
}

GPaletteItem* GPalette::GetPaletteItem(PALETTE_NUM nNum, PALETTE_SLOT nSlot)
{
	if (false == gsys.pPaletteSystem->IsValidNumAndSlot(nNum, nSlot)) return NULL;
	uint8 nIndex = gsys.pPaletteSystem->NumAndSlotToIndex(nNum, nSlot);

	return &m_vecPaletteItem[nIndex];
}

void GPalette::GetNumAndSlotByTalentID(int nTalentID, vector<pair<PALETTE_NUM, PALETTE_SLOT>>& outvecPaletteNumAndSlot)
{	
	for (int i=0; i<PALETTESLOT_MAX*PALETTENUM_MAX; ++i)
	{
		if (PIT_TALENT != m_vecPaletteItem[i].m_nType) continue;
		if (nTalentID != m_vecPaletteItem[i].m_nItemIDorTalentID) continue;		

		PALETTE_NUM nNum;
		PALETTE_SLOT nSlot;
		gsys.pPaletteSystem->IndexToNumAndSlot(i, nNum, nSlot);

		outvecPaletteNumAndSlot.push_back(vector<pair<PALETTE_NUM, PALETTE_SLOT>>::value_type(nNum, nSlot));
	}
}

void GPalette::GetNumAndSlotByItemID(int nItemID, vector<pair<PALETTE_NUM, PALETTE_SLOT>>& outvecPaletteNumAndSlot)
{
	for (int i=0; i<PALETTESLOT_MAX*PALETTENUM_MAX; ++i)
	{
		if (PIT_ITEM != m_vecPaletteItem[i].m_nType) continue;
		if (nItemID != m_vecPaletteItem[i].m_nItemIDorTalentID) continue;		

		PALETTE_NUM nNum;
		PALETTE_SLOT nSlot;
		gsys.pPaletteSystem->IndexToNumAndSlot(i, nNum, nSlot);

		outvecPaletteNumAndSlot.push_back(vector<pair<PALETTE_NUM, PALETTE_SLOT>>::value_type(nNum, nSlot));
	}
}

bool GPalette::IsEmpty(PALETTE_NUM nNum, PALETTE_SLOT nSlot)
{
	if (false == gsys.pPaletteSystem->IsValidNumAndSlot(nNum, nSlot)) return false;
	uint8 nIndex = gsys.pPaletteSystem->NumAndSlotToIndex(nNum, nSlot);

	return m_vecPaletteItem[nIndex].IsEmpty();
}

void GPalette::SetPrimary(PALETTE_NUM nNum)
{
	if (m_bLock)
		return;

	if (nNum == m_nSecondarySetPaletteNum)
	{
		m_nSecondarySetPaletteNum = PALETTENUM_MAX;
	}

	m_nPrimarySetPaletteNum = nNum;	
}

void GPalette::SetSecondary(PALETTE_NUM nNum)
{
	if (m_bLock)
		return;

	if (nNum == m_nPrimarySetPaletteNum)
	{
		m_nPrimarySetPaletteNum = PALETTENUM_MAX;
	}

	m_nSecondarySetPaletteNum = nNum;	
}

int GPalette::GetTalentCountOnPalette()
{
	int nTalentCountOnPalette = 0;
	for each(const GPaletteItem& pi in m_vecPaletteItem)
	{
		if (PIT_TALENT == pi.m_nType)
			++nTalentCountOnPalette;		
	}

	return nTalentCountOnPalette;
}

void GPalette::Lock()
{
	m_bLock = true;
}

void GPalette::Unlock()
{	
	m_bLock = false;
}
