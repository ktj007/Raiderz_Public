#ifndef _X_REPAIRITEM_CONTAINER_H
#define _X_REPAIRITEM_CONTAINER_H

struct REPAIRITEM 
{
	MUID	ItemUID;
	int		RepairPrice;
};

class XRepairItemContainer
{
protected:
	float					m_fRate;
	vector<REPAIRITEM>		vecItemList;

public:
	XRepairItemContainer()	{}
	virtual ~XRepairItemContainer() {}

	void	Init()
	{
		m_fRate = 0.0f;
		vecItemList.clear();
	}

	float GetRate()						{ return m_fRate;				}
	void SetRate(float val)				{ m_fRate = val;				}

	int GetItemCount()					{ return vecItemList.size();	}

	void PushBackItem(REPAIRITEM item)	{ vecItemList.push_back(item);	}
	void PopBackItem()					{ vecItemList.pop_back();		}
	void PopItem(REPAIRITEM item)
	{
		PopItem(item.ItemUID);
	}

	void PopItem(MUID ItemUID)
	{
		vector<REPAIRITEM>::iterator itor;
		for (itor = vecItemList.begin(); itor != vecItemList.end(); ++itor) 
		{
			REPAIRITEM curitem = *itor;
			if (ItemUID == curitem.ItemUID)
			{
				vecItemList.erase(itor);
				break;
			}
		}
	}

	bool IsItem(MUID ItemUID)
	{
		vector<REPAIRITEM>::iterator itor;
		for (itor = vecItemList.begin(); itor != vecItemList.end(); ++itor) 
		{
			REPAIRITEM curitem = *itor;
			if (ItemUID == curitem.ItemUID)
			{
				return true;
			}
		}

		return false;
	}

	const wchar_t* GetItemUIDList(wstring& szText)
	{
		wchar_t buff[512] = {0,};
		szText = L"";
		for( vector<REPAIRITEM>::iterator it = vecItemList.begin(); it != vecItemList.end(); ++it)
		{
			if( it == vecItemList.begin())
				swprintf_s(buff, L"\"%I64u\"", it->ItemUID.Value);
			else
				swprintf_s(buff, L",\"%I64u\"", it->ItemUID.Value);

			szText += buff;
		}

		return szText.c_str();
	}

	void GetItemUIDList(vector<MUID>& vecItemUID)
	{
		for( vector<REPAIRITEM>::iterator it = vecItemList.begin(); it != vecItemList.end(); ++it)
		{
			REPAIRITEM curitem = *it;
			vecItemUID.push_back(curitem.ItemUID);
		}
	}

	const wchar_t* GetItemRepairPriceList(wstring& szText)
	{
		wchar_t buff[512] = {0,};
		szText = L"";
		for( vector<REPAIRITEM>::iterator it = vecItemList.begin(); it != vecItemList.end(); ++it)
		{
			if( it == vecItemList.begin())
				swprintf_s(buff, L"%d", it->RepairPrice);
			else
				swprintf_s(buff, L",%d", it->RepairPrice);

			szText += buff;
		}

		return szText.c_str();
	}

	int GetTotalPrice()
	{
		int nSilver = 0;
		for( vector<REPAIRITEM>::iterator it = vecItemList.begin(); it != vecItemList.end(); ++it)
		{
			REPAIRITEM curitem = *it;
			nSilver += curitem.RepairPrice;
		}

		return nSilver;
	}
};

#endif //_X_REPAIRITEM_CONTAINER_H