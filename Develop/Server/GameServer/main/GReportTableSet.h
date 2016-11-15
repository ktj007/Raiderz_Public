#pragma once

#include "GReportTableItem.h"

class GReportTableSet
{
public:
	GReportTableSet(): m_strSeparate(L"    ")	{}

	void SetTitle(wstring strTitle);
	int AddColumn(wstring strName, int nWidth, int nSubColCount = 0, int nSubColWidth = 0);
	int AddRow(wstring strName);
	void SetItem(int nRow, int nCol, wstring strData);
	void SetItem(int nRow, int nCol, int nData, bool isPercentage = false);
	void SetItem(int nRow, int nCol, float fData, bool isPercentage = false);
	void SetSubItem(int nRow, int nCol, int nSubCol, int nData);
	void SetSubItem(int nRow, int nCol, int nSubCol, float fData, bool isPercentage = false);
	void SetFillItem(int nRow, int nCol, wchar_t cCharacter);	
	void Report(wofstream& rofStream);

	void Sort(int nBasisCol, int nStartRow);

private:
	wstring m_strTitle;
	vector<pair<wstring,int> > m_vecHeader;
	vector<vector<GReportTableItem> > m_vecRow;
	vector<pair<int,int> > m_vecSubCol;

	const wstring m_strSeparate;
};
