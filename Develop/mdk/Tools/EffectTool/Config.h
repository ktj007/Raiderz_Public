#pragma once

#include <vector>
#include "wx/string.h"
#include "MXml.h"

namespace EffectTool
{

static const char*	EFFECT_FILE_EXT = ".effect.scene.xml";
static const size_t	EFFECT_FILE_EXT_LENGTH = strlen(EFFECT_FILE_EXT);

class CMostRecentlyUsedList
{
public:
	CMostRecentlyUsedList() {}
	~CMostRecentlyUsedList() {}

	void Add(const wxString& str);
	wxString Get(size_t index);
	size_t GetCount() { return m_list.size(); }

	bool Save(MXmlElement* pElement);
	bool Load(MXmlElement* pElement);

protected:
	static const int MAX_COUNT = 9;
	std::vector<wxString> m_list;
};

class CConfig
{
public:
	CConfig();
	~CConfig();

	void Init();

	bool LoadConfig();
	bool SaveConfig();

	void AddEluFileToMRUList(const wxString& fileName);

	int			CAMERA_FOV;		// FOV (각도)
	int			start_X;
	int			start_Y;
	bool		maximize;
	int			WinForm_width;
	int			WinForm_height;

	//int			split2;
	//int			split3;
	
	wxString	m_strDataPath;
	wxString	m_strEngineResourcePath;
	wxString	m_strFileSystemPath;
	wxString	m_strEluPath;			///< 모델파일 열기 경로
	wxString	m_strEffectPath;		///< SFX 열기 경로
	wxString	m_strEffectTexturePath;	///< SFX 텍스쳐 열기 경로

	CMostRecentlyUsedList m_eluFileMRUList;

	static const int MAX_CUSTOMCOLOR_COUNT = 16;
	int			m_customColors[MAX_CUSTOMCOLOR_COUNT];

	static CConfig& GetInstance()
	{
		static CConfig instance;
		return instance;
	}
};

}