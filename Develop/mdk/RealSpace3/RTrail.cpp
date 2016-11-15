#include "stdafx.h"
#include "RTrail.h"
#include "RRenderHelper.h"
#include "RXMLTemplate.h"
#include "REffectManager.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "REffectPassRenderer.h"
#include "RParticleShader.h"

namespace rs3 {

RTrail::RTrailCreaterUVLeft			RTrail::m_sTrailCreaterUVLeft;
RTrail::RTrailCreaterUVTop			RTrail::m_sTrailCreaterUVTop;
RTrail::RTrailCreaterUVRight		RTrail::m_sTrailCreaterUVRight;
RTrail::RTrailCreaterUVBottom		RTrail::m_sTrailCreaterUVBottom;

int RTrail::RTrailCreaterUVLeft::AddVertex(PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
	int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition)
{
	float u = pTexUV->x*uvPosition + pTexUV->z*(1-uvPosition);

	int nCount = 0;
	while ( nCount <= nSegment )
	{
		float f = (float)nCount/(float)nSegment;
		pVertex->vPos = p1*f + p2*(1.0f-f);
		pVertex->dwColor = color;
		pVertex->u = u;
		pVertex->v = pTexUV->w*f + pTexUV->y*(1.0f-f);
		++nCount;
		++pVertex;
	}
	return nCount;
}

int RTrail::RTrailCreaterUVTop::AddVertex(PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
	int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition)
{
	float v = pTexUV->y*uvPosition + pTexUV->w*(1-uvPosition);

	int nCount = 0;
	while ( nCount <= nSegment )
	{
		float f = (float)nCount/(float)nSegment;
		pVertex->vPos = p1*f + p2*(1.0f-f);
		pVertex->dwColor = color;
		pVertex->u = pTexUV->x*f + pTexUV->z*(1.0f-f);
		pVertex->v = v;
		++nCount;
		++pVertex;
	}
	return nCount;
}

int RTrail::RTrailCreaterUVRight::AddVertex(PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
	int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition)
{
	float u = pTexUV->z*uvPosition + pTexUV->x*(1-uvPosition);

	int nCount = 0;
	while ( nCount <= nSegment )
	{
		float f = (float)nCount/(float)nSegment;
		pVertex->vPos = p1*f + p2*(1.0f-f);
		pVertex->dwColor = color;
		pVertex->u = u;
		pVertex->v = pTexUV->y*f + pTexUV->w*(1.0f-f);
		++nCount;
		++pVertex;
	}
	return nCount;
}

int RTrail::RTrailCreaterUVBottom::AddVertex(PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
	int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition)
{
	float v = pTexUV->w*uvPosition + pTexUV->y*(1-uvPosition);

	int nCount = 0;
	while ( nCount <= nSegment )
	{
		float f = (float)nCount/(float)nSegment;
		pVertex->vPos = p1*f + p2*(1.0f-f);
		pVertex->dwColor = color;
		pVertex->u = pTexUV->x*f + pTexUV->z*(1.0f-f);
		pVertex->v = v;
		++nCount;
		++pVertex;
	}
	return nCount;
}

MImplementRTTI(RTrail, REffectBase);

const float TRAIL_FRAME = 60.f;

RTrail::RTrail()
	: REffectBase(RRP_PARTICLE)
	, m_fTrailLife(0.f)
	, m_fTrailWidth(0.f)
	, m_vFrontColor(1, 1, 1, 1)
	, m_vBackColor(1, 1, 1, 1)
	, m_bAlign(false)
	, m_bCreateState(false)
	, m_bLoop(false)
	, m_fSumElpasedTIme(0)
	, m_nSegment(1)
	, m_nVertexSize(0)
	, m_nIndexSize(0)
	, m_vTexUV(0, 0, 1, 1)
	, m_UVType(RTRAILUV_LEFT)
	, m_pTrailCreater(&m_sTrailCreaterUVLeft)
{
	SetBlendModeAdd();
	SetAutoUpdate(true);
}

RTrail::~RTrail()
{
}

RResource* RTrail::GetResource()
{
	return NULL;
}

void RTrail::SetUVGenLeft()
{
	m_UVType = RTRAILUV_LEFT;
	m_pTrailCreater = &m_sTrailCreaterUVLeft;
}

void RTrail::SetUVGenTop()
{
	m_UVType = RTRAILUV_TOP;
	m_pTrailCreater = &m_sTrailCreaterUVTop;
}

void RTrail::SetUVGenRight()
{
	m_UVType = RTRAILUV_RIGHT;
	m_pTrailCreater = &m_sTrailCreaterUVRight;
}

void RTrail::SetUVGenBottom()
{
	m_UVType = RTRAILUV_BOTTOM;
	m_pTrailCreater = &m_sTrailCreaterUVBottom;
}

bool RTrail::IsUVGenLeft() const
{
	if( m_UVType == RTRAILUV_LEFT )
		return true;

	return false;
}

bool RTrail::IsUVGenTop() const
{
	if( m_UVType == RTRAILUV_TOP )
		return true;

	return false;
}

bool RTrail::IsUVGenRight() const
{
	if( m_UVType == RTRAILUV_RIGHT )
		return true;

	return false;
}

bool RTrail::IsUVGenBottom() const
{
	if( m_UVType == RTRAILUV_BOTTOM )
		return true;

	return false;
}

bool RTrail::CreateQueue()
{
	int nTrailCount = m_fTrailLife * TRAIL_FRAME + 1;
	m_queueTrails.resize(nTrailCount);

	m_nVertexSize = nTrailCount*(m_nSegment+1);
	m_nIndexSize = nTrailCount*6*m_nSegment;

	return true;
}

bool RTrail::UpdateAABB()
{
	RBoundingBox aabb = GetAABB();
	aabb.Add( GetWorldPosition() );
	SetAABB( aabb );
	return true;
}

void RTrail::Start()
{
	m_kTimer.Reset();
	m_kTimer.Start();

	m_queueTrails.clear();
	m_fSumElpasedTIme = 0.0f;

	UpdateTransform();
	m_bCreateState = true;
}

void RTrail::Stop()
{
	m_bCreateState = false;
}

bool RTrail::IsAlive()
{
	return IsLooping();
}

int RTrail::RenderTrail( RCameraSceneNode* pCurrentCamera)
{
	int count = (int)m_queueTrails.count();
	if ( count == 0 )
		return 0;

	const RVector4 *pTexUV = GetTexUV();

	unsigned int vertexIndex = 0;
	const float TRAIL_HALF_WIDTH = m_fTrailWidth/2.0f * GetResultScale(m_scale.x, GetParent());

	RBoundingBox aabb;
	aabb.Initialize();

	PVERTEXFORMAT* pVertexArray = REffectManager::GetVertexArray( m_nVertexSize );

	float f = 0.0f;
	for ( int c = 0; c<count; ++c )
	{
		f = c/(float)(count);
		RMarks *pMark = &m_queueTrails[c];
		const RVector &vPos = pMark->vPosition;
		const RVector V = pMark->vNormal * TRAIL_HALF_WIDTH;
		aabb.Add(vPos);

		RVector4 vColor = m_vFrontColor*f + m_vBackColor*(1-f);
		DWORD dwColor = D3DCOLOR_XRGB((int)(vColor.r*255), (int)(vColor.g*255), (int)(vColor.b*255));

		vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], vPos - V, vPos + V, m_nSegment, dwColor, pTexUV, f);
	}

	f = 1.0f;
	const RVector &vPos = GetWorldPosition();
	const RVector V = GetWorldUp() * TRAIL_HALF_WIDTH;

	aabb.Add(vPos);
	SetAABB( aabb );

	RVector4 vColor = m_vFrontColor;
	DWORD dwColor = D3DCOLOR_XRGB((int)(vColor.r*255), (int)(vColor.g*255), (int)(vColor.b*255));

	vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], vPos - V, vPos + V, m_nSegment, dwColor, pTexUV, f);
	return vertexIndex;
}

int RTrail::RenderAlignTrail( RCameraSceneNode* pCurrentCamera)
{
	const float TRAIL_HALF_WIDTH = m_fTrailWidth/2.0f * GetResultScale(m_scale.x, GetParent());

	int count = (int)m_queueTrails.count();
	if ( count == 0 )
		return 0;

	const RCameraSceneNode *pCameraSceneNode = pCurrentCamera;
	const RVector &C = pCameraSceneNode->GetWorldPosition();
	const RVector4 *pTexUV = GetTexUV();

	int vertexIndex = 0;

	PVERTEXFORMAT* pVertexArray = REffectManager::GetVertexArray( m_nVertexSize );

	if ( count == 2 )
	{
		RVector V0 = m_queueTrails[0].vPosition;
		RVector V1 = m_queueTrails[1].vPosition;
		RVector V2 = GetWorldPosition();

		RVector T0 = V1 - V0;
		RVector T1 = V2 - V0;
		RVector T2 = V2 - V1;
		_ASSERT(T0.LengthSq()>FLT_EPSILON);
		_ASSERT(T1.LengthSq()>FLT_EPSILON);
		
		RBoundingBox aabb;
		aabb.Initialize();
		aabb.Add(V0);
		aabb.Add(V1);
		aabb.Add(V2);
		SetAABB( aabb );

		if ( T2.LengthSq() < FLT_EPSILON )
			return 0;

		RVector Z0 = C - V0;
		RVector Z1 = C - V1;
		RVector Z2 = C - V2;

		RVector H0 = T0.CrossProduct(Z0);
		RVector H1 = T1.CrossProduct(Z1);
		RVector H2 = T2.CrossProduct(Z2);

		H0.FastNormalize();
		H1.FastNormalize();
		H2.FastNormalize();

		H0 *= TRAIL_HALF_WIDTH;
		H1 *= TRAIL_HALF_WIDTH;
		H2 *= TRAIL_HALF_WIDTH;

		DWORD dwFrontColor = D3DCOLOR_XRGB((int)(m_vFrontColor.r*255), (int)(m_vFrontColor.g*255), (int)(m_vFrontColor.b*255));
		DWORD dwBackColor = D3DCOLOR_XRGB((int)(m_vBackColor.r*255), (int)(m_vBackColor.g*255), (int)(m_vBackColor.b*255));
		
		RVector4 vMidColor = m_vFrontColor*.5f + m_vBackColor*.5f;
		DWORD dwMidColor = D3DCOLOR_XRGB((int)(vMidColor.r*255), (int)(vMidColor.g*255), (int)(vMidColor.b*255));

		float mu = (pTexUV->x+pTexUV->z)*.5f;
		vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], V0 - H0, V0 + H0, m_nSegment, dwBackColor, pTexUV, 1.f);
		vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], V1 - H1, V1 + H1, m_nSegment, dwMidColor , pTexUV, mu);
		vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], V2 - H2, V2 + H2, m_nSegment, dwFrontColor, pTexUV, 0.f);
		return vertexIndex;
	}
	else if ( count >= 3 )
	{
		// 첫 머리 부분
		RVector V0, V1, V2;
		RVector T0, T1, T2;
		RVector Z0, Z1, Z2;
		RVector H0, H1, H2;

		DWORD dwColor;
		RVector4 vColor;

		V0 = m_queueTrails[0].vPosition;
		V1 = m_queueTrails[1].vPosition;

		RBoundingBox aabb;
		aabb.Initialize();
		aabb.Add(V0);
		aabb.Add(V1);
		aabb.Add(GetWorldPosition());

		T0 = V1 - V0;
		_ASSERT(T0.LengthSq()>FLT_EPSILON);

		Z0 = C - V0;

		H0 = T0.CrossProduct(Z0);
		H0.FastNormalize();

		H0 *= TRAIL_HALF_WIDTH;

		int c = 0;
		float f = 0;
		
		f = (float)c/(float)count;
		dwColor = D3DCOLOR_XRGB((int)(m_vBackColor.r*255), (int)(m_vBackColor.g*255), (int)(m_vBackColor.b*255));

		vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], V0 - H0, V0 + H0, m_nSegment, dwColor, pTexUV, f);
		++c;

		// 가운데 부분
		for ( ; c<count-1; c++)
		{
			const RVector &V = m_queueTrails[c].vPosition;
			const RVector &V0 = m_queueTrails[c-1].vPosition;
			const RVector &V1 = m_queueTrails[c+1].vPosition;

			aabb.Add(V);

			T0 = V1 - V0;
			_ASSERT(T0.LengthSq()>FLT_EPSILON);
			Z0 = C - V;
			H0 = T0.CrossProduct(Z0);
			H0.FastNormalize();

			H0 *= TRAIL_HALF_WIDTH;
			f = (float)c/(float)count;

			vColor = m_vFrontColor*f + m_vBackColor*(1-f);
			dwColor = D3DCOLOR_XRGB((int)(vColor.r*255), (int)(vColor.g*255), (int)(vColor.b*255));

			vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], V0 - H0, V0 + H0, m_nSegment, dwColor, pTexUV, f);
		}

		// 끝 부분
		V0 = m_queueTrails[c-1].vPosition;
		V1 = m_queueTrails[c].vPosition;
		V2 = GetWorldPosition();

		aabb.Add(V1);
		SetAABB( aabb );

		T1 = V2 - V0;
		T2 = V2 - V1;
		_ASSERT(T1.LengthSq()>FLT_EPSILON);
		if (T2.LengthSq() < FLT_EPSILON)
			T2 = T1;

		Z1 = C - V1;
		Z2 = C - V2;

		H1 = T1.CrossProduct(Z1);
		H2 = T2.CrossProduct(Z2);

		H1.FastNormalize();
		H2.FastNormalize();

		H1 *= TRAIL_HALF_WIDTH;
		H2 *= TRAIL_HALF_WIDTH;

		f = (float)c/(float)count;
		vColor = m_vFrontColor*f + m_vBackColor*(1-f);
		dwColor = D3DCOLOR_XRGB((int)(vColor.r*255), (int)(vColor.g*255), (int)(vColor.b*255));

		vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], V1 - H1, V1 + H1, m_nSegment, dwColor, pTexUV, f);

		f = 1.0f;
		dwColor = D3DCOLOR_XRGB((int)(m_vFrontColor.r*255), (int)(m_vFrontColor.g*255), (int)(m_vFrontColor.b*255));
		vertexIndex += m_pTrailCreater->AddVertex(&pVertexArray[vertexIndex], V2 - H2, V2 + H2, m_nSegment, dwColor, pTexUV, f);

		return vertexIndex;
	}

	RBoundingBox aabb;
	aabb.Initialize();
	aabb.Add(GetWorldPosition());
	SetAABB( aabb );
	return 0;
}

void RTrail::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	RPFC_THISFUNC;

	if ( GetVisible() == false || GetOcclued() )
		return;

	int count = (int)m_queueTrails.count();
	if ( count <= 1 )
		return;

	int nVertexCount = ( m_bAlign ? RenderAlignTrail(pCurrentCamera) : RenderTrail(pCurrentCamera));
	if ( nVertexCount == 0 )
		return;

	// index buffer 만들기 (tri-list)
	WORD *pIndexBegin = REffectManager::GetIndexArray(m_nIndexSize);
	WORD *pIndex = pIndexBegin;
	for ( int n = 0; n<m_nSegment; ++n )
	{
		int ni = n;
		if ( n != 0 )
		{
			*pIndex = ni;
			++pIndex;
		}

		for ( int i = 0; i<count+1; ++i )
		{
			ni = n+i*(m_nSegment+1);
			*pIndex = ni;	++pIndex;
			*pIndex = ni+1;	++pIndex;
			_ASSERT(ni+1<nVertexCount);
		}

		if ( n != m_nSegment-1 )
		{
			*pIndex = ni+1;
			++pIndex;
		}
	}

	int indexSize = pIndex - pIndexBegin;
#ifdef _DEBUG
	_ASSERT(indexSize<=m_nIndexSize);
#endif

	// Emitter와 Trail에서 쓰는 버텍스 포맷이 동일하다. 그러므로 같이 공용 버퍼를 사용 한다.
	// SetFvF()및 SetVertexBuffer()는 내부적으로 중복 처리를 해 주므로 명시적이 되게 호출은 해 준다.
	if ( nVertexCount >= 4 &&
		 REffectManager::WriteIndexBuffer(indexSize) && REffectManager::WriteVertexBuffer(nVertexCount) )
	{
		RenderPrimitive( pCurrentCamera );
	}
}

void RTrail::RenderPrimitive( RCameraSceneNode* pCurrentCamera)
{
	bool bDistortion = BeginDistortion(pCurrentCamera);

	RParticleShaderCreateParam shaderCreateParam;
	shaderCreateParam.bAlphaTest				= IsUseAlphaTest();
	shaderCreateParam.bSelfIllumination			= GetSITexture() ? true : false;
	shaderCreateParam.bDistortion				= bDistortion;
	shaderCreateParam.bSoftParticleBlendAlpha	= false;
	shaderCreateParam.bSoftParticleBlendColor	= false;
	shaderCreateParam.bMultiLayerBlend			= false;

	RParticleShader* pParticleShader = REngine::GetSceneManager().GetPassRenderer<REffectPassRenderer>()->GetParticleShader();
	_ASSERT( pParticleShader );
	RShaderFX* pShader = pParticleShader->GetShader( shaderCreateParam );
	_ASSERT( pShader );
	if( pShader )
	{
		SetRenderState( pCurrentCamera, pParticleShader, bDistortion );
		{
			RTechnique technique = pShader->GetTechniqueByIndex(0);

			technique.Begin(NULL, TECH_PRESERVE_NOT);
			technique.BeginPass();

			if( int nPrimitive = REffectManager::DrawIndexedPrimitiveTriangleStrip() )
				REngine::GetResourceProfiler().AddPolygon( PPT_PARTICLE, nPrimitive );

			technique.EndPass();
			technique.End();
		}
		RestoreRenderState( pCurrentCamera, pParticleShader );
	}

	if(bDistortion)
		EndDistortion(pCurrentCamera, true);


	// debug
#ifdef _DEBUG
// 	RRenderHelper::SetRenderState();
// 	RRenderHelper::RenderAxis(GetWorldTransform(), 10.f);
// 	RRenderHelper::EndRenderState();
#endif
}

void RTrail::OnUpdate()
{
	RPFC_THISFUNC;

	const float UNIT_TIME = 1.0f/TRAIL_FRAME;

	m_kTimer.UpdateCustom(REngine::GetTimer().GetElapsedTime());
//		m_kTimer.SyncToApplicationElapsedTime( REngine::GetTimer().GetElapsedTime() );

	m_fSumElpasedTIme += m_kTimer.GetElapsedTime()/1000.f;
	if ( m_fSumElpasedTIme < UNIT_TIME )
		return;

	// trail update	
	int n = -1;
	int count = (int)m_queueTrails.count();
	for ( int c = 0; c<count; ++c )
	{
		m_queueTrails[c].fLife -= m_fSumElpasedTIme;
		if ( m_queueTrails[c].fLife <= 0.0f )
			n = c;
	}

	m_queueTrails.pop(n+1);

	// trail create
	if ( m_bCreateState )
	{
		// 현재 OnUpdate 이므로 UpdateTransform 이전이므로 미리 실행해준다
		UpdateTransform();

		RMarks mark;
		mark.vPosition = GetWorldPosition();
		mark.vNormal = GetWorldUp();
		mark.fLife = m_fTrailLife;

		RMarks *pMark = m_queueTrails.peekHead();
		if ( pMark == NULL )
		{
			RMarks *pMark = m_queueTrails.push(mark);
			_ASSERT(pMark!=NULL);
		}
		else if ((mark.vPosition - pMark->vPosition).LengthSq() >= 1.f )
		{
			RMarks *pMark = m_queueTrails.push(mark);
			_ASSERT(pMark!=NULL);
		}
	}

	m_fSumElpasedTIme = 0.0f;
}

void RTrail::OnUpdateAABB()
{
	UpdateAABB();
}

/**
<RTRAIL>
	<TEXTURE></TEXTURE>
	<WIDTH></WIDTH>
	<LIFE></LIFE>
<RTRAIL>
*/
bool RTrail::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	using namespace TXML;
	// background loading 사용하지 않음
	MXmlElement *pRoot = &element;

	this->SetNodeName(pRoot->Attribute("name"));

	float fLife = _GETElement<float>::Value(pRoot, "LIFE", "float");
	SetLife(fLife);

	float fWidth = _GETElement<float>::Value(pRoot,"WIDTH", "float");
	SetWidth(fWidth);

	LoadXMLBlendInfo( pRoot );

	bool bAlign = (_GETElement<int>::Value(pRoot, "ALIGN", "int") ? true : false );
	SetAlign(bAlign);

	int nSegment = _GETElement<int>::Value(pRoot, "SEGMENT", "int");
	SetSegment(max(1,nSegment));

	bool bLoop = (_GETElement<int>::Value(pRoot, "LOOP", "int") ? true : false );
	SetLooping(bLoop);

	_GetElementValue(&m_vFrontColor, pRoot, "FRONTCOLOR", "vec4");
	_GetElementValue(&m_vBackColor, pRoot, "BACKCOLOR", "vec4");

	RVector4 vec4(0, 0, 1, 1);
	_GetElementValue(&vec4, pRoot, "TEXUV","vec4");
	SetTexUV(vec4);

	if( pRoot->FirstChildElement("TEXUVTYPE_LEFT") )
		SetUVGenLeft();
	else
	if( pRoot->FirstChildElement("TEXUVTYPE_TOP") )
		SetUVGenTop();
	else
	if( pRoot->FirstChildElement("TEXUVTYPE_RIGHT") )
		SetUVGenRight();
	else
	if( pRoot->FirstChildElement("TEXUVTYPE_BOTTOM") )
		SetUVGenBottom();

	if( MXmlElement *pElement = pRoot->FirstChildElement("TEXTURE") )
	{
		string strTextureName;
		_Contents(strTextureName, pElement);
		SetTextureName(strTextureName.c_str());
	}

	if( MXmlElement *pElement = pRoot->FirstChildElement("SELFILLUMINATION") )
	{
		string strSITextureName;
		_Contents(strSITextureName, pElement);
		SetSITextureName( strSITextureName.c_str() );
	}

	SetSelfIlluminationScale( 1.f );
	if ( pRoot->FirstChildElement("SELFILLUMINATIONSCALE"))
	{
		float fSelfIlluminationScale = _GETElement<float>::Value(pRoot,"SELFILLUMINATIONSCALE", "float");
		SetSelfIlluminationScale( fSelfIlluminationScale );
	}
	
	if( pRoot->FirstChildElement("INVISIBLE") )
		SetVisibleSwitch( false );

	if( MXmlElement* pDistortionElement = pRoot->FirstChildElement("DISTORTION") )
	{
		SetDistortion(true);
		m_DistortionParam.CreateToXML(*pDistortionElement);
	}

	LoadAllTexture( bBackgroundCreation );
	m_DistortionParam.LoadNormalMap( bBackgroundCreation );

	return true;
}


void RTrail::SaveToXML(MXmlElement &element)
{
	using namespace TXML;

	MXmlElement *pRoot = &element;
	pRoot->SetAttribute("name", GetNodeName());

	_SetElementValue(pRoot, "LIFE", "float", GetLife());
	_SetElementValue(pRoot, "WIDTH", "float", GetWidth());
	_SetElementValue(pRoot, "FRONTCOLOR", "vec4", m_vFrontColor);
	_SetElementValue(pRoot, "BACKCOLOR", "vec4", m_vBackColor);
	_SetElementValue(pRoot, "ALIGN", "int", (m_bAlign ? 1 : 0));
	_SetElementValue(pRoot, "SEGMENT", "int", m_nSegment);
	_SetElementValue(pRoot, "LOOP", "int", (m_bLoop ? 1 : 0));
	_SetElementValue(pRoot, "TEXUV" , "vec4", m_vTexUV);

	if( IsUVGenLeft() )
	{
		MXmlElement *pUVTypeElement = new MXmlElement("TEXUVTYPE_LEFT");
		pRoot->LinkEndChild(pUVTypeElement);
	}
	else
	if( IsUVGenTop() )
	{
		MXmlElement *pUVTypeElement = new MXmlElement("TEXUVTYPE_TOP");
		pRoot->LinkEndChild(pUVTypeElement);
	}
	else
	if( IsUVGenRight() )
	{
		MXmlElement *pUVTypeElement = new MXmlElement("TEXUVTYPE_RIGHT");
		pRoot->LinkEndChild(pUVTypeElement);
	}
	else
	if( IsUVGenBottom() )
	{
		MXmlElement *pUVTypeElement = new MXmlElement("TEXUVTYPE_BOTTOM");
		pRoot->LinkEndChild(pUVTypeElement);
	}

	SaveXMLBlendInfo( pRoot );

	MXmlElement *pTextureElement = new MXmlElement("TEXTURE");
	_SetContents(pTextureElement, GetTextureName());
	pRoot->LinkEndChild(pTextureElement);

	if( GetSITextureName().length() )
	{
		_SetElementValue(pRoot,"SELFILLUMINATION", "string", GetSITextureName().c_str());
		_SetElementValue(pRoot,"SELFILLUMINATIONSCALE","float", GetSelfIlluminationScale());
	}

	if( GetVisibleSwitch() == false )
	{
		MXmlElement *pVisibleElement = new MXmlElement("INVISIBLE");
		pRoot->LinkEndChild(pVisibleElement);
	}

	if(IsDistortion())
	{
		MXmlElement *pDistortionElement = new MXmlElement("DISTORTION");
		pRoot->LinkEndChild(pDistortionElement);

		m_DistortionParam.SaveToXML(*pDistortionElement);
	}

}

RSceneNode* RTrail::Clone()
{
	RTrail *pNew = new RTrail();

	pNew->CopyEffectBaseParameter( this );

	pNew->SetLife(this->GetLife());
	pNew->SetWidth(this->GetWidth());
	pNew->SetTextureName(this->GetTextureName().c_str());
	pNew->SetFrontColor(this->GetFrontColor());
	pNew->SetBackColor(this->GetBackColor());
	pNew->SetAlign(this->GetAlign());
	pNew->SetSegment(this->GetSegment());
	pNew->SetTexUV(*this->GetTexUV());
	pNew->SetLooping(m_bLoop);

	pNew->m_UVType = this->m_UVType;
	pNew->m_pTrailCreater = this->m_pTrailCreater;

	return pNew;
}

bool RTrail::IsLoadingCompleted()
{
	if( GetTexture()==R_NONE) return true;

	return GetTexture()->IsLoadingCompleted();
}
}