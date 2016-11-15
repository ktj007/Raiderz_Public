#pragma once

#include "RLib.h"
#include <string>
#include "MXml.h"
#include "MMath.h"

// mesh 의 데미지판정(혹은 충돌)처리를 위한 box,sphere,cylinder 로 간략화된 정보
// 후에 RCollisionTree 와 연동해서 동적인 지형 처리를 위해서 같이 쓰자

namespace rs3 {

class MCollision
{
public:
	enum MCollisionType {
		CYLINDER = 0,
		SPHERE,
		BOX,
		CAPSULE,

		TYPE_MAX
	};

	enum MCollisionMeshType {
		MESHTYPE_NORMAL = 0,	// 일반 충돌용 메쉬
		MESHTYPE_CLOTH,			// 옷감 출돌용 메쉬
	};

	MCollision(MCollisionType type) : m_meshType(MESHTYPE_NORMAL), m_type(type), m_fHeight(0), m_fRadius(0), m_fLength(0) { }

	MCollisionMeshType	m_meshType;
	MCollisionType		m_type;

	MMatrix	m_matLocal;
	string	m_strName;				// Capsule Object Name
	string	m_strParentName;		// 업데이트시에 참조하는 씬노드
	string	m_strRelatedNodeNames;	// 충돌시에 연관있는 노드이름들

	float	m_fRadius;		// x
	float	m_fHeight;		// z
	float	m_fLength;		// y

	// for save
	bool Save(MXmlElement* pParentElement);

	// for load
	static MCollision* CreateFromElement(MXmlElement* pElement);
	bool Load(MXmlElement* pElement);
};

}
