#pragma once

class CSGameWordsFilter
{
public:
	~CSGameWordsFilter();

	void		Clear();

	bool		LoadFromFile( const TCHAR* szFileName);
	void		LoadFromStringDB( TCHAR* pStringDB);

	void		AddWord( tstring strWord);
	int			GetWordsCount()	{ return m_listWords.size(); }

	bool		IsValidStr( tstring strSrcString );
	const TCHAR*	GetLastFilteredStr()	{ return m_strLastFilterdWord.c_str(); }

	static CSGameWordsFilter* GetInstance();

private:
	CSGameWordsFilter();

	void				ParseLine( TCHAR*& prfBuf, TCHAR* szText);
	void				NextLine( TCHAR*& prfBuf);

	list<tstring>		m_listWords;		///< 금칙단어 목록
	tstring				m_strLastFilterdWord;
};



/// 인스턴스를 구함
inline CSGameWordsFilter* GetGameWordsFilter()
{
	return CSGameWordsFilter::GetInstance();
}


