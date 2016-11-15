#include "stdafx.h"
#include "MDevice.h"

using namespace RSManaged;
using namespace System;

MDevice::MDevice() : m_pDevice(NULL)
{

}

void MDevice::Set(RDeviceD3D* pDev)
{
	m_pDevice = pDev;
}

bool MDevice::Init(IntPtr hWnd, int nWidth, int nHeight, bool bFullScreen, RSManaged::RFORMAT format)
{
	if (m_pDevice)
	{
		//m_pDevice = (rs3::RDeviceD3D*)createDevice();
		//m_pDevice->Create((HWND)hWnd.ToInt32(), nWidth, nHeight, bFullScreen, (rs3::RFORMAT)format);
		m_pDevice->Create((HWND)hWnd.ToInt32());
//		m_pDevice->SetBackScreenColor(0xFF000000);
	}

	return true;
}

void MDevice::BeginScene()
{
	if (m_pDevice) m_pDevice->BeginScene();
}

void MDevice::EndScene()
{
	if (m_pDevice) m_pDevice->EndScene();
}

void MDevice::Flip()
{
	if (m_pDevice) m_pDevice->Flip();
}

void MDevice::Clear(unsigned int dwColor)
{
	m_pDevice->Clear(true, true, true, dwColor);
}

void MDevice::SetTexture(int nStage, RSManaged::HTEXTURE nTexture)
{
	m_pDevice->SetTexture(nStage, (rs3::HTEXTURE)nTexture);
}

//void MDevice::SetTexture(const char *name, RSManaged::HTEXTURE nTexture)
//{
//	m_pDevice->SetTexture
//}

void MDevice::SetTextureStageState(int nStage, RSManaged::RTEXTURESTAGESTATETYPE nStageStateType, unsigned int value)
{
	m_pDevice->SetTextureStageState(nStage, (rs3::RTEXTURESTAGESTATETYPE)nStageStateType, value);
}

void MDevice::SetTextureFilter(int nSampler, RSManaged::RTEXTUREFILTERTYPE type)
{
	m_pDevice->SetTextureFilter(nSampler, (rs3::RTEXTUREFILTERTYPE)type);
}

void MDevice::SetTextureAddress(int nSampler, RSManaged::RTEXTUREADDRESS u, RSManaged::RTEXTUREADDRESS v, RSManaged::RTEXTUREADDRESS w)
{
	m_pDevice->SetTextureAddress(nSampler, (rs3::RTEXTUREADDRESS)u, (rs3::RTEXTUREADDRESS)v, (rs3::RTEXTUREADDRESS)w);
}

void MDevice::SetTextureAddressClamp(int nSampler)
{
	m_pDevice->SetTextureAddressClamp(nSampler);
}

void MDevice::SetTextureAddressWrap(int nSampler)
{
	m_pDevice->SetTextureAddressWrap(nSampler);
}

void MDevice::SetDepthFunc(RSManaged::RCMPFUNC depthFunc)
{
	m_pDevice->SetDepthFunc((rs3::RCMPFUNC)depthFunc);
}

void MDevice::SetDepthEnable(bool bEnable, bool bWriteEnable)
{
	m_pDevice->SetDepthEnable(bEnable, bWriteEnable);
}

void MDevice::SetColorWriteEnable(bool bEnable)
{
	m_pDevice->SetColorWriteEnable(bEnable);
}

void MDevice::SetCullMode(RSManaged::RCULL cullMode)
{
	m_pDevice->SetCullMode((rs3::RCULL)cullMode);
}
void MDevice::SetFillMode(RSManaged::RFILLMODE fillMode)
{
	m_pDevice->SetFillMode((rs3::RFILLMODE)fillMode);
}

//void MDevice::SetClipPlanes(const RPlane *pPlanes, int nCount)
//{
//	
//}
void MDevice::SetDepthBias(float fDepthBias, float fSlopeScaleDepthBias)
{
	m_pDevice->SetDepthBias(fDepthBias, fSlopeScaleDepthBias);
}

void MDevice::SetStencilEnable(bool bEnable)
{
	m_pDevice->SetStencilEnable(bEnable);
}
void MDevice::SetStencilTwoSide(bool bEnable)
{
	m_pDevice->SetStencilTwoSide(bEnable);
}
void MDevice::SetStencilRef(unsigned int dwValue)
{
	m_pDevice->SetStencilRef(dwValue);
}

void MDevice::SetStencilMask(unsigned int dwValue)
{
	m_pDevice->SetStencilMask(dwValue);
}
void MDevice::SetStencilWriteMask(unsigned int dwValue)
{
	m_pDevice->SetStencilWriteMask(dwValue);
}
void MDevice::SetStencilFunc(RSManaged::RCMPFUNC stencilFunc)
{
	m_pDevice->SetStencilFunc((rs3::RCMPFUNC)stencilFunc);
}
void MDevice::SetStencilOp(RSManaged::RSTENCILOP opPass,RSManaged::RSTENCILOP opFail,RSManaged::RSTENCILOP opZFail)
{
	m_pDevice->SetStencilOp((rs3::RSTENCILOP)opPass , (rs3::RSTENCILOP)opFail , (rs3::RSTENCILOP)opZFail );
}
void MDevice::SetStencilCCWFunc(RSManaged::RCMPFUNC stencilFunc)
{
	m_pDevice->SetStencilCCWFunc((rs3::RCMPFUNC)stencilFunc);
}

// twosideÀÇ µÞ¸é
void MDevice::SetStencilCCWOp(RSManaged::RSTENCILOP opPass,RSManaged::RSTENCILOP opFail,RSManaged::RSTENCILOP opZFail)
{
	m_pDevice->SetStencilCCWOp((rs3::RSTENCILOP)opPass , (rs3::RSTENCILOP)opFail , (rs3::RSTENCILOP)opZFail );
}	

// light

void MDevice::SetLight(int index,void* pLight)
{
	m_pDevice->SetLight(index, pLight);
}

void MDevice::SetLightEnable(int index,bool bEnable)
{
	m_pDevice->SetLightEnable(index, bEnable);
}

void MDevice::SetLighting(bool bEnable)
{
	m_pDevice->SetLighting(bEnable);
}

// material 

void MDevice::SetMaterial(void* pMtrl)
{
	m_pDevice->SetMaterial(pMtrl);
}

// render state

void MDevice::SetNormalizeNormals(bool bEnable)
{
	m_pDevice->SetNormalizeNormals(bEnable);
}

void MDevice::SetAmbient(unsigned int dwColor)
{
	m_pDevice->SetAmbient(dwColor);
}

//void MDevice::SetAlphaBlendEnable(bool bEnable)
//{
//	m_pDevice->SetAlphaBlendEnable(bEnable);
//}

void MDevice::SetAlphaTestEnable(bool bEnable)
{
	m_pDevice->SetAlphaTestEnable(bEnable);
}
void MDevice::SetAlphaRef(unsigned int dwRef)
{
	m_pDevice->SetAlphaRef(dwRef);
}
void MDevice::SetAlphaFunc(RSManaged::RCMPFUNC Func)
{
	m_pDevice->SetAlphaFunc((rs3::RCMPFUNC)Func);
}
void MDevice::SetClipping(bool bEnable)
{
	m_pDevice->SetClipping(bEnable);
}

void MDevice::SetFogEnable(bool bEnable)
{
	m_pDevice->SetFogEnable(bEnable);
}
void MDevice::SetVertexBlendEnable(bool bEnable)
{
	m_pDevice->SetVertexBlendEnable(bEnable);
}
void MDevice::SetIndexedVertexBlendEnable(bool bEnable)
{
	m_pDevice->SetIndexedVertexBlendEnable(bEnable);
}
void MDevice::SetSpecularEnable(bool bEnable)
{
	m_pDevice->SetSpecularEnable(bEnable);
}
