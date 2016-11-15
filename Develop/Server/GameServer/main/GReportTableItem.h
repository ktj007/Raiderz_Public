#pragma once

class GReportTableItem
{
public:
	void SetData(wstring strData);
	void SetSubData(int nSubCol, wstring strData);
	wstring GetData(void) const;
	wstring GetSubData(int nSubCol) const;
	int GetSubDataCount(void) const;

private:
	wstring m_strData;
	vector<wstring> m_vecSubData;
};
