#include "stdafx.h"
#include "RMemPoolReleaser.h"
#include "RActorNode.h"
#include "RActorNodePhysique.h"
#include "RClothWorldActorNode.h"
#include "RClothLocalActorNode.h"
#include "RTreeSceneNode.h"
#include "RMeshNode.h"
#include "REffectActor.h"

namespace rs3 {

RMemPoolReleaser::~RMemPoolReleaser()
{
	RActorMLeaf::release();
	RActorNodeMLeaf::release();
	RActorNodePhysiqueGPU::release();
	RClothWorldActorNode::release();
	RClothLocalActorNode::release();
	RMeshNodeMLeaf::release();

	// SceneNode
	RTreeSceneNode::release();
	REffectActorMLeaf::release();
}

}