#pragma once

#include <string>
#include <vector>
using namespace std;



class XTips;
class XTipCategory;




/// class : XTipCategory
class XTipCategory
{
	friend XTips;


protected:
	int					m_nParam1;
	int					m_nParam2;
	vector< wstring>	m_Tips;
	wstring				m_strDescription;
	
	wstring				m_strBuffer;	// 문자열 테이블 값 임시 저장 변수

public:
	XTipCategory( int nParam1, int nParam2);
	virtual ~XTipCategory();

	void AddTip( const wchar_t* szTip);
	const wchar_t* GetRandomTip();

	void SetDescription( const wchar_t* szDesc);
	const wchar_t* GetDescription();
};





/// class : XTips
class XTips
{
/// Member variables
private:
	multimap< wstring, XTipCategory*>	m_Categories;		// < category name , category data >
	wstring		m_strString;



/// Member functions
public:
	XTips();
	virtual ~XTips();

	bool Load();

	const wstring& GetRandomTip();
	const wstring& GetFieldDesc();

	const wchar_t* GetRandomCommonTip();
	const wchar_t* GetRandomLevelTip();
	const wchar_t* GetRandomFieldTip();
};

class XTipInGameUpdater
{
private:
	MRegulator		m_RegUpdater;
public:
	XTipInGameUpdater();
	~XTipInGameUpdater() {}
	void Update(float fDeltaTime, XTips* pTips, XUISystem* pUISystem);
};


