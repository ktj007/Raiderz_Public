#pragma once

#include "d3d9.h"
#include <windef.h>
#include <windows.h>
#include "Vector3.h"
#include "Matrix4.h"
#include "MEnums.h"

#pragma unmanaged
#include "RTexture.h"
#include "RTypes.h"
#include "RToolApplication.h"
#include "RLibTypes.h"
#include "RDeviceD3D.h"
#include "RDevice.h"
#include "RSceneManager.h"
#include "RSceneNode.h"
#include "RLightSceneNode.h"
#include "RPolarCameraSceneNode.h"
#include "RAABBTreeSceneNode.h"
#include "RToolHelper.h"
#include "RD3DVertexUtil.h"

#ifdef _DEBUG
#pragma comment (lib, "../../RealSpace3/DebugDLL/RealSpace3.lib")
#else
#pragma comment (lib, "../../RealSpace3/ReleaseDLL/RealSpace3.lib")
#endif


#pragma managed

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MDevice;
	public __gc class MSceneManager;
	public __gc class MHelper;
	public __gc class MMeshMgr;

    // MFramework class.
    public __gc class MFramework
    {
    public:
        // Properties.
        __property static MFramework*	get_Instance();
		__property MDevice*				get_Device();
		__property MSceneManager*		get_SceneManager();
		__property MHelper*				get_Helper();
		__property MMeshMgr*			get_MeshMgr();

        static void Init();
        static void Final();
		static bool InstanceIsValid();

    protected:
        MFramework();

        // Static this pointer for Singleton.
        static MFramework* ms_pkThis = 0;

		rs3::RToolApplication* m_pApplication;
		MDevice* m_pDevice;
		MSceneManager* m_pSceneManager;
		MHelper*		m_pHelper;
		MMeshMgr*		m_pMeshMgr;
    };

}
