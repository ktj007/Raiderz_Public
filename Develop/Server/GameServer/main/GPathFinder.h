#ifndef _GPATH_FINDER_h
#define _GPATH_FINDER_h

#include "GPathFinderImpl.h"

class GQueryCallBack : public iQueryCallBack
{
private:
	unsigned int m_nStartTime;
	bool m_bAborted;

	tSigned32 _frequency;
	tSigned32 _count;
	tSigned32 __count;
	cPosition _startPos;
	cPosition _destPos;
	iMesh* _mesh;

public:
	GQueryCallBack() : m_bAborted(false) {}
	void Set(tSigned32 frequency, tSigned32 count);
	void reset(iMesh* mesh, cPosition startPos, cPosition destPos);

	tSigned32 desiredCallBackFrequency()
	{
		return _frequency;
	}
	bool abort();
	bool IsAborted() { return m_bAborted; }
};

class GPathFinder : public MTestMemPool<GPathFinder>
{
public:
	enum QueryResult
	{
		QUERY_SUCCESS = 1,
		QUERY_SUCCESS_BUT_ABORTED,
		QUERY_FAILED
	};

private:
	static float OFFSET_Z;
	GPathFinderImpl	m_PathFinder;
	GQueryCallBack	m_QueryCallback;
private:
	QueryResult _FindPath(MPath& outPath, SPathMesh* pPathMesh, const MAgent& agent, const vec3& vEnd);

	iShape* GetShape()
	{
		return m_PathFinder.GetShape();
	}

public:
	GPathFinder();
	virtual ~GPathFinder();

	/**
	* 해당 파일(네비게이션메시)을 해당 존 ID에 로딩한다.
	*
	* @param	nZoneID		존 ID
	* @param	szFileName	파일 이름
	*/
	bool AddZone(int nZoneID, const wchar_t* szFileName, RHeightField *pHeightField);
	virtual bool NewPlaceAgent(MAgent &agent, const vec3& position, int nZoneID, SPathMesh* pPathMesh);
	virtual bool NewContext(MContext &context, SPathMesh* pPathMesh);


	/**
	* 해당하는 3D 좌표가 유효한지 그렇지 않은지 검사한다. 3D 좌표에서 Navigation mesh로 수직 Picking을 해서 판단한다.
	*
	* @param	*height	높이를 얻어올 float형 포인터, 필요없으면 NULL을 넣으면 된다.
	* @param	nID		존 ID
	* @param	pContext	충돌 문맥 ( 이 값이 NULL 이 아니면, context 에 동록된 agent에 대한 충돌검사도 행한다 )
	* @param	position	검사할 3D 위치
	* @param	strShape	Shape 이름 (기본값은 small)
	* @return	상태에 따라 PATHENGINE_RESULT_TYPE을 리턴
	*/
	virtual PATHENGINE_RESULT_TYPE IsValidPosition(float *height, int nID, const MContext *pContext, const MVector3& position);
	virtual PATHENGINE_RESULT_TYPE IsValidPosition(float *height, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position);

	/**
	* 해당하는 3D 좌표가 유효한지 그렇지 않은지 검사한다. 3D 좌표에서 Navigation mesh로 수직 Picking을 해서 판단한다.
	*
	* @param	*height	높이를 얻어올 float형 포인터, 필요없으면 NULL을 넣으면 된다.
	* @param	nID		존 ID
	* @param	position	검사할 3D 위치
	* @return	유효한지 여부
	*/
	bool IsValidPosition(SPathMesh* pPathMesh, const MVector3& position);

	virtual PATHENGINE_RESULT_TYPE GetHeightAtPosition(float *height, SPathMesh* pPathMesh, const MVector3& position);

	/**
	* 해당하는 3D 좌표에서 수평 영역 및 수직 영역 +- 범위 이내의 충돌하지 않는 좌표를 찾는다. 
	(Navigation Mesh를 벗어난 영역은 찾지 못하며, 범위 값을 벗어나면 찾지 못한다.)

	nHorizoneRange나, nVerticalRange는 적당한 임의의 정수로 정할 수 있다. 충돌하지 않는지 아닌지는 내부 ShapeRange 값을 써서 찾아낸다.
	따라서 이전 예제처럼 nHorizonRange가 GetShapeRange 값을 얻어와서 할 필요는 없다.

	가장 가까운 점을 찾는 것이 아니다. 쿼리 범위 안에 맞는 적당한 점을 찾는 것이며, L'Generate and test' 방법을 쓴다고 한다.
	*
	* @param pOut 찾아낸 좌표(3D)를 저장할 MVector3 포인터
	* @param nID 존 ID
	* @param pContext	충돌 문맥 ( 이 값이 NULL 이 아니면, context 에 동록된 agent에 대한 충돌검사도 행한다 )
	* @param position 기준 좌표
	* @param nHorizonRange	X,Y 범위(+-)로 인근 좌표를 찾는다. 
	* @param nVerticalRange	Z 범위(+-)로 좌표를 찾는다.
	* @param strShape Shape 스트링, 현재 small/medium/large 세 가지
	*
	* @return 상태에 따라 PATHENGINE_RESULT_TYPE을 리턴
	*/
	virtual PATHENGINE_RESULT_TYPE FindNearPosition(MVector3 *pOut, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position ,int nHorizonRange, int nVerticalRange);


	virtual bool FindPath_NoAgent(SPathMesh* pPathMesh, vec3 vStart, vec3 vEnd, vector<vec3>& vPath);
	virtual GPathFinder::QueryResult FindPath(MPath &outPath, SPathMesh* pPathMesh, const MAgent &agent, MVector3 vEnd);

	virtual float FindPathAway(SPathMesh* pPathMesh, vec3 vStart, vec3 vCenter, float fSize, int nFreq, int nCount, vector<vec3>& vPath);

	
	virtual void					PathTo3DPoints(std::vector<MVector3> &out3DPoints, SPathMesh* pPathMesh, MPath &basePath, int offset = 0);
	virtual void					PathTo3DPoint(MVector3 &outPos, SPathMesh* pPathMesh, MPath &basePath, int index);

	SPathMesh*				GetMesh(int id)
	{
		return m_PathFinder.GetMesh(id);
	}

	static int TEST_PathfindZoneID;
	static cPosition TEST_PathfindStartRawPos;
	static cPosition TEST_PathfindEndRawPos;
	static cPosition TEST_PathfindStartPos;
	static cPosition TEST_PathfindEndPos;
};



#endif // _GPATH_FINDER_h