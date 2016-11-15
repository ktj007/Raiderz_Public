#pragma once

#include "MPathFinder.h"
#include "RSceneManager.h"
#include <vector>
#include <string>

using namespace rs3;

/////////////////////////////////////////////////////////////////////////////
//

class MPathEngineExporter
{
	/// Exporter Error Handler
	class MPathEngineExporterError : public iErrorHandler
	{
		virtual eAction handle(const char* type, const char* description, const char *const* attributes);
	};
private:
	/**
	 	iFaceVertex 를 분석해서 결과를 mlog쪽으로 리포트 한다. (debug를 하기 위한 함수)
	 */
	void GroundMeshLogging(const iFaceVertexMesh *pFaceVertex);

	/**
	 RActor Array를 받아서 iContentChunkInstance 관련 데이터를 만든다.
	 @param pPathEngine	
	 @param chunkArray
	 @param chunkInstanceArray
	 @param vActorArray
	 @param vExActorArray
	*/

	// Actor path를 순회하고,
	void	MakeChunkInstanceListActors(MPathFinder *pPathEngine
		, std::vector<iContentChunk*> &chunkArray
		, std::vector<iContentChunkInstance*> &chunkInstanceArray
		, std::vector<RSceneNode*> &chunkInstanceSources
		, std::vector<RSceneNode*> &vActorArray
		, std::vector<RSceneNode*> *pExActorArray = NULL
		);

	// Zone path 내부만 검색한다.
	void	MakeChunkInstanceListStaticMesh(MPathFinder *pPathEngine
		, const std::string &strZoneFilePath
		, std::vector<iContentChunk*> &chunkArray
		, std::vector<iContentChunkInstance*> &chunkInstanceArray
		, std::vector<RSceneNode*> &vStaticMeshArray);

	static bool GetAllActor(rs3::RSceneNode *pSceneNode);
	static bool GetAllStaticMesh(rs3::RSceneNode *pSceneNode);

	static std::string m_strErrorBuffer;

	bool ReadObstacleFile(std::vector<std::vector<MVector3> > &outArray, const char *filename);

	// 유효하지 않은 ex_mesh 정보 (문제 별로)
public:
	std::vector<MVector3> m_invaildExPosition;

public:
	/// NavigationMesh(Ground mesh)를 만든다.
	bool	MakeNavMesh(MPathFinder* pPathEngine, RSceneManager* pManager, std::vector<MVector3> &seeds);
	/// Speed Tree 의 Object를 static - obstacles 로 만든다.
	void	ExportTrees(MPathFinder* pPathEngine, RSceneManager* pManager);
	/// RActor 에 딸린 .obstacles를 static - obstacles 로 만든다. (단, collision 플래그가 true 가 된 것만)
	void	ExportActor(MPathFinder* pPathEngine, std::vector<RSceneNode*> *pNodeArray);
	/// RStaticMesh의 .obstacles를 static - obstacles 로 만든다.
	void	ExportStaticMesh(MPathFinder* pPathEngine, std::vector<RSceneNode*> *pNodeArray, const string &strPath);

	void	SaveToFile(MPathFinder* pPathEngine, const string& strFilename);
	void	SaveToFile(MPathFinder* pPathEngine, const string& strFilename, const vector<string> &vShapes);

	/// vPos에 가까운 점을 iMesh에서 가져온다.
	static	cPosition	GetClosestPosition(iMesh *pMesh,MVector3& vPos);
	/// vPos와 cPos의 거리가 threshold 안의 값이냐?
	static	bool		IsPositionInThreshold(iMesh *pMesh,MVector3& vPos,cPosition& cPos);

	iMesh	*m_pRawMesh;
	iPathEngine *m_pEngine;

	const std::string& GetErrorMessage()
	{
		return m_strErrorBuffer;
	}

	MPathEngineExporter();
	virtual ~MPathEngineExporter();
};

/////////////////////////////////////////////////////////////////////////////
