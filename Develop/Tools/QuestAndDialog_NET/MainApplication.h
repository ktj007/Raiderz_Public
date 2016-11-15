#pragma once

class CMainApplication
{
protected:

public:
	CMainApplication(void);
	virtual ~CMainApplication(void);

	bool SetDialogTreeView();
};
extern 	CMainApplication *g_pMainApp;
