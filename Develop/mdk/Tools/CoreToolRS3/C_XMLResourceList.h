#pragma once
#include <vector>
#include <string>

class CORETOOL_API CXMLResourceList
{
public:
	inline static CXMLResourceList& GetReference()
	{
		static CXMLResourceList resList;
		return resList;
	}
	enum LIST_TYPE
	{
		ZONE_LIST = 0,
		FIELD_LIST,
	};

	void InitResourcePath();

	//zone - field list in system folder
	bool LoadZoneList();
	bool LoadFieldList();
	
	void PutComboBox(CComboBox& _refComboBox, LIST_TYPE _type);
	void PutListCtrl(CListCtrl& _refListCtrl,
		std::map<int, std::pair<std::string, std::string> >& _refFileName,
		LIST_TYPE _type );

	const std::vector<std::string>& GetZoneList(){ return m_zoneList_; }
	const std::vector<std::string>& GetFieldList(){ return m_fieldList_; }

	bool	MakeZoneIdFileNameMap();
	int		GetZoneIdFromFileName(const char* _pSzZoneName);
	bool	GetZoneFileNameFromId(int _nId, std::string& _refZoneFileName);

	const char* GetZoneListPath() { return m_zoneListPath_.c_str(); }
	const char* GetZoneFolderPath() { return m_zoneFolderPath_.c_str(); }
	const char* GetZoneUtilsPath() { return m_zoneUtilsPath_.c_str(); }
	const char* GetFieldListPath() { return m_fieldListPath_.c_str(); }
	const char* GetNpcListPath() { return m_npcListPath_.c_str(); }
	const char* GetNpcModelFolderPath() { return m_npcModelFolderPath_.c_str(); }
	const char* GetTileFolderPath() { return m_tileFolderPath_.c_str(); }
	const char* GetPorpertyListPath() { return m_propertyListPath_.c_str(); }

	//grass list loader (.grl)
	bool LoadGrassList(const char *filename, std::vector<std::string>& _refVecFile);

private:
	CXMLResourceList(void);
	~CXMLResourceList(void);
	std::vector<std::string> m_zoneList_;
	std::vector<std::string> m_fieldList_;

	std::string m_zoneListPath_;
	std::string m_zoneFolderPath_;
	std::string m_zoneUtilsPath_;
	std::string m_fieldListPath_;
	std::string m_npcListPath_;
	std::string m_npcModelFolderPath_;

	std::string m_tileFolderPath_;
	std::string m_propertyListPath_;

	std::vector< std::pair<int, std::string> > m_vecZoneIdFileNameMap;
};
