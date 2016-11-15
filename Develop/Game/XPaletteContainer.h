#ifndef _X_PALETTE_CONTAINER_H
#define _X_PALETTE_CONTAINER_H

#include "XContainer.h"
#include "XPost_Palette.h"
#include "CSHelper.h"

class XPaletteItem
{
public:
	int		itemID;			//¾ÆÀÌÅÛ id
	int		talentID;		//Å»·»Æ® id

	XPaletteItem()
	{
		Init();
	}

	void Init()
	{
		itemID = 0;
		talentID = 0;
	}

	PALETTE_ITEM_TYPE GetItemType()
	{
		if (itemID == 0 && talentID == 0)	return PIT_NONE;
		if (itemID > 0 && talentID == 0)	return PIT_ITEM;
		if (itemID == 0 && talentID > 0)	return PIT_TALENT;

		return PIT_MAX;
	}
};

class XPaletteContainer : public XContainer<XPaletteItem>
{
private:
	static const int CONTAINER_SIZE = PALETTENUM_MAX*PALETTESLOT_MAX;
	wchar_t		m_szFileName[_MAX_PATH];
	bool		m_bLoaded;

	PALETTE_NUM	m_curPaletteListIndex;

	PALETTE_NUM	m_BindingPaletteIndex[2];

public:
	XPaletteContainer()		{ Clear();}
	~XPaletteContainer()	{};

	int GetContainerSize()	{return CONTAINER_SIZE;}
	void Clear()
	{
		m_szFileName[0] = 0;
		m_bLoaded = false;

		XPaletteItem item;
		m_vContains.resize(CONTAINER_SIZE, item);

		m_curPaletteListIndex = PALETTENUM_1;

		m_BindingPaletteIndex[0] = PALETTENUM_MAX;
		m_BindingPaletteIndex[1] = PALETTENUM_MAX;
	}

	virtual bool IsLock_PaletteList() const = 0;
	virtual bool IsLock_PaletteSlot() const = 0;

	PALETTE_NUM GetCurPaletteListIndex() const { return m_curPaletteListIndex; }
	void SetCurPaletteListIndex(PALETTE_NUM val)
	{
		if (IsLock_PaletteList())
			return; // locked

		if (val < PALETTENUM_1 || val >= PALETTENUM_MAX)
			return;

		m_curPaletteListIndex = val;
	}

	void		SetItemAtIndex(int index, XPaletteItem item)	{ m_vContains[index] = item;}

	void		ChangeItem(int nIndex1, int nIndex2)
	{
		XPaletteItem item = m_vContains[nIndex2];
		m_vContains[nIndex2] = m_vContains[nIndex1];
		m_vContains[nIndex1] = item;
	}

	XPaletteItem* GetSelectedItem(int index)
	{
		if (index < 0 || index >= PALETTESLOT_MAX)
		{
			return NULL;
		}

		return &m_vContains[(m_curPaletteListIndex * 10) + index];
	}

	const wchar_t* GetItemIDList(wstring& szText)
	{
		wchar_t buff[512] = {0,};
		szText = L"";

		size_t i = m_curPaletteListIndex * 10;
		size_t end = (m_curPaletteListIndex+1) * 10;
		for( ; i < end; ++i)
		{
			if( i != (m_curPaletteListIndex * 10))
			{
				szText += L",";
			}

			XPaletteItem item = m_vContains[i];
			if (item.itemID != 0)
			{
				_itow_s(item.itemID, buff, 10);
				szText += L"I";
			}
			else if(item.talentID != 0)
			{
				_itow_s(item.talentID, buff, 10);
				szText += L"T";
			}
			else
			{
				_itow_s(item.talentID, buff, 10);
			}
			szText += buff;
		}

		return szText.c_str();
	}

	void	DeleteItem(int ItemID)
	{
		TDeque::iterator itor;
		for (itor = m_vContains.begin(); itor != m_vContains.end(); ++itor) 
		{
			XPaletteItem* curid = &(*itor);
			if (curid->itemID == ItemID)
			{
				curid->Init();
				break;
			}
		}
	}
	
	virtual void DeleteAllItem()
	{
		XContainer::DeleteAllItem();
		Clear();
	}

	void	DeleteTalents()
	{
		TDeque::iterator itor;
		for (itor = m_vContains.begin(); itor != m_vContains.end(); ++itor) 
		{
			XPaletteItem* curid = &(*itor);
			if (curid->talentID != 0)
			{
				curid->Init();
			}
		}
	}

	void Binding(int index)
	{
		if (IsLock_PaletteSlot())
			return; // locked

		if(index == 2)
		{
			if (m_BindingPaletteIndex[0] == m_curPaletteListIndex)
			{
				m_BindingPaletteIndex[0] = PALETTENUM_MAX;
				XPostPalette_SetPrimary(PALETTENUM_MAX);
			}
			else if (m_BindingPaletteIndex[1] == m_curPaletteListIndex)
			{
				m_BindingPaletteIndex[1] = PALETTENUM_MAX;
				XPostPalette_SetSecondary(PALETTENUM_MAX);				
			}
			return;
		}
		else if (index == 0)
		{
			if (m_BindingPaletteIndex[1] == m_curPaletteListIndex)
			{
				m_BindingPaletteIndex[1] = PALETTENUM_MAX;
			}

			m_BindingPaletteIndex[0] = m_curPaletteListIndex;
			XPostPalette_SetPrimary(m_curPaletteListIndex);
		}
		else if (index == 1)
		{
			if (m_BindingPaletteIndex[0] == m_curPaletteListIndex)
			{
				m_BindingPaletteIndex[0] = PALETTENUM_MAX;
			}

			m_BindingPaletteIndex[1] = m_curPaletteListIndex;
			XPostPalette_SetSecondary(m_curPaletteListIndex);
		}
	}

	bool BindingIndex(int index)
	{
		if (IsLock_PaletteSlot())
			return false; // locked

		if(m_BindingPaletteIndex[index] == m_curPaletteListIndex)
			return true;

		return false;
	}


	void BindingByPaletteNum(PALETTE_NUM nPrimaryNum, PALETTE_NUM nSecondaryNum)
	{
		if (IsLock_PaletteSlot())
			return; // locked

		m_BindingPaletteIndex[0] = nPrimaryNum;
		m_BindingPaletteIndex[1] = nSecondaryNum;
	}

	void ChangeWeaponBindingPalette(int8 nWeaponSet)
	{
		if (IsLock_PaletteSlot())
			return; // locked

		SetCurPaletteListIndex(m_BindingPaletteIndex[nWeaponSet]);
	}

	bool IsRegistedTalentLine(int nTalentLine)
	{
		int nCurList = m_curPaletteListIndex*PALETTESLOT_MAX;
		for (int i = nCurList; i < (nCurList+PALETTESLOT_MAX); i++)
		{
			XPaletteItem item = m_vContains[i];
			if (item.GetItemType() == PIT_TALENT)
			{
				XTalentInfo* pTalentInfo = info.talent->Get(item.talentID);
				if (pTalentInfo == NULL) continue;

				if (nTalentLine == pTalentInfo->m_nTalentLine)
				{
					return true;
				}
			}
		}

		return false;
	}

	int GetRegisteredPaletteIndex(PALETTE_ITEM_TYPE _type, int nItemIDorTalentID)
	{
		int nCurList = m_curPaletteListIndex*PALETTESLOT_MAX;
		for (int i = nCurList; i < (nCurList+PALETTESLOT_MAX); i++)
		{
			XPaletteItem item = m_vContains[i];
			PALETTE_ITEM_TYPE _curtype = item.GetItemType();
			if(_curtype == PIT_NONE) continue;

			if (_type == PIT_ITEM && _curtype == PIT_ITEM)
			{
				if (item.itemID == nItemIDorTalentID) return (i%PALETTESLOT_MAX);
			}
			else if (_type == PIT_TALENT && _curtype == PIT_TALENT)
			{
				if (item.talentID == nItemIDorTalentID) return (i%PALETTESLOT_MAX);
			}
		}

		return -1;
	}

	int GetEmptySlotIndex()
	{
		int nCurList = m_curPaletteListIndex*PALETTESLOT_MAX;
		for (int i = nCurList; i < (nCurList+PALETTESLOT_MAX); i++)
		{
			XPaletteItem item = m_vContains[i];
			if (item.GetItemType() == PIT_NONE) return (i%PALETTESLOT_MAX);
		}

		return -1;
	}

};

class XPaletteManager : public XPaletteContainer
{
public:
	class Locker
	{
	public:
		virtual bool IsLock_PaletteList() const = 0;
		virtual bool IsLock_PaletteSlot() const = 0;
	};

	class Unlocker : public Locker
	{
	public:
		virtual bool IsLock_PaletteList() const { return false; }
		virtual bool IsLock_PaletteSlot() const { return false; }
	};

	class BuffLocker : public Locker
	{
	public:
		virtual bool IsLock_PaletteList() const { return true; }
		virtual bool IsLock_PaletteSlot() const { return true; }
	};

	class SlotChanger
	{
	public:
		void Change(deque<XPaletteItem>& qRealSlots, int arrPalettes[])
		{
			m_qOriginalSlots = qRealSlots;

			for (int i=0; i<PALETTESLOT_MAX; ++i)
			{
				XPaletteItem item;
				item.talentID = arrPalettes[i];
				qRealSlots[i] = item;
			}
		}

		void Restore(deque<XPaletteItem>& qRealSlots)
		{
			qRealSlots = m_qOriginalSlots;
			m_qOriginalSlots.clear();
		}
	private:
		deque<XPaletteItem> m_qOriginalSlots;
	};

public:
	XPaletteManager() 
		: m_pLocker(NULL)
	{
		LockFree();
	}

	virtual ~XPaletteManager() 
	{
		SAFE_DELETE(m_pLocker);
	}

	virtual bool IsLock_PaletteList() const override
	{
		return m_pLocker->IsLock_PaletteList();
	}

	virtual bool IsLock_PaletteSlot() const override
	{
		return m_pLocker->IsLock_PaletteSlot();
	}

	void LockFree()
	{
		SAFE_DELETE(m_pLocker);
		m_pLocker = new Unlocker();
	}

	void LockForBuff()
	{
		SAFE_DELETE(m_pLocker);
		m_pLocker = new BuffLocker();
	}

	void ChangePalette(int arrPalettes[])
	{
		m_SlotChanger.Change(m_vContains, arrPalettes);
		LockForBuff();
	}

	void RestorePalette()
	{
		LockFree();
		m_SlotChanger.Restore(m_vContains);
	}
private:
	Locker* m_pLocker;
	SlotChanger m_SlotChanger;
};

#endif //_X_PALETTE_CONTAINER_H