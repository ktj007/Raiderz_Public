#include "stdafx.h"
#include "XZone.h"
#include "XCollisionTree.h"
#include "XTextureMtInfo.h"

#include "XPhysX.h"
#include "MPhysX4R3/MPhysX4R3.h"

XZone::XZone() : m_nLastCollisionTree_ZoneID(0), m_nLastPhysX_ZoneID(0), m_pCollisionTree(NULL), m_pPhysXStatic(NULL), m_bPickZPartialArea(false)
{
	
}

XZone::~XZone()
{
	Clear();
}

void XZone::Load( XFieldInfo* pFieldInfo, RTerrain* pTerrain )
{
	if (m_nLastCollisionTree_ZoneID != 0 && pFieldInfo->m_nZoneID == m_nLastCollisionTree_ZoneID && m_pCollisionTree != NULL)
	{
		m_pCollisionTree->SetTerrain(pTerrain);
		return;
	}
	m_nLastCollisionTree_ZoneID = pFieldInfo->m_nZoneID;

	ClearCollisionTree();

	m_pCollisionTree = new XCollisionTree();

	wstring strCollisionPath( PATH_ZONE + pFieldInfo->m_strZoneFile + L"/" + pFieldInfo->m_strZoneFile + FILEEXT_ZONE_COLLISION );

	m_pCollisionTree->Load(MLocale::ConvUTF16ToAnsi(strCollisionPath.c_str()).c_str());
	m_pCollisionTree->SetTerrain(pTerrain);

	wstring strPickZPath( PATH_ZONE + pFieldInfo->m_strZoneFile + L"/" + pFieldInfo->m_strZoneFile + FILEEXT_ZONE_PICKZ );
	LoadPickingZSize(strPickZPath);
}

void XZone::InitPxStaticBody( XFieldInfo* pFieldInfo, RTerrain* pTerrain, vec3 vMyPos )
{
	//if (m_nLastPhysX_ZoneID != 0 && pFieldInfo->m_nZoneID == m_nLastCollisionTree_ZoneID)
	if (m_nLastPhysX_ZoneID != 0 && pFieldInfo->m_nZoneID == m_nLastPhysX_ZoneID)
	{
		return;
	}

	m_nLastPhysX_ZoneID = pFieldInfo->m_nZoneID;

	ClearPhysX();

	global.phx->SetSceneGravity( GRAVITY * 1.5f );
	m_pPhysXStatic = global.phx->CreatePxObject<physx::MPxObjStatic>();

	// static body builder
	RPxObjStaticSubActorBuilder rs3MeshBuilder;

	// 피직스 충돌체를 지형으로부터 만들기
	if (NULL != pTerrain)
	{
		_ASSERT( (RRS_FILLED == pTerrain->GetState()) || (RRS_LOADED == pTerrain->GetState()) );

		// 지형분활 수 및 사이즈 계산
		int nDivisionCount = 64;
		float fTerrainWidthSize = pTerrain->GetWidthSize();
		float fXCount = fTerrainWidthSize / pTerrain->GetXScale();
		float fYCount = fTerrainWidthSize / pTerrain->GetYScale();
		float fMinCount = fXCount;
		if(fXCount > fYCount)
			fMinCount = fYCount;

		if(nDivisionCount > fMinCount)
		{
			nDivisionCount = (int)fMinCount;
		}

		rs3MeshBuilder.SetTerrain( pTerrain, nDivisionCount );
	}

	// 피직스 충돌체를 collision tree로부터 만들기
	if (NULL != GetCollisionTree())
	{
		bool bFromCollisionTree = false;
		const RCollisionTreeNode* pCollisionRoot = GetCollisionTree()->GetRootNode();
		if (pCollisionRoot)
		{
			if (pCollisionRoot->GetLeftNode())
				bFromCollisionTree = true;

			if (pCollisionRoot->GetRightNode())
				bFromCollisionTree = true;
		}

		if (bFromCollisionTree)
		{
			// 주변 10미터를 하나의 섹터 단위로 한다.
			rs3MeshBuilder.SetCollisionTreeRootNode(GetCollisionTree()->GetRootNode(), 1000.f);
		}
	}

	// 피직스 스태틱 바디 빌드
	m_pPhysXStatic->AddStaticSubActorsFromBuilder(rs3MeshBuilder);

	// 현재 위치에서 지연생성을 하면 랙에 쩔수 있으니까 한번 활성화 해준다.
// 	MBox vCurrentBox( vMyPos - 5000.f, vMyPos + 5000.f );
// 	std::vector< const MBox* > vList;
// 	vList.push_back(&vCurrentBox);
// 	m_pPhysXStatic->ActivateStaticSubActorsByAreaList(vList);
}


void XZone::Clear()
{
	ClearCollisionTree();
	ClearPhysX();

	m_nLastCollisionTree_ZoneID = 0;
	m_nLastPhysX_ZoneID = 0;
}

void XZone::ClearCollisionTree()
{
	if (m_pCollisionTree)
	{
		m_pCollisionTree->Destroy();
		SAFE_DELETE(m_pCollisionTree);
	}
}

void XZone::ClearPhysX()
{
	//PhysX
	if (m_pPhysXStatic)
	{
		global.phx->RemovePxObject(m_pPhysXStatic);
		m_pPhysXStatic = NULL;
	}
}

float XZone::GetGroundZ( vec3& vPos, float fPickAddOriginZ /*= 10.0f*/ )
{
	const float PICK_MIN_Z = TERRAIN_HEIGHT_COLLISION_VALUE;
	const float PICK_MAX_Z = TERRAIN_HEIGHT_COLLISION_VALUE * -1.0f;
	RCollisionPickInfo pick_info;
	pick_info.m_inPickOrigin = vPos+RVector(0,0,fPickAddOriginZ);
	pick_info.m_inPickTo = vPos+RVector(0,0,PICK_MIN_Z);

	if (m_pCollisionTree)
	{
		if ( m_pCollisionTree->Pick(pick_info, ~RCollisionTriangle::WATER))
		{
			return pick_info.m_outPosition.z;
		}
		else 
		{
			// 밑에 아무것도 없다. 그럼 위쪽으로
			RCollisionPickInfo pick_info_Up;
			pick_info_Up.m_inPickOrigin = vPos+RVector(0,0,-fPickAddOriginZ);
			pick_info_Up.m_inPickTo = vPos+RVector(0,0,PICK_MAX_Z);
			if (m_pCollisionTree->Pick(pick_info_Up, ~RCollisionTriangle::WATER))
			{
				return pick_info_Up.m_outPosition.z;
			}
		}
	}

	return vPos.z;
}


bool XZone::PickCollision( vec3 vOrigin, vec3 vTo, vec3* pvOutPos/*=NULL*/, vec3* pvOutNormal/*=NULL*/, DWORD dwAttribute )
{
	if (m_pCollisionTree == NULL) return false;

	RCollisionPickInfo pick_info;
	pick_info.m_inPickOrigin = vOrigin;
	pick_info.m_inPickTo = vTo;

	bool bCheckWater = false;
	if(dwAttribute == RCollisionTriangle::WATER)	bCheckWater = true;
	if (m_pCollisionTree->Pick(pick_info, dwAttribute, bCheckWater))
	{
		if (pvOutPos) *pvOutPos = pick_info.m_outPosition;
		if (pvOutNormal) *pvOutNormal = pick_info.m_outNormal;
		return true;
	}
	return false;
}

wstring XZone::GetMaterialName(vec3 vEntityPosition)
{
	if (m_pCollisionTree == NULL) return NULL;

	const float PICK_MIN_Z = TERRAIN_HEIGHT_COLLISION_VALUE;
	RCollisionPickInfo pick_info;
	pick_info.m_inPickOrigin = vEntityPosition + RVector(0,0,SWIMMING_HEIGHT_VALUE+50.f);
	pick_info.m_inPickTo = vEntityPosition + RVector(0,0,PICK_MIN_Z);

	if (m_pCollisionTree->Pick(pick_info, RCollisionTriangle::MESH | RCollisionTriangle::TERRAIN | RCollisionTriangle::WATER))
	{
		if(pick_info.m_outMaterialName != NULL)
		{
			if ( pick_info.m_outNodeAttr & RCollisionTriangle::TERRAIN )
			{
				return info.textureMt->GetMaterial(MLocale::ConvAnsiToUTF16(pick_info.m_outMaterialName).c_str());
			}
			else if ( pick_info.m_outNodeAttr & RCollisionTriangle::MESH )
			{
				wstring mt = MLocale::ConvAnsiToUTF16(pick_info.m_outMaterialName);

				// TODO : 메터리얼 이름 가장 뒤에 붙는 사운드 설정 파싱
				int nPos = mt.find_last_of(L"_");
				return mt.substr(nPos+1, 3);
			}
			else if ( pick_info.m_outNodeAttr & RCollisionTriangle::WATER)
			{
				// 물 위쪽에 있는지... 아래에 있는지 판단
				if(pick_info.m_outPosition.z >= vEntityPosition.z)
					return L"wat";
				
				return L"";
			}
		}
	}

	return L"pnt";
}

rs3::RMatrix XZone::GetTerrainMatrix( const vec3& vPos, const vec3& vDir, const vec3& vUp, bool bUseAlign )
{
	vec3 vNewPos = vPos;
	vec3 vNewDir = vDir;
	vec3 vAlign = vec3::AXISZ;

	float fGroundZ = GetGroundZ(vNewPos);

	vec3 vStart		= vPos;
	vStart.z		= fGroundZ;
	vStart			+= vec3(0, 0, 200.0f);
	vec3 vTo		= vStart + vec3(0, 0, -10000.0f);
	vec3 vOutPos	= vPos;
	vec3 vOutNor	= vAlign;

	if(PickCollision(vStart, vTo, &vOutPos, &vOutNor))
	{
		vNewPos.Set(vOutPos);
		vAlign.Set(vOutNor);

		vec3 vTempDir		= vDir;
		vec3 vTempCross		= vTempDir.CrossProduct(vOutNor);
		vNewDir				= vOutNor.CrossProduct(vTempCross);
	}

	if(bUseAlign == false)
	{
		vAlign = vUp;
		vNewDir = vDir;
	}

	// 매트릭스 계산
	RMatrix matWorld;
	matWorld.SetLocalMatrix(vNewPos, vNewDir, vAlign);

	return matWorld;
}

void XZone::LoadPickingZSize(wstring& strFilename)
{
	MXml kXml;
	if ( !kXml.LoadFile(MLocale::ConvUTF16ToAnsi(strFilename.c_str()).c_str()))
		return;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("AREA");
	if (pRootElement == NULL)
		return;

	string strText;
	if ( _Contents(strText, pRootElement))
	{
		sscanf(strText.c_str(),"%d %d %d %d %d",
			&m_bPickZPartialArea, &m_rtPickZPartialArea.left, &m_rtPickZPartialArea.top, &m_rtPickZPartialArea.right, &m_rtPickZPartialArea.bottom);
	}
}
