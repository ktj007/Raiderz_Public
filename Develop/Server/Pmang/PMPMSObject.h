#pragma once

#include "PmLib.h"
#include "IPMSObject.h"

class PMPMSConnector;

class PMANG_API PMPMSObject : public IPMSObject
{
public:
	PMPMSObject(PMPMSConnector* pPMS);
	virtual ~PMPMSObject(void);

	/*
		Heart Beat Request 가 왔을때 불리는 함수
		@param1 : sequence index
		@return value : 정상 상태 TRUE , 비정상 FALSE 
	*/
	virtual BOOL OnHeartbeatReq(LONG lIndex) override;
	/*
		Order Request 가 왔을때 불리는 함수
		@param1 : Order 이름 - NULL Terminate String
		@param2 : Order 인자 - NULL Terminate String
		@param3 : 결과갚 버퍼 - 성공일때 무시, 실패일때 실패 사유 입력
		@param4 : 결과값 버퍼 크기 
		@param5 : 카테고리 ID
		@return value : 성공 TRUE , 실패 FALSE 
	*/
	virtual BOOL OnOrderReq(LPCSTR lpszCmdName, LPCSTR lpszCtlVal, LPSTR lpResult, LONG *lpResultLen, DWORD dwSSN, DWORD dwCategoryID) override;
	/*
	공지 메시지가 왔음을 알리는 함수
	@param1 : 게임 Serial Number
	@param2 : 게임별 카테고리
	@param3 : 공지 메시지 - NULL Terminate String
	@return value : 성공 TRUE , 실패 FALSE 	
	*/
	virtual BOOL OnAnnounceReq(DWORD dwSSN, DWORD dwCategoryID, LPCSTR lpszMsg) override;
	/*
	@param1 : IPMSPerformanceInfo * 의 포인터 
	@return value : 성능정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnPerformInfoReq(IPMSPerformanceInfo *pvPerform) override;
	/*
	@param1 : (IPMSRegionInfoList* * 의 포인터 
	@return value : 지역정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnRegionInfoReq(IPMSRegionInfoList* plstRegionInfo) override;
	/*
	@param1 : IPMSStatInfoList * 의 포인터 
	@return value : 통계정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnStatInfoReq(IPMSStatInfoList* plstStatInfo) override;
	/*
	@param1 : (IPMSRegionInfoListPC* * 의 포인터 
	@return value : 지역정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnRegionInfoPCReq(IPMSRegionInfoListPC* plstRegionInfo) override;
	/*
	@param1 : IPMSStatInfoListPC * 의 포인터 
	@return value : 통계정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnStatInfoPCReq(IPMSStatInfoListPC* plstStatInfo) override;

protected:
	PMPMSConnector* GetPMS();
private:
	PMPMSConnector* m_pPMS;
};
