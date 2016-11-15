#pragma once

#include "MFramework.h"
#include "MSceneNode.h"

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MCameraSceneNode : public MSceneNode
	{
	public:
		MCameraSceneNode();
		MCameraSceneNode(RSceneNode* pRealSceneNode);
		~MCameraSceneNode();

		void Set(Vector3 pos, Vector3 dir, Vector3 up);
		void SetFov(float fFovY);
		void SetFov(float fFovY,float fAspectRatio);
		void SetNearFarZ(float fNearZ,float fFarZ);

		__property Vector3 get_Position();
		__property Vector3 get_Direction();
	protected:

	};

	public __gc class MPolarCameraSceneNode : public MCameraSceneNode
	{
	public:
		MPolarCameraSceneNode();
		MPolarCameraSceneNode(RSceneNode* pRealSceneNode);
		~MPolarCameraSceneNode();
		
		void UpdateCamera();
		
		__property void set_Coord(RSMPolarCoord pc);
		__property RSMPolarCoord get_Coord();

		__property void set_LookAt(Vector3 at);
		__property Vector3 get_LookAt();
	};
}
