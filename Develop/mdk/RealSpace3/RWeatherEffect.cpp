#include "stdafx.h"
#include "RWeatherEffect.h"

#include "RCameraSceneNode.h"
#include "RDeviceD3D.h"
#include "MDebug.h"
#include "RRenderHelper.h"
#include "RDirectionalLightSceneNode.h"
#include "MMath.h"
#include "RSceneSky.h"
#include "RMesh.h"
#include "RMeshMgr.h"

#include "REmitter.h"
#include "REffectSceneNode.h" //for m_pWaterEffect
#include "RStaticMeshSceneNode.h"

#include "RXMLTemplate.h"
#include "RCollisionTree.h"

namespace rs3 {

/**
	비는 LINE으로, 눈은 POINT_SPRITE로 그렸다.
*/

RWeatherEffect::RWeatherEffect() : RSceneNode(RRP_WEATHEREFFECT)
, m_fBaseTime(0.f)
, m_hVertexBuffer(R_NONE)
, m_nVertexNum(0)
, m_hTexture(R_NONE)
, m_pScreenMeshNode(NULL)
, m_ePrimType(RPT_POINTLIST)
, m_nVertexNumPerPrim(1)
{
	// 범위 설정 ( 이것은 제대로 계산해서 할 필요가 있음 )
	m_fRangeX = 2000.f;
	m_fRangeY = 2000.f;
	m_fRangeZ = DEFAULT_RANGE_Z;
	m_fRadius = max(m_fRangeX, m_fRangeY);

	m_vWind = RVector::ZERO;

	m_currentParameter.eWeatherType = WEATHER_NONE;
	m_currentParameter.fSize = 4.5f;
	m_currentParameter.nCreateNum = 0;
	m_currentParameter.fVelocity = 1000.f;
	m_currentParameter.v1stColor = RVector4(1, 1, 1, 1);
	m_currentParameter.v2ndColor = RVector4(1, 1, 1, 1);
	m_currentParameter.f1stLuminosity = 1.f;
	m_currentParameter.f2ndLuminosity = 1.f;
	m_currentParameter.strMeshFilename = "";
	m_currentParameter.fWindForce = 1.0f;

	m_vWind = RVector(1, 2, 0);
	m_vWind.Normalize();

	m_fMass = 1;
	ptAddVertexBuffer = NULL;
	m_bFill = false;

	m_fVisibility = 1.0f;
	m_pWaterEffect = NULL;
	m_vCameraPosition = RVector::ZERO;

#if (_MSC_VER >= 1900)
	SetAABB(RBoundingBox::_INFINITY);
#else
	SetAABB( RBoundingBox::INFINITY );
#endif
}

RWeatherEffect::~RWeatherEffect()
{
	SAFE_DELETE(m_pWaterEffect);

	if ( m_hVertexBuffer != R_NONE )
		REngine::GetDevice().DeleteVertexBuffer(m_hVertexBuffer);

	if ( m_hTexture != R_NONE )
		REngine::GetDevice().DeleteTexture(m_hTexture);

	DeleteMesh();
}

void RWeatherEffect::DeleteMesh()
{
	if ( m_pScreenMeshNode )
	{
		delete m_pScreenMeshNode;
		m_pScreenMeshNode = NULL;
	}
}

void RWeatherEffect::SetParticleCount(int n)
{
	if ( n >= VERTEX_SIZE )		n = VERTEX_SIZE;

	m_currentParameter.nCreateNum = n;
}

void RWeatherEffect::SetVelocity(float v)
{
	m_currentParameter.fVelocity = v;
}

void RWeatherEffect::SetScale(float s)
{
	m_currentParameter.fSize = s;
}

void RWeatherEffect::SetFill(bool b)
{
	m_fBaseTime -= 2.0f;
}

void RWeatherEffect::SetFirstColor(const RVector4 &vColor4)
{
	m_currentParameter.v1stColor = vColor4;
	m_currentParameter.f1stLuminosity = ( vColor4.x * 0.3f + vColor4.y * 0.6f + vColor4.z * 0.1f );
	if ( m_currentParameter.f1stLuminosity > 1.0f )	
		m_currentParameter.f1stLuminosity = 1.0f;
	else if ( m_currentParameter.f1stLuminosity < 0.0f )	
		m_currentParameter.f1stLuminosity = 0.0f;
}

void RWeatherEffect::SetSecondColor(const RVector4 &vColor4)
{
	m_currentParameter.v2ndColor = vColor4;
	m_currentParameter.f2ndLuminosity = ( vColor4.x * 0.3f + vColor4.y * 0.6f + vColor4.z * 0.1f );
	if ( m_currentParameter.f2ndLuminosity > 1.0f )	
		m_currentParameter.f2ndLuminosity = 1.0f;
	else if ( m_currentParameter.f2ndLuminosity < 0.0f )	
		m_currentParameter.f2ndLuminosity = 0.0f;
}

void RWeatherEffect::SetWindForce(float f)
{
	m_currentParameter.fWindForce = f;
}

void RWeatherEffect::SetValue(float f)
{
	m_currentParameter.fValue = f;
}

// cl2 에서의 충돌 높이값(Z)을 구한다.
float RWeatherEffect::PickToCollisionTree(RVector vPos, RVector vDir, RCollisionTree* pCollisionTree)
{
	RVector3 vPickPos = vPos + (-vDir) * PICKINGRAY_DISTANCE;
	RVector3 vPickTo  = vPos + vDir * PICKINGRAY_DISTANCE;

	// 비나 눈의 좌표는 월드를 제외한 VIEW 에서의 좌표이다.
	// cl2 는 월드 기준의 좌표이므로 월드로 변환해서 pick을 한 후 다시 VIEW 좌표로 돌려준다
	RCollisionPickInfo pick_info;
	pick_info.m_inPickOrigin	= vPickPos + REngine::GetSceneManager().GetPrimaryCamera()->GetPosition();
	pick_info.m_inPickTo		= vPickTo  + REngine::GetSceneManager().GetPrimaryCamera()->GetPosition();

	// heavy rain 기준 - 0.44ms 정도 걸림
	//if(dwAttribute == RCollisionTriangle::WATER)	bCheckWater = true;
	bool bCheckWater = true;
	DWORD dwAttribute = RCollisionTriangle::ALL;
	if (pCollisionTree && pCollisionTree->Pick(pick_info, dwAttribute, bCheckWater))
	{
		return pick_info.m_outPosition.z - REngine::GetSceneManager().GetPrimaryCamera()->GetPosition().z;
	}

	return -DEFAULT_RANGE_Z;
}

void RWeatherEffect::UpdateParticle(const RVector &vOffset, float fTime, RCollisionTree* pCollisionTree)
{
	float fDeltaTime = fTime - m_fBaseTime;
	m_fBaseTime += fDeltaTime;

	//float fHeight = m_fRangeZ*2.f;
	const float DELTA_LIMIT = 1.f;

	bool bFill = m_bFill;
	if ( fDeltaTime > DELTA_LIMIT ) 
	{
		fDeltaTime = DELTA_LIMIT;
		m_fBaseTime = fTime;
		bFill = true;
	}

	// Create
	int nNum = m_currentParameter.nCreateNum * fDeltaTime;
	//dlog("Time : %f, Base : %f\n", fTime, m_fBaseTime);

	for ( int i = 0; i<nNum; ++i )
	{
		RWParticle *ptr = m_poolParticles.getAddPtr();
		if ( ptr == NULL )
			break;

		RVector3 &vPos = ptr->vPos;

		RVector vRandomVector;
		REmitter::REmitterRenderer::GetRandomVector(&vRandomVector);

		RVector vDirection = -RVector::AXISZ + m_vWind*RandomNumber(0.4f, 0.9f)*m_currentParameter.fWindForce;
		vDirection.Normalize();
		ptr->vDir = vDirection;
		ptr->fVelocity = RandomNumber(0.64f, 1.0f);

		vPos.x = RandomNumber(-m_fRangeX, m_fRangeX);
		vPos.y = RandomNumber(-m_fRangeY, m_fRangeY);
		vPos.z = ( bFill ? RandomNumber(-m_fRangeZ, m_fRangeZ) : RandomNumber(0.9f, 1.0f) * m_fRangeZ );
		ptr->fRand = (float)::rand()/(float)RAND_MAX;
		ptr->fRangeZ = -m_fRangeZ;

		// cl2충돌 지점을 구한다.
		ptr->fRangeZ = PickToCollisionTree(vPos, ptr->vDir, pCollisionTree);
	}

	// Update
	const float &fVelocity = m_currentParameter.fVelocity;
	//float fVariation = 0.5f;

	for ( unsigned int ui = 0; ui<m_poolParticles.count(); )
	{
		RWParticle *ptr = m_poolParticles.getPtr(ui);
		
		//RVector vRandomVector;
		//REmitter::REmitterRenderer::GetRandomVector(&vRandomVector);

		RVector3 vVel = ptr->vDir*ptr->fVelocity * fVelocity * m_fMass;// - (vRandomVector*fVariation*2.f);
		RVector3 vPos = ptr->vPos + vVel * fDeltaTime;

		// 파티클이 카메라를 따라오지 않게 하는 구문인데 현재 사용안함.
		// 즉, 현재는 밖에 있다가 집안으로 들어가면 내리던 비가 잠깐 내리는 현상이 있음
		// 이것을 사용하지 않는 이유는 먼거리를 갑자기 이동할 경우 파티클이 다시 처음부터 생성되는데
		// 그 시점에 날씨가 안나오는것 처럼 보이기 때문에 사용하지 않는다.
		//vPos.x += vOffset.x; 
		//vPos.y += vOffset.y;
		//vPos.z += vOffset.z;

		if ( vPos.z < ptr->fRangeZ )
		{
			m_poolParticles.erase(ui);
			continue;
		}

		bool bRePick = false;
		if ( vPos.x < -m_fRangeX ) 
		{
			vPos.x += m_fRangeX*2;
			bRePick = true;
		}
		else if ( vPos.x > m_fRangeX )	
		{
			vPos.x -= m_fRangeX*2;
			bRePick = true;
		}
		if ( vPos.y < -m_fRangeY )
		{
			vPos.y += m_fRangeY*2;
			bRePick = true;
		}
		else if ( vPos.y > m_fRangeY )
		{
			vPos.y -= m_fRangeY*2;
			bRePick = true;
		}

		if( bRePick )	// 위치가 변경 되었으므로 다시 충돌 지점을 구한다.
		{
			ptr->fRangeZ = PickToCollisionTree(vPos, ptr->vDir, pCollisionTree); 

		}

		ptr->vPos = vPos;
	
		++ui;
	}
}

void RWeatherEffect::LoadTexture(const char *filename)
{
	if ( REngine::GetDevicePtr() == NULL )	return;
	if ( m_hTexture != R_NONE )
		REngine::GetDevice().DeleteTexture(m_hTexture);

	m_hTexture = REngine::GetDevice().CreateTexture(filename, RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE);
}

void RWeatherEffect::LoadMesh(const char *filename)
{
	DeleteMesh();
	if (filename == NULL || filename[0] == 0)
		return;

	m_pScreenMeshNode = new RSceneSky( NULL );

	bool bResult = m_pScreenMeshNode->Create( filename );
	if ( bResult == false )
	{
		SAFE_DELETE(m_pScreenMeshNode);
	}
}

void RWeatherEffect::SetWeatherType(WEATHER_TYPE eType)
{
	m_currentParameter.eWeatherType = eType;
}

bool RWeatherEffect::Create()
{
	_ASSERT(REngine::GetDevicePtr());

	if ( m_hVertexBuffer != R_NONE )
	{
		REngine::GetDevice().DeleteVertexBuffer(m_hVertexBuffer);
		m_hVertexBuffer = R_NONE;
	}

	m_nVertexNum = 0;
	int nSize = VERTEX_SIZE;

	const char *SNOW_TEXTURE = "snow.tga";
	const char *RAIN_TEXTURE = "rain.tga";

	SAFE_DELETE(m_pWaterEffect);

	switch ( m_currentParameter.eWeatherType )
	{
	case WEATHER_NONE:
		// WEATHER_NONE 일 때도 아무것도 하지 않는다.
		return true;
	case WEATHER_SNOW:
		m_nVertexNumPerPrim = 1;	// 1 프리미티브에 해당하는 버텍스 수
		nSize *= 1;					// 1 개의 파티클에 해당하는 프리미티브 수
		m_fMass = 1.0f;				// 질량
		m_ePrimType = RPT_POINTLIST;// 렌더링 프리미티브 
		ptAddVertexBuffer = &RWeatherEffect::AddVertexBufferSnow;	// 생성 함수 포인터
		LoadTexture(SNOW_TEXTURE);	// 텍스쳐를 로드한다.
		break;
	case WEATHER_RAIN:
		m_nVertexNumPerPrim = 2;
		nSize *= 2;
		m_fMass = 20.0f;
		m_ePrimType = RPT_LINELIST;
		ptAddVertexBuffer = &RWeatherEffect::AddVertexBufferRain;
		LoadTexture(RAIN_TEXTURE);
		break;
	default:
		return false;
	}

	// dynamic vertex buffer
	m_hVertexBuffer = REngine::GetDevice().CreateVertexBuffer(nSize*sizeof(PVertexType), sizeof(PVertexType), NULL, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);
	_ASSERT(m_hVertexBuffer!=R_NONE);
	
	PVertexType *pVertex = (PVertexType*)REngine::GetDevice().LockVertexBuffer(m_hVertexBuffer, 0);
	if ( pVertex )
	{
		memset(pVertex, 0x00, sizeof(PVertexType)*nSize);
		REngine::GetDevice().UnlockVertexBuffer(m_hVertexBuffer);
	}

	m_poolParticles.resize(VERTEX_SIZE);
	m_poolParticles.reset();
	
	m_fBaseTime = 0.0f;
	//LoadMesh(m_currentParameter.strMeshFilename.c_str());

	// effect 

	return true;
}

void RWeatherEffect::OnAdd()
{
//	mlog("RWeatherEffect OnAdd\n");
	if ( m_pScreenMeshNode )
		m_pScreenMeshNode->Play();
	
	if ( m_pWaterEffect )
		m_pWaterEffect->Start();
}

void RWeatherEffect::OnRemove()
{
//	mlog("RWeatherEffect OnRemove\n");
	if ( m_pScreenMeshNode )
		m_pScreenMeshNode->Pause();
	
	if ( m_pWaterEffect )
		m_pWaterEffect->Stop();
}

int RWeatherEffect::AddVertexBufferRain(PVertexType *pVertex, int nIndex, const RWParticle *particle)
{
	if ( particle->fRand > m_fVisibility )
		return nIndex;

	RCameraSceneNode *pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	const RMatrix &viewMatrix = pCamera->GetViewMatrix();

	// X, Y Vector를 구한다.
	RVector vBaseXAxis( viewMatrix._11, viewMatrix._21, viewMatrix._31);
	RVector vBaseYAxis = RVector::AXISZ;
	RVector vBaseZAxis( viewMatrix._13, viewMatrix._23, viewMatrix._33);

	float fLeft = 0.f;
	float fTop = 0.f;
	float fRight = 1.0f;
	float fBottom = 1.0f;

	const RVector &vPos = particle->vPos;

	float fL = vPos.Length()/(m_fRadius);
	float fLength = 1.0f - fL;
	if ( fLength < 0 )	fLength = 0;
	if ( fLength > 1 )	fLength = 1;

	RVector4 v1stColor = m_currentParameter.v1stColor;
	RVector4 v2ndColor = m_currentParameter.v2ndColor;

	float fColor = m_currentParameter.f1stLuminosity*fLength + m_currentParameter.f2ndLuminosity*fL;
	DWORD dwColor = D3DCOLOR_ARGB( (int)(fColor*255), 255, 255, 255);

	float f = sqrt(particle->vPos.x * particle->vPos.x + particle->vPos.y * particle->vPos.y);
	float fSize = m_currentParameter.fSize;
	float fValue = m_currentParameter.fValue / 100.f;
	if ( (fSize/f) < fValue )
		fSize = fValue * f;

	pVertex->vPos = particle->vPos - particle->vDir*fSize;
	pVertex->dwColor = dwColor;
	pVertex->u = fLeft;
	pVertex->v = fTop;

	++nIndex;
	++pVertex;

	pVertex->vPos = particle->vPos;
	pVertex->dwColor = dwColor;
	pVertex->u = fRight;
	pVertex->v = fBottom;

	++pVertex;
	++nIndex;

	return nIndex;
}

int RWeatherEffect::AddVertexBufferSnow(PVertexType *pVertex, int nIndex, const RWParticle *particle)
{
	if ( particle->fRand > m_fVisibility )
		return nIndex;

	RCameraSceneNode *pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	const RMatrix &viewMatrix = pCamera->GetViewMatrix();

	// X, Y Vector를 구한다.
	RVector vBaseXAxis( viewMatrix._11, viewMatrix._21, viewMatrix._31);
	RVector vBaseYAxis( viewMatrix._12, viewMatrix._22, viewMatrix._32);
	RVector vBaseZAxis( viewMatrix._13, viewMatrix._23, viewMatrix._33);

	float fLeft = 0.f;
	float fTop = 0.f;

	const RVector &vPos = particle->vPos;

	float fL = vPos.Length()/(m_fRadius);
	float fLength = 1.0f - fL;
	if ( fLength < 0 )	fLength = 0;
	if ( fLength > 1 )	fLength = 1;

	RVector4 v1stColor = m_currentParameter.v1stColor;
	RVector4 v2ndColor = m_currentParameter.v2ndColor;

	//RVector4 vColor = RVector4ToDWORD((v1stColor*fLength) + (v2ndColor*fL));
	float fColor = m_currentParameter.f1stLuminosity*fLength + m_currentParameter.f2ndLuminosity*fL;
	DWORD dwColor = D3DCOLOR_ARGB( (int)(fColor*255), 255, 255, 255);

	pVertex->vPos = vPos;
	pVertex->dwColor = dwColor;
	pVertex->u = fTop;
	pVertex->v = fLeft;
	++pVertex;
	++nIndex;

	return nIndex;
}

void RWeatherEffect::Update(float fTime, RCollisionTree* pCollisionTree)
{
	RPFC_THISFUNC;

	if( NULL == ptAddVertexBuffer )
		return;

	RSceneNodeUpdateVisitor v;
	SetPosition(0, 0, 0);
	Traverse( &v );

	if ( m_pScreenMeshNode )
	{
		m_pScreenMeshNode->SetPosition(0, 0, 0);
		m_pScreenMeshNode->Traverse( &v );
	}

	static RVector vBasePosition;

	if ( m_fBaseTime == 0.0f )
	{
		m_fBaseTime = fTime;
		vBasePosition = REngine::GetSceneManager().GetPrimaryCamera()->GetPosition();
		m_bFill = true;
	}

	RCameraSceneNode *pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	pCamera->UpdateViewFrustrum();

	RVector vOffset = vBasePosition - pCamera->GetPosition();

	vBasePosition = pCamera->GetPosition();

	vOffset.x = fmod(vOffset.x, m_fRangeX);
	vOffset.y = fmod(vOffset.y, m_fRangeY);
	vOffset.z = fmod(vOffset.z, m_fRangeZ);

	UpdateParticle(vOffset, fTime, pCollisionTree);
	m_bFill = false;

	int idx = 0;
	PVertexType *pVertexPtr = (PVertexType*)REngine::GetDevice().LockVertexBuffer(m_hVertexBuffer, D3DLOCK_DISCARD );
	if ( pVertexPtr )
	{
		for ( unsigned int ui = 0; ui<m_poolParticles.count(); ++ui )
		{
			RWParticle *ptr = m_poolParticles.getPtr(ui);
			RVector vLen = RVector( ptr->vPos.x, ptr->vPos.y,  ptr->vPos.z);

			// sphere Culling
			if ( vLen.Length() > m_fRadius )
				continue;

			_ASSERT(ptAddVertexBuffer!=NULL);
			idx = (*this.*ptAddVertexBuffer)(&pVertexPtr[idx], idx, ptr);
		}

		REngine::GetDevice().UnlockVertexBuffer(m_hVertexBuffer);
	}

	m_nVertexNum = idx;
}

void RWeatherEffect::Render( RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;

	_ASSERT(REngine::GetDevicePtr()!=NULL);
	
	//const RSceneNodeRenderArray *pWaterArray = pCurrentCamera->GetCulledNodeArray(RRP_WATER);

	RBoundingBox cameraAABB;
	RVector3 vRange(m_fRangeX, m_fRangeY, m_fRangeZ);

	cameraAABB.vmin = m_vCameraPosition - vRange;
	cameraAABB.vmax = m_vCameraPosition + vRange;

	if ( m_pWaterEffect )
	{
#if 0
		for ( RSceneNodeArray::const_iterator it = pWaterArray->begin(); it != pWaterArray->end(); ++it )
		{
			RBoundingBox bbox = (*it)->GetAABB();
			if ( bbox.ContainsAABB(cameraAABB))
			{
				RBoundingBox andBox = cameraAABB.And(bbox);
				andBox.vmin.z = andBox.vmax.z;
				int nNum = m_pWaterEffect->GetElementNum();
				for ( int i = 0; i<nNum; ++i )
				{
					REffectBasic *pElement = m_pWaterEffect->GetElement(i);
					if ( pElement )
						((RAnyEmitter*)pElement->GetElement(true))->AddEmitterArray(andBox);
				}
			}
		}
#endif
	}
	
	if ( m_nVertexNum == 0 )	return;

	DWORD dwFVF = PVertexType::FVF;
	RDeviceD3D *pd3dDevice = (RDeviceD3D*)REngine::GetDevicePtr();

	if ( m_ePrimType == RPT_POINTLIST )
	{
		//dwFVF &= (~D3DFVF_TEX1);

		pd3dDevice->SetPointSpriteEnable(true);

		float fZero = 0.0f;
		float fOne = 1.0f;
		float fMin = 5.f;
		float fMax = m_currentParameter.fValue/10.f;
		float fSize = m_currentParameter.fSize/10.f;
		if ( fSize < fMin )
			fSize = fMin;

		pd3dDevice->SetPointSizeAndScale(fSize, fMin, fMax, true, fZero, fZero, fOne);
	}

	pd3dDevice->SetTextureStageState( 0, RTSS_ALPHAOP, RTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, RTSS_ALPHAARG2, RTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 1, RTSS_ALPHAOP, RTOP_DISABLE);

	pd3dDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG2);
	pd3dDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, RTSS_COLORARG2, RTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 1, RTSS_COLOROP, RTOP_DISABLE);

	pd3dDevice->SetTexture(0, m_hTexture);
	pd3dDevice->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
	pd3dDevice->SetAlphaTestEnable(true);
	pd3dDevice->SetAlphaRef(0);
	pd3dDevice->SetAlphaFunc(RCMP_GREATEREQUAL);

	// Particle Render
	pd3dDevice->SetFvF(dwFVF);
	pd3dDevice->SetVertexBuffer(m_hVertexBuffer, 0, 0);
	pd3dDevice->DrawPrimitive(m_ePrimType, 0, m_nVertexNum/m_nVertexNumPerPrim);

	pd3dDevice->SetBlending(RBLEND_NONE, RBLEND_NONE);
	pd3dDevice->SetAlphaTestEnable(false);

	if ( m_ePrimType == RPT_POINTLIST )
		REngine::GetDevice().SetPointSpriteEnable(false);

	// screen mesh render
	if ( m_pScreenMeshNode )
	{	
		RVector4 vColor(1, 1, 1, m_fVisibility);
		
		m_pScreenMeshNode->SetVisibility(1.f);
		m_pScreenMeshNode->Render( pCurrentCamera, vColor, 1.0f, RENDERING_NULL);
	}	

	return;
}

void RWeatherEffect::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	RPFC_THISFUNC;

	RDevice *pDevice = REngine::GetDevicePtr();

	RCameraSceneNode *pCamera = pCurrentCamera;
	float fPrevNearZ = pCamera->GetNearZ();
	float fPrevFarZ = pCamera->GetFarZ();
	bool bClip = pCamera->GetClipPlaneEnable();
	m_vCameraPosition = pCamera->GetPosition();

	pDevice->SetTransform(RST_WORLD, RMatrix::IDENTITY);

	pCamera->SetPosition(0, 0, 0);
	pCamera->SetClipPlaneEnable(false);
	pCamera->UpdateViewFrustrum();

	// matrix
	bool bFog = REngine::GetDevice().GetFogEnable();
	pDevice->SetFogEnable(false);
	pDevice->SetLighting(false);
	pDevice->ShaderOff();

	Render( pCurrentCamera);

	// restore state
	pDevice->SetFogEnable( bFog);

	pCamera->SetNearFarZ( fPrevNearZ, fPrevFarZ);
	pCamera->SetPosition(m_vCameraPosition);
	pCamera->SetClipPlaneEnable(bClip);
	pCamera->UpdateViewFrustrum();
}

bool RWeatherEffect::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	{
		MXmlElement *pElement = &element;
		const char *pAttribute = pElement->Attribute("type");
		if ( pAttribute == NULL )
			return false;

		WEATHER_TYPE eType = WEATHER_NONE;
		if ( _stricmp(pAttribute,"RAIN") == 0 )
			eType = WEATHER_RAIN;
		else if ( _stricmp(pAttribute,"SNOW") == 0 )
			eType = WEATHER_SNOW;

		using namespace TXML;

		SetWeatherType	( eType );
		SetParticleCount( _GETElement<int>::Value(pElement,"CREATENUM","int"));
		SetScale		( _GETElement<float>::Value(pElement,"SIZE","float"));
		SetVelocity		( _GETElement<float>::Value(pElement,"VELOCITY","float"));
		SetValue		( _GETElement<float>::Value(pElement,"VALUE","float"));

		if ( pElement->FirstChildElement("WINDFORCE"))
			SetWindForce	( _GETElement<float>::Value(pElement,"WINDFORCE","float"));

		RVector4 v1stColor(1, 1, 1, 1);
		RVector4 v2ndColor(1, 1, 1, 1);

		_GetElementValue(&v1stColor, pElement, "COLOR1", "vec4");
		_GetElementValue(&v2ndColor, pElement, "COLOR2", "vec4");
		SetFirstColor(v1stColor);
		SetSecondColor(v2ndColor);

		MXmlElement *pEffectMeshElement = pElement->FirstChildElement("EFFECTMESH");
		m_currentParameter.strMeshFilename.clear();
		if (pEffectMeshElement)
			_Contents(m_currentParameter.strMeshFilename, pEffectMeshElement);
	}
	return true;
}

void RWeatherEffect::SaveToXML(MXmlElement &element)
{
	if ( m_currentParameter.eWeatherType == WEATHER_NONE )
		return;

	using namespace TXML;

	MXmlElement *pElement = &element;
	switch ( m_currentParameter.eWeatherType )
	{
	case WEATHER_RAIN:
		pElement->SetAttribute("type","RAIN");
	break;
	case WEATHER_SNOW:
		pElement->SetAttribute("type","SNOW");
	break;
	}

	_SetElementValue(pElement,"CREATENUM","int", m_currentParameter.nCreateNum);
	_SetElementValue(pElement,"WINDFORCE","float", m_currentParameter.fWindForce);
	_SetElementValue(pElement,"SIZE"	 ,"float", m_currentParameter.fSize);
	_SetElementValue(pElement,"VELOCITY" ,"float", m_currentParameter.fVelocity);
	_SetElementValue(pElement,"VALUE"	 ,"float", m_currentParameter.fValue);
	_SetElementValue(pElement,"COLOR1"	 ,"vec4" , m_currentParameter.v1stColor);
	_SetElementValue(pElement,"COLOR2"	 ,"vec4" , m_currentParameter.v2ndColor);
	_SetElementValue(pElement,"EFFECTMESH", "string", m_currentParameter.strMeshFilename.c_str());

	return;
}

};
