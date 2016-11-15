#include "stdafx.h"
#include "XIsAnimationValidator.h"
#include "XCharacter.h"

bool XIsAnimationValidator::Check()
{
	_VLI(L"data/model/Player/hf/hf.elu.animation.xml");

	bool ret = true;

	if( IsLootAni() == false) ret = false;
	if( IsBuffAni() == false) ret = false;
	if( IsMaleDummyloc() == false) ret = false;
	if( IsFeMaleDummyloc() == false) ret = false;

	return ret;
}

bool XIsAnimationValidator::IsLootAni()
{
	MXml xml;
	bool bOk = xml.LoadFile("data/model/Player/hf/hf.elu.animation.xml");
	if(!bOk) return false;

	MXmlElement* pRootNode = xml.Doc()->FirstChildElement();

	MXmlElement* pChild = pRootNode->FirstChildElement();

	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		char NodeName[256];
		char IDName[256];

		strcpy(NodeName, pChild->Value());
		if(strcmp(NodeName, "AddAnimation")==0)
		{
			_Attribute(IDName, pChild,			"name");

			_VLE(L"AddAnimation");	
			_VLA(L"name", MLocale::ConvAnsiToUTF16(IDName));
			_VLP;

			if (strcmp(IDName, "none_loot_start") == 0)
			{
				return true;
			}
			
			continue;
		}
	}

	m_Logger.Log(L"루팅 애니메이션이 없어요.\n");
	return false;
}

bool XIsAnimationValidator::IsBuffAni()
{
	MXml xml;
	bool bOk = xml.LoadFile("data/model/Player/hf/hf.elu.animation.xml");
	if(!bOk) return false;

	MXmlElement* pRootNode = xml.Doc()->FirstChildElement();

	MXmlElement* pChild = pRootNode->FirstChildElement();

	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		char NodeName[256];
		char IDName[256];

		strcpy(NodeName, pChild->Value());
		if(strcmp(NodeName, "AddAnimation")==0)
		{
			_Attribute(IDName, pChild,			"name");

			_VLE(L"AddAnimation");	
			_VLA(L"name", MLocale::ConvAnsiToUTF16(IDName));
			_VLP;

			if (strcmp(IDName, "sta_spell_buff_cast") == 0)
			{
				return true;
			}

			continue;
		}
	}

	m_Logger.Log(L"sta_spell_buff_cast 애니메이션이 없어요.\n");
	return false;
}

bool XIsAnimationValidator::IsMaleDummyloc()
{
	XCharacter* pActor = new XCharacter;
	bool bLoad = pActor->Create(L"data/model/player/hm/hm.elu");

	bool bAni = pActor->SetAnimation("none_idle");
	if (bAni)
	{
		if(pActor->IsExistActorNode(L"dummy_loc"))
		{
			SAFE_DELETE(pActor);
			return false;
		}
	}

	SAFE_DELETE(pActor);
	return true;
}

bool XIsAnimationValidator::IsFeMaleDummyloc()
{
	XCharacter* pActor = new XCharacter;
	bool bLoad = pActor->Create(L"data/model/player/hf/hf.elu");

	bool bAni = pActor->SetAnimation("none_idle");
	if (bAni)
	{
		if(pActor->IsExistActorNode(L"dummy_loc"))
		{
			SAFE_DELETE(pActor);
			return false;
		}
	}

	SAFE_DELETE(pActor);
	return true;
}