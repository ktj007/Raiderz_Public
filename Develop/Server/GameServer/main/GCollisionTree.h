#ifndef _GCOLLISIONTREE_H_
#define _GCOLLISIONTREE_H_

#include "RCollisionTree.h"
#include "MBox.h"
#include "MPathFinder.h"

/**
 RCollisionTree 와 RHeightField를 써서 전체 지역을 Picking 하고 검출해내는 Class
 이름은 비슷하게 지었습니다. (혼동되지 않도록 하기 위해)
*/

class RHeightField;

class GCollisionTree : public MTestMemPool<GCollisionTree>
{
private:
	rs3::RCollisionTree *m_pCollisionTree;
	RHeightField *m_pHeightField;

	MBox m_aabb;
	int m_nZoneID;

public:
	GCollisionTree();
	virtual ~GCollisionTree();

	void Destroy();

	/// 월드의 바운딩을 구합니다. ( load되어있어야만 합니다 )
	const MBox& GetBoundingBox();

	/// zone name 을 이용하여, cl2와 .xml(지형관련정보를 얻기 위해), _dis.dds 를 읽어들인다.
	bool LoadZone(int nZoneID, const wstring& strZoneFileName);
	bool Pick(rs3::RCollisionPickInfo& pickInfo, bool bNoCull = false, DWORD dwAttributeMask = rs3::RCollisionTriangle::ALL);
	RHeightField* GetHeightField();
};

#endif