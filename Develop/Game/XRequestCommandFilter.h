#pragma once

/// 같은 Request 커맨드를 반복해서 보내는 것을 방지하기 위한 클래스
class XRequestCommandFilter
{
private:
	set<int>			m_RequestCommandFilter;			// 필터링할 전체 Request Command 집합
	map<int, int>		m_FilterMap;					// response command id가 Key이다.
	set<int>			m_CurrentSendRequestCommands;	// 현재 필터링중인 Request Command
public:
	XRequestCommandFilter() {}
	~XRequestCommandFilter() {}
	void AddFilter(int nRequestCommandID, int nResponseCommandID);

	bool CheckRequestEnable(int nRequestCommandID);
	void OnResponse(int nResponseCommandID);
	void ClearSendRequestCommands();
};


