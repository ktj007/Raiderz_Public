#pragma once

#include "PmDef.h"
#include "PMPMSObject.h"

class GPMSObject : public PMPMSObject
{
public:
	GPMSObject(PMPMSConnector* pPMS);
	virtual ~GPMSObject(void);

	/*
		Heart Beat Request 가 왔을때 불리는 함수
		@param1 : sequence index
		@return value : 정상 상태 TRUE , 비정상 FALSE 
	*/
	virtual BOOL OnHeartbeatReq(LONG lIndex) override;

	/*
	공지 메시지가 왔음을 알리는 함수
	@param1 : 게임 Serial Number
	@param2 : 게임별 카테고리
	@param3 : 공지 메시지 - NULL Terminate String
	@return value : 성공 TRUE , 실패 FALSE 	
	*/
	virtual BOOL OnAnnounceReq(DWORD dwSSN, DWORD dwCategoryID, LPCSTR lpszMsg) override;

	/*
	@param1 : (IPMSRegionInfoList* * 의 포인터 
	@return value : 지역정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnRegionInfoReq(IPMSRegionInfoList* plstRegionInfo) override;


	virtual BOOL OnStatInfoReq( IPMSStatInfoList* plstStatInfo ) override;

	BOOL OnRegionInfoPCReq(IPMSRegionInfoListPC* plstRegionInfo) override;
	BOOL OnStatInfoPCReq(IPMSStatInfoListPC* plstStatInfo) override;
};
