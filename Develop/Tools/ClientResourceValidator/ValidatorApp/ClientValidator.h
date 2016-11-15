#pragma once

#include "VValidatorInfoMgr.h"
#include "VReportMgr.h"

struct FILENAME_DATA;

class ClientValidator
{
private:
	VValidatorInfoMgr				m_ValidatorInfoMgr;
	VReportMgr						m_ReportMgr;

private:
	void							Check_Validate();

	void							Create();
	void							Destroy();

	void							Loading_Validator();
	void							Result_Validator();

	void							Validator_MeshInfo();

	string							SetCurrentDir();
public:
	ClientValidator();
	virtual ~ClientValidator();

	void							Init();
	void							Start();
};

extern 	ClientValidator *g_pValidatorApp;
