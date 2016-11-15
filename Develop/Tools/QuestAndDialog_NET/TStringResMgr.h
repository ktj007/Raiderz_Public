#ifndef _TSTRING_MGR_H
#define _TSTRING_MGR_H

#include "CStringResMgr.h"

#define MAX_TEMPKEY_LEN 1024
#define TEMPKEY_DEFAULT "D_TEMP"
#define TEMPKEY_QUEST "Q_TEMP"
#define TEMPKEY_ONSCREENDIALOG "OND_TEMP"
#define TEMPKEY_OFFSCREENDIALOG "OFFD_TEMP"

class TStringResMgr : public CStringResMgr
{
private:
	CStringRes<string>				m_StrQuestToolTable;

public:
	TStringResMgr(void);
	virtual ~TStringResMgr(void);

	static TStringResMgr* GetInstance();
	bool BeginQuestTool(String^ strPath);
	void EndQuestTool();

	virtual String^ GetStringValue(String^ strKey);
	String^ GetStringFromStrQuestToolTable(String^ strKey);
	
	bool BeginQuest(String^ strPath);
	void EndQuest();

	bool BeginDialog(String^ strPath);
	void EndDialog();
};

inline TStringResMgr* TGetStringResMgr()
{
	return TStringResMgr::GetInstance();
}

#endif //_TSTRING_MGR_H
