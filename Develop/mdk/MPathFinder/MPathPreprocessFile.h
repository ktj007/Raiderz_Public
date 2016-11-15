#pragma once

/**
 Preprocess 앞에 부가 정보를 붙여 자체 포맷을 만든다.

 file format
 {
	// header info
	DWORD dwHeader;
	DWORD dwVersion;
	DWORD dwCount;
	
	// shape info
	DWORD dwShapeSize
	MVector2 *pVertex2DArray;
 }
*/

class iMesh;
class iShape;
class MPathFindPreprocess
{
private:
	struct PATH_HEADER
	{
		DWORD dwHeader;
		DWORD dwVersion;
	};

	// Mesh-data는 MD5 로 판단한다.
	// Shape 판별 정보
	struct PATH_SHAPEINFO
	{
		DWORD dwShapeSize;
		long *pVertex2DiArray;
	};

	static DWORD CURRENT_VERSION;
	static DWORD CURRENT_HEADER;
	
	static bool IsVaildHeader(PATH_HEADER *pHeader);
	static bool IsValidShapeInfo(iShape *shape, PATH_SHAPEINFO *pShapeInfo);
public:
	static void GenerateFile(iMesh *mesh, const char *pcStringMD5, iShape *shape
		,const char *pathfindFilename
		,const char *collisionFilename
		,const char *const* CollisionAttributes = NULL
		,const char *const* PathfindAttribute = NULL);
	static bool LoadPreprocess(iPathEngine *pathengine, iMesh *mesh, const char *pcStringMD5, iShape *shape
		,const char *pathfindFilename
		,const char *collisionFilename
		);
};

/**
sample code
{
	// 저장할 때 ...
	MPathFindPreprocess::generateFile(mesh, shapeA, "shapeFile.small.navigationmesh");

	// 읽어들일 때 ...
	bool bVaild = MPathFindPreprocess::loadPreprocess(mesh, shapeA);
	if (!bValid)	// shape 형식과 불일치 !!
		MPathFindPreprocess::generateFile(mesh, shapeA, "shapeFile.small.navigationmesh");
}
*/