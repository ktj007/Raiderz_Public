
#pragma once


#include <map>
#include <list>

using namespace std;


// XAccountParam
class XAccountParam
{
public:
	map< wstring, wstring>		m_Param;

	XAccountParam()					{}
	virtual ~XAccountParam();

	bool Set( wstring& _type, wstring& _val);
	const wchar_t* Get( wstring& _type);
};



// XAccountData
class XAccountData
{
public:
	// Parameters
	map< wstring, XAccountParam*>			m_Parameters;			// name, info

	// Journal indicator
	vector< pair< int, int>>				m_JournalIndicator;		// Type, ID


public:
	XAccountData();
	virtual ~XAccountData();

	void Destroy();

	bool SetParameter( wstring& strName, wstring& strType, wstring& strParam);
	const wchar_t* GetParameter( wstring& strName, wstring& strType);
	bool DeleteParameter( wstring& strName);
	void ClearParameters();


	bool AddJournal( int nType, int nID);
	bool RemoveJournal( int nType, int nID);
	bool IsExistJournal( int nType, int nID);
	void ClearJournalIndicator();
};









/// class :XAccountInfo
class XAccountInfo
{
/// Member variables
public:
	bool				m_bLoaded;
	XAccountData*		m_pAccountInfo;



/// Member functions
public:
	XAccountInfo();
	virtual	~XAccountInfo();

	void Clear();

	XAccountData* GetAccountInfo();


	bool Save();
	bool Load();
	void ArrangeJournalIndicator();

protected:
	bool GatherAccountSaveData();
};
