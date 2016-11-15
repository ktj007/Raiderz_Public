#pragma once

namespace rs3 {

struct RHEADER {
	DWORD dwID;
	DWORD dwVersion;
	
	RHEADER() : dwID( 0 ), dwVersion( 0 ) {}
	RHEADER(DWORD id, DWORD version) { dwID=id;dwVersion=version; }
};

#define RS_ID			0x12345678			// .rs 파일
#define RS_VERSION		8

#define RBSP_ID			0x35849298			// .bsp 파일
#define RBSP_VERSION	2

#define R_PAT_ID		0x09784348			// .pat 파일 ( 경로데이터 )
#define R_PAT_VERSION	0

#define R_LM_ID			0x30671804			// .lm 파일 ( 라이트맵 )
#define R_LM_VERSION	3

#define R_COL_ID		0x5050178f			// .col 파일 ( 충돌용 bsp )
#define R_COL_VERSION	1

#define R_NAV_ID		0x8888888f			// .nav 파일 (퀘스트 네비게이션용)
#define R_NAV_VERSION	2


#define RS3_ID			0xface5678			// .rs3 파일
#define RS3_VER1		5
#define RS3_VER2		6					// 정점 포멧에 color 추가 ( 알파로 사용됨 )
#define RS3_VER3		7					// 폴리곤 정보(m_dwFlag)에 Collision 정보 추가
#define RS3_VER4		8					// 디버깅용 셀 버텍스 정보 추가
#define RS3_VERSION		RS3_VER4

const DWORD R_COL2_ID		= 0x59249834;	// .cl2 파일 ( 충돌용 AABBTree )
const DWORD R_COL2_VERSION	= 1;			// save 전용, 읽을 때는 버전별 impl 클래스가 담당

const DWORD R_AUV_ID		= 0x97127371;	// .auv 파일 ( 라이트맵용 uv )
const DWORD R_AUV_VERSION	= 2;
// 1 : 2007.10.17 라이트맵의 크기가 커져서 어러장을 쓸수 있도록 개선
// 2 : 안만들어도 되는 노드는 건너뛰도록

const DWORD R_ZUV_ID		= 0xdefcfead;	// .zuv 파일 ( zone 라이트맵용 uv )
const DWORD R_ZUV_VERSION	= 1;

}
