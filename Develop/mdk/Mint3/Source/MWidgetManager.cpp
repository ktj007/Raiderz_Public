#include "stdafx.h"
#include "MWidgetManager.h"
#include "Mint.h"

namespace mint3 {


MWidgetManager::MWidgetManager(Mint* pMint)
{
	m_pMint = pMint;
}

MWidgetManager::~MWidgetManager()
{

}

void MWidgetManager::Clear()
{
	m_WidgetMap.Clear();
}

MWidget* MWidgetManager::FindWidget(string& strName)
{
	MWidgetMMap::iterator itor = m_WidgetMap.find(strName);
	if (itor != m_WidgetMap.end())
	{
		return (MWidget*)(*itor).second;
	}
	else
	{
		return NULL;
	}
}

void MWidgetManager::FindWidgets(MWidgetList& widgetList, string& strName)
{
	pair<MWidgetMMap::iterator, MWidgetMMap::iterator> p = m_WidgetMap.equal_range(strName);

	for (MWidgetMMap::iterator itor = p.first; itor != p.second; ++itor)
	{
		MWidget* pWidget = (*itor).second;
		widgetList.push_back(pWidget);
	}
}

void MWidgetManager::AddWidget(const char* szName, MWidget* pWidget )
{
	m_WidgetMap.insert(MWidgetMMap::value_type(string(szName), pWidget) );
}

void MWidgetManager::RemoveWidget(MWidget* pWidget)
{
	pair<MWidgetMMap::iterator, MWidgetMMap::iterator> p = m_WidgetMap.equal_range(pWidget->GetName());

	for (MWidgetMMap::iterator itor = p.first; itor != p.second; ++itor)
	{
		MWidget* pWidget2 = (*itor).second;
		if (pWidget == pWidget2)
		{
			m_WidgetMap.erase(itor);
			return;
		}
	}
}


} // namespace mint3