#pragma once

class CStringResMgr;

class GExportDBStringTable : public MTestMemPool<GExportDBStringTable>
{
public:
	GExportDBStringTable();
	virtual ~GExportDBStringTable();

	bool Load(wstring strLocale);
	
	wstring GetString(wstring strKey) const;

private:
	CStringResMgr* m_pTable;
};
