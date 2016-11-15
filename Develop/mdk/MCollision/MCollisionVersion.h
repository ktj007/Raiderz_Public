#pragma once

struct RHEADER 
{
	DWORD dwID;
	DWORD dwVersion;

	RHEADER() {}
	RHEADER(DWORD id, DWORD version)
		: dwID(id)
		, dwVersion(version)
	{
	}
};

const DWORD R_COL2_ID		= 0x59249834;	// .cl2 파일 ( 충돌용 AABBTree )
const DWORD R_COL2_VERSION	= 1;			// save 전용, 읽을 때는 버전별 impl 클래스가 담당
