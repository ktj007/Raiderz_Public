#include "StdAfx.h"
#include "AnimationEventTabPage.h"


void Character_NET::AnimationEventTabPage::AllChecked(bool bChecked)
{
	for(int i = 0; i < Event_checkedListBox->Items->Count; ++i)
	{
		CheckedEvent(i, bChecked);
	}
}

void Character_NET::AnimationEventTabPage::CheckedEvent( int nIndex, bool bChecked )
{
	bool bTempChecked = m_bEnableCheck;

	m_bEnableCheck = true;
	Event_checkedListBox->SetItemChecked(nIndex, bChecked);

	m_bEnableCheck = bTempChecked;
}

void Character_NET::AnimationEventTabPage::GetCheckedEventList( vector<bool>& vecbChecked )
{
	for(int i = 0; i < Event_checkedListBox->Items->Count; ++i)
	{
		bool bChecked = Event_checkedListBox->GetItemChecked(i);
		vecbChecked.push_back(bChecked);
	}
}

bool Character_NET::AnimationEventTabPage::GetCheckedEvent( int nIndex )
{
	if(Event_checkedListBox->Items->Count == 0 ||
		Event_checkedListBox->Items->Count <= nIndex)
		return true;

	return  Event_checkedListBox->GetItemChecked(nIndex);
}