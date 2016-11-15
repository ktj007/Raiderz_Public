// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PMSCONN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PMSCONN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PMSCONN_EXPORTS
#define PMSCONN_API __declspec(dllexport)
#else
#define PMSCONN_API __declspec(dllimport)
#endif

#include "IPMSObject.h"
#include "PMSCodes.h"
/*
PMSInitConn : PMSConn 초기화를 하기 위한 함수, Console 모드일때는 main함수의 인자를 , Service Mode일때는 ServiceMain 함수의 인자를 전달
@param1 : 실행 인자 개수
@param2 : 실행인자의 포인터 배열
@return value : PMSCONN_ERRORCODE 참조
*/
extern PMSCONN_API	DWORD	PMSInitConn(DWORD argc, LPSTR argv[]);	// LPTSTR -> LPSTR로 변경합니다. 유니코드 문제로 링크가 안되네요. [2010/6/9 praptor]
/*
PMSRunConn : PMSConn을 실제적으로 동작 시키는 함수
@param1 : Call Back을위한 IPMSObject 포인터 IPMSObject는 IPMSObject관련 설명 참조
@return value : PMSCONN_ERRORCODE 참조
*/
extern PMSCONN_API  DWORD	PMSRunConn(IPMSObject *pObj);
/*
PMSStopConn : PMSConn을 종료 하기 위한 함수
*/
extern PMSCONN_API  void	PMSStopConn();
/*
PMSSendWarningMsg : PMS로 Warning 메시지를 보내기 위한 함수
@param1 : 장애 레벨 FAULT_LEVEL 참조
@param2 : 장애 내용
@param3 : 장애 처리 방법
@param4 : 장애 발생 게임 Serial
@param5 : 장애 발생 카테고리
*/
extern PMSCONN_API	BOOL	PMSSendWarningMsg(DWORD dwErrLvl, LPCSTR pszWarningMsg, LPCSTR pszTreatMsg, DWORD dwSSN, DWORD dwCategory);

/*
PMSGetConfigFileName PMS로 넘겨진 성능 정보 Config 파일의 이름을 가져 오는 함수
@return value PMS인자중 /CNFGFILE:xxx.xxx 중 xxx.xxx값을 리턴 
*/
extern PMSCONN_API	LPCSTR	PMSGetConfigFileName();		// LPCTSTR -> LPCSTR 유니코드 문제로 링크안되서 수정합니다. [2010/6/9 praptor]
/*
PMSGetStatus PMSConn의 현재 상태를 가져 오기 위한 함수
@return value : PMSCONN_STATUS_CODE 값을 리턴  
*/
extern PMSCONN_API	DWORD	PMSGetStatus();
