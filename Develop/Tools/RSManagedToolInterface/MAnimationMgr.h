#pragma once

#include "MFramework.h"

#pragma unmanaged
#include "RAnimationMgr.h"
#pragma managed

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MAnimationMgr
	{
	public:
		MAnimationMgr();
		~MAnimationMgr();
		void DelAll();
		void ReloadAll();
		bool LoadAnimationFileList(System::String* strFileName);
		void Add(System::String* strName, System::String* strFileName,int nID, int nMotionTypeID);
	protected:
		rs3::RAnimationMgr* m_pAnimationMgr;
	};
}
