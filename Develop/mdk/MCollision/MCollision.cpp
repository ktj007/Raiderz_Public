#include "StdAfx.h"
#include "MCollision.h"

#pragma warning( disable : 4996 )

namespace rs3 {

MCollision* MCollision::CreateFromElement(MXmlElement* pElement)
{
	if(_stricmp(pElement->Value(),"SPHERE")==0)
		return new MCollision(SPHERE);
	if(_stricmp(pElement->Value(),"CYLINDER")==0)
		return new MCollision(CYLINDER);
	if(_stricmp(pElement->Value(),"BOX")==0)
		return new MCollision(BOX);
	if(_stricmp(pElement->Value(),"CAPSULE")==0)
		return new MCollision(CAPSULE);
	return NULL;
}

bool MCollision::Save(MXmlElement* pParentElement)
{
	char szBuffer[256];
	MXmlElement* pElement = NULL;

	switch(m_type)
	{
		case CYLINDER:
			pElement = new MXmlElement("CYLINDER");
			_SetAttribute(pElement,"radius",m_fRadius);
			_SetAttribute(pElement,"height",m_fHeight);
			break;
		case SPHERE:
			pElement = new MXmlElement("SPHERE");
			_SetAttribute(pElement,"radius",m_fRadius);
			break;
		case BOX:
			pElement = new MXmlElement("BOX");
			sprintf(szBuffer,"%f %f %f", m_fRadius, m_fLength, m_fHeight);
			_SetAttribute(pElement,"size",szBuffer);
			break;
		case CAPSULE:
			pElement = new MXmlElement("CAPSULE");
			_SetAttribute(pElement,"radius",m_fRadius);
			_SetAttribute(pElement,"height",m_fHeight);
			break;
		default:
			_ASSERT(FALSE);
			return false;
	}
	pParentElement->LinkEndChild(pElement);

	switch (m_meshType)
	{
	case MESHTYPE_NORMAL:
		_SetAttribute(pElement,"meshtype","normal");
		break;
	case MESHTYPE_CLOTH:
		_SetAttribute(pElement,"meshtype","cloth");
		_SetAttribute(pElement,"related", m_strRelatedNodeNames);
		break;
	default:
		_ASSERT(FALSE);
		return false;
	}

	_SetAttribute(pElement,"name",m_strName);
	_SetAttribute(pElement,"parent",m_strParentName);

	sprintf(szBuffer,"%f %f %f", m_matLocal._11, m_matLocal._12, m_matLocal._13 );
	_SetAttribute(pElement,"row0",szBuffer);

	sprintf(szBuffer,"%f %f %f", m_matLocal._21, m_matLocal._22, m_matLocal._23 );
	_SetAttribute(pElement,"row1",szBuffer);

	sprintf(szBuffer,"%f %f %f", m_matLocal._31, m_matLocal._32, m_matLocal._33 );
	_SetAttribute(pElement,"row2",szBuffer);

	sprintf(szBuffer,"%f %f %f", m_matLocal._41, m_matLocal._42, m_matLocal._43 );
	_SetAttribute(pElement,"row3",szBuffer);
	return true;
}

bool MCollision::Load(MXmlElement* pElement)
{
	char szBuffer[256];
	switch(m_type)
	{
	case CAPSULE:
	case CYLINDER:
		_Attribute(&m_fRadius,pElement,"radius");
		_Attribute(&m_fHeight,pElement,"height");
		break;
	case SPHERE:
		_Attribute(&m_fRadius,pElement,"radius");
		break;
	case BOX:
		_Attribute(szBuffer,pElement,"size");
		sscanf(szBuffer,"%f %f %f", &m_fRadius, &m_fLength, &m_fHeight);
		break;
	default:
		_ASSERT(FALSE);
		return false;
	}

	_Attribute(szBuffer,pElement,"meshtype");
	if(strcmp(szBuffer,"normal")==0)
		m_meshType = MESHTYPE_NORMAL;
	else if(strcmp(szBuffer,"cloth")==0)
	{
		m_meshType = MESHTYPE_CLOTH;
		_Attribute(m_strRelatedNodeNames, pElement, "related");
	}

	_Attribute(m_strName,pElement,"name");
	_Attribute(m_strParentName,pElement,"parent");

	// tm
	_Attribute(szBuffer,pElement,"row0");
	sscanf(szBuffer,"%f %f %f",&m_matLocal._11,&m_matLocal._12,&m_matLocal._13);
	_Attribute(szBuffer,pElement,"row1");
	sscanf(szBuffer,"%f %f %f",&m_matLocal._21,&m_matLocal._22,&m_matLocal._23);
	_Attribute(szBuffer,pElement,"row2");
	sscanf(szBuffer,"%f %f %f",&m_matLocal._31,&m_matLocal._32,&m_matLocal._33);
	_Attribute(szBuffer,pElement,"row3");
	sscanf(szBuffer,"%f %f %f",&m_matLocal._41,&m_matLocal._42,&m_matLocal._43);

	m_matLocal._14 = m_matLocal._24 = m_matLocal._34 = 0; m_matLocal._44 = 1;
	return true;
}


}