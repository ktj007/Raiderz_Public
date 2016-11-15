#include "stdafx.h"
#include "GCommand.h"
#include "GCommandCenter.h"
#include "GGlobal.h"
#include "GServer.h"

MCommand* MakeNewCommand(int nCmdID, MUID TargetUID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	if (gsys.pServer == NULL ||
		!gsys.pServer->IsCreated())
	{
		va_list marker;
		MCommandParameter* pParam = pCmdParam;

		va_start( marker, pCmdParam );
		for (int i = 0; i < nParamCount; i++)
		{
			SAFE_DELETE(pParam);
			pParam = va_arg( marker, MCommandParameter*);
		}
		va_end( marker );
		return NULL;
	}
	MCommand* pNewCommand = gsys.pCommandCenter->NewCommand(nCmdID, TargetUID);

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */

	for (int i = 0; i < nParamCount; i++)
	{
		pNewCommand->AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	return pNewCommand;
}

MCommand* MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	if (gsys.pServer == NULL ||
		!gsys.pServer->IsCreated())
	{
		va_list marker;
		MCommandParameter* pParam = pCmdParam;

		va_start( marker, pCmdParam );
		for (int i = 0; i < nParamCount; i++)
		{
			SAFE_DELETE(pParam);
			pParam = va_arg( marker, MCommandParameter*);
		}
		va_end( marker );
		return NULL;
	}


	MCommand* pNewCommand = gsys.pCommandCenter->NewCommand(nCmdID, 0);

	if (pNewCommand == NULL)
	{
		return NULL;
	}


	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */
	for (int i = 0; i < nParamCount; i++)
	{
		if (!pNewCommand->AddParameter(pParam))
		{
			SAFE_DELETE(pNewCommand);
			return NULL;
		}

		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	return pNewCommand;
}
