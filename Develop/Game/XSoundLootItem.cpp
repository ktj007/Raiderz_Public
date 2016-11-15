#include "StdAfx.h"
#include "XSoundLootItem.h"
#include "XSoundHelper.h"

XSoundLootItem::XSoundLootItem(void)
{
	m_Timer.Stop();
	m_Timer.SetTickTime(0.2f);
}

XSoundLootItem::~XSoundLootItem(void)
{
}

void XSoundLootItem::Update( float fDelta )
{
	if (m_Timer.IsReady(fDelta))
	{
		m_Timer.Stop();

		vector<XItem*>	vecLootItem;

		while (!m_vecLootItem.empty())
		{
			vecLootItem.push_back(m_vecLootItem.back());
			m_vecLootItem.pop_back();
		}

		PlayLootItemSound(vecLootItem);
	}
}

void XSoundLootItem::PushBack( XItem* pItem )
{
	if(m_Timer.IsActive() == false)
	{
		m_Timer.Start();
	}

	//mlog("timegettime = %d \n", timeGetTime());

	m_vecLootItem.push_back(pItem);
}

void XSoundLootItem::PlayLootItemSound(vector<XItem*>& vecLootItem)
{
	if (!vecLootItem.empty())
	{
		struct GreateTier
		{
			bool operator()(const XItem* pItem1,const XItem* pItem2) const
			{
				return pItem1->m_pItemData->m_nTier > pItem2->m_pItemData->m_nTier;
			}
		};

		std::sort(vecLootItem.begin(), vecLootItem.end(), GreateTier());
		string str = XSoundHelper::GetItemPutDownSound(vecLootItem[0]->m_pItemData->m_nID);

		if ( global.sound)
			global.sound->Play(str.c_str());

		vecLootItem.clear();
	}
}
