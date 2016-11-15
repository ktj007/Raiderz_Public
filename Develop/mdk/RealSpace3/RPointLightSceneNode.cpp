#include "StdAfx.h"
#include "RPointLightSceneNode.h"
#include "RToken.h"

namespace rs3 {

MImplementRTTI(RPointLightSceneNode, RLightSceneNode);

RPointLightSceneNode::RPointLightSceneNode(RRENDERPASS eRenderPass) 
: RLightSceneNode(eRenderPass), m_vAreaRange(RVector::ZERO), m_fRadius( 100.f )
{
}

RPointLightSceneNode::~RPointLightSceneNode(void)
{
}

bool RPointLightSceneNode::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	//PointLightSceneNode는 BackgroundLoading을 사용하지 않습니다.

	char szTagName[256];
	char szContents[256];

	szTagName[0] = '\0';
	szContents[0] = '\0';

	MXmlElement* pChild = element.FirstChildElement();
	for ( ; pChild != NULL; pChild = pChild->NextSiblingElement() )
	{
//		if(!LoadCommonProperty(*pChild))
		{
			strcpy(szTagName, pChild->Value());
			_Contents(szContents, pChild);

			if (stricmp(szTagName,RTOK_COLOR)==0)			sscanf(szContents,"%f %f %f",&m_color.x,&m_color.y,&m_color.z); else
			if (stricmp(szTagName,RTOK_INTENSITY)==0)		sscanf(szContents,"%f",&m_fIntensity); else
			if (stricmp(szTagName,RTOK_ATTNEND)==0)			sscanf(szContents,"%f",&m_fRadius); else
			if (stricmp(szTagName,RTOK_ATTNSTART)==0)		sscanf(szContents,"%f",&m_fAttenuationStart); else
			if (stricmp(szTagName,"AREARANGE")==0)			sscanf(szContents,"%f %f %f",&m_vAreaRange.x,&m_vAreaRange.y,&m_vAreaRange.z);
		}
	}

	// 실제 적용되는 Intensity를 초기화
	m_fApplyIntensity = m_fIntensity;

	//Actor가 부모가 될 경우 Actor와 같이 애니메이션되게 하기 위해
	//매프레임 업데이트해준다.
	if( GetParent() )
		SetAutoUpdate(true);

	return true;
}

void RPointLightSceneNode::SaveToXML(MXmlElement &element)
{
	char buffer[256];

	MXmlElement* pChild = NULL;
	pChild = new MXmlElement(RTOK_COLOR);
	element.LinkEndChild(pChild);


	sprintf(buffer,"%.3f %.3f %.3f",m_color.x,m_color.y,m_color.z);
	_SetContents(pChild, buffer);

	pChild = new MXmlElement(RTOK_INTENSITY);
	element.LinkEndChild(pChild);
	_SetContents(pChild, m_fIntensity);

	pChild = new MXmlElement(RTOK_ATTNEND);
	element.LinkEndChild(pChild);
	_SetContents(pChild, m_fRadius);

	pChild = new MXmlElement(RTOK_ATTNSTART);
	element.LinkEndChild(pChild);
	_SetContents(pChild, m_fAttenuationStart);

	pChild = new MXmlElement("AREARANGE");
	element.LinkEndChild(pChild);
	sprintf(buffer,"%.3f %.3f %.3f",m_vAreaRange.x,m_vAreaRange.y,m_vAreaRange.z);	
	_SetContents(pChild, buffer);
}

float RPointLightSceneNode::GetScale()
{
	RVector3 vScale = GetScaleFromWorldTransform();
	return max(max(vScale.x, vScale.y), vScale.z);
}

void RPointLightSceneNode::UpdateAABB()
{
	_ASSERT(m_fRadius >= 0.0f);

	//Actor에 붙어 애니메이션될 경우 부모의 스케일값을 적용하기위해
	//Light의 범위를 스케일
	float fScale = GetScale();

	if(IsAreaLight())
	{
		RVector3 vmin = RVector3(-1,-1,-1)*(RVector3(m_fRadius)+m_vAreaRange*0.5f) *fScale;
		RVector3 vmax = RVector3(1,1,1)*(RVector3(m_fRadius)+m_vAreaRange*0.5f) *fScale;
		RAABB localAABB(vmin, vmax);

		RAABB worldAABB;
		TransformBox(&worldAABB, localAABB, GetWorldTransform());
		SetAABB( worldAABB );
	}
	else
	{
		float fRadius = m_fRadius*fScale;

		RVector3 center = GetWorldPosition();

		RAABB aabb;
		aabb.vmin = center - fRadius;
		aabb.vmax = center + fRadius;
		SetAABB( aabb );
	}
}

void RPointLightSceneNode::OnUpdateAABB()
{
	UpdateAABB();
}

bool RPointLightSceneNode::OnUpdateSphere()
{
	float fRadius = 0;
	
	if(IsAreaLight())
		fRadius =GetAABB().GetRadius();
	else
	{
		//Actor에 붙어 애니메이션될 경우 부모의 스케일값을 적용하기위해
		//Light의 범위를 스케일
		float fScale = GetScale();
		fRadius =m_fRadius*fScale;
	}

	RSphere boundingSphere( GetWorldPosition(), fRadius );
	SetSphere( boundingSphere );

	return true;
}

RSceneNode*	RPointLightSceneNode::Clone()
{
	RPointLightSceneNode* pPointLight = new RPointLightSceneNode;
	
	// common
	pPointLight->m_strNodeName = m_strNodeName;
	pPointLight->SetTransform(GetLocalTransform());

	// property
	pPointLight->m_color = m_color;
	pPointLight->m_fIntensity = m_fIntensity;

	pPointLight->m_fApplyIntensity = m_fApplyIntensity;

	pPointLight->m_fRadius = m_fRadius;
	pPointLight->m_fAttenuationStart = m_fAttenuationStart;

	pPointLight->SetAreaRange(m_vAreaRange);
	
	if( IsAutoUpdate() )
		pPointLight->SetAutoUpdate(true);
	
	return pPointLight;
}

ROBB RPointLightSceneNode::GetOBB()
{
	ROBB obb(GetWorldTransform());
	obb.vRange = (RVector3(m_fRadius)+m_vAreaRange*0.5f) *GetScale();
	
	return obb;
}

}