#pragma once

#include "MFramework.h"
#include "MMesh.h"
#include "MActor.h"

#pragma unmanaged
#include "RActorMgr.h"
#pragma managed


using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MActorMgr
	{
	public:
		MActorMgr();
		~MActorMgr();

		int Add(MMesh* pMesh);
		int Add(MActor* pActor);
		void Del(int id);
		void Del(MActor* pActor);
		void DelAll();
		void Frame(float fTime);
		void Frame(float fTime, int id);
		MActor* GetFast(int id);

	protected:
		rs3::RActorMgr* m_pActorMgr;
	};
}
