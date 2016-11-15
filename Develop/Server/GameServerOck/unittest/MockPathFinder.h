#pragma once

#include "GPathFinder.h"

class MockPathFinder : public GPathFinder
{
public:
	MockPathFinder() {}
	virtual ~MockPathFinder() {}
	virtual bool NewPlaceAgent(MAgent &agent, const vec3& position, int nZoneID, SPathMesh* pPathMesh) override
	{
		return true;
	}
	virtual bool NewContext(MContext &context, SPathMesh* pPathMesh) override
	{
		return true;
	}

	virtual PATHENGINE_RESULT_TYPE IsValidPosition(float *height, int nID, const MContext *pContext, const MVector3& position) override
	{
		return PATH_SUCCESS;
	}
	virtual PATHENGINE_RESULT_TYPE IsValidPosition(float *height, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position) override
	{
		return PATH_SUCCESS;
	}
	virtual PATHENGINE_RESULT_TYPE GetHeightAtPosition(float *height, SPathMesh* pPathMesh, const MVector3& position) override
	{
		return PATH_SUCCESS;
	}

	virtual PATHENGINE_RESULT_TYPE FindNearPosition(MVector3 *pOut, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position ,int nHorizonRange, int nVerticalRange) override
	{
		return PATH_SUCCESS;
	}
	virtual bool FindPath_NoAgent(SPathMesh* pPathMesh, vec3 vStart, vec3 vEnd, vector<vec3>& vPath) override
	{
		vPath.push_back(vStart);
		return true;
	}
	virtual GPathFinder::QueryResult FindPath(MPath &outPath, SPathMesh* pPathMesh, const MAgent &agent, MVector3 vEnd) override
	{
		return QUERY_SUCCESS;
	}

	virtual float FindPathAway(SPathMesh* pPathMesh, vec3 vStart, vec3 vCenter, float fSize, int nFreq, int nCount, vector<vec3>& vPath) override
	{
		vec3 vTar;
		vTar.x = vCenter.x + fSize;
		vTar.y = vCenter.y + fSize;
		vTar.z = vCenter.z + fSize;
		vPath.push_back( vTar);
		return 1.0f;
	}
	virtual void					PathTo3DPoints(std::vector<MVector3> &out3DPoints, SPathMesh* pPathMesh, MPath &basePath, int offset = 0) override
	{
		out3DPoints.push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	virtual void					PathTo3DPoint(MVector3 &outPos, SPathMesh* pPathMesh, MPath &basePath, int index) override
	{
		outPos = vec3(0.0f, 0.0f, 0.0f);
	}
};

