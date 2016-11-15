#pragma once

#include <string>
#include <list>
#include <map>
#include <algorithm>
using namespace std;

#include "MXml.h"

namespace mint3 {

class Mint;
class MWidget;


class MWidgetMMap : public multimap<string, MWidget*>
{
public:
	virtual ~MWidgetMMap(void){ Clear(); }

	void Clear() {
		while(empty()==false)
		{
			erase(begin());
		}
	}
};

class MWidgetList : public list<MWidget*> { };


class MWidgetManager
{
private:
protected:
	Mint*			m_pMint;
	MWidgetMMap		m_WidgetMap;
public:
	MWidgetManager(Mint* pMint);
	virtual ~MWidgetManager();
	void Clear();
	MWidgetMMap* GetWidgetMap() { return &m_WidgetMap; }

	void AddWidget(const char* szName, MWidget* pWidget);
	void RemoveWidget(MWidget* pWidget);

	MWidget* FindWidget(string& strName);
	void FindWidgets(MWidgetList& widgetList, string& strName);

};



} // namespace mint3