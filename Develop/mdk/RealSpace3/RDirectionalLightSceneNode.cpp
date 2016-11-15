#include "StdAfx.h"
#include "RDirectionalLightSceneNode.h"
#include "RSceneManager.h"
#include "RToken.h"

namespace rs3 {

RDirectionalLightSceneNode::RDirectionalLightSceneNode(RRENDERPASS eRenderPass)
: RLightSceneNode(eRenderPass)
, m_diffuse(1.0f, 1.0f, 1.0f, 1.0f)
, m_ambient(0.4f, 0.4f, 0.4f, 1.0f)
, m_specular(1.0f, 1.0f, 1.0f, 1.0f)
, m_fPower(1.0f)
, m_fSize(10.f)
, m_fShadowLuminosity(DEFAULT_SHADOW_LUMINOSITY)
, m_fSkySpecular(0.0f)
{
	InitFakeAABB();	//거짓된 AABB정보생성
}

RDirectionalLightSceneNode::~RDirectionalLightSceneNode(void)
{
}


bool RDirectionalLightSceneNode::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	char szBuffer[128];
	szBuffer[0] = '\0';

	MXmlElement *pElement = NULL;
	if ( pElement = element.FirstChildElement("SHADOWLUMINOSITY"))
		_Contents(&m_fShadowLuminosity, pElement);

	if ( pElement = element.FirstChildElement("POWER"))
		_Contents(&m_fPower, pElement);

	szBuffer[0] = '\0';
	if ( pElement = element.FirstChildElement("AMBIENT"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer,"%f %f %f",&m_ambient.x,&m_ambient.y,&m_ambient.z);
	}
	if ( pElement = element.FirstChildElement("DIFFUSE"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer,"%f %f %f",&m_diffuse.x,&m_diffuse.y,&m_diffuse.z);
	}
	if ( pElement = element.FirstChildElement("SPECULAR"))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer,"%f %f %f",&m_specular.x,&m_specular.y,&m_specular.z);
	}
	if ( pElement = element.FirstChildElement("SKYSPECULAR"))
		_Contents(&m_fSkySpecular, pElement);

	return true;
}

void RDirectionalLightSceneNode::SaveToXML(MXmlElement &element)
{
	char szBuffer[256];
	szBuffer[0] = '\0';

	MXmlElement *pElement = new MXmlElement("SHADOWLUMINOSITY");
	_SetContents(pElement,m_fShadowLuminosity);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("POWER");
	_SetContents(pElement,m_fPower);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("AMBIENT");
	sprintf(szBuffer,"%.3f %.3f %.3f",m_ambient.x,m_ambient.y,m_ambient.z);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("DIFFUSE");
	sprintf(szBuffer,"%.3f %.3f %.3f",m_diffuse.x,m_diffuse.y,m_diffuse.z);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("SPECULAR");
	sprintf(szBuffer,"%.3f %.3f %.3f",m_specular.x,m_specular.y,m_specular.z);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("SKYSPECULAR");
	_SetContents(pElement,m_fSkySpecular);
	element.LinkEndChild(pElement);
}

void RDirectionalLightSceneNode::OnAddScene()
{
	_ASSERT( NULL != GetRootSceneNode() );
	// 없으면 등록한다
	if( GetRootSceneNode() )
	{
		//TODO : 기존에 DirectionalLight를 월드씬에서 삭제하는 코드가 필요함

		GetRootSceneNode()->SetDirectionalLight(this);
	}
}

void RDirectionalLightSceneNode::OnRemoveScene()
{
	// this가 기본광원이면 제거한다
	if( GetRootSceneNode() && GetRootSceneNode()->GetDirectionalLight() == this )
	{
			GetRootSceneNode()->SetDirectionalLight(NULL);
	}
}

void RDirectionalLightSceneNode::InitFakeAABB()
{
	RAABB aabb;
	aabb.vmin = RVector(-1,-1,-1);
	aabb.vmax = RVector( 1, 1, 1);
	SetAABB( aabb );
}



bool RSubLightSceneNode::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	char szBuffer[128];
	szBuffer[0] = '\0';

	MXmlElement *pElement = NULL;
	if ( pElement = element.FirstChildElement(RTOK_COLOR))
	{
		_Contents(szBuffer, pElement);
		sscanf(szBuffer,"%f %f %f",&m_color.x,&m_color.y,&m_color.z);
	}

	return true;
}

void RSubLightSceneNode::SaveToXML(MXmlElement &element)
{
	char szBuffer[256];
	szBuffer[0] = '\0';

	MXmlElement* pElement = new MXmlElement(RTOK_COLOR);
	sprintf(szBuffer,"%.3f %.3f %.3f",m_color.x,m_color.y,m_color.z);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);
}

RSceneNode*	RSubLightSceneNode::Clone()
{
	RSubLightSceneNode* pSubLight = new RSubLightSceneNode;

	// common
	pSubLight->m_strNodeName = m_strNodeName;
	pSubLight->SetTransform(GetLocalTransform());

	// property
	pSubLight->SetColorVector(GetColorVector());

	return pSubLight;
}

}
