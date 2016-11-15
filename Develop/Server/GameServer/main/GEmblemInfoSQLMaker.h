#ifndef _GEMBLEMINFO_SQL_MAKER_H
#define _GEMBLEMINFO_SQL_MAKER_H


#include "GSQLMaker.h"
#include "GEmblemInfoMgr.h"

#include <string>


class GEmblemInfoSQLMaker : public GSQLMaker, public MTestMemPool<GEmblemInfoSQLMaker>
{
public :
	GEmblemInfoSQLMaker(const std::wstring& strFileName);

	bool BuildSQL() override;


private :
	class XmlLoader : public GEmblemInfoMgr
	{
	public :
		EmblemMap& GetContainer() { return m_mapEmblems; }
	};

	bool LoadXml();


private :
	std::wstring m_strFielName;
	XmlLoader	m_XmlLoader;
};


#endif