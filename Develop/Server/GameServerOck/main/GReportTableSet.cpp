#include "stdafx.h"
#include "GReportTableSet.h"
#include "GReportTableItem.h"
#include <iomanip>
#include <fstream>


void GReportTableSet::SetTitle(wstring strTitle)
{
	m_strTitle = strTitle;
}

int GReportTableSet::AddColumn(wstring strName, int nWidth, int nSubColCount, int nSubColWidth)
{
	_ASSERT(0 <= nSubColCount);

	m_vecHeader.push_back(make_pair(strName, nWidth));
	m_vecSubCol.push_back(make_pair(nSubColCount, nSubColWidth));

	for(int i = 0; i < (int)m_vecRow.size(); i++)
	{
		m_vecRow[i].resize(m_vecHeader.size());		
	}

	return m_vecHeader.size() - 1;
}

int GReportTableSet::AddRow(wstring strName)
{
	vector<GReportTableItem> vecColumn(m_vecHeader.size());
	vecColumn[0].SetData(strName);

	m_vecRow.push_back(vecColumn);

	return m_vecRow.size() - 1;
}

void GReportTableSet::SetItem(int nRow, int nCol, wstring strData)
{
	_ASSERT(0 <= nRow && 0 <= nCol);
	m_vecRow[nRow][nCol].SetData(strData);
}

void GReportTableSet::SetItem(int nRow, int nCol, int nData, bool isPercentage)
{
	_ASSERT(0 <= nRow && 0 <= nCol);

	wstring strData;
	wstringstream strStream;
	strStream << nData;
	
	if (isPercentage)
	{
		strStream << L"%";
		strStream >> strData;
	}
	else
	{
		// 천단위 콤마
		strStream >> strData;

		for(int i = (int)strData.size() - 3; 0 < i; i -= 3)
		{
			strData.insert(i, L",");
		}	
	}

	m_vecRow[nRow][nCol].SetData(strData);
}

void GReportTableSet::SetItem(int nRow, int nCol, float fData, bool isPercentage)
{
	_ASSERT(0 <= nRow && 0 <= nCol);
	
	wstringstream strStream;

	if (isPercentage)
	{
		// 소숫점 2자리
		strStream << setiosflags(ios::fixed) << setprecision(2) << fData << L"%";
	}
	else
	{
		// 소숫점 4자리
		strStream << setiosflags(ios::fixed) << setprecision(4) << fData;
	}

	m_vecRow[nRow][nCol].SetData(strStream.str());
}

void GReportTableSet::SetSubItem(int nRow, int nCol, int nSubCol, int nData)
{
	_ASSERT(0 <= nRow && 0 <= nCol);

	wstring strData;
	wstringstream strStream;
	strStream << nData;
		
	// 천단위 콤마
	strStream >> strData;

	for(int i = (int)strData.size() - 3; 0 < i; i -= 3)
	{
		strData.insert(i, L",");
	}

	m_vecRow[nRow][nCol].SetSubData(nSubCol, strData);
}

void GReportTableSet::SetSubItem(int nRow, int nCol, int nSubCol, float fData, bool isPercentage)
{
	_ASSERT(0 <= nRow && 0 <= nCol);
	
	wstringstream strStream;

	if (isPercentage)
	{
		// 소숫점 2자리
		strStream << setiosflags(ios::fixed) << setprecision(2) << fData << L"%";
	}
	else
	{
		// 소숫점 4자리
		strStream << setiosflags(ios::fixed) << setprecision(4) << fData;
	}

	m_vecRow[nRow][nCol].SetSubData(nSubCol, strStream.str());
}

void GReportTableSet::SetFillItem(int nRow, int nCol, wchar_t cCharacter)
{
	_ASSERT(0 <= nRow && 0 <= nCol);

	wstringstream strStream;
	strStream << setfill(cCharacter) << setw(m_vecHeader[nCol].second) << L"";
	
	m_vecRow[nRow][nCol].SetData(strStream.str());
}

void GReportTableSet::Report(wofstream& rofStream)
{
	// Title
	rofStream << L"# " << m_strTitle << endl;


	// Header
	for(int i = 0; i < (int)m_vecHeader.size(); i++)
	{
		if (i == 0)
		{
			rofStream << setiosflags(ios::left);
		}
		else
		{
			rofStream << setiosflags(ios::right);
		}
		
		rofStream << setw(m_vecHeader[i].second) << m_vecHeader[i].first;
		rofStream << m_strSeparate;

		rofStream << resetiosflags(ios::left);
		rofStream << resetiosflags(ios::right);
	}

	rofStream << endl;


	// Header Separate
	for each(const pair<wstring,int>& pairColumn in m_vecHeader)
	{
		rofStream << setw(pairColumn.second) << setfill(L'=') << L"" << setfill(L' ');
		rofStream << m_strSeparate;
	}
	
	rofStream << endl;


	// Row
	for(int nRow = 0; nRow < (int)m_vecRow.size(); nRow++)
	{
		bool isSeparateCol = false;
		int nSeparateCol = 0;

		for(int nCol = 0; nCol < (int)m_vecHeader.size(); nCol++)
		{
			if (m_vecSubCol[nCol].first == 0)
			{
				// 서브 컬럼 없음
				if (nCol == 0)
				{
					rofStream << setiosflags(ios::left);
				}
				else
				{
					rofStream << setiosflags(ios::right);
				}

				rofStream << setw(m_vecHeader[nCol].second) << m_vecRow[nRow][nCol].GetData();
				rofStream << m_strSeparate;
			}
			else
			{				
				if (0 == m_vecRow[nRow][nCol].GetSubDataCount())
				{	
					// 서브 컬럼 해드 - 중앙 정렬
					int nFrontWidth = m_vecSubCol[nCol].second * (m_vecSubCol[nCol].first - 1);

					if (nFrontWidth == 0)
					{
						// 서브컬럼이 한개라면, 서브컬럼해드는 좌측정렬
						rofStream << setiosflags(ios::right);
						rofStream << setw(m_vecSubCol[nCol].second) << m_vecRow[nRow][nCol].GetData();
						rofStream << m_strSeparate;
					}
					else
					{
						int nRearWidth = m_vecHeader[nCol].second - nFrontWidth;

						rofStream << setiosflags(ios::left);
						rofStream << setw(nFrontWidth) << L"";
						rofStream << setw(nRearWidth) << m_vecRow[nRow][nCol].GetData();
						rofStream << m_strSeparate;
					}

					if (!m_vecRow[nRow][nCol].GetData().empty())
					{
						isSeparateCol = true;
						nSeparateCol = nCol;
					}
				}
				else
				{
					// 서브 컬럼
					rofStream << setiosflags(ios::right);

					for(int i = 0; i < m_vecSubCol[nCol].first; i++)
					{						
						rofStream << setw(m_vecSubCol[nCol].second) << m_vecRow[nRow][nCol].GetSubData(i);
						rofStream << m_strSeparate;
					}
				}
			}

			rofStream << resetiosflags(ios::left);
			rofStream << resetiosflags(ios::right);
		}

		rofStream << endl;


		// 서브 컬럼 해더 다음 행 구분자 삽입
		if (isSeparateCol)
		{
			for(int i = 0; i < nSeparateCol; i++)
			{
				rofStream << setw(m_vecHeader[i].second) << L"";
				rofStream << m_strSeparate;
			}

			rofStream << setfill(L'=') << setw(m_vecHeader[nSeparateCol].second) << L"" << setfill(L' ');
			rofStream << endl;
		}
	}

	rofStream << endl;
	rofStream << endl;
	rofStream << endl;
}

void GReportTableSet::Sort(int nBasisCol, int nStartRow)
{	
	_ASSERT(0 <= nBasisCol && 0 <= nStartRow);
	
	for(int i = nStartRow; i < (int)m_vecRow.size() - 1; i++)
	{
		for(int j = i + 1; j < (int)m_vecRow.size(); j++)
		{			 
			CString str1 = m_vecRow[i][nBasisCol].GetData().c_str();
			CString str2 = m_vecRow[j][nBasisCol].GetData().c_str();

			str1.Replace(L",", L"");
			str2.Replace(L",", L"");			

			if (_wtof(str1) < _wtof(str2))
			{
				m_vecRow[i].swap(m_vecRow[j]);
			}
		}
	}
}
