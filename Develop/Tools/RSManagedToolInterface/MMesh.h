#pragma once

#include "MFramework.h"

#pragma unmanaged
#include "RMesh.h"
#pragma managed

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MMesh
	{
	public:
		MMesh();
		MMesh(rs3::RMesh* pMesh);
		~MMesh();
		

		__property bool get_Valid();
		__property rs3::RMesh* get_NativeMesh();
	protected:
		rs3::RMesh* m_pMesh;
	};
}
