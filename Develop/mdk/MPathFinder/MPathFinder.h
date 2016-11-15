#pragma once

#include "../../../sdk/pathengine/i_pathengine.h"
#include "MPathAgentShape.h"
#include <vector>
#include "MVector3.h"

/////////////////////////////////////////////////////////////////////////////
//

enum PATHENGINE_RESULT_TYPE 
{
	PATH_SUCCESS = 0,
	INVALID_PARAMETER,
	INVALID_POSITION,
	COLLISION,
};

class MAgent;
class MPath;
class MCollisionInfo;
class MContext;
class RHeightField;
class MPathTerrainCallback;

struct SPathMesh
{
	/**
	m_nID에 따라 Ground Mesh (Navigation Mesh)를 관리
	하나의 iMesh에 ShapeSet(여러 개의 Shape)이 들어간다.
	Shape는 통합적으로 MPathFinder에 들어있고, name으로 각 SPathMesh 구조체에 기록되어 관리/생성된다. ( pre-process Path/Collision data )

	m_nID 하나에 충돌 컨텍스트는 현재 하나
	*/
	int	m_nID;
	iMesh *m_pMesh;

	MPathTerrainCallback *m_pTerrainCallback;	
	vector<string> m_vShapeName;

	SPathMesh()
		: m_nID(-1)
		, m_pMesh(NULL)
		, m_pTerrainCallback(NULL)
	{
	}
};

class MPathFinder
{
	/////////////////////////////////////////////////////////////////////////////
	//

	class MPathFinderError : public iErrorHandler
	{
		virtual eAction handle(const char* type, const char* description, const char *const* attributes); 
	};

	/////////////////////////////////////////////////////////////////////////////
	//


	/////////////////////////////////////////////////////////////////////////////
	//

	class MQueryCallBack : public iQueryCallBack
	{
		tSigned32 _frequency;
		tSigned32 _count;
		tSigned32 __count;
	public:
		void Set(tSigned32 frequency, tSigned32 count)
		{
			_frequency = frequency;
			_count = __count = count;
		}

		void reset() 
		{
			_count = __count;
		}

		tSigned32 desiredCallBackFrequency()
		{
			return _frequency;
		}
		bool abort()
		{
			--_count;
			return _count == 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////

public:
	SPathMesh*				GetMesh(int id);
	iMesh*					GetIMesh(int id);
	iShape*					GetShape(const string& strShape);

	int						GetShapeRange(const string& strShape);
private:

		bool					LoadPathEngine();

		MPathFinderError		m_ErrorHandler;
		
		bool					PreProcessQuery(iMesh* pMesh, MPathAgentShape* pShape, const MVector3& vPos, cPosition &cPos
												,int nHorizonRange = 1000, int nVerticalRange = 500);
		void					BuildPath(iMesh* pMesh, iPath* pPath, vector<MVector3>& ret, int startoffset);

		iPathEngine*			m_pPathEngine;
		MPathAgentShapeSet		m_Shapes;
		HINSTANCE				m_hInstance;

		bool					LoadNavigationMeshFile(int nID, SPathMesh &outPathMesh, char pcOutMD5String[16], const string &strFilename, RHeightField *pHeightField);

		bool					LoadPreprocessFile(SPathMesh &pathMesh, const char pcMD5String[16], const string &strFilename, const vector<string>& vShapes, bool bAutoGeneration);


public:

		vector<SPathMesh>		m_vZoneList;
		MQueryCallBack			m_QueryCallBack;

		bool					Create();
		void					Destroy();

		bool					AddZone(int nID, const string& strFilename, RHeightField *pHeightField = NULL, bool bAutoGeneratePreprocess = true);
		bool					AddZone(int nID, const string& strFilename, const vector<string>& vShapes, RHeightField *pHeightField = NULL, bool bAutoGeneratePreprocess = true);
 
		/**
		좌표의 유효성 검사 및 높이 추출
		 @param	pContext	충돌 문맥 ( 이 값이 NULL 이 아니면, context 에 동록된 agent에 대한 충돌검사도 행한다 )
	    */
		PATHENGINE_RESULT_TYPE	IsValidPosition(float *height, int nID, const MContext *pContext, const MVector3& position, const string& strShape = "small");

		/**
		 인근의 유효한 좌표를 찾는다.

		 @param pOut 찾아낸 좌표(3D)를 저장할 MVector3 포인터
		 @param nID Mesh ID
		 @param	pContext	충돌 문맥 ( 이 값이 NULL 이 아니면, context 에 동록된 agent에 대한 충돌검사도 행한다 )
		 @param position 기준 좌표
		 @param nHorizoneRange	X,Y 범위(+-)로 인근 좌표를 찾는다. 
		 @param nVerticalRange	Z 범위(+-)로 좌표를 찾는다.
		 @param strShape Shape 스트링, 현재 small/medium/large 세가지

		 @return 찾는데 성공하면 true를 리턴, 실패하면 false를 리턴
		 */
		PATHENGINE_RESULT_TYPE	IsNearPosition(MVector3 *pOut, int nID, const MContext *pContext, const MVector3& position
			,int nHorizonRange, int nVerticalRange, const string& strShape = "small");

		// true를 리턴하면 성공적으로 MAgent를 얻어온 것이다.
		bool					NewPlaceAgent(MAgent &agent, const MVector3& position, int nID, const string& strShape = "small");

		// true를 리턴하면 성공적으로 MContext를 얻어온 것이다.
		bool					NewContext(MContext &context, int nID);
		
		// PATHENGINE_RESULT_TYPE 을 리턴, SUCCESS = 0 이 아니면 무언가 잘못된 것이다.
		PATHENGINE_RESULT_TYPE	AdvancedAlongPath(MCollisionInfo *pCollisionInfoOut, int nID, MAgent &agent, MPath &path, float fDistance);
		
		// ???
		float					FindPath(int nID, const MVector3 &vStart, const MVector3 &vEnd, vector<MVector3>& vPath
										, const string& strShape = "small");
		float					FindPath(int nID, const MVector3 &vStart, const MVector3 &vEnd, vector<MVector3>& vPath
										, int nFreq, int nCount, const string& strShape = "small");

		// true를 리턴하면 성공적으로 MPath를 얻어온 것이다.
		// MPath를 얻어내는 FindPath , 전자의 벡터를 쓰는 것 보다 이쪽이 나을 것 같아서 테스트로 만들어보자.
		bool					FindPath(int nID, const MVector3 &vStart, const MVector3 &vEnd, MPath &outPath, MContext *pContext, const string& strShape = "small");

		// agent를 사용하는 PathFind
		bool					FindPath(MPath &outPath, int nID, const MAgent &agent, const MVector3 &vEnd
										, const string& strShape = "small");
		bool					FindPath(MPath &outPath, int nID, const MAgent &agent, const MVector3 &vEnd, int nFreq, int nCount
										, const string& strShape = "small");

		// vCenter 중심 fSize 길이의 정사각형 밖으로 탈출하는 최단경로
		// 반환값 : 경로 길이
		float					FindPathAway(int nID, const MVector3& vStart, const MVector3& vCenter, float fSize, vector<MVector3>& vPath, const string& strShape = "small");
		float					FindPathAway(int nID, const MVector3& vStart, const MVector3& vCenter, float fSize, int nFreq, int nCount, vector<MVector3>& vPath, const string& strShape = "small");

		// start에서 end로 이동할떄 첫 충돌 지점을 알려준다. 반환값은 충돌여부
		PATHENGINE_RESULT_TYPE FindFirstCollision(int nID, const MVector3& vStart, const MVector3& vEnd, MVector3& vRet, const string& strShape = "small");
public:

		void					PathTest(int nID, const MVector3& vStart, const MVector3& vEnd, FILE* fp, const string& strShape = "small");

		// MPath를 실제적인 3D 정점 배열로 가져온다. 크게 필요 없을 듯 싶다.
		void					PathTo3DPoints(std::vector<MVector3> &out3DPoints, int nID, MPath &basePath, int offset = 0);
		void					PathTo3DPoint(MVector3 &outPos, int nID, MPath &basePath, int index);

		// Navigation Mesh 데이터를 Triangle-List 폴리곤으로 구성한다.
		int						GetStaticObstacleDebugMesh(int nID, std::vector<MVector3> &triangleArray, float fShapeHeight = 100.f);

		int						GetNavigationMeshFaceCount(int nID);
		int						GetNavigationMeshFaceList(int nID, MVector3 *pVertices, int nFaceCount);

		void					ClearZone();
		iPathEngine*			GetPathEngineInterface()
		{
			return m_pPathEngine;
		}

		MPathFinder();
		~MPathFinder();
};
