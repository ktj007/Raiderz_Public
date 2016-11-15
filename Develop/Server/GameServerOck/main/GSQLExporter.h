#ifndef _GSQL_EXPORTER_H
#define _GSQL_EXPORTER_H


class GSQLExporter
{
public :
	GSQLExporter();
	~GSQLExporter();

	bool ExportToDB(const std::vector<std::wstring>& vSQL);
	bool ExportToFile(const wstring& strFile, const std::vector<std::wstring>& vSQL);
};


#endif