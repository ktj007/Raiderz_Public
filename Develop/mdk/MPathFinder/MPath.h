#pragma once

class iPath;
class MPathFinder;
class MCollisionInfo;
class MPathAgentShape;
class MContext;
class MAgent;

#include <vector>
#include "MVector3.h"

/**
	All paths created on a mesh get destroyed automatically when the mesh is destroyed. 
	이기 때문에 함부로, 소멸자에서 m_pPath를 삭제할 수가 없다.
*/
class MPath
{
	friend MPathFinder;
	friend MAgent;
private:

	//MPath operator =(const MPath &path);
public:
	iPath *m_pPath;
	MPath();
	virtual ~MPath();

	bool IsValid()
	{
		return ( m_pPath != NULL ? true : false );
	}
	unsigned long GetLength();
	unsigned long GetSize();

	void Release();		// 사용한 다음, 반드시 명시적으로 Release 호출
};
