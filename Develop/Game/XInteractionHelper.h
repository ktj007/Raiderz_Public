#pragma once

#include "RDecalProjectionSceneNode.h"

class XObject;

/// 인터랙션과 관련된 헬퍼 - 계산 공식 등
class XInteractionHelper
{
public:
	static RBoundingBox GetObjectAABB(XObject* pObject);
	static vec3 GetInteractionCenter( XObject* pObject, RBoundingBox& rbb );
	static float GetInteractionSize( RBoundingBox& rbb );

	static bool IsLootableDistance(vec3 vMyPos, XObject* pTarget);

	static float GetInteractionDistance(XObject* pObject);

	static bool IsInteractableNPC( XNonPlayer* pNPC );
	static bool IsInteractablePC( XPlayer* pPC );

	static NPC_ICON_STATE ConvertNPCIConTypeToIconState( NPC_ICON_TYPE NIT );
};