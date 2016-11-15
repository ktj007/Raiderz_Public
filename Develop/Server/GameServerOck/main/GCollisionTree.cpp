#include "StdAfx.h"
#include "RHeightField.h"
#include "GCollisionTree.h"
#include "MXml.h"
#include "MTime.h"
#include "GZoneInfo.h"
#include <stdlib.h>
#include <string>
#include "MLocale.h"

using namespace rs3;
using namespace std;
//////////////////////////////////////////////////////////////////////////
//

GCollisionTree::GCollisionTree()
: m_pCollisionTree(NULL)
, m_pHeightField(NULL)
, m_nZoneID(-1)
{
	m_aabb.Initialize();
}

GCollisionTree::~GCollisionTree()
{
	Destroy();
}

void GCollisionTree::Destroy()
{
	SAFE_DELETE(m_pCollisionTree);
	SAFE_DELETE(m_pHeightField);
}

const MBox& GCollisionTree::GetBoundingBox()
{
	return m_aabb;
}

bool GCollisionTree::LoadZone(int nZoneID, const wstring& strZoneFileName)
{
	// .cl2 를 읽는다.
	wstring strCl2Filename = strZoneFileName + L".cl2";

	m_pCollisionTree = new RCollisionTree();

	if ( !m_pCollisionTree->Load(MLocale::ConvUTF16ToAnsi(strCl2Filename.c_str()).c_str()) )
	{
		SAFE_DELETE(m_pCollisionTree);
		return false;
	}

	m_aabb = m_pCollisionTree->GetBoundingBox();
	m_nZoneID = nZoneID;

	// .zone.xml 을 읽어서 지형이 있는지 판별
	wstring strXmlFilename = strZoneFileName + L".server.zone.xml";

	GZoneInfo zoneInfo;
	GZoneInfoFileLoader zoneFileLoader;

	if (!zoneFileLoader.Load(strXmlFilename.c_str(), zoneInfo))
	{
		// zone 파일이 없으면 무시
		return true;
	}

	// TERRAIN_SIZE를 Size를 읽어서 m_pHeightField를 생성
	if (zoneInfo.m_vTerrainSize.x != 0.0f && zoneInfo.m_vTerrainSize.y != 0.0f)
	{
		wstring strDisplaceFilename = strZoneFileName + L"_dis.dds";
		m_pHeightField = new RHeightField();
		if ( !m_pHeightField->LoadHeightFieldmapFromR32F(MLocale::ConvUTF16ToAnsi(strDisplaceFilename.c_str()).c_str(), MVector2(zoneInfo.m_vMapSize.x, zoneInfo.m_vMapSize.y)) )
		{
			SAFE_DELETE(m_pHeightField);
			return true;
		}

		m_aabb.Add(m_pHeightField->GetAABB());
	}

	return true;
}

bool GCollisionTree::Pick(rs3::RCollisionPickInfo& pickInfo, bool bNoCull, DWORD dwAttributeMask)
{
	_ASSERT(m_pCollisionTree);

	// .cl2 (RCollisionTree 검사) , HeightField 데이터가 없으면 그 결과 그대로 Return
	bool bResultCl2 = m_pCollisionTree->Pick(pickInfo, dwAttributeMask, bNoCull);
	if ( m_pHeightField == NULL )
		return bResultCl2;

	// HeightField - Picking 실패이면 .cl2 로 결과 그대로
	MVector3 vPickPos, vPickNormal;
	float fHitDistance;
	if ( m_pHeightField->GetPickLine(&vPickPos, &vPickNormal,&fHitDistance, pickInfo.m_inPickOrigin, pickInfo.m_inPickTo) == false )
		return bResultCl2;

	// 거리가 가까운 쪽을 선택해서 리턴한다.
	if ( bResultCl2 == true && (pickInfo.m_outDistanceSq < fHitDistance*fHitDistance) )
		return bResultCl2;

	pickInfo.m_outDistanceSq = fHitDistance;
	pickInfo.m_outNormal = vPickNormal;
	pickInfo.m_outPosition = vPickPos;
	
	return true;
}

RHeightField *GCollisionTree::GetHeightField()
{
	return m_pHeightField;
}