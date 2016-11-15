#pragma once

//////////////////////////////////////////////////////////////////////////
/// LOD¸¦ °ü¸®
//////////////////////////////////////////////////////////////////////////
namespace rs3
{

class RLODContext;
class RCameraSceneNode;
class RLODMetricInfo;

class RS_API RLODNode
{

public:
	RLODNode();
	virtual ~RLODNode();

private:
	RLODContext* m_pLODContext;
	int m_nLODLevel;

protected:
	bool CreateLODContext( DWORD dwTypeFlags);
	void DeleteLODContext();

public:
	virtual bool CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere);
	void ResetLOD();
	bool IsLODCullingPassed() const;
	bool SetMetricInfo( RLODMetricInfo* pInfo);
	bool SetProjectionIndex( RLODMetricInfo* pInfo);
	int GetLODLevel();
};

}																																											
