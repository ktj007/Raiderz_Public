
#pragma once

#ifndef __IPMSOBJECT_H_
#define __IPMSOBJECT_H_


typedef std::vector<LONG> VLONG;
/*
OnRegionInfoReq 함수가 불리워 졌을때 사용자 지역 정보를 PMSConn을 통해 HA로 전송하기 위한 데이터를 생성하는 Interface 
@param1 : 게임 Serial Number
@param2 : 게임별 카테고리
@param3 : 사용자 지역정보를 가지고 있는 Vector<LONG> 일반적으로 UserInfo[i][j][k] Array로 되어 있을 경우 (i, j, k) 일때 (0,0,0) (0,0,1) ~ (0,1,0) ~ (1,0.0) 의 순으로 입력 하면 된다.
@return value 없음
*/
interface IPMSRegionInfoList
{
	virtual void AddRegionInfo(DWORD dwSSN, DWORD dwCategoryID, VLONG* pvRegionStat) = 0;
// by truewise - 2006.05.09 
	virtual void AddRegionInfo2(DWORD dwSSN, DWORD dwCategoryID, LONG pvRegionStat[238]) = 0;
	virtual void AddRegionInfo238(DWORD dwSSN, DWORD dwCategoryID, LONG pvRegionStat[238]) = 0;
	virtual void AddRegionInfo272(DWORD dwSSN, DWORD dwCategoryID, LONG pvRegionStat[272]) = 0;
};

// by truewise - 2007.03.22 for PC방 데이터 
interface IPMSRegionInfoListPC
{
	virtual void AddRegionInfoPC(DWORD dwSSN, DWORD dwCategoryID, VLONG* pvRegionStat) = 0;
	virtual void AddRegionInfoPC238(DWORD dwSSN, DWORD dwCategoryID, LONG pvRegionStat[238]) = 0;
	virtual void AddRegionInfoPC272(DWORD dwSSN, DWORD dwCategoryID, LONG pvRegionStat[272]) = 0;
};

/*
IPMSStatInfoList 현재 게임 서버의 통계 정보를 PMSConn을 이용하여 전송하기 위한 데이터를 생성 하는 Interface
@param1 : 게임 Serial Number
@param2 : 게임별 카테고리
@param3 : @param1, @param2의 조건을 만족하는 사용자 수
@param4 : 현재 게임 서버에 생성되어 있고 @param1, @param2의 조건을 만족하는 Channel 수
@param5 : 현재 게임 서버에 생성되어 있고 @param1, @param2의 조건을 만족하는 게임 방 수
@param6 : Optional 한 데이터
@return value 없음
*/
interface IPMSStatInfoList
{
	virtual void AddStatInfoList( DWORD dwSSN, DWORD dwCategory, DWORD dwCU,
        	DWORD dwSession, 	DWORD dwChannelCnt, DWORD dwRoomCnt, LPCSTR pszOptionInfo) = 0;
};

// by truewise - 2007.03.22 for PC방 데이터 
interface IPMSStatInfoListPC
{
	virtual void AddStatInfoListPC( DWORD dwSSN, DWORD dwCategory, DWORD dwCU,
        	DWORD dwSession, 	DWORD dwChannelCnt, DWORD dwRoomCnt, LPCSTR pszOptionInfo) = 0;
};

/*
IPMSPerformanceInfo현재 게임 서버의 성능 정보를 PMSConn을 이용하여 전송하기 위한 데이터를 생성 하는 Interface
@param1 : 게임 서버의 상태 정보를 담고 있는 long type의 vector pointer
@return value 없음
*/

interface IPMSPerformanceInfo
{
	virtual void AddPerformanceInfo(VLONG* pvPerformance) = 0;
// by truewise - 2006.05.09 
	virtual void AddPerformanceInfo2(LONG pvPerformance[10]) = 0;
};

/*
IPMSObject interface
PMSConn에서 호출을 위하여 생성된 Call Back Object Interface
*/
interface IPMSObject
{
public:
	/*
		Heart Beat Request 가 왔을때 불리는 함수
		@param1 : sequence index
		@return value : 정상 상태 TRUE , 비정상 FALSE 
	*/
	virtual BOOL OnHeartbeatReq(LONG lIndex) = 0;
	/*
		Order Request 가 왔을때 불리는 함수
		@param1 : Order 이름 - NULL Terminate String
		@param2 : Order 인자 - NULL Terminate String
		@param3 : 결과갚 버퍼 - 성공일때 무시, 실패일때 실패 사유 입력
		@param4 : 결과값 버퍼 크기 
		@param5 : 카테고리 ID
		@return value : 성공 TRUE , 실패 FALSE 
	*/
	virtual BOOL OnOrderReq(LPCSTR lpszCmdName, LPCSTR lpszCtlVal, LPSTR lpResult, LONG *lpResultLen, DWORD dwSSN, DWORD dwCategoryID) = 0;
	/*
	공지 메시지가 왔음을 알리는 함수
	@param1 : 게임 Serial Number
	@param2 : 게임별 카테고리
	@param3 : 공지 메시지 - NULL Terminate String
	@return value : 성공 TRUE , 실패 FALSE 	
	*/
	virtual BOOL OnAnnounceReq(DWORD dwSSN, DWORD dwCategoryID, LPCSTR lpszMsg) = 0;
	/*
	@param1 : IPMSPerformanceInfo * 의 포인터 
	@return value : 성능정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnPerformInfoReq(IPMSPerformanceInfo *pvPerform) = 0;
	/*
	@param1 : (IPMSRegionInfoList* * 의 포인터 
	@return value : 지역정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnRegionInfoReq(IPMSRegionInfoList* plstRegionInfo) = 0;
	/*
	@param1 : IPMSStatInfoList * 의 포인터 
	@return value : 통계정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnStatInfoReq(IPMSStatInfoList* plstStatInfo) = 0;
	/*
	@param1 : (IPMSRegionInfoListPC* * 의 포인터 
	@return value : 지역정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnRegionInfoPCReq(IPMSRegionInfoListPC* plstRegionInfo) = 0;
	/*
	@param1 : IPMSStatInfoListPC * 의 포인터 
	@return value : 통계정보값이 있을때 TRUE, 없을때 FALSE 리턴
	*/
	virtual BOOL OnStatInfoPCReq(IPMSStatInfoListPC* plstStatInfo) = 0;
};



#endif