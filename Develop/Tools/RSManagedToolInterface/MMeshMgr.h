#pragma once

#include "MFramework.h"

#pragma unmanaged
#include "RMeshMgr.h"
#pragma managed

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MMesh;

	public __gc class MMeshMgr
	{
	public:
		MMeshMgr();
		~MMeshMgr();

		MMesh*	Add(System::String* name, System::String* modelname);
		int		AddXml(System::String* name, System::String* modelname, bool autoload);
        int		LoadXmlList(System::String* name);
		void	Del(int id);
		void	Del(MMesh* pMesh);

		int		LoadList(System::String* name);
		int		SaveList(System::String* name);

		void	DelAll();

		MMesh*	GetFast(int id);
		MMesh*	Get(System::String* name);
	protected:
		rs3::RMeshMgr* m_pMeshMgr;
	};
}
