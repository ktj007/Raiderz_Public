#pragma once

/// 전역적 임시로 사용하는 변수들
struct GlobalVar_Temp
{
	wchar_t	szPassword[256];		///< 로그인할때 사용하는 임시 패스워드 - 로그인후 초기화된다.

	string		szTemp;				/// c에서 루아로 넘길 임시 변수. 한번사용후 데이타 없어짐.

	int			nNextFieldID;

	GlobalVar_Temp()
	{
		szPassword[0] = 0;
	}
};
