#include "StdAfx.h"
#include "RPointLightPassRenderer.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RLightingShader.h"

namespace rs3 {

RPointLightPassRenderer::RPointLightPassRenderer(void)
{
	RVector3 baseVertices[] = 
	{
		RVector3(-1, 1, 1),
		RVector3( 1, 1, 1),
		RVector3(-1,-1, 1),
		RVector3( 1,-1, 1),
		RVector3(-1, 1,-1),
		RVector3( 1, 1,-1),
		RVector3(-1,-1,-1),
		RVector3( 1,-1,-1)
	};

	WORD IndexArray[] =
	{
		6, 5, 4, 7, 5, 6,	//front
		1, 2, 0, 3, 2, 1,	//back
		4, 1, 0, 4, 5, 1,	//top
		3, 6, 2, 7, 6, 3,	//bottom
		7, 3 ,1, 5, 7, 1,	//left
		2, 4, 0, 6, 4, 2,	//right
	};

	memcpy(m_GeometryData, baseVertices, sizeof(RVector3)*8);
	memcpy(m_IndexData, IndexArray, sizeof(WORD)*36);

	RVertexAttributeEx Attribs[] = 
	{
		RVertexAttributeEx(0, TYPE_VERTEX, 0, FORMAT_FLOAT, 3),

		RVertexAttributeEx(1, TYPE_NORMAL, 0, FORMAT_FLOAT, 3),
		RVertexAttributeEx(1, TYPE_TEXCOORD, 0, FORMAT_FLOAT, 3),

		RVertexAttributeEx(1, TYPE_TEXCOORD, 1, FORMAT_FLOAT, 4),
		RVertexAttributeEx(1, TYPE_TEXCOORD, 2, FORMAT_FLOAT, 4),
		RVertexAttributeEx(1, TYPE_TEXCOORD, 3, FORMAT_FLOAT, 4),

		RVertexAttributeEx(1, TYPE_COLOR, 0, FORMAT_D3DCOLOR, 4),		
	};

	m_VertexFormat = REngine::GetDevice().CreateVertexFormat(Attribs, 7);

	m_GeometryBuffer =REngine::GetDevice().CreateVertexBuffer(sizeof(RVector3)*8, sizeof(RVector3), m_GeometryData);
	m_GeometryIndexBuffer =REngine::GetDevice().CreateIndexBuffer(36, RFMT_INDEX16, IndexArray);

	m_nMaxLightCount = 65535/36;	//indexbuffer가 한번에 처리할 수 있는 instance 개수
	m_InstanceBuffer =REngine::GetDevice().CreateVertexBuffer(sizeof(PL_INSTANCE_DATA)*m_nMaxLightCount, sizeof(PL_INSTANCE_DATA), NULL, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

	m_nOffsetForWrite = m_nOffsetForRender =0;
}

RPointLightPassRenderer::~RPointLightPassRenderer(void)
{
}

void RPointLightPassRenderer::CategorizeLight(RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, 
											  RSceneNodeList* outPoint, RSceneNodeList* outPointWithCamera,
											  RSceneNodeList* outArea, RSceneNodeList* outAreaWithCamera)
{
	RPFC_THISFUNC;
	SCENE_SET::iterator iter;
	for( iter = pCulledList->begin() ; iter != pCulledList->end() ; ++iter)
	{
		RSceneNode* pSceneNode = (*iter);

		RPointLightSceneNode* pPointLightSceneNode = MDynamicCast(RPointLightSceneNode, pSceneNode);
		if(pPointLightSceneNode==NULL)
			continue;

		if(pPointLightSceneNode->IsAreaLight())
		{
			if( pPointLightSceneNode->CheckCameraInLight(pCurrentCamera) )
				outAreaWithCamera->push_back(pPointLightSceneNode);
			else
				outArea->push_back(pPointLightSceneNode);
		}
		else
		{
			if( pPointLightSceneNode->CheckCameraInLight(pCurrentCamera) )
				outPointWithCamera->push_back(pPointLightSceneNode);
			else
				outPoint->push_back(pPointLightSceneNode);
		}
	}
}

bool RPointLightPassRenderer::GetInstanceData(RSceneNode* pSceneNode, PL_INSTANCE_DATA& outInstanceData)
{
	RPointLightSceneNode* pPointLightSceneNode = MDynamicCast(RPointLightSceneNode, pSceneNode);
	if(pPointLightSceneNode==NULL)
		return false;

	//view-space Light position
	RVector3 vLightPosition = pPointLightSceneNode->GetWorldPosition();


	//Light range
	float fScale = pPointLightSceneNode->GetScale();
	float fRadius =pPointLightSceneNode->GetRadius() *fScale;
	float fAttenuationStart =pPointLightSceneNode->GetAttenuationStart() *fScale;


	//Light color
	RVector3 vecColor = pPointLightSceneNode->m_color;
	DWORD dwColor = D3DCOLOR_ARGB(0xFF, int(vecColor.x*255), int(vecColor.y*255), int(vecColor.z*255));


	//Visibility
	float fVisibility = pPointLightSceneNode->GetVisibilityResult() *pPointLightSceneNode->GetApplyIntensity();


	//Light Axis
	RMatrix matLightWorld =pPointLightSceneNode->GetWorldTransform();


	//Set per-Instance Data
	outInstanceData.vPosition=vLightPosition;
	outInstanceData.vLightInfo =RVector3(fRadius, fAttenuationStart, fVisibility);
	outInstanceData.dwColor =dwColor;

	if(pPointLightSceneNode->IsAreaLight())
	{
		RVector3 vAreaRange =pPointLightSceneNode->GetAreaRange() *fScale;
		RVector3 vScale = pPointLightSceneNode->GetScaleFromWorldTransform();

		//Scale값은 다른 데이터들을 통해 적용되기 때문에 여기서는 scale값을 없애주기위해 vScale로 나눈다.
		RVector3 vWorldAxisX = RVector3(matLightWorld._11, matLightWorld._21, matLightWorld._31)/vScale; 
		RVector3 vWorldAxisY = RVector(matLightWorld._12, matLightWorld._22, matLightWorld._32)/vScale; 
		RVector3 vWorldAxisZ = RVector(matLightWorld._13, matLightWorld._23, matLightWorld._33)/vScale; 

		outInstanceData.vAxisX =RVector4( vWorldAxisX.Normalize(), vAreaRange.x);
		outInstanceData.vAxisY =RVector4( vWorldAxisY.Normalize(), vAreaRange.y);
		outInstanceData.vAxisZ =RVector4( vWorldAxisZ.Normalize(), vAreaRange.z);
	}	

	return true;
}

int RPointLightPassRenderer::UpdateInstanceVertexBuffer(RCameraSceneNode* pCamera, RSceneNodeList* pSceneNodeList)
{
	WORD nNumSceneNode = pSceneNodeList->size();
	if(nNumSceneNode==0)
		return 0;

	DWORD dwFlag = 0;
	if(m_nOffsetForWrite+nNumSceneNode > m_nMaxLightCount)
	{
		dwFlag = D3DLOCK_DISCARD; 
		m_nOffsetForWrite =0;
	}
	else
		dwFlag = D3DLOCK_NOOVERWRITE; 

	PL_INSTANCE_DATA* pIntanceDataBuffer = (PL_INSTANCE_DATA*)REngine::GetDevice().LockVertexBuffer(m_InstanceBuffer, dwFlag, m_nOffsetForWrite, nNumSceneNode);
	if(pIntanceDataBuffer==NULL)
		return 0;


	int nLightCount =0;

	RSceneNodeList::iterator iter;
	for( iter = pSceneNodeList->begin() ; iter != pSceneNodeList->end() ; ++iter)
	{
		RSceneNode* pSceneNode = (*iter);

		// Visibility 체크. LOD cull 역시 이때 체크 된다.(GetVisible)
		float fVisibility = pSceneNode->GetVisibilityResult();
		if(fVisibility <= 0.f || pSceneNode->GetVisible() == false )
			continue;

		if( !GetInstanceData(pSceneNode, pIntanceDataBuffer[nLightCount]) )
			continue;

		++nLightCount;
	}

	REngine::GetDevice().UnlockVertexBuffer(m_InstanceBuffer);

	m_nOffsetForRender = m_nOffsetForWrite;
	m_nOffsetForWrite += nLightCount;

	return nLightCount;
}

void RPointLightPassRenderer::RenderPointLightingToLBuffer(RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RLightingShader* pLightingShader, RRENDERING_MODE eRRM)
{
	RPFC_THISFUNC;
	RSceneNodeList PointLightList, PointLightWithCameraList, AreaLightList, AreaLightWithCameraList;

	CategorizeLight(pCurrentCamera, pCulledList, &PointLightList, &PointLightWithCameraList, &AreaLightList, &AreaLightWithCameraList);
	
	REngine::GetDevice().SetDepthEnable(true, false);
	REngine::GetDevice().SetBlending(RBLEND_ONE, RBLEND_ONE);

	bool bSpecular = eRRM != RRM_NO_SPECULAR_RIM_LIGHT;

	//Light 범위 밖에 카메라가 있는 Light 렌더링
	{
		REngine::GetDevice().SetCullMode(RCULL_CCW);
		REngine::GetDevice().SetDepthFunc(RCMP_LESSEQUAL);


		if(!PointLightList.empty())
		{
			RShaderFX* pShader = pLightingShader->CreateShaderForPointLight( bSpecular);
			RenderLight(pCurrentCamera, &PointLightList, pShader);
		}

		if(!AreaLightList.empty())
		{
			RShaderFX* pShader = pLightingShader->CreateShaderForAreaLight( bSpecular);
			RenderLight(pCurrentCamera, &AreaLightList, pShader);
		}
	}


	//Light 범위 안에 카메라가 있는 Light 렌더링
	{
		REngine::GetDevice().SetCullMode(RCULL_CW);
		REngine::GetDevice().SetDepthFunc(RCMP_GREATER);

		if(!PointLightWithCameraList.empty())
		{
			RShaderFX* pShader = pLightingShader->CreateShaderForPointLight( bSpecular);
			RenderLight(pCurrentCamera, &PointLightWithCameraList, pShader);
		}

		if(!AreaLightWithCameraList.empty())
		{
			RShaderFX* pShader = pLightingShader->CreateShaderForAreaLight( bSpecular);
			RenderLight(pCurrentCamera, &AreaLightWithCameraList, pShader);	
		}
	}


	//Restore Render State
	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetBlending(RBLEND_NONE);
	REngine::GetDevice().SetCullMode(RCULL_CW);
	REngine::GetDevice().SetDepthFunc(RCMP_LESSEQUAL);
}

void RPointLightPassRenderer::RenderLight(RCameraSceneNode* pCurrentCamera, RSceneNodeList* pLightList, RShaderFX* pShader)
{
	RPFC_THISFUNC;
	int nLightCount = UpdateInstanceVertexBuffer(pCurrentCamera, pLightList);
	if(nLightCount==0)
		return;

	//Set Render State
	REngine::GetDevice().SetVertexBuffer(m_GeometryBuffer, 0, 0);
	REngine::GetDevice().SetIndexBuffer(m_GeometryIndexBuffer);
	REngine::GetDevice().SetupForRenderInstancing(m_InstanceBuffer, nLightCount, m_nOffsetForRender*sizeof(PL_INSTANCE_DATA));
	REngine::GetDevice().SetVertexFormat(m_VertexFormat);

	//Rendering
	RTechnique technique = pShader->GetTechnique( "PointLightTechnique" );
	technique.Begin(NULL, TECH_PRESERVE_NOT);
	technique.BeginPass();
	REngine::GetDevice().DrawIndexedPrimitive(RPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	technique.EndPass();
	technique.End();


	//Restore RenderState
	REngine::GetDevice().RestoreForRenderInstancing();
}

}
