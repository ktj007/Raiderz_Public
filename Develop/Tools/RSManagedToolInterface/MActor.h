#pragma once

#include "MFramework.h"
#include "MSceneNode.h"

#pragma unmanaged
#include "RActor.h"
#pragma managed

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MSceneNode;
	public __gc class MMesh;

	public __gc class MActor : public MSceneNode
	{
	public:
		MActor();
		MActor(RSceneNode* pRealSceneNode);
		~MActor();

		bool Create(MMesh* pMesh);
		void Destroy();
		bool SetAnimation(RSManaged::RAniPartsMode animode,System::String* ani_name);

		//void Frame(float fTime);
		//void Render(float fTime);

		__property rs3::RActor* get_NativeActor();
		__property bool get_Valid();
	protected:
		rs3::RActor* m_pActor;
	};
}