#pragma once

#include "MMemPool.h"
#include "MPath.h"
#include "MAgent.h"

class GEntityNPC;

class GNPCPathFinder : public MMemPool<GNPCPathFinder>
{
private:
	GEntityNPC*					m_pOwner;
	int							m_nZoneID;
	MPath						m_Path;
	MAgent						m_PathAgent;
	SPathMesh*					m_pPathMesh;
public:
	GNPCPathFinder(GEntityNPC* pOwner);
	~GNPCPathFinder();
	void Create(int nZoneID);
	void Destroy();

	void OnEnterToCombat();
	void OnLeaveFromCombat();
	bool MoveAgentTo(const vec3& pos);
	void RegisterToContext(MContext& context);
	void RevokeFromContext();


	bool FindPath(const vec3 vStartPos, const vec3 vDestPos, vector<vec3>& outvPathList);

	void PathTo3DPoints(std::vector<MVector3> &out3DPoints, int offset = 0);
	void PathTo3DPoint(MVector3 &outPos, int index);

	int GetGroupAgentCount();
	float GetHeightAtPosition(vec3& vPos);


	// for test
	MAgent& GetAgent() { return m_PathAgent; }
};