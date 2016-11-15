#include "stdafx.h"
#include "XGlueCharFunction.h"
#include "XGlobalVar.h"
#include "XCharFrame.h"
#include "XStrings.h"
#include "XGameFrameManager.h"
#include "XFieldInfo.h"
#include "CCommandResultTable.h"
#include "XPost_Char.h"

void XGlueCharFunction::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueCharFunction>(pLua->GetState(), "_shChar")
		.def("GetCharCount",		&XGlueCharFunction::GetCharCount)
		.def("GetCharName",		&XGlueCharFunction::GetCharName)
		.def("GetCharLevel",		&XGlueCharFunction::GetCharLevel)
		.def("GetCharInFieldName",	&XGlueCharFunction::GetCharInFieldName)
		.def("CreateCharacter",		&XGlueCharFunction::CreateCharacter)
		.def("DeleteCharacter",		&XGlueCharFunction::DeleteCharacter)
		.def("SelectCharacter",		&XGlueCharFunction::SelectCharacter)
		.def("StartGame",		&XGlueCharFunction::StartGame)
		.def("ChangeCreationModel",	&XGlueCharFunction::ChangeCreationModel)
		.def("GetHairMaxCount",		&XGlueCharFunction::GetHairMaxCount)
		.def("GetFaceMaxCount",		&XGlueCharFunction::GetFaceMaxCount)
		.def("GetMaxHairCount",		&XGlueCharFunction::GetHairMaxCount)
		.def("GetMaxFaceCount",		&XGlueCharFunction::GetFaceMaxCount)
		.def("GetCharacterFaceCount",   &XGlueCharFunction::GetFaceMaxCount)
		.def("GetCharacterHairCount",   &XGlueCharFunction::GetHairMaxCount)
		
		;
}

int XGlueCharFunction::GetCharCount()
{
	return (int)gvar.Char.m_CharInfos.size();	
}

const char* XGlueCharFunction::GetCharName( int nCharIndex )
{
	if (nCharIndex < 0 || nCharIndex >= GetCharCount()) return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.Char.m_CharInfos[nCharIndex].szName);
	return gvar.Temp.szTemp.c_str();
}

int XGlueCharFunction::GetCharLevel( int nCharIndex )
{
	if (nCharIndex < 0 || nCharIndex >= GetCharCount()) return 0;
	return gvar.Char.m_CharInfos[nCharIndex].nLevel;
}

const char* XGlueCharFunction::GetCharInFieldName( int nCharIndex )
{
	if (nCharIndex < 0 || nCharIndex >= GetCharCount()) return "";

	const wchar_t* name = info.field->GetFieldName(gvar.Char.m_CharInfos[nCharIndex].nFieldID);

	if (name == NULL)
	{
		return "NONE";
	}
	

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(name);

	return gvar.Temp.szTemp.c_str();
}

void XGlueCharFunction::StartGame( int nCharIndex )
{
	if (!CheckCurrFrame()) return;

	if (nCharIndex < 0 || nCharIndex >= GetCharCount()) return;

	XPostChar_RequestSelMyChar(nCharIndex);
}

void XGlueCharFunction::CreateCharacter(const char* szName, MWLua::table t)
{
	if (!CheckCurrFrame()) return;

	if (GetCharCount() >= MAX_ACCOUNT_CHAR_COUNT) return;

	int nNameLen = strlen(szName);
	if (nNameLen <= 0) 
	{
		global.script->GetGlueCharEvent().OnCreatedCharacter(false, MLocale::ConvUTF16ToAnsi(XCmdResultStr(CR_FAIL_SYSTEM_TOO_SHORT_NAME_LEN)).c_str());
		return;
	}
	if (nNameLen > 12) 
	{
		global.script->GetGlueCharEvent().OnCreatedCharacter(false, MLocale::ConvUTF16ToAnsi(XCmdResultStr(CR_FAIL_SYSTEM_TOO_LONG_NAME_LEN)).c_str());
		return;
	}

//	XPostChar_RequestInsertCharacter(szName, SEX(t.get<int>("nSex")), char(t.get<int>("nHair")), char(t.get<int>("nFace")), short(t.get<int>("nHairColor")), short(t.get<int>("nSkinColor")));
}

void XGlueCharFunction::DeleteCharacter( const char* szName, int nCharIndex )
{
	if (!CheckCurrFrame()) return;
	if (nCharIndex < 0 || nCharIndex >= GetCharCount()) return;

	XPostChar_RequestDeleteCharacter(char(nCharIndex));
}

void XGlueCharFunction::SelectCharacter( int nCharIndex )
{
	if (!CheckCurrFrame()) return;

	XCharFrame* pCharFrame = static_cast<XCharFrame*>(global.gfmgr->GetCurrFrame());

	if (nCharIndex < 0 || nCharIndex >= GetCharCount())
	{
		pCharFrame->DeleteCurrChar();
	}
	else
	{
		pCharFrame->SelectChar(nCharIndex);
	}
}

bool XGlueCharFunction::CheckCurrFrame()
{
	return (global.gfmgr && global.gfmgr->GetCurrFrameID() == CHAR_FRAME_ID);
}

void XGlueCharFunction::ChangeCreationModel(MWLua::table t)
{
	if (!CheckCurrFrame()) return;

	XCharFrame* pCharFrame = static_cast<XCharFrame*>(global.gfmgr->GetCurrFrame());
//	pCharFrame->ChangeMockChar(SEX(t.get<int>("nSex")), t.get<int>("nHair"),
//		t.get<int>("nFace"), t.get<short>("nHairColor"), t.get<short>("nSkinColor"));

}

int XGlueCharFunction::GetHairMaxCount( int nSex )
{
	SEX sex = (SEX)nSex;
	if (sex == SEX_MALE) return MAX_CREATION_PLAYER_HAIR_MALE;
	return MAX_CREATION_PLAYER_HAIR_FEMALE;
}

int XGlueCharFunction::GetFaceMaxCount( int nSex )
{
	SEX sex = (SEX)nSex;
	if (sex == SEX_MALE) return MAX_CREATION_PLAYER_FACE_MALE;
	return MAX_CREATION_PLAYER_FACE_FEMALE;
}