#include "stdafx.h"
#include "XMotionMgr.h"
#include "XDef.h"
#include "MLocale.h"

///////////////////////////////////////////////////////////////////////////////
XMotionMgr::XMotionMgr()
{
}

XMotionMgr::~XMotionMgr()
{
	Clear();
}

void XMotionMgr::Clear()
{
	while (!empty())
	{
		iterator itor = begin();
		delete (*itor).second;
		erase(itor);
	}
}

void XMotionMgr::AddMotion(XMotion* pMotion)
{
	pMotion->Cook();

	iterator itor = find(pMotion->GetName());
	if (itor != end())
	{
		mlog("Error : same motion name(%s)\n", MLocale::ConvTCHARToAnsi(pMotion->GetName().c_str()).c_str());
		return;
	}

	insert(value_type(pMotion->GetName(), pMotion));
}

XMotion* XMotionMgr::GetMotion(const TCHAR* szName)
{
	iterator itor = find(tstring(szName));
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}


#define MOTION_XML_TAG_MOTION			"MOTION"
#define MOTION_XML_TAG_MAIET			"maiet"
#define MOTION_XML_TAG_DEFAULT			"Default"
#define MOTION_XML_TAG_FORWARD			"Forward"
#define MOTION_XML_TAG_BACKWARD			"Backward"
#define MOTION_XML_TAG_LEFT				"Left"
#define MOTION_XML_TAG_RIGHT			"Right"
#define MOTION_XML_TAG_SEQUENCE			"Sequence"
#define MOTION_XML_TAG_POST_TRIGGER		"PostTrigger"

#define MOTION_XML_ATTR_NAME			"name"
#define MOTION_XML_ATTR_CANCELENABLED	"cancelEnabled"
#define MOTION_XML_ATTR_WEAPONANI		"weaponAni"
#define MOTION_XML_ATTR_ANI				"ani"
#define MOTION_XML_ATTR_LOOP			"loop"
#define MOTION_XML_ATTR_EVENT			"event"
#define MOTION_XML_ATTR_CAMERA_LOOK		"cameraLook"
#define MOTION_XML_ATTR_CAMERA_LOOK_STANCE		"cameraLookStance"
#define MOTION_XML_ATTR_STANCE			"stance"
#define MOTION_XML_ATTR_FRAMEOVER		"frameover"
#define MOTION_XML_ATTR_TIMEOUT			"timeout"
#define MOTION_XML_ATTR_SPEED			"speed"
#define MOTION_XML_ATTR_TRIGGER_IF		"if"
#define MOTION_XML_ATTR_TRIGGER_THEN	"then"
#define MOTION_XML_ATTR_TRIGGER_PARAM1	"param1"
#define MOTION_XML_ATTR_TRIGGER_PARAM2	"param2"
#define MOTION_XML_ATTR_POSTFIX			"postfix"
#define MOTION_XML_ATTR_AUTO_PREFIX		"auto_prefix"
#define MOTION_XML_ATTR_USABLEUPPER		"usable_upper"

#define MOTION_XML_VALUE_ALL			"all"
#define MOTION_XML_VALUE_NORMAL			"normal"
#define MOTION_XML_VALUE_BATTLE			"battle"

#define MOTION_XML_VALUE_TRIGGER_CHECK_TRUE				"True"
#define MOTION_XML_VALUE_TRIGGER_CHECK_DEAD				"Dead"
#define MOTION_XML_VALUE_TRIGGER_THEN_HOLD				"Hold"
#define MOTION_XML_VALUE_TRIGGER_THEN_CHANGE_MOTION		"ChangeMotion"

bool XMotionMgr::Load(const TCHAR* szFileName)
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(MOTION_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), MOTION_XML_TAG_MOTION))
		{
			ParseMotion(pElement, &xml);
		}
	}

	return true;
}

void XMotionMgr::AddMotionForTest( const std::tstring& strXmlData )
{
	MXmlDocument doc;
	doc.Parse(MLocale::ConvTCHARToAnsi(strXmlData.c_str()).c_str());
	MXmlElement* pRootElement = doc.FirstChildElement();
	if(pRootElement == NULL) return;

	ParseMotion(pRootElement, NULL);
}

void XMotionMgr::ParseMotion(MXmlElement* pElement, MXml* pXml)
{
	XMotion* pNewMotion = new XMotion();

	_Attribute(pNewMotion->m_strName, pElement, MOTION_XML_ATTR_NAME);
	_Attribute(&pNewMotion->m_bLoop, pElement, MOTION_XML_ATTR_LOOP);
	_Attribute(&pNewMotion->m_bCancelEnabled, pElement, MOTION_XML_ATTR_CANCELENABLED);
	_Attribute(&pNewMotion->m_bCameraLook[XMotion::MS_NORMAL], pElement, MOTION_XML_ATTR_CAMERA_LOOK);
	_Attribute(&pNewMotion->m_bCameraLook[XMotion::MS_BATTLE], pElement, MOTION_XML_ATTR_CAMERA_LOOK);
	_Attribute(&pNewMotion->m_bCameraLook[XMotion::MS_ALL], pElement, MOTION_XML_ATTR_CAMERA_LOOK);
	_Attribute(&pNewMotion->m_bWeaponAni, pElement, MOTION_XML_ATTR_WEAPONANI);
	_Attribute(&pNewMotion->m_bAddPostfix, pElement, MOTION_XML_ATTR_POSTFIX, false);
	_Attribute(&pNewMotion->m_bAutoPrefix, pElement, MOTION_XML_ATTR_AUTO_PREFIX, true);

	char szValue[256];
	if (_Attribute(szValue, pElement, MOTION_XML_ATTR_STANCE))
	{
		if (!_stricmp(szValue, MOTION_XML_VALUE_NORMAL)) pNewMotion->m_nStanceType = XMotion::MS_NORMAL;
		else if (!_stricmp(szValue, MOTION_XML_VALUE_BATTLE)) pNewMotion->m_nStanceType = XMotion::MS_BATTLE;
		else pNewMotion->m_nStanceType = XMotion::MS_ALL;
	}

	if (_Attribute(szValue, pElement, MOTION_XML_ATTR_CAMERA_LOOK_STANCE))
	{
		if (!_stricmp(szValue, MOTION_XML_VALUE_NORMAL))
		{
			pNewMotion->m_bCameraLook[XMotion::MS_BATTLE] = false;
			pNewMotion->m_bCameraLook[XMotion::MS_ALL] = false;
		}
		else if (!_stricmp(szValue, MOTION_XML_VALUE_BATTLE)) 
		{
			pNewMotion->m_bCameraLook[XMotion::MS_NORMAL] = false;
			pNewMotion->m_bCameraLook[XMotion::MS_ALL] = false;
		}
	}

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), MOTION_XML_TAG_DEFAULT))
		{
			ParseMotionType(pChild, pXml, pNewMotion, MT_DEFAULT);
		}
		else if (!_stricmp(pChild->Value(), MOTION_XML_TAG_FORWARD))
		{
			ParseMotionType(pChild, pXml, pNewMotion, MT_FORWARD);
		}
		else if (!_stricmp(pChild->Value(), MOTION_XML_TAG_BACKWARD))
		{
			ParseMotionType(pChild, pXml, pNewMotion, MT_BACKWARD);
		}
		else if (!_stricmp(pChild->Value(), MOTION_XML_TAG_LEFT))
		{
			ParseMotionType(pChild, pXml, pNewMotion, MT_LEFT);
		}
		else if (!_stricmp(pChild->Value(), MOTION_XML_TAG_RIGHT))
		{
			ParseMotionType(pChild, pXml, pNewMotion, MT_RIGHT);
		}
	}


	// 추가되는 모션을 텍스트 파일로 뽑아볼려면 아래 디파인을 풀어서 빌드하세요
#ifdef _DEBUG
#define _WRITE_MOTION_DATA
#endif

#ifdef _WRITE_MOTION_DATA

	static bool bFirst = true;

	const TCHAR CONST_NAME_TYPE[ MT_COUNT][ 32]		= { _T("MT_DEFAULT"), _T("MT_FORWARD"), _T("MT_BACKWARD"), _T("MT_LEFT"), _T("MT_RIGHT") };
	const TCHAR CONST_NAME_WEAPON[ WEAPON_MAX][ 32] = { _T("WEAPON_NONE      "), _T("WEAPON_1H_SLASH  "), _T("WEAPON_1H_BLUNT  "), _T("WEAPON_1H_PIERCE "), _T("WEAPON_TWO_HANDED"), _T("WEAPON_STAFF     "), _T("WEAPON_ARCHERY   "), _T("WEAPON_DUEL_WIELD") };

	if ( bFirst == true)
	{
		FILE* pFile;

		errno_t err = _tfopen_s(&pFile, _T("motion_list.txt"), _T("w"));
		if ( 0 != err ) return;

		fclose( pFile);
		bFirst = false;
	}

	FILE* pFile;
	errno_t err = fopen_s(&pFile, "motion_list.txt", "a+");

	if ( pFile && err == 0)
	{
		fprintf( pFile, "Motion name = \"%s\"\n\n", pNewMotion->m_strName.c_str());

		for (int i = 0; i < MT_COUNT; i++)
		{
			if ( pNewMotion->m_vecAniSequence[ i].size() == 0)
				continue;

			fprintf( pFile, "  Motion type = %s\n", CONST_NAME_TYPE[ i]);

			for ( size_t j = 0;  j < pNewMotion->m_vecAniSequence[ i].size();  j++)
			{
				fprintf( pFile, "\n    Ani name = \"%s\"\n", pNewMotion->m_vecAniSequence[ i][ j]->GetAniName());

				for ( int k = 0;  k < WEAPON_MAX;  k++)
					fprintf( pFile, "      %s = %s\n", CONST_NAME_WEAPON[ k], pNewMotion->m_vecAniSequence[ i][ j]->GetAniName( (WEAPON_TYPE)k));
			}

			fprintf( pFile, "\n");
		}
		fprintf( pFile, "-----------------------------------------------\n\n");

		fclose( pFile);
	}
#endif

	AddMotion(pNewMotion);
}


void XMotionMgr::ParseMotionType(MXmlElement* pElement, MXml* pXml, XMotion* pMotion, XMotionTypes nType)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL;  pChild=pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), MOTION_XML_TAG_SEQUENCE))
		{
			ParseMotionSequence(pChild, pMotion, nType);
		}
	}
}

void XMotionMgr::InitCustomMotions()
{
	// custom
	XMotion* pCMotion = new XMotion();
	pCMotion->m_bLoop = false;
	pCMotion->m_bCancelEnabled = true;
	pCMotion->m_strName = MOTION_NAME_CUSTOM;
	pCMotion->SetCameraLook(true);
	pCMotion->m_nStanceType = XMotion::MS_ALL;
	pCMotion->m_bCustomAni = true;
	AddMotion(pCMotion);

	// Talent_Casting
	XMotion* pSCMotion = new XMotion();
	pSCMotion->m_bLoop = false;
	pSCMotion->m_bCancelEnabled = true;
	pSCMotion->m_strName = MOTION_NAME_TALENT_CASTING;
	pSCMotion->SetCameraLook(true);
	pSCMotion->m_nStanceType = XMotion::MS_BATTLE;
	pSCMotion->m_bCustomAni = true;
	AddMotion(pSCMotion);

	// Talent_Casting_loop
	XMotion* pSCLoopMotion = new XMotion();
	pSCLoopMotion->m_bLoop = true;
	pSCLoopMotion->m_bCancelEnabled = true;
	pSCLoopMotion->m_strName = MOTION_NAME_TALENT_CASTING_LOOP;
	pSCLoopMotion->SetCameraLook(true);
	pSCLoopMotion->m_nStanceType = XMotion::MS_BATTLE;
	pSCLoopMotion->m_bCustomAni = true;
	AddMotion(pSCLoopMotion);

	// Talent_Use
	XMotion* pSUMotion = new XMotion();
	pSUMotion->m_bLoop = false;
	pSUMotion->m_bCancelEnabled = true;
	pSUMotion->m_strName = MOTION_NAME_TALENT_USE;
	pSUMotion->SetCameraLook(false);
	pSUMotion->m_nStanceType = XMotion::MS_BATTLE;
	pSUMotion->m_bCustomAni = true;
	AddMotion(pSUMotion);

	// Talent_Extra
	XMotion* pSEMotion = new XMotion();
	pSEMotion->m_bLoop = false;
	pSEMotion->m_bCancelEnabled = false;
	pSEMotion->m_strName = MOTION_NAME_TALENT_EXTRA;
	pSEMotion->SetCameraLook(false);
	pSEMotion->m_nStanceType = XMotion::MS_BATTLE;
	pSEMotion->m_bCustomAni = true;
	AddMotion(pSEMotion);

	// Talent_Extra2
	XMotion* pSE2Motion = new XMotion();
	pSE2Motion->m_bLoop = false;
	pSE2Motion->m_bCancelEnabled = false;
	pSE2Motion->m_strName = MOTION_NAME_TALENT_EXTRA2;
	pSE2Motion->SetCameraLook(false);
	pSE2Motion->m_nStanceType = XMotion::MS_BATTLE;
	pSE2Motion->m_bCustomAni = true;
	AddMotion(pSE2Motion);

	// Talent_Extra3
	XMotion* pSE3Motion = new XMotion();
	pSE3Motion->m_bLoop = false;
	pSE3Motion->m_bCancelEnabled = false;
	pSE3Motion->m_strName = MOTION_NAME_TALENT_EXTRA3;
	pSE3Motion->SetCameraLook(false);
	pSE3Motion->m_nStanceType = XMotion::MS_BATTLE;
	pSE3Motion->m_bCustomAni = true;
	AddMotion(pSE3Motion);
}

bool XMotionMgr::Init(TCHAR* szMotionFIleName, TCHAR* szPCMotionFileName, TCHAR* szNPCMotionFileName)
{
	if (szMotionFIleName == NULL) szMotionFIleName = FILENAME_MOTION;
	if (szPCMotionFileName == NULL) szPCMotionFileName = FILENAME_MOTION_PC;
	if (szNPCMotionFileName == NULL) szNPCMotionFileName = FILENAME_MOTION_NPC;


	if (Load(szMotionFIleName) == false)
	{
		mlog("file not found : %s\n", MLocale::ConvTCHARToAnsi(szMotionFIleName).c_str());
		_ASSERT(0);
	}

	if (Load(szPCMotionFileName) == false)
	{
		mlog("file not found : %s\n", MLocale::ConvTCHARToAnsi(szPCMotionFileName).c_str());
		_ASSERT(0);
	}

	if (Load(szNPCMotionFileName) == false)
	{
		mlog("file not found : %s\n", MLocale::ConvTCHARToAnsi(szNPCMotionFileName).c_str());
		_ASSERT(0);
	}

	// 특별한 모션들 추가
	InitCustomMotions();

	return true;
}

bool XMotionMgr::IsTalentMotionName(const TCHAR* szName)
{
	// 스킬 모션 이름은 앞에 'NS'나 'PS'로 시작한다.
	if ((szName[0] == 'N' || szName[0] == 'P') && szName[1] == 'S') return true;
	return false;
}

void XMotionMgr::Dump()
{
#ifdef _PUBLISH
	return;
#endif

	mlog("==== Motion Dump ====\n");

	for (iterator itor = begin(); itor != end(); ++itor)
	{
		XMotion* pMotion = (*itor).second;
		mlog("Motion Name(%s), WeaponAni(%s)---\n", MLocale::ConvTCHARToAnsi(pMotion->GetName().c_str()).c_str(), (pMotion->IsWeaponAni()) ? "True" : "False");

		for (int i = 0; i < MT_COUNT; i++)
		{
			mlog(" MotionType:%d\n", i);
			for (size_t j = 0; j < pMotion->m_vecAniSequence[i].size(); j++)
			{
				XMotionAniSequence* pAniSeq = pMotion->m_vecAniSequence[i][j];

				mlog("  Ani:");
				for (int t = 0; t < WEAPON_MAX; t++)
				{
					mlog("%s ", MLocale::ConvTCHARToAnsi(pAniSeq->GetAniName(WEAPON_TYPE(t))).c_str());
				}
				mlog("\n");

			}
		}
		mlog("-----------------\n");
	}
}

void XMotionMgr::ParseMotionSequence( MXmlElement* pSeqElement, XMotion* pMotion, XMotionTypes nType )
{
	string strAni, strEvent, strFrameOver, strSpeed;

	XMotionAniSequence* pNewAniSeq = new XMotionAniSequence();

	if (_Attribute(strAni, pSeqElement, MOTION_XML_ATTR_ANI))
	{
		bool bHaveWeaponAni = true;
		if (pMotion->GetStanceType() == XMotion::MS_NORMAL) bHaveWeaponAni = false;
		pNewAniSeq->SetAniName(MLocale::ConvAnsiToTCHAR(strAni.c_str()).c_str(), bHaveWeaponAni);
	}

	if (_Attribute(strEvent, pSeqElement, MOTION_XML_ATTR_EVENT))
	{
		_tcscpy_s(pNewAniSeq->szEvent, MLocale::ConvAnsiToTCHAR(strEvent.c_str()).c_str());
		pNewAniSeq->bExistEvent = true;
	}

	if (_Attribute(strFrameOver, pSeqElement, MOTION_XML_ATTR_FRAMEOVER))
	{
		pNewAniSeq->m_nFrameOver = atoi(strFrameOver.c_str());
	}

	_Attribute(&pNewAniSeq->m_fTimeOut, pSeqElement, MOTION_XML_ATTR_TIMEOUT);

	if (_Attribute(strSpeed, pSeqElement, MOTION_XML_ATTR_SPEED))
	{
		pNewAniSeq->m_fSpeed = (float)atof(strSpeed.c_str());
	}

	MXmlElement* pChild = pSeqElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), MOTION_XML_TAG_POST_TRIGGER))
		{
			string strIf, strThen;

			if (_Attribute(strIf, pChild, MOTION_XML_ATTR_TRIGGER_IF))
			{
				if (!_stricmp(MOTION_XML_VALUE_TRIGGER_CHECK_TRUE, strIf.c_str()))
				{
					pNewAniSeq->m_PostTrigger.m_nIfType = MTRIGGER_IF_TRUE;
				}
				if (!_stricmp(MOTION_XML_VALUE_TRIGGER_CHECK_DEAD, strIf.c_str()))
				{
					pNewAniSeq->m_PostTrigger.m_nIfType = MTRIGGER_IF_DEAD;
				}
			}
			if (_Attribute(strThen, pChild, MOTION_XML_ATTR_TRIGGER_THEN))
			{
				if (!_stricmp(MOTION_XML_VALUE_TRIGGER_THEN_HOLD, strThen.c_str()))
				{
					pNewAniSeq->m_PostTrigger.m_nThenType = MTRIGGER_THEN_HOLD;
				}
				if (!_stricmp(MOTION_XML_VALUE_TRIGGER_THEN_CHANGE_MOTION, strThen.c_str()))
				{
					pNewAniSeq->m_PostTrigger.m_nThenType = MTRIGGER_THEN_CHANGE_MOTION;
				}
			}

			_Attribute(pNewAniSeq->m_PostTrigger.m_strThenParam1, pChild, MOTION_XML_ATTR_TRIGGER_PARAM1);
			_Attribute(pNewAniSeq->m_PostTrigger.m_strThenParam2, pChild, MOTION_XML_ATTR_TRIGGER_PARAM2);
		}
	}

	pMotion->m_vecAniSequence[nType].push_back(pNewAniSeq);
}

bool XMotionMgr::ReLoad()
{
	Clear();
	return Init();
}