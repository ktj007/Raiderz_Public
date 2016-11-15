#pragma once

#include <queue>

class CMostRecentlyUsedList
{
public:
	CMostRecentlyUsedList() {}
	~CMostRecentlyUsedList() {}

	void Add( const string& str );
	string Get( size_t index );
	size_t GetCount() { return m_list.size(); }

	bool Save( MXmlElement* pElement );
	bool Load( MXmlElement* pElement );

protected:
	static const int MAX_COUNT = 9;
	vector<string> m_list;
};

class CToolConfig
{
public:
	CToolConfig(void);
	~CToolConfig(void);

	bool LoadConfig();
	bool SaveConfig();

	void AddEluFileToMRUList( const char* pFileName );

	int			CAMERA_FOV;		// FOV (각도)
	int			start_X;
	int			start_Y;
	bool		maximize;
	int			WinForm_width;
	int			WinForm_height;

	int			split2;
	int			split3;
	
	string		m_strDataPath;
	string		m_strEngineResourcePath;
	string		m_strFileSystemPath;
	string		m_strEluPath;			///< 모델파일 열기 경로
	string		m_strEffectPath;		///< SFX 열기 경로
	string		m_strEffectTexturePath;	///< SFX 텍스쳐 열기 경로

	CMostRecentlyUsedList m_eluFileMRUList;

	static const int MAX_CUSTOMCOLOR_COUNT = 16;
	int			m_customColors[MAX_CUSTOMCOLOR_COUNT];

	static CToolConfig& GetInstance()
	{
		static CToolConfig instance;
		return instance;
	}

private:
	void InitPath();
};
