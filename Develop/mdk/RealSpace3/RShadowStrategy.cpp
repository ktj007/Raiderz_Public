#include "stdafx.h"

#include "d3d9.h"
#pragma comment(lib, "d3d9.lib")

#include "RShadowStrategy.h"
#include "RSceneManager.h"

#include "RPointLightSceneNode.h"
#include "RActor.h"
#include "RTerrain.h"

#include "RCameraSceneNode.h"
#include "RViewFrustum.h"

#include "RDeviceD3D.h"

#include "MObox.h"
#include "RFont.h"

#include "RRenderHelper2.h"

#include "RTreePassRenderer.h"
#include "RTreeSceneNode.h"

#include "RRenderingCallbackInterface.h"

namespace rs3 {

RShadowDesc::RShadowDesc() : eMethod(SHADOW_PSSM), nTextureNum(3), nTextureSize(512), bDoubleShadow(true)
{
	fSplitDistance[0] = 500.f;
	fSplitDistance[1] = 1500.f;
	fSplitDistance[2] = 10000.f;
	fSplitDistance[3] = 20000.f;
}

RShadowStrategy::RShadowStrategy()
: m_bShadowReceiveEnable(false)
, m_bNeedHWFiltering(false)
{
	m_shadowDesc.eMethod = SHADOW_NONE;
}

RShadowStrategy::~RShadowStrategy()
{
}

}
