#ifndef _GSQL_MAKER_H
#define _GSQL_MAKER_H


#include <string>
#include <vector>


class GSQLMaker
{
public :
	GSQLMaker(const bool bExportToDB, const bool bExportToFile) : m_bExportToDB(bExportToDB), m_bExportToFile(bExportToFile)
	{
		m_vSQL.push_back(L"SET NOCOUNT ON;\n");
	}

	virtual ~GSQLMaker() {}

	

	const std::vector<std::wstring>& GetSQLW() { return m_vSQL; }

	virtual bool BuildSQL() = 0;	

	bool IsExportToDB()		{ return m_bExportToDB; }
	bool IsExportToFile()	{ return m_bExportToFile; }

protected :
	std::vector<std::wstring>	m_vSQL;
	bool						m_bExportToDB;
	bool						m_bExportToFile;
};


class GSQLMakerDeleter
{
public :
	GSQLMakerDeleter(GSQLMaker* pMaker) : m_pMaker(pMaker) {}
	~GSQLMakerDeleter() { delete m_pMaker; }

private :
	GSQLMaker*	m_pMaker;
};


#endif