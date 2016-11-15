//////////////////////////////////////////////////////////////////////////
///
///	문자 치환 기능 참조 코드 : http://serious-code.net/moin.cgi/_bf_e5_bc_b3_c7_ca_c5_cd_b8_b5
///
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "CSCommonLib.h"

class CSAbuseCharNode;


class CSCOMMON_API CSStringFilter
{
public:
	~CSStringFilter();

	void					Clear();

	bool					LoadFromFile( const TCHAR* szFileName);
	void					LoadFromStringDB( TCHAR* pStringDB);

	bool					IsValidStr( const TCHAR* szSrcString, bool bCheckSpecialChar =false);
	tstring					Filter( const TCHAR* szSrcString );

	const TCHAR*			GetLastFilteredStr()	{ return m_strLastFilterdWord.c_str(); }


	static CSStringFilter*	GetInstance();

	/// 스트링을 소문자화하고 특수문자를 제거한다.
	static bool				PreTranslateStr(const string& strInText, string& strOutText);

	/// 게임에서 사용하는 특수 문자(중괄호 등으로 묶인 컬러 코드 등)을 제거한다.
	static wstring			RemoveGameSystemCharacter(wstring strText);

private:
	CSStringFilter();

	CSAbuseCharNode*		m_pRoot;				///< 비속어 최상위 노드 - 모든 비속어들의 첫 글자를 담고 있는 노드.
	tstring					m_strLastFilterdWord;

	static const tstring	ms_CheatingChar;		///< 필터링시 감지할 문장 부호들. 비속어 사이에 끼워 넣은 것을 감지하기 위해.

	void					ParseLine( TCHAR*& prfBuf, TCHAR* szText);
	bool					AddAbuseWord( const TCHAR* szAbuse );
	void					NextLine( TCHAR*& prfBuf);

	size_t					Match(const std::tstring& text) const;	///< 문장의 첫 바이트부터 비속어가 포함되어있는지 검사한다.
	bool					IsPunctutation(TCHAR nChar) const;		///< 특정 글자가 문장 부호인지 검사.

};


//////////////////////////////////////////////////////////////////////////
///	비속어 문자 하나에 대한 노드.
///	최상위 노드가 비속어들의 첫 글자들을 담는 노드이고 자식 노드들이 다음 글자가 되는 식으로 비속어를 탐색.
class CSCOMMON_API CSAbuseCharNode : public map<TCHAR, CSAbuseCharNode* >
{
public:
	CSAbuseCharNode();
	~CSAbuseCharNode();

	// 특정 글자가 하위 노드로 있는지 검사.
	CSAbuseCharNode*	FindChild(TCHAR nChar) const;

	// 비속어로 이어질 글자 추가.
	CSAbuseCharNode*	AddChild(TCHAR nChar);

	bool				IsLeafNode() const		{ return m_bLeafNode; }
	void				SetLeafNode(bool value)	{ m_bLeafNode = value; } 

private:
	bool m_bLeafNode; ///< 단어의 끝 글자 여부

};


//////////////////////////////////////////////////////////////////////////

/// 인스턴스를 구함
inline CSStringFilter* GetStringFilter()
{
	return CSStringFilter::GetInstance();
}


