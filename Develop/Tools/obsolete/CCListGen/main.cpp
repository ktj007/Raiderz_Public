#include "stdafx.h"
#include <conio.h>
#include <crtdbg.h>

#include "cclistgen.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CCListGen aCCListGen;

	// set options
	char szPath[MAX_PATH]=".";
	if(argc>1)
	{
		for(int i=1;i<argc;i++)
		{
			if(_stricmp("-path",argv[i])==0)
			{
				if(argc<=i+1)
					goto usage;
				strcpy_s(szPath, MAX_PATH, argv[i+1]);
				i++;
			}else
			goto usage;
		}
	}

	// gen
	char seps[] = ";";
	char *next_token1 = NULL;
	char* token1 = strtok_s( szPath, seps, &next_token1);

	while (token1 != NULL)
	{
		aCCListGen.GenCCList(token1);

		token1 = strtok_s( NULL, seps, &next_token1);

	}

	aCCListGen.Save("CodeCoverages.inc");

	return 0;

usage:
	printf("\n usage : CCListGen [-path TARGETPATH]\n\n");
	printf("   -path  :  대상 경로 설정 (기본값은 현재 경로)\n\n");
	printf(" Copyright (C) MAIET Entertainment\n");
	return 0;
}
