#include "StdAfx.h"
#include "RMeshCollision.h"
#include "MCollision.h"

namespace rs3 {

////////////////////////////////////////////////////////////////
// RMeshCollision

RMeshCollision::RMeshCollision(void)
{
}

RMeshCollision::~RMeshCollision(void)
{
	while(!empty())
	{
		delete back();
		pop_back();
	}
}

bool RMeshCollision::Load(MXmlElement& collisionElement)
{
	MXmlElement* pElement = collisionElement.FirstChildElement();
	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		MCollision* pCollision = MCollision::CreateFromElement(pElement);
		if(pCollision)
		{
			pCollision->Load(pElement);

			push_back(pCollision);
		}
	}

	struct SFuncLessCollision
	{
		bool operator()(const MCollision* _pLhs, const MCollision* _pRhs)
		{
			return _pLhs->m_strParentName < _pRhs->m_strParentName;
		}
	};

	std::sort( begin(), end(), SFuncLessCollision() );
	return true;
}

void RMeshCollision::GetCollsionFromParentName( std::vector<MCollision*>& _rOutCollisions, const std::string& _rParentSceneNodeName )
{
	MCollision compareDummy(MCollision::TYPE_MAX);
	compareDummy.m_strParentName = _rParentSceneNodeName;

	struct SFuncEqualCollsion
	{
		bool operator()(const MCollision* _pLhs, const MCollision* _pRhs)
		{
			return (_pLhs->m_strParentName < _pRhs->m_strParentName);
		}
	};

	MCollisionsRangePair collisionsRagnePair = std::equal_range( begin(), end(), &compareDummy, SFuncEqualCollsion() );
	if (collisionsRagnePair.first != collisionsRagnePair.second)
		_rOutCollisions.assign(collisionsRagnePair.first, collisionsRagnePair.second);
}

MCollision* RMeshCollision::GetCollsionFromName(const std::string& _rSceneNodeName)
{
	for(vector<MCollision*>::iterator itr = begin(); itr != end(); ++itr)
	{
		if((*itr)->m_strName == _rSceneNodeName)
			return *itr;
	}
	return NULL;

}

}