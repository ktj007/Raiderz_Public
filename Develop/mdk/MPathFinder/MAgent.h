#pragma once

#include "../../../sdk/pathengine/i_pathengine.h"
#include "MVector3.h"
#include "MVector2.h"

class MPathFinder;
class MPath;
class MCollisionInfo;
class MContext;

/**
 MAgent와 iAgent를 연결시키기 위해 처음에는 상속을 받을 까 생각 했으나 ,
 iAgent의 setUserData로 모체가 되는 MAgent의 클래스 포인터를 저장해줌으로서 , 서로 참조가 가능하다.

 혹시 MAgent에 임의 지정 가능한 포인터를 달고 싶으면 MAgent를 확장하라.

 All agents placed on a mesh get destroyed automatically when the mesh is destroyed. 
 
 이므로 MAgent도 소멸자에서 Release를 호출하지 못한다.
*/

enum PATHENGINE_RESULT_TYPE;
class MAgent
{
	friend MPathFinder;
	friend MContext;

private:
	int m_nMeshID;

	bool AddToContext(MContext *pContext);
	bool RemoveFromContext(MContext *pContext);
	
	// link-error 나는 것이 정상. = 연산자 및 복사 생성자는 사용할 수 없음.
	MAgent operator =(const MAgent &agent);
	MAgent(const MAgent &agent);
public:
	iAgent *m_pAgent;
	MContext *m_pContext;

public:
	MAgent();
	virtual ~MAgent();
	virtual void Release();		// 사용한 다음, 반드시 명시적으로 Release 호출

	bool GetPosition(MVector3 *pOut) const;

	/**
	 Agent를 지정한 위치로 세팅하는 함수
	 @param pos			3D 좌표 위치
	 @param bForceMove	context를 활성화 시킬 것인지 아닌지 여부, 기본 값 false는 context를 활성화 시킨다.
	 @return PathEngine PATH_ENGINE_RESULT 결과 값
	*/
	PATHENGINE_RESULT_TYPE MoveTo(const MVector3 &pos, bool bForceMove);
	PATHENGINE_RESULT_TYPE MoveTo(const MVector3 &pos);
	void MakeCurvePath(MPath &basePath, int sectionLength = 300, float turnRatio1 = 0.3f, float turnRation2 = 0.4f);

	bool IsInContext() const;

	bool IsValid() const
	{
		return (m_pAgent != NULL ? true : false);
	}

	void SetMeshID(int nID) { m_nMeshID = nID; }
	void SetTraverseCost(float fCost);
	float GetTraverseCost();
	MContext* GetContext() const { return m_pContext; }
};

