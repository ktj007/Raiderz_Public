#pragma once

#include "MFramework.h"

using namespace System;
using namespace rs3;

namespace RSManaged
{
	typedef int HTEXTURE;

	public __gc class MDevice
	{
	public:
		MDevice();
		void Set(RDeviceD3D* pDev);
		bool Init(IntPtr hWnd, int nWidth, int nHeight, bool bFullScreen, RSManaged::RFORMAT format);
		void BeginScene();
		void EndScene();
		void Flip();
		void Clear(unsigned int dwColor);


	void SetTexture(int nStage, RSManaged::HTEXTURE nTexture);
	//void SetTexture(const char *name, RSManaged::HTEXTURE nTexture);
	void SetTextureStageState(int nStage, RSManaged::RTEXTURESTAGESTATETYPE nStageStateType, unsigned int value);
	void SetTextureFilter(int nSampler, RSManaged::RTEXTUREFILTERTYPE type);
	void SetTextureAddress(int nSampler, RSManaged::RTEXTUREADDRESS u, RSManaged::RTEXTUREADDRESS v, RSManaged::RTEXTUREADDRESS w);
	void SetTextureAddressClamp(int nSampler);
	void SetTextureAddressWrap(int nSampler);

	void SetDepthFunc(RSManaged::RCMPFUNC depthFunc);
	void SetDepthEnable(bool bEnable, bool bWriteEnable);
	void SetColorWriteEnable(bool bEnable);
	void SetCullMode(RSManaged::RCULL cullMode);
	void SetFillMode(RSManaged::RFILLMODE fillMode);

	//void SetClipPlanes(const RPlane *pPlanes, int nCount);
	void SetDepthBias(float fDepthBias, float fSlopeScaleDepthBias);

	void SetStencilEnable(bool bEnable);
	void SetStencilTwoSide(bool bEnable);
	void SetStencilRef(unsigned int dwValue);
	void SetStencilMask(unsigned int dwValue);
	void SetStencilWriteMask(unsigned int dwValue);
	void SetStencilFunc(RSManaged::RCMPFUNC stencilFunc);
	void SetStencilOp(RSManaged::RSTENCILOP opPass,RSManaged::RSTENCILOP opFail,RSManaged::RSTENCILOP opZFail);
	void SetStencilCCWFunc(RSManaged::RCMPFUNC stencilFunc);	// twoside¿« µﬁ∏È
	void SetStencilCCWOp(RSManaged::RSTENCILOP opPass,RSManaged::RSTENCILOP opFail,RSManaged::RSTENCILOP opZFail);

	// light

	void SetLight(int index,void* pLight);
	void SetLightEnable(int index,bool bEnable);
	void SetLighting(bool bEnable);

	// material 

	void SetMaterial(void* pMtrl);

	// render state

	void SetNormalizeNormals(bool bEnable);
	void SetAmbient(unsigned int dwColor);

	//void SetAlphaBlendEnable(bool bEnable);

	void SetAlphaTestEnable(bool bEnable);
	void SetAlphaRef(unsigned int dwRef);
	void SetAlphaFunc(RSManaged::RCMPFUNC Func);
	void SetClipping(bool bEnable);

	void SetFogEnable(bool bEnable);
	void SetVertexBlendEnable(bool bEnable);
	void SetIndexedVertexBlendEnable(bool bEnable);
	void SetSpecularEnable(bool bEnable);

	protected:
		RDeviceD3D* m_pDevice;
	};
}
