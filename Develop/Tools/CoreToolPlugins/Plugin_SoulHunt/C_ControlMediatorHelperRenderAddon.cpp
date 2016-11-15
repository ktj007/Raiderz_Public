#include "StdAfx.h"

#include "C_ControlMediatorHelperRenderAddon.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlPreviewSceneManager.h"

#include "M_ToolCmdMgr.h"
#include "M_CommandBuffer.h"
#include "M_ToolLogicObjectManager.h"

#include "C_XMLEditInfoFile.h"
#include "C_XMLUserConfig.h"
#include "RFont.h"

#include "RSUtil.h"
#include "RRenderHelper.h"
#include "RRenderHelper2.h"

#include "RMeshUtil.h"
#include "RUtil.h"

#include "VL_SceneNodeList.h"

#include "VR_TerrainGrass.h"
#include "VR_EntityObjectList.h"
#include "VR_EntityMaterial.h"
#include "VR_EtcPathFind.h"

#include "M_PathFinder.h"
#include "M_CollisionTree.h"

#include "C_ControlCursor.h"
#include "C_MintManager.h"

#include "RActor.h"

#include "REffectSceneNode.h"
#include "M_ToolStaticMeshSceneNode.h"

#include "V_MFCHelper.h"
#include "VD_DlgProfile.h"
#include "VD_DlgClipPlaneHeight.h"

#include "WS_WorkSpaceManager.h"

#include "RSpotLightSceneNode.h"

#include <algorithm>

#include "DLL_Interface.h"

#include "../../../Game/XSceneNodeEnvModifier.h"

#define SOUND_ICON_FILENAME		"sensor_sound.png"
#define SENSOR_ICON_FILENAME	"sensor.png"

CControlMediatorHelperObjectRendererImpl::CControlMediatorHelperObjectRendererImpl()
: m_mat_PI_RotForCorn_(
1,  0,  0,  0,
0, -1,  0,  0,
0,  0, -1,  0,
0,  0,  0,  1 )
{
	m_pSoundIcon = NULL;
	m_pSensorIcon = NULL;

	m_matTranslation_.MakeIdentity();
}

void CControlMediatorHelperObjectRendererImpl::OnInitDevice()
{
	// icon 생성 ( from resource )
	_ASSERT(m_pSoundIcon==NULL);

	HINSTANCE hInstResourceDll = AfxGetResourceHandle();
	HINSTANCE hInstResourceClient = (HINSTANCE) ::GetModuleHandle(CORETOOL_DLL_NAME);

	AfxSetResourceHandle(hInstResourceClient);
	m_pSoundIcon = m_pDeviceD3D_->CreateTexture(SOUND_ICON_FILENAME);
	m_pSensorIcon = m_pDeviceD3D_->CreateTexture(SENSOR_ICON_FILENAME);
	AfxSetResourceHandle(hInstResourceDll);

	_ASSERT(m_pSoundIcon);
}

void CControlMediatorHelperObjectRendererImpl::OnDestroyDevice()
{
	if ( m_pSoundIcon != NULL )
	{
		m_pDeviceD3D_->DeleteTexture(m_pSoundIcon);
		m_pSoundIcon = NULL;
	}

	if(m_pSensorIcon != NULL)
	{
		m_pDeviceD3D_->DeleteTexture(m_pSensorIcon);
		m_pSensorIcon = NULL;
	}
}

void CControlMediatorHelperObjectRendererImpl::OnRenderNavigationMesh()
{
	// 네비게이션 메쉬
	CRollupEtcPathFind* pPathFinder = m_pControlMediator_->GetViewPtr<CRollupEtcPathFind>();
	if( pPathFinder->m_btnCheckNaviActivity.GetCheck() )
	{
		m_pDeviceD3D_->SetDepthEnable(false);

		// 네비메쉬
		g_PathfinderTest.RenderNavMesh(m_pDeviceD3D_);
		// 찾은 길 렌더링
		g_PathfinderTest.RenderCurrentPath(m_pDeviceD3D_);

		// 렌더 스테이트 리셋
		RRenderHelper::SetRenderState();

		// 시작점 - 끝점 도우미
		int nUnitRadius = g_PathfinderTest.m_PathFinder.GetShapeRange("small");

		RBoundingBox aabb;
		aabb.vmin = RVector(-50, -50, 0);
		aabb.vmax = RVector( 50, 50, 100);

		RVector vPos;
		pPathFinder->GetStartPosition(vPos);
		m_matTranslation_.SetTranslation(vPos);
		RRenderHelper::RenderCylinder(m_matTranslation_, 1, 150, 0xff000000);
		RRenderHelper::RenderCylinder(m_matTranslation_, (float)nUnitRadius, 100);
		RRenderHelper::RenderBox(m_matTranslation_, aabb);

		pPathFinder->GetEndPosition(vPos);
		m_matTranslation_.SetTranslation(vPos);
		RRenderHelper::RenderCylinder(m_matTranslation_, 1, 150, 0xffffffff);
		RRenderHelper::RenderCylinder(m_matTranslation_, (float)nUnitRadius, 100);
		RRenderHelper::RenderBox(m_matTranslation_, aabb);

		m_pDeviceD3D_->SetDepthEnable(true);
		//Rendering PathFinder Seed
		pPathFinder->RenderSeed();

		RRenderHelper::EndRenderState();
	}
}

void CControlMediatorHelperObjectRendererImpl::RenderHelper_Spawn()
{
	CToolLogicObjectManager* pLogicObjectMgr = m_pControlMediator_->GetToolLogicObjectMgr();
	CRollupEntityObjectList* pRollupEntityObjectList = m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>();

	ENTITY_OBJECTLIST* pSpawnList = pLogicObjectMgr->GetEntityObjectList<CEntityObject>( CEntityObject::ENTITY_SPAWN );
	if (!pSpawnList)
		return;

	RVector vPos;
	RVector vDir;
	const DWORD INVAILD_COLOR = D3DCOLOR_XRGB(0, 0, 0);

	RRenderHelper::SetRenderState();
	for(ENTITY_OBJECTLIST::iterator itr = pSpawnList->begin(); itr != pSpawnList->end(); ++itr)
	{
		CEntitySpawn* pEntitySpawn = (CEntitySpawn*)(*itr);

		if( pEntitySpawn->GetVisible() == false)
			continue;

		// Object 타입은 핸들러 렌더링 안해주기
		if (pEntitySpawn->IsMapObject() && pRollupEntityObjectList->IsViewMapObjHandler() == false )
			continue;

		// 핸들러 보여주기로 설정되어 있다면, 핸들러 보여주기
		if (pRollupEntityObjectList->IsHideSpawnHandler() == false)
		{
			pEntitySpawn->GetPosition(vPos);
			vPos.z += pEntitySpawn->GetHeightWithNpc();
			m_matTranslation_.SetTranslation(vPos);

			DWORD dwColor = (pEntitySpawn->GetValid() ? pEntitySpawn->GetColor() : INVAILD_COLOR);
			RRenderHelper::RenderCorn( m_mat_PI_RotForCorn_ * m_matTranslation_, CEntityObject::SPAWN_HEIGHT, CEntityObject::SPAWN_HEIGHT
				, dwColor );

			if(pEntitySpawn->IsRandomDir() == false)
			{
				vPos.z -= CEntityObject::SPAWN_HEIGHT;
				pEntitySpawn->GetDirection(vDir);
				RRenderHelper::RenderArrow(vPos, vDir, 10, CEntityObject::DIRECTION_DISTANCE, 20, 50, 0xff000000);
			}
		}
	}
	RRenderHelper::EndRenderState();
}

void CControlMediatorHelperObjectRendererImpl::RenderHelper_Marker()
{
	RVector vPos;
	RVector vDir;
	const DWORD INVAILD_COLOR = D3DCOLOR_XRGB(0, 0, 0);
	CToolLogicObjectManager* pLogicObjectMgr = m_pControlMediator_->GetToolLogicObjectMgr();

	ENTITY_OBJECTLIST* pMarkerList = pLogicObjectMgr->GetEntityObjectList<CEntityObject>( CEntityObject::ENTITY_MARKER );
	if (!pMarkerList)
		return;

	RRenderHelper::SetRenderState();

	//Marker
	for(ENTITY_OBJECTLIST::iterator itr = pMarkerList->begin(); itr != pMarkerList->end(); ++itr)
	{
		CEntityMarker* pEntityMarker = (CEntityMarker*)(*itr);
		if(pEntityMarker->GetVisible() == false) continue;

		// position, 거꾸로된 화살표
		pEntityMarker->GetPosition(vPos);
		vPos.z = vPos.z + (CEntityObject::MARKER_HEIGHT*2);
		static const RVector vMarkerDir(0,0,-1);
		RRenderHelper::RenderArrow( vPos, vMarkerDir, 40, 100, 60, 40, pEntityMarker->GetValid() ? 0xff00ff00 : INVAILD_COLOR);

		// direction
		pEntityMarker->GetDirection(vDir);
		vPos.z = vPos.z - (CEntityObject::MARKER_HEIGHT*0.5f);
		RRenderHelper::RenderArrow(vPos, vDir, 10, CEntityObject::DIRECTION_DISTANCE, 20, 50, 0xff000000);
	}
	RRenderHelper::EndRenderState();
}

void CControlMediatorHelperObjectRendererImpl::RenderHelper_Sensor()
{
	float fRadius;
	const DWORD INVAILD_COLOR = D3DCOLOR_XRGB(0, 0, 0);
	CToolLogicObjectManager* pLogicObjectMgr = m_pControlMediator_->GetToolLogicObjectMgr();

	ENTITY_OBJECTLIST* pSensorList = pLogicObjectMgr->GetEntityObjectList<CEntityObject>( CEntityObject::ENTITY_SENSOR );
	if (!pSensorList)
		return;

	vector<Draw_Icon_Data> positions;
	positions.reserve(pSensorList->size());

	RRenderHelper::SetRenderState();
	//Sensor
	for(ENTITY_OBJECTLIST::iterator itr = pSensorList->begin(); itr != pSensorList->end(); ++itr)
	{
		CEntitySensor* pEntitySensor = (CEntitySensor*)(*itr);
		if(pEntitySensor->GetVisible() == false)
			continue;

		// 구
		pEntitySensor->GetSensorRenderPos(m_matTranslation_);
		pEntitySensor->GetScale(fRadius);
		RRenderHelper::RenderSphere(m_matTranslation_, fRadius, pEntitySensor->GetValid() ? 0xff0000ff : INVAILD_COLOR);

		// 아이콘
		RVector3 pos;
		if ( pEntitySensor->GetPosition(pos))
		{
			float fSize = 0.0f;
			float fOffset = 0.0f;
			pEntitySensor->UpdatePickingForSelection(fSize, fOffset);

			pos.z += fSize + fOffset;

			Draw_Icon_Data iconData;
			iconData.position = pos;
			iconData.fSize = fSize;

			positions.push_back(iconData);
		}
	}

	if ( !positions.empty())
	{
		m_pDeviceD3D_->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		Render_DrawIcon(m_pDeviceD3D_, positions, m_pSensorIcon, D3DCOLOR_XRGB(255, 255, 255));
		m_pDeviceD3D_->SetBlending(RBLEND_NONE, RBLEND_NONE);
	}

	RRenderHelper::EndRenderState();
}

void CControlMediatorHelperObjectRendererImpl::RenderHelper_Sound()
{
	const int ICON_SIZE = 300;
	CToolLogicObjectManager* pLogicObjectMgr = m_pControlMediator_->GetToolLogicObjectMgr();

	ENTITY_OBJECTLIST* pSoundList = pLogicObjectMgr->GetEntityObjectList<CEntityObject>( CEntityObject::ENTITY_SOUND );
	if (!pSoundList)
		return;

	vector<Draw_Icon_Data> positions;
	positions.reserve(pSoundList->size());

	RRenderHelper::SetRenderState();

	//Sound Entity Render
	float fSize = ICON_SIZE;
	for(ENTITY_OBJECTLIST::iterator itr = pSoundList->begin(); itr != pSoundList->end(); ++itr)
	{
		CEntitySound* pEntitySound = (CEntitySound*)(*itr);
		if(pEntitySound->GetVisible() == false) 
			continue;

		RVector3 pos;
		if ( pEntitySound->GetPosition(pos))
		{
			float fSize = 0.0f;
			float fOffset = 0.0f;
			pEntitySound->UpdatePickingForSelection(fSize, fOffset);

			pos.z += fSize + fOffset;

			Draw_Icon_Data iconData;
			iconData.position = pos;
			iconData.fSize = fSize;

			positions.push_back(iconData);
		}
	}

	if ( !positions.empty())
	{
		m_pDeviceD3D_->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		Render_DrawIcon(m_pDeviceD3D_, positions, m_pSoundIcon, D3DCOLOR_XRGB(255, 255, 255));
		//RenderHelper::DrawIcon3Ds(m_pDeviceD3D_,&positions[0],(int)positions.size(), (float)fSize, (float)fSize, m_pSoundIcon, D3DCOLOR_XRGB(255, 255, 255));
		m_pDeviceD3D_->SetBlending(RBLEND_NONE, RBLEND_NONE);
	}
	RRenderHelper::EndRenderState();
}

void CControlMediatorHelperObjectRendererImpl::RenderHelper_Area()
{
	CToolLogicObjectManager* pLogicObjectMgr = m_pControlMediator_->GetToolLogicObjectMgr();
	ENTITY_OBJECTLIST* pAreaList = pLogicObjectMgr->GetEntityObjectList<CEntityObject>( CEntityObject::ENTITY_AREA );
	if (!pAreaList)
		return;

	float fRadius(10.f), fHeight(10);
	RRenderHelper::SetRenderState();
	for(ENTITY_OBJECTLIST::iterator itr = pAreaList->begin(); itr != pAreaList->end(); ++itr)
	{
		CEntityArea* pEntityArea = (CEntityArea*)(*itr);
		if(pEntityArea->GetVisible() == false) 
			continue;

		SElementAtXML* pRootElement = pEntityArea->GetRootElement();
		for(SElementAtXML::ElementList::iterator itr_element = pRootElement->m_vecChildElement.begin();
			itr_element != pRootElement->m_vecChildElement.end(); ++itr_element)
		{
			if( (*itr_element)->m_strNodeName == "CYLINDER" )
			{
				SElementAtXML* pCylinderElement = (*itr_element);
				for(SElementAtXML::AttributeList::iterator itr_attr = pCylinderElement->m_vecAttribute.begin();
					itr_attr != pCylinderElement->m_vecAttribute.end(); ++itr_attr)
				{
					SAttributeAtXML* pAttribute = (*itr_attr);
					if (pAttribute->m_strAttrName == "Position")
					{
						m_matTranslation_._41 = (float)(pAttribute->m_vGenericNum[0]);
						m_matTranslation_._42 = (float)(pAttribute->m_vGenericNum[1]);
						m_matTranslation_._43 = (float)(pAttribute->m_vGenericNum[2]);
					}
					else if (pAttribute->m_strAttrName == "radius")
					{
						fRadius = (float)(pAttribute->m_vGenericNum[0]);
					}
					else if (pAttribute->m_strAttrName == "height")
					{
						fHeight = (float)(pAttribute->m_vGenericNum[0]);
					}
				}
			}

			RRenderHelper::RenderCylinder(m_matTranslation_, (float)fRadius, fHeight, 0x77ffffff);
		}
	}

	RRenderHelper::EndRenderState();
}


void CControlMediatorHelperObjectRendererImpl::RenderHelper_Room()
{
	RVector vPos;
	RVector vDir;
	const DWORD INVAILD_COLOR = D3DCOLOR_XRGB(0, 0, 0);
	CToolLogicObjectManager* pLogicObjectMgr = m_pControlMediator_->GetToolLogicObjectMgr();

	ENTITY_OBJECTLIST* pMarkerList = pLogicObjectMgr->GetEntityObjectList<CEntityRoom>( CEntityObject::ENTITY_ROOM);
	if (!pMarkerList)
		return;

	RRenderHelper::SetRenderState();

	// Room
	for(ENTITY_OBJECTLIST::iterator itr = pMarkerList->begin(); itr != pMarkerList->end(); ++itr)
	{
		CEntityRoom* pRoom = (CEntityRoom*)(*itr);
		if(pRoom->GetVisible() == false) continue;

		// position, 거꾸로된 화살표
		pRoom->GetPosition(vPos);
		vPos.z = vPos.z + (CEntityObject::MARKER_HEIGHT*2);
		static const RVector vMarkerDir(0,0,-1);
		RRenderHelper::RenderArrow( vPos, vMarkerDir, 40, 100, 60, 40, pRoom->GetValid() ? 0xff00ff00 : INVAILD_COLOR);
		RRenderHelper::RenderObb(pRoom->GetOBB());
	}
	RRenderHelper::EndRenderState();
}

void CControlMediatorHelperObjectRendererImpl::OnRenderSelection()
{
	ENTITY_OBJECTLIST* pListSelectedEntityObject = m_pControlMediator_->GetHelperAction()->GetSelectedEntityObject();
	rs3::RMatrix viewMat(m_pDeviceD3D_->GetTransform(RST_VIEW));
	rs3::RVector vViewPos;

	RenderHelper::RSphere3D sphere(1, 7);

	// selected Entity Object ( Spawn, Maker, Sensor, Sound )
	rs3::RVector vPos, vUp;
	for(ENTITY_OBJECTLIST::iterator itr = pListSelectedEntityObject->begin();
		itr != pListSelectedEntityObject->end(); ++itr)
	{
		CEntityObject* pEntityObject = *itr;
		if( pEntityObject->GetVisible() == false) continue;  //그냥 스킵

		pEntityObject->GetPosition(vPos);
		viewMat.TransformVect(vPos, vViewPos);
		m_matTranslation_.SetTranslation(vPos);

		float fFactor = fabs(vViewPos.z) * 0.3f;
		float fSize = fFactor;
		float fCylRad = fFactor * 0.003f;
		float fCornRad = fFactor * 0.01f;
		float fCornLen = fFactor * 0.015f;

		if(pEntityObject->GetType() == CEntityObject::ENTITY_SPAWN)
		{
			CEntitySpawn* pEntitySpawn = static_cast<CEntitySpawn*>(pEntityObject);

			float fRadius;
			pEntityObject->GetScale(fRadius);
			RRenderHelper::RenderCylinder(m_matTranslation_, fRadius, CEntityObject::SPAWN_HEIGHT, pEntitySpawn->GetColor());

			if ( pEntitySpawn )
			{
				float fRoamRadius = 0.f;
				float fRoamDepth = 0.f;

				pEntitySpawn->GetRoamRadius(fRoamRadius);
				pEntitySpawn->GetRoamDepth(fRoamDepth);

				RRenderHelper::SetRenderState();
				RRenderHelper::RenderCylinder(m_matTranslation_, fRoamRadius, fRoamDepth, 0xb0808080);
			}

			// combat radius
			if ( pEntitySpawn )
			{
				float fCombatRadius = 0.f;
				const float fCombatDepth = 200.f;

				pEntitySpawn->GetCombatRadius(fCombatRadius);

				RRenderHelper::SetRenderState();
				RRenderHelper::RenderCylinder(m_matTranslation_, fCombatRadius, fCombatDepth, 0x60808040);
			}

			RRenderHelper::RenderAxis(m_matTranslation_, fSize, fCylRad, fCornRad, fCornLen);
		}
		else if (pEntityObject->GetType() == CEntityObject::ENTITY_SOUND)
		{				
			CEntitySound* pEntitySound = static_cast<CEntitySound*>(pEntityObject);

			pEntitySound->GetPosition(vPos);
			m_matTranslation_.SetTranslation(vPos);

			float fMin = 1.f;
			float fMax = 1.f;
			float fVolume = 0.0f;

			pEntitySound->GetDistanceMin(fMin);
			pEntitySound->GetDistanceMax(fMax);
			pEntitySound->GetVolume(fVolume);

			RMatrix matScale;		
			matScale.MakeIdentity();

			RRenderHelper::RenderAxis(m_matTranslation_, 100, 0.5f, 2.0f, 10.f);

			m_pDeviceD3D_->SetDepthEnable(true, false);
			m_pDeviceD3D_->SetBlending(RBLEND_ONE, RBLEND_ONE);
			m_pDeviceD3D_->SetCullMode(RCULL_NONE);

			if ( fabs(fMax-fMin) < 0.01f )
			{
				matScale.SetScale(fMax);
				sphere.Draw(m_pDeviceD3D_, matScale * m_matTranslation_, D3DCOLOR_XRGB(52, 52, 52));
			}
			else
			{
				matScale.SetScale(fMin);
				sphere.Draw(m_pDeviceD3D_, matScale * m_matTranslation_, D3DCOLOR_XRGB(112, 52, 52));

				matScale.SetScale(fMax);
				sphere.Draw(m_pDeviceD3D_, matScale * m_matTranslation_, D3DCOLOR_XRGB(52, 52, 112));
			}

			m_pDeviceD3D_->SetBlending(RBLEND_NONE, RBLEND_NONE);
			m_pDeviceD3D_->SetDepthEnable(true, true);
		}
		else if (pEntityObject->GetType() == CEntityObject::ENTITY_AREA)
		{
			RRenderHelper::RenderBox( RMatrix::IDENTITY, pEntityObject->GetAABB());

			CEntityArea* pEntityArea = static_cast<CEntityArea*>(pEntityObject);
			if (-1 != pEntityArea->m_nSubCylinderSelectedIdx)
			{
				RVector& vCylinderPos = pEntityArea->m_CylinderPos;

				viewMat.TransformVect(vCylinderPos, vViewPos);
				m_matTranslation_.SetTranslation(vCylinderPos);

				fFactor = fabs(vViewPos.z) * 0.3f;
				fSize = fFactor;
				fCylRad = fFactor * 0.003f;
				fCornRad = fFactor * 0.01f;
				fCornLen = fFactor * 0.015f;

				RRenderHelper::RenderAxis(m_matTranslation_, fSize, fCylRad, fCornRad, fCornLen);
			}
		}
		else if (pEntityObject->GetType() == CEntityObject::ENTITY_ROOM)
		{
			CEntityRoom* pEntityArea = static_cast<CEntityRoom*>(pEntityObject);
			pEntityArea->GetPosition(vPos);
			m_matTranslation_.SetTranslation(vPos);
			RRenderHelper::RenderAxis(m_matTranslation_, 200, 0.5f, 2.0f, 10.f);

			//m_pDeviceD3D_->SetDepthEnable(true, false);
			//m_pDeviceD3D_->SetBlending(RBLEND_ONE, RBLEND_ONE);
			//m_pDeviceD3D_->SetCullMode(RCULL_NONE);

			//RMatrix matScale;		
			//matScale.MakeIdentity();
			//matScale.SetScale(.0f);
			//sphere.Draw(m_pDeviceD3D_, matScale * m_matTranslation_, D3DCOLOR_XRGB(52, 52, 52));

			//m_pDeviceD3D_->SetBlending(RBLEND_NONE, RBLEND_NONE);
			//m_pDeviceD3D_->SetDepthEnable(true, true);
		}
		else
		{
			if (pEntityObject->GetUp(vUp))
			{
				RVector3 vDir(RVector3::AXISY);
				pEntityObject->GetDirection(vDir);
				pEntityObject->GetPosition(vPos);

				RBoundingBox box = pEntityObject->GetAABB();
				box.vmax -= vPos;
				box.vmin -= vPos;

				RVector3 vScale(1.f, 1.f, 1.f);
				pEntityObject->GetRenderHelperScale(vScale);
				box.vmax *= vScale;
				box.vmin *= vScale;
				m_matTranslation_.SetLocalMatrix(vPos, vDir, vUp);
				RRenderHelper::RenderBox( m_matTranslation_, box);
				RRenderHelper::RenderAxis(m_matTranslation_, fSize, fCylRad, fCornRad, fCornLen);

				m_matTranslation_.MakeIdentity();
			}
			else
			{
				RRenderHelper::RenderBox( RMatrix::IDENTITY, pEntityObject->GetAABB());
				RRenderHelper::RenderAxis(m_matTranslation_, fSize, fCylRad, fCornRad, fCornLen);
			}
		}
	}
}

void CControlMediatorHelperObjectRendererImpl::OnRenderObjectTextInfo()
{
	CToolLogicObjectManager* pLogicObjectMgr = m_pControlMediator_->GetToolLogicObjectMgr();

	CString cstrEntityObjectInfo;
	ENTITY_OBJECTLIST* pList = NULL;

	pList = pLogicObjectMgr->GetEntityObjectList<CEntityObject>(CEntityObject::ENTITY_SPAWN);
	if (pList)
	{
		cstrEntityObjectInfo.Format("스폰 : %d", (int)(pList->size()) );
		m_pFont_->DrawText_UNICODE(10, 85, MLocale::ConvAnsiToUTF16(cstrEntityObjectInfo).c_str(), -1, 0xffffffff);
	}

	pList = pLogicObjectMgr->GetEntityObjectList<CEntityObject>(CEntityObject::ENTITY_MARKER);
	if (pList)
	{
		cstrEntityObjectInfo.Format("마커 : %d", (int)(pList->size()) );
		m_pFont_->DrawText_UNICODE(110, 85, MLocale::ConvAnsiToUTF16(cstrEntityObjectInfo).c_str(), -1, 0xffffffff);
	}

	pList = pLogicObjectMgr->GetEntityObjectList<CEntityObject>(CEntityObject::ENTITY_SENSOR);
	if (pList)
	{
		cstrEntityObjectInfo.Format("센서 : %d", (int)(pList->size()) );
		m_pFont_->DrawText_UNICODE(210, 85, MLocale::ConvAnsiToUTF16(cstrEntityObjectInfo).c_str(), -1, 0xffffffff);
	}
}

void CControlMediatorHelperObjectRendererImpl::Render_DrawIcon( rs3::RDevice *pDevice, vector<Draw_Icon_Data>& vecPositions, rs3::RTexture *pTexture, DWORD dwColor )
{
	if(vecPositions.empty())
		return;

	int nCount = vecPositions.size();
	
	std::vector<RVt_pos_tex1> vertices;
	std::vector<WORD> indices;

	vertices.resize(4 * nCount);
	indices.resize(6 * nCount);

	for ( int i = 0; i < nCount; ++i )
	{
		int ix = i * 6;
		int vt = i * 4;
		indices[ix++] = vt;
		indices[ix++] = vt + 1;
		indices[ix++] = vt + 2;
		indices[ix++] = vt + 2;
		indices[ix++] = vt + 1;
		indices[ix] = vt + 3;
	}

	int i = 0;
	for(vector<Draw_Icon_Data>::iterator it = vecPositions.begin(); it != vecPositions.end(); ++it)
	{
		RMatrix matView = pDevice->GetTransform(RST_VIEW);

		RVector vBaseXAxis( matView._11, matView._21, matView._31);
		RVector vBaseYAxis( matView._12, matView._22, matView._32);

		float w = it->fSize / 2;
		float h = it->fSize / 2;

		vBaseXAxis.FastNormalize();
		vBaseYAxis.FastNormalize();

		vBaseXAxis *= w;
		vBaseYAxis *= h;

		int ix = i * 4;
		const RVector &vPos = it->position;

		vertices[ix].vPos = vPos - vBaseXAxis + vBaseYAxis;
		vertices[ix].u = 0.0f;	vertices[ix].v = 0.0f;
		++ix;

		vertices[ix].vPos = vPos + vBaseXAxis + vBaseYAxis;
		vertices[ix].u = 1.0f;	vertices[ix].v = 0.0f;
		++ix;

		vertices[ix].vPos = vPos - vBaseXAxis - vBaseYAxis;
		vertices[ix].u = 0.0f;	vertices[ix].v = 1.0f;
		++ix;

		vertices[ix].vPos = vPos + vBaseXAxis - vBaseYAxis;
		vertices[ix].u = 1.0f;	vertices[ix].v = 1.0f;

		++i;
	}

	pDevice->SetTextureStageState(0 ,RTSS_COLORARG2, RTA_TEXTURE);
	pDevice->SetTextureStageState(0, RTSS_ALPHAARG2, RTA_TEXTURE);
	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_MODULATE);
	pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_MODULATE);
	pDevice->SetTextureFactor(dwColor);

	pDevice->SetTransform(RST_WORLD, RMatrix::IDENTITY);
	pDevice->SetFvF(RVt_pos_tex1::FVF);
	pDevice->SetTexture(0, pTexture);
	pDevice->DrawIndexedPrimitiveUP(RPT_TRIANGLELIST, 0, vertices.size(), nCount*2,static_cast<void*>(&indices[0]),static_cast<void*>(&vertices[0]), sizeof(RVt_pos_tex1), RFMT_INDEX16);

	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
}
