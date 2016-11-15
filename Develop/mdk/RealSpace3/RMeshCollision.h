#pragma once

#include <vector>
#include "RLibTypes.h"
#include "MXml.h"

namespace rs3 {

class MCollision;
typedef std::vector<MCollision*>					MCollisionVector;
typedef MCollisionVector::iterator					MCollisionItr;
typedef std::pair< MCollisionItr, MCollisionItr >	MCollisionsRangePair;

#pragma warning ( push )
#pragma warning ( disable : 4275 )
class RS_API RMeshCollision : public vector<MCollision*>
{
public:
	RMeshCollision(void);
	~RMeshCollision(void);

	bool Load(MXmlElement& collisionElement);
	void GetCollsionFromParentName(std::vector<MCollision*>& _rOutCollisions, const std::string& _rParentSceneNodeName);
	MCollision* GetCollsionFromName(const std::string& _rSceneNodeName);
};
#pragma warning ( pop )

}