#include "stdafx.h"
#include "XPhysX.h"
#include "XAppErrorCollector.h"

#define XPX_BPART_DYN_SELF_COL "bpart_dyn_self_col"

XPhysX::XPhysX()
: m_nMaterialIdx_BPart(0)
{
}

XPhysX::~XPhysX()
{
	//ClearRagDollInformation();
}

bool XPhysX::Init(bool bUseRemoteDebugger)
{
	const char* pSzDebug = NULL;

	if (bUseRemoteDebugger)
	{
		pSzDebug = "localhost";
	}

	MPhysX::PHYSX_INIT_CODE code = MPhysX::Init(10.f, pSzDebug);

	if (code != PIC_SUCCESS)
	{
		return false;
	}

	NxMaterial* defaultMaterial = m_pScene->getMaterialFromIndex(0); 

	NxMaterialDesc descForBPart;
	defaultMaterial->saveToDesc(descForBPart);
	NxMaterial* materialForBPart = m_pScene->createMaterial(descForBPart);
	materialForBPart->setRestitution(0.01f);		// 탄성 회복도
	materialForBPart->setStaticFriction(500.0f);	// 정지마찰력 : 물체가 처음 움직일때 필요한 마찰력
	materialForBPart->setDynamicFriction(500.0f);	// 운동마찰력 : 물체가 움직일때 적용되는 마찰력
	m_nMaterialIdx_BPart = materialForBPart->getMaterialIndex();

	// BPart용 그룹설정
	AddCollisionGroup(XPX_BPART_DYN_SELF_COL, true, false, false);
	//MPhysX::GetInstance()->AddCollisionGroup(SH_PX_COL_RAGDOLL, false);
	//MPhysX::GetInstance()->SetLOD(MPhysX::PL_NO_STATIC);
	return (PIC_SUCCESS == code);
}

void XPhysX::UpdateXPhysX( float _fElapsed )
{
	PFC_THISFUNC;
	//Sleep(10);

	/*if (REngine::m_strDebugFlags == "a")
	{
		_fElapsed = 10.f;
		REngine::m_strDebugFlags.clear();
	}*/
	MPhysX::Update(_fElapsed);

	//// Stress Test
	//int a = 0;
	//while (true)
	//{
	//	a++;
	//	if (a == 36)
	//	{
	//		break;
	//	}

	//	Sleep(2);
	//}
}

std::wstring XPhysX::GetLastErrorMessage()
{
	NxSDKCreateError _errcode = MPhysX::GetLastError();

	wstring strError;
	switch ( _errcode)
	{
	case NXCE_NO_ERROR :			strError = L"Unknown error";																		break;
	case NXCE_PHYSX_NOT_FOUND :		strError = L"Unable to find the PhysX libraries. The PhysX drivers are not installed correctly.";	break;
	case NXCE_WRONG_VERSION :		strError = L"The application supplied a version number that does not match with the libraries.";	break;
	case NXCE_DESCRIPTOR_INVALID :	strError = L"The supplied SDK descriptor is invalid.";												break;
	case NXCE_CONNECTION_ERROR :	strError = L"A PhysX card was found, but there are problems when communicating with the card.";		break;
	case NXCE_RESET_ERROR :			strError = L"A PhysX card was found, but it did not reset (or initialize) properly.";				break;
	case NXCE_IN_USE_ERROR :		strError = L"A PhysX card was found, but it is already in use by another application.";				break;
	case NXCE_BUNDLE_ERROR :		strError = L"A PhysX card was found, but there are issues with loading the firmware.";				break;
	default :						strError = L"Unknown error";																		break;
	}

	return strError;
}

int XPhysX::GetLastError()
{
	NxSDKCreateError _errcode = MPhysX::GetLastError();
	return (int)(_errcode);
}

int XPhysX::GetBPartGroupID()
{
	return GetCollsionGroupId(XPX_BPART_DYN_SELF_COL);
}

//void XPhysX::AddRagDollInformation( const std::wstring& strModelName )
//{
//	if (m_mapRagdollInfo.find(strModelName) != m_mapRagdollInfo.end())
//		return;
//
//	const RPathSplitter* pPathSpliter = REngine::GetConfig().GetPathSplitter( RP_MESH );
//	int nPathCount = pPathSpliter->GetPathCount();
//
//	MXml aXml;
//	std::wstring strRealFileName(strModelName);
//
//	int i = 0;
//	bool bRet = true;
//	while (aXml.LoadFile(MLocale::ConvUTF16ToAnsi(strRealFileName.c_str()).c_str()) == false)
//	{
//		if (i >= nPathCount )
//		{
//			bRet = false;
//			break;
//		}
//
//		strRealFileName = MLocale::ConvAnsiToUTF16(pPathSpliter->GetPath(i).c_str()) + strModelName;
//		++i;
//	}
//
//	if (bRet)
//	{
//		MXmlElement* pElement = aXml.DocHandle().FirstChildElement("XML").ToElement();
//		if (pElement)
//		{
//			MXmlElement* pAvailableElement = new MXmlElement(*pElement);
//			m_mapRagdollInfo.insert( std::map<std::wstring, MXmlElement*>::value_type(strModelName, pAvailableElement) );
//		}
//	}
//}
//
//MXmlElement* XPhysX::GetRagDollInformation( const std::wstring& strModelName )
//{
//	std::map< std::wstring, MXmlElement* >::iterator itr = m_mapRagdollInfo.find(strModelName);
//	if (itr == m_mapRagdollInfo.end())
//	{
//		return NULL;
//	}
//
//	return itr->second;
//}
//
//void XPhysX::ReloadRagDollInformation()
//{
//	std::vector< std::wstring > vecStrInfos;
//	for (std::map< std::wstring, MXmlElement* >::iterator itr = m_mapRagdollInfo.begin(); itr != m_mapRagdollInfo.end(); ++itr)
//	{
//		vecStrInfos.push_back(itr->first);
//	}
//
//	ClearRagDollInformation();
//	for (std::vector< std::wstring >::iterator itr = vecStrInfos.begin(); itr != vecStrInfos.end(); ++itr)
//	{
//		AddRagDollInformation(*itr);
//	}
//}
//
//void XPhysX::ClearRagDollInformation()
//{
//	for (std::map< std::wstring, MXmlElement* >::iterator itr = m_mapRagdollInfo.begin(); itr != m_mapRagdollInfo.end(); ++itr)
//	{
//		delete itr->second;
//	}
//	m_mapRagdollInfo.clear();
//}
