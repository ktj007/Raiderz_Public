#pragma once

class XSoundLootItem
{
	vector<XItem*>	m_vecLootItem;
	MRegulator		m_Timer;

public:
	XSoundLootItem(void);
	virtual ~XSoundLootItem(void);

	void Update( float fDelta );
	void PushBack(XItem* pItem);
	void PlayLootItemSound(vector<XItem*>& vecLootItem);
};
