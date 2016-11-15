#pragma once

class iCollisionInfo;
class iCollisionContext;
class MPathFinder;
class MPath;
class MAgent;

/**
	All iCollisionInfo objects relating to a given mesh get destroyed automatically when that mesh is destroyed. 
	이므로 소멸자에서 m_pCollisionInfo를 삭제하지 않는다.
*/
class MCollisionInfo
{
	friend MPathFinder;
	friend MPath;
	

	MCollisionInfo operator =(const MCollisionInfo &collisioninfo);
	MCollisionInfo(MCollisionInfo &collisionInfo);
public:
	MCollisionInfo();
	virtual ~MCollisionInfo();

	bool IsValid()
	{
		return ( m_pCollisionInfo ? true : false );
	}
	void Release(); // 사용한 다음, 반드시 명시적으로 Release 호출
	
	MAgent* GetCollisionAgent();
	iCollisionInfo *m_pCollisionInfo;
};

/**
*/
class MContext
{
	friend MPathFinder;
	friend MAgent;

	MContext operator =(const MContext &context);
	MContext(const MContext &context);
public:
	iCollisionContext *m_pContext;

	MContext();
	virtual ~MContext();

	bool IsValid()
	{
		return ( m_pContext ? true : false );
	}

	void Release();
	/// AddAgent, RemoveAgent의 bool 값이 true이면, 정상적으로 함수가 실행되었음을 의미한다.
	bool AddAgent(MAgent &agent);
	bool RemoveAgent(MAgent &agent);

	int GetAgentNum();

	iCollisionContext* GetCollisionContext()
	{
		return m_pContext;
	}
};
