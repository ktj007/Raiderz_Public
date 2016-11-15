#include "stdafx.h"
#include "MCodeCoverage.h"
#include "MDebug.h"

MCodeCoverageChecker* g_pCodeCoverageChecker = NULL;

void MCodeCoverageChecker::Clear()
{
	for (MCodeCoverageItemMap::iterator itor = m_Items.begin(); itor != m_Items.end(); ++itor)
	{
		MCodeCoverageItem* pItem = (*itor).second;
		delete pItem;
	}
	m_Items.clear();
}

void MCodeCoverageChecker::AddItem( const char* szName, const char* szFunction, const char* szFile )
{
	std::string strName = szName;

	MCodeCoverageItem* pNewItem = new MCodeCoverageItem();
	pNewItem->strName = szName;
	pNewItem->strFunction = szFunction;
	pNewItem->strFile = szFile;
	pNewItem->nRunCount = 1;

	m_Items.insert(MCodeCoverageItemMap::value_type(pNewItem->strName, pNewItem));
}

void MCodeCoverageChecker::Check( const char* szName, const char* szFunction, const char* szFile )
{
	if (CheckFilter(szName) == false) return;

	MCodeCoverageItemMap::iterator itor = m_Items.find(std::string(szName));
	if (itor != m_Items.end())
	{
		MCodeCoverageItem* pItem = (*itor).second;
		pItem->nRunCount++;
	}
	else
	{
		AddItem(szName, szFunction, szFile);
	}
}

void MCodeCoverageChecker::OutputResult()
{
	mlog ("=============== Code Coverage ===============\n");

	set<string>	setRun;
	set<string>	setNotRun;

	int nTotal = 0;
	int nRun = 0;

	for (size_t i = 0 ; i < m_vecCheckList.size(); ++i)
	{
		if (CheckFilter(m_vecCheckList[i].c_str()) == false) continue;

		nTotal++;

		MCodeCoverageItem* pItem = FindItem(m_vecCheckList[i]);
		if (pItem)
		{
			nRun++;
			setRun.insert(m_vecCheckList[i]);
			//mlog("#있음 : %s (%d회)\n", m_vecCheckList[i].c_str(), pItem->nRunCount);
		}
		else
		{
			setNotRun.insert(m_vecCheckList[i]);
			//mlog("없음 : %s\n", m_vecCheckList[i].c_str());
		}
	}

	mlog("Total Code Coverage (%d / %d) %.1f%%\n", nRun, nTotal, (nRun / (float)(nTotal)) * 100);

	mlog ("\n - Run -\n  ");

	int index = 0;
	for (set<string>::iterator itor = setRun.begin(); itor != setRun.end(); ++itor)
	{
		mlog("%s ", (*itor).c_str());
		index++;

		if (index % 10 == 0)
		{
			mlog("\n  ");
		}
	}

	mlog ("\n\n - Not Run -\n  ");

	index = 0;
	for (set<string>::iterator itor = setNotRun.begin(); itor != setNotRun.end(); ++itor)
	{
		mlog("%s ", (*itor).c_str());
		index++;

		if (index % 10 == 0)
		{
			mlog("\n  ");
		}
	}
	mlog("\n=============================================\n");
}

MCodeCoverageChecker::MCodeCoverageItem* MCodeCoverageChecker::FindItem( string strName )
{
	MCodeCoverageItemMap::iterator itor = m_Items.find(strName);
	if (itor != m_Items.end())
	{
		return (*itor).second;
	}
	return NULL;
}

bool MCodeCoverageChecker::CheckFilter( const char* szName )
{
	if (strlen(szName) == 0) return true;
	if (NULL != strstr(szName, m_strFilter.c_str())) return true;

	return false;
}