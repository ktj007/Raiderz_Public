#pragma once

#include "FSFsm.h"

class CStringResMgr;

class FSContext: public FSFsm<FSContext>
{
public:
	FSContext(const vector<wstring>& param, CStringResMgr* pDictionary = NULL);
	virtual ~FSContext();

	wstring GetResult(void) const;

	void NormalTextState(wchar_t token);
	void OpeningBracketState(wchar_t token);
	void IndexState(wchar_t token);
	void CastingState(wchar_t token);
	void CastEndState(wchar_t token);
	void FormatState(wchar_t token);
	void NestedBracketState(wchar_t token);
	void EscapeBracketState(wchar_t token);

private:
	bool HasJonsung(void) const;

private:
	wstring m_strResult;
	wstring m_strBracket;
	wstring m_strCasting;
	wstring m_strPostfix;
	wstring m_strNestedBracket;
	wstring m_strFormat;
	int	m_nFormatGroup;
	int m_nEscapeBracketDepth;

	vector<wstring> m_vecParam;
	CStringResMgr* m_pDictionary;
};
