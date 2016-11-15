#include "stdafx.h"
#include "RWaterSceneNode.h"
#include "RMesh.h"
#include "RMeshMgr.h"
#include "RMeshNode.h"

#include "RWaterPassRenderer.h"
#include "RSceneManager.h"
#include "FileInfo.h"
#include "RProfileInfo.h"

#include "RBackgroundWorker.h"
#include "RCollisionTreeBuilder.h"

namespace rs3 {

MImplementRTTI(RWaterPlane, RSceneNode);

int RWaterPlane::CalcPriority()
{
	int nPriority(0);
	RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	if (pCamera)
		nPriority = static_cast<int>(pCamera->GetDistanceTo( GetWorldPosition()));

	return nPriority;
}

RWaterPlane::RWaterPlane() : RSceneNode(RRP_WATER)
, m_fWaterLevel(0.f)
, m_pWaterMesh(NULL)
, m_pWaterPlaneProperty(NULL)
, m_pBumpTexture1(NULL)
, m_pBumpTexture2(NULL)
, m_ShoreTexture(NULL)
{
}

RWaterPlane::~RWaterPlane()
{
	Clear();
}

void RWaterPlane::Clear()
{
	SAFE_DELETE( m_pWaterMesh );
	SAFE_DELETE( m_pWaterPlaneProperty );
	DestroyBumpMap();
	DestroyShoreTexture();
}

void RWaterPlane::CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const
{
	if (!GetVisible() || GetOcclued() )
		return;

	for ( int i = 0; i<m_pWaterMesh->m_nIBSize; i+=3 )
	{
		int nIndex0 = m_pWaterMesh->m_ib[i+0];
		int nIndex1 = m_pWaterMesh->m_ib[i+1];
		int nIndex2 = m_pWaterMesh->m_ib[i+2];
		RVector& v0 = m_pWaterMesh->m_vb[nIndex0].m_vPosition;
		RVector& v1 = m_pWaterMesh->m_vb[nIndex1].m_vPosition;
		RVector& v2 = m_pWaterMesh->m_vb[nIndex2].m_vPosition;

		pCollector->OnAddPolygon(v0, v1, v2, RCollisionTriangle::WATER, 0, 0);
	}
	
	return;
}

bool RWaterPlane::PickPlane(RPICKINFO& pickInfo)
{
	if (!GetVisible() || GetOcclued() )
		return false;

	if (!GetAABB().ContainsLineSegment(pickInfo.inPickOrigin, pickInfo.inPickTo))
		return false;

	RVector &vP = pickInfo.inPickOrigin;
	RVector vDir = pickInfo.inPickTo - vP;
	vDir.Normalize();

	float fLen = FLT_MAX;
	for ( int i = 0; i<m_pWaterMesh->m_nIBSize; i+=3 )
	{
		int nIndex0 = m_pWaterMesh->m_ib[i+0];
		int nIndex1 = m_pWaterMesh->m_ib[i+1];
		int nIndex2 = m_pWaterMesh->m_ib[i+2];
		RVector& v0 = m_pWaterMesh->m_vb[nIndex0].m_vPosition;
		RVector& v1 = m_pWaterMesh->m_vb[nIndex1].m_vPosition;
		RVector& v2 = m_pWaterMesh->m_vb[nIndex2].m_vPosition;
	
		RVector kOut;
		if ( MMath::IsIntersect(vP,vDir,v0,v2,v1,&kOut))
		{
			RVector V = kOut - vP;
			if ( vDir.DotProduct(V) > 0.0f )
			{
				float len = (kOut - vP).Length();
				if (len < fLen)
					fLen = len;
			}
		}
	}

	if ( fLen == FLT_MAX)
		return false;

	pickInfo.outPosition = vP + vDir*fLen;
	return true;
}

bool RWaterPlane::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	MXmlElement *pElement = element.FirstChildElement("MESHFILE");

	// xml node check
	if( pElement == NULL )
		return false;
	
	char szBuffer[256];
	szBuffer[0] = '\0';
	_Contents( szBuffer, pElement );
	string fileName(szBuffer);

	return Build( fileName.c_str(), &element, bBackgroundCreation );
}

void RWaterPlane::SetUsableAndUpdate_()
{
	SetUsable(true);
	SetVisible(true);
}

bool RWaterPlane::Build( const char* _pSzFileName, MXmlElement* pElementForProperty, bool bBackgroundCreation)
{
	Clear();

	m_pWaterMesh = new RTreeMeshBuffer<SWaterVertex>( REngine::GetDevicePtr(), BT_INDEXEDTRIANGLELIST);
	m_pWaterPlaneProperty = new SWaterPlaneProperty;

	if( pElementForProperty != NULL )
	{
		MXmlPrinter xmlPrinter;
		pElementForProperty->Accept( &xmlPrinter );

		MXmlDocument doc;
		doc.Parse( xmlPrinter.CStr() );
		MXmlElement* pProertyElement = doc.FirstChildElement();
		SetWaterPlaneInfoFromXML( *pProertyElement );
	}

	RMesh* pWaterMesh = REngine::GetSceneManagerPtr()->CreateResource<RMesh>( _pSzFileName );

	bool bRet = pWaterMesh->BuildAll();
	_ASSERT( bRet );
	if( !bRet )
		return false;

	if( !pWaterMesh->GetNodeCount() )
	{
		REngine::GetSceneManagerPtr()->ReleaseResource( pWaterMesh );
		return false;
	}

	RMeshNode* mnode = pWaterMesh->GetNode(0);
	RVector* v = mnode->m_pPointTable;
	WORD* w = mnode->m_pFaceIndexTable;

	vector<SWaterVertex> wv(mnode->m_nPointCnt);
	vector<int> idx;

	for(int i=0;i<mnode->m_nFaceIndexTableCnt/3;i++)
	{
		idx.push_back(mnode->m_pVertexIndexTable[mnode->m_pFaceIndexTable[i*3+0]].p);
		idx.push_back(mnode->m_pVertexIndexTable[mnode->m_pFaceIndexTable[i*3+1]].p);
		idx.push_back(mnode->m_pVertexIndexTable[mnode->m_pFaceIndexTable[i*3+2]].p);
	}

	RMatrix m = mnode->m_matBase;
	RVector translate = m.GetTranslation();

	m_fWaterLevel = mnode->m_pPointTable[0].z + translate.z + GetPosition().z;

	RBoundingBox aabb;
	aabb.vmin.Set(FLT_MAX, FLT_MAX, m_fWaterLevel-.5f);
	aabb.vmax.Set(FLT_MIN, FLT_MIN, m_fWaterLevel+.5f);
	for (size_t i=0; i<wv.size(); i++)
	{
		wv[i].m_vPosition = mnode->m_pPointTable[i];
		wv[i].m_vPosition += translate + GetPosition();

		//TODO: m_fDepth 값을 특별히 사용하는 곳이 없어서 우선은 계산하지 않아도 될 것 같음.
		//필요하면 다른 곳으로 옮기자.
		wv[i].m_fDepth = 1;

		if (aabb.maxx < wv[i].m_vPosition.x) aabb.maxx = wv[i].m_vPosition.x;
		if (aabb.maxy < wv[i].m_vPosition.y) aabb.maxy = wv[i].m_vPosition.y;
		if (aabb.minx > wv[i].m_vPosition.x) aabb.minx = wv[i].m_vPosition.x;
		if (aabb.miny > wv[i].m_vPosition.y) aabb.miny = wv[i].m_vPosition.y;
	}
	SetAABB( aabb );

	m_pWaterMesh->SetVertexBuffer(wv);
	m_pWaterMesh->AddIndex(idx);
	m_pWaterMesh->CreateDXBuffers();

	REngine::GetSceneManagerPtr()->ReleaseResource( pWaterMesh );

	LoadBumpMap();
	LoadShoreTexture();

	SetUsableAndUpdate_();
	return true;
}

void RWaterPlane::SaveToXML(MXmlElement &element)
{
	SWaterPlaneProperty* pWaterPlaneProperty = GetWaterPlaneProperty();
	_ASSERT( pWaterPlaneProperty );

	char szBuffer[128];
	szBuffer[0] = '\0';

	MXmlElement *pElement = NULL;

	pElement = new MXmlElement("MESHFILE");
	string fileName = GetNodeName();
	_ASSERT( false == fileName.empty() );
	fileName = MGetPureFileNameExt( fileName );
	_SetContents(pElement, fileName.c_str());
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("DEPTHLIMIT");
	_SetContents(pElement, pWaterPlaneProperty->m_fDepthLimit);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BUMPUVSCALE");
	sprintf(szBuffer,"%f %f %f %f",
		pWaterPlaneProperty->m_fBumpUVScale1,
		pWaterPlaneProperty->m_fBumpUVScale2,
		pWaterPlaneProperty->m_fBumpUVScale3,
		pWaterPlaneProperty->m_fBumpUVScale4
	);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BUMPUVMOVE");
	sprintf(szBuffer,"%f %f %f %f %f %f %f %f", 
		pWaterPlaneProperty->m_vBumpMove1.x,
		pWaterPlaneProperty->m_vBumpMove1.y,
		pWaterPlaneProperty->m_vBumpMove2.x,
		pWaterPlaneProperty->m_vBumpMove2.y, 
		pWaterPlaneProperty->m_vBumpMove3.x,
		pWaterPlaneProperty->m_vBumpMove3.y,
		pWaterPlaneProperty->m_vBumpMove4.x,
		pWaterPlaneProperty->m_vBumpMove4.y
	);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BUMPADJUST");
	_SetContents(pElement, pWaterPlaneProperty->m_fBumpAdjustFactor);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BUMPWEIGHT");
	sprintf(szBuffer,"%f %f %f %f",
		pWaterPlaneProperty->m_fBumpWeight1,
		pWaterPlaneProperty->m_fBumpWeight2,
		pWaterPlaneProperty->m_fBumpWeight3,
		pWaterPlaneProperty->m_fBumpWeight4
	);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BUMPTEX1");
	_SetContents(pElement, pWaterPlaneProperty->m_strBumpTex1.c_str());
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BUMPTEX2");
	_SetContents(pElement, pWaterPlaneProperty->m_strBumpTex2.c_str());
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("CUSTOMFOG");
	_SetContents(pElement, pWaterPlaneProperty->m_bUseCustomFog);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("FOGNEAR");
	_SetContents(pElement, pWaterPlaneProperty->m_fFogNear);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("FOGFAR");
	_SetContents(pElement, pWaterPlaneProperty->m_fFogFar);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("UNDERWATERFOGNEAR");
	_SetContents(pElement, pWaterPlaneProperty->m_fUnderWaterFogNear);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("UNDERWATERFOGFAR");
	_SetContents(pElement, pWaterPlaneProperty->m_fUnderWaterFogFar);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("UNDERWATERBUMPUVSCALE");
	_SetContents(pElement, pWaterPlaneProperty->m_fUnderWaterBumpUVScale);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("UNDERWATERDEPTH");
	_SetContents(pElement, pWaterPlaneProperty->m_fUnderWaterDepth);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("FOGCOLOR");
	sprintf(szBuffer,"%f %f %f",
		pWaterPlaneProperty->m_vFogColor.x,
		pWaterPlaneProperty->m_vFogColor.y,
		pWaterPlaneProperty->m_vFogColor.z
	);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("WATERCOLOR");
	sprintf(szBuffer,"%f %f %f",
		pWaterPlaneProperty->m_vDiffuseColor.x,
		pWaterPlaneProperty->m_vDiffuseColor.y,
		pWaterPlaneProperty->m_vDiffuseColor.z
	);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("RGBHALFLIFE");
	sprintf(szBuffer, "%f %f %f",
		pWaterPlaneProperty->m_vWaterRGBHalflife.x,
		pWaterPlaneProperty->m_vWaterRGBHalflife.y,
		pWaterPlaneProperty->m_vWaterRGBHalflife.z
	);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("FRESNELCONSTANT");
	_SetContents(pElement, pWaterPlaneProperty->m_fFresnelConstant);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("REFLECTADJUST");
	_SetContents(pElement, pWaterPlaneProperty->m_fReflectAdjust);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("VERTEXWAVESCALE");
	_SetContents(pElement, pWaterPlaneProperty->m_fScale);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("VERTEXWAVEAMPLITUDE1");
	_SetContents(pElement, pWaterPlaneProperty->m_fAmplitude1);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("VERTEXWAVEPHASE1");
	_SetContents(pElement, pWaterPlaneProperty->m_fPhase1);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BLENDING");
	_SetContents(pElement, pWaterPlaneProperty->m_fBlending);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("REFRACTATTENUATIONDIST");
	_SetContents(pElement, pWaterPlaneProperty->m_fRefractAttenuationDist);
	element.LinkEndChild(pElement);	

	pElement = new MXmlElement("REFRACTATTENUATIONFACTOR");
	_SetContents(pElement, pWaterPlaneProperty->m_fRefractAttenuationFactor);
	element.LinkEndChild(pElement);	

	if(!pWaterPlaneProperty->m_strShoreTexture.empty())
	{
		pElement = new MXmlElement("SHORETEXTURE");
		_SetContents(pElement, pWaterPlaneProperty->m_strShoreTexture);
		element.LinkEndChild(pElement);

		pElement = new MXmlElement("SHOREINTENSITY");
		_SetContents(pElement, pWaterPlaneProperty->m_fShoreIntensity);
		element.LinkEndChild(pElement);

		pElement = new MXmlElement("SHOREUVSCALE");
		sprintf( szBuffer,"%f %f", pWaterPlaneProperty->m_vShoreUVScale.x, pWaterPlaneProperty->m_vShoreUVScale.y);
		_SetContents(pElement, szBuffer);
		element.LinkEndChild(pElement);
	}
}

bool RWaterPlane::IsUnderWater(RVector3 vCameraPosition)
{
	return vCameraPosition.z<m_fWaterLevel;
}

bool RWaterPlane::IsApplyUnderWaterEffect(RVector3 vCameraPosition)
{
	float fZPos = vCameraPosition.z;

	bool bInnerXY, bUnderZ =false;

	vCameraPosition.z = m_fWaterLevel;
	bInnerXY = GetAABB().Contains(vCameraPosition);

	float fUnderWaterDepth = GetWaterPlaneProperty()->m_fUnderWaterDepth;
	bUnderZ = (m_fWaterLevel-fUnderWaterDepth < fZPos) && (fZPos < m_fWaterLevel);

	return bInnerXY&&bUnderZ;
}

void RWaterPlane::LoadBumpMap()
{
	_ASSERT( m_pWaterPlaneProperty );

	m_pBumpTexture1 = REngine::GetDevice().CreateTexture( m_pWaterPlaneProperty->m_strBumpTex1.c_str(), RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE );
	m_pBumpTexture2 = REngine::GetDevice().CreateTexture( m_pWaterPlaneProperty->m_strBumpTex2.c_str(), RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE );	
}

void RWaterPlane::DestroyBumpMap()
{
	REngine::GetDevice().DeleteTexture(m_pBumpTexture1);
	REngine::GetDevice().DeleteTexture(m_pBumpTexture2);
	
	m_pBumpTexture1 = NULL;
	m_pBumpTexture2 = NULL;
}

void RWaterPlane::LoadShoreTexture()
{
	if(m_pWaterPlaneProperty->m_strShoreTexture.empty())
		return;

	if(m_ShoreTexture)
		REngine::GetDevice().DeleteTexture(m_ShoreTexture);

	m_ShoreTexture = REngine::GetDevice().CreateTexture(  m_pWaterPlaneProperty->m_strShoreTexture.c_str(), RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE );
}

void RWaterPlane::DestroyShoreTexture()
{
	REngine::GetDevice().DeleteTexture(m_ShoreTexture);
	m_ShoreTexture = NULL;
}

bool RWaterPlane::SetWaterPlaneInfoFromXML(MXmlElement& element)
{
//	_ASSERT( m_pWaterPlaneProperty );
	if( !m_pWaterPlaneProperty )
		return false;

	char szBuffer[256];
	szBuffer[0] = '\0';

	MXmlElement *pElement = NULL;

	if ( pElement = element.FirstChildElement("DEPTHLIMIT"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fDepthLimit, pElement);
	}

	if ( pElement = element.FirstChildElement("BUMPUVSCALE"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer,"%f %f %f %f",
			&m_pWaterPlaneProperty->m_fBumpUVScale1,
			&m_pWaterPlaneProperty->m_fBumpUVScale2,
			&m_pWaterPlaneProperty->m_fBumpUVScale3,
			&m_pWaterPlaneProperty->m_fBumpUVScale4
		);
	}

	if ( pElement = element.FirstChildElement("BUMPUVMOVE"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer,"%f %f %f %f %f %f %f %f", 
			&m_pWaterPlaneProperty->m_vBumpMove1.x,
			&m_pWaterPlaneProperty->m_vBumpMove1.y,

			&m_pWaterPlaneProperty->m_vBumpMove2.x,
			&m_pWaterPlaneProperty->m_vBumpMove2.y,

			&m_pWaterPlaneProperty->m_vBumpMove3.x,
			&m_pWaterPlaneProperty->m_vBumpMove3.y,
			
			&m_pWaterPlaneProperty->m_vBumpMove4.x,
			&m_pWaterPlaneProperty->m_vBumpMove4.y
		);
	}

	if ( pElement = element.FirstChildElement("BUMPADJUST"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fBumpAdjustFactor, pElement);
		m_pWaterPlaneProperty->m_fBumpAdjustFactor = min(m_pWaterPlaneProperty->m_fBumpAdjustFactor, 1.0f);
	}

	if ( pElement = element.FirstChildElement("BUMPWEIGHT"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer,"%f %f %f %f",
			&m_pWaterPlaneProperty->m_fBumpWeight1,
			&m_pWaterPlaneProperty->m_fBumpWeight2,
			&m_pWaterPlaneProperty->m_fBumpWeight3,
			&m_pWaterPlaneProperty->m_fBumpWeight4
		);
	}

	if ( pElement = element.FirstChildElement("BUMPTEX1"))
	{
		_Contents(m_pWaterPlaneProperty->m_strBumpTex1, pElement);
	}

	if ( pElement = element.FirstChildElement("BUMPTEX2"))
	{
		_Contents(m_pWaterPlaneProperty->m_strBumpTex2, pElement);
	}

	if ( pElement = element.FirstChildElement("CUSTOMFOG"))
	{
		_Contents(&m_pWaterPlaneProperty->m_bUseCustomFog, pElement);
	}

	if ( pElement = element.FirstChildElement("FOGNEAR"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fFogNear, pElement);
	}

	if ( pElement = element.FirstChildElement("FOGFAR"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fFogFar, pElement);
	}

	if ( pElement = element.FirstChildElement("UNDERWATERFOGNEAR"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fUnderWaterFogNear, pElement);
	}

	if ( pElement = element.FirstChildElement("UNDERWATERFOGFAR"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fUnderWaterFogFar, pElement);
	}

	if ( pElement = element.FirstChildElement("UNDERWATERBUMPUVSCALE"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fUnderWaterBumpUVScale, pElement);
	}
	
	if ( pElement = element.FirstChildElement("UNDERWATERDEPTH"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fUnderWaterDepth, pElement);
	}

	if ( pElement = element.FirstChildElement("FOGCOLOR"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer, "%f %f %f",
			&m_pWaterPlaneProperty->m_vFogColor.x,
			&m_pWaterPlaneProperty->m_vFogColor.y,
			&m_pWaterPlaneProperty->m_vFogColor.z
		);
	}

	if ( pElement = element.FirstChildElement("WATERCOLOR"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer, "%f %f %f",
			&m_pWaterPlaneProperty->m_vDiffuseColor.x,
			&m_pWaterPlaneProperty->m_vDiffuseColor.y,
			&m_pWaterPlaneProperty->m_vDiffuseColor.z
		);
	}

	if ( pElement = element.FirstChildElement("RGBHALFLIFE"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer, "%f %f %f",
			&m_pWaterPlaneProperty->m_vWaterRGBHalflife.x,
			&m_pWaterPlaneProperty->m_vWaterRGBHalflife.y,
			&m_pWaterPlaneProperty->m_vWaterRGBHalflife.z
		);
	}

	if ( pElement = element.FirstChildElement("FRESNELCONSTANT"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fFresnelConstant, pElement);
	}

	if ( pElement = element.FirstChildElement("REFLECTADJUST"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fReflectAdjust, pElement);
	}

	if ( pElement = element.FirstChildElement("VERTEXWAVESCALE"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fScale, pElement);
	}

	if ( pElement = element.FirstChildElement("VERTEXWAVEAMPLITUDE1"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fAmplitude1, pElement);
	}

	if ( pElement = element.FirstChildElement("VERTEXWAVEPHASE1"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fPhase1, pElement);
	}

	if( pElement = element.FirstChildElement("BLENDING"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fBlending, pElement);
	}

	if( pElement = element.FirstChildElement("REFRACTATTENUATIONDIST"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fRefractAttenuationDist, pElement);
	}

	if( pElement = element.FirstChildElement("REFRACTATTENUATIONFACTOR"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fRefractAttenuationFactor, pElement);
	}
	
	if( pElement = element.FirstChildElement("SHORETEXTURE"))
	{
		_Contents(m_pWaterPlaneProperty->m_strShoreTexture, pElement);
	}	

	if( pElement = element.FirstChildElement("SHOREINTENSITY"))
	{
		_Contents(&m_pWaterPlaneProperty->m_fShoreIntensity, pElement);
	}

	if( pElement = element.FirstChildElement("SHOREUVSCALE"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer,"%f %f", &m_pWaterPlaneProperty->m_vShoreUVScale.x, &m_pWaterPlaneProperty->m_vShoreUVScale.y);
	}
	

	return true;
}

void RWaterPlane::Render()
{
	RPFC_THISFUNC;
	_ASSERT( m_pWaterMesh );

	REngine::GetDevice().SetVertexBuffer(m_pWaterMesh->m_hVertexBuffer, 0, 0);
	REngine::GetDevice().SetIndexBuffer(m_pWaterMesh->m_hIndexBuffer);
	int ret = m_pWaterMesh->Render(0);
	REngine::GetResourceProfiler().AddPolygon(PPT_WATER,ret);
}

}
