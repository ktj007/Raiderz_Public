#ifndef __PMSCODES_H_
#define __PMSCODES_H_

enum PMSCONN_STATUSCODE
{
	PMSC_STATUS_INIT_BEFORE = 0, // PMS초기화전
	PMSC_STATUS_INIT, // PMS 초기화됨
	PMSC_STATUS_RUN,  // PMS 동작중
	PMSC_STATUS_STOP, // PMS 정지중
	PMSC_STATUS_NOUSE, // PMS 사용 하지 않는 MODE 설정됨
	PMSC_STATUS_TRY_CONNECTING, // PMS가 HA에 연결을 시도 하려고 하고 있음
};

enum PMSCONN_ERRORCODE
{
	PMSC_ERR_OK = 0, // 성공
	PMSC_ERR_NOT_INITIALIZED, // PMS가 아직 초기화 되지 않았음
	PMSC_ERR_INIT_INVALIDPARAM, // 잘못된 실행 인자
	PMSC_ERR_RUN_ALREADY,	// PMS가 이미 동작중임
	PMSC_ERR_RUN_THREAD_FAILED, // PMS ADL Message 처리를 위한 Thread 생성 실패
};

enum FAULT_LEVEL
{
	FL_WARNING = 1, // only saved DB
	FL_ALERT ,	// saved DB and MC notified as ALERT Level
	FL_CRITICAL , // saved DB and MC notified as Critical Level
	FL_MAX, 
};

#endif 