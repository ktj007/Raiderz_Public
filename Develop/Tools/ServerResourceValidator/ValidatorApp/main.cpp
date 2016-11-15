#include "stdafx.h"
#include "ServerValidator.h"
#include "MFileSystem.h"


void Initialize();
int Run(void);

#define VALIDATOR_RET_SUCCESS			0
#define VALIDATOR_RET_FAILED			1

int wmain(int argc, WCHAR* argv[])
{
	// 메모리 풀 해제책임 객체
	GScopeMempoolReleaser MemPoolReleaser;

	Initialize();
	return Run();
}

void Initialize()
{
	// 파일시스템 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);


	// 작업 디렉토리 설정
	wchar_t szModuleFileName[_MAX_DIR] = {0,};

	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	wcscat(szModuleFileName, L"\\..\\GameServer");	// GameServer 상대경로 (dll, lua 의존성 있음)

	SetCurrentDirectory(szModuleFileName);	
}

enum ConsoleColor {
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW
};

WORD GetColorAttribute(ConsoleColor color) {
	switch (color) {
	case COLOR_RED:    return FOREGROUND_RED;
	case COLOR_GREEN:  return FOREGROUND_GREEN;
	case COLOR_YELLOW: return FOREGROUND_RED | FOREGROUND_GREEN;
	}
	return 0;
}

void ColoredPrintf(ConsoleColor nColor, const wchar_t* str)
{
	const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Gets the current text color.
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	GetConsoleScreenBufferInfo(stdout_handle, &buffer_info);
	const WORD old_color_attrs = buffer_info.wAttributes;

	SetConsoleTextAttribute(stdout_handle,
		GetColorAttribute(nColor) | FOREGROUND_INTENSITY);

	printf_s("%s", str);

	// Restores the text color.
	SetConsoleTextAttribute(stdout_handle, old_color_attrs);
}

int Run(void)
{
	int result = VALIDATOR_RET_SUCCESS;
	ServerValidator validate;

	cout << "유효성 검사를 시작합니다." << endl;

	ServerValidator::ReturnValue validatorRetValue = validate.Check();

	switch (validatorRetValue)
	{
	case ServerValidator::VALIDATOR_SUCCESS:
		{
			ColoredPrintf(COLOR_GREEN, L"[=======Success=======]\n");
			result = VALIDATOR_RET_SUCCESS;
		}
		break;
	case ServerValidator::VALIDATOR_INITIALIZE_FAILED:
		{
			cout << "유효성 검사를 위한 초기화에 실패하였습니다." << endl;
			ColoredPrintf(COLOR_RED, L"[=======Failed=======]\n");
			result = VALIDATOR_RET_FAILED;
		}
		break;
	case ServerValidator::VALIDATOR_FAILED:
		{
			cout << "유효하지 않은 정보가 포함되어 있습니다." << endl;
			ColoredPrintf(COLOR_RED, L"[=======Failed=======]\n");
			result = VALIDATOR_RET_FAILED;
		}
		break;
	}

	cout << "유효성 검사가 끝났습니다." << endl;

	return result;
}
