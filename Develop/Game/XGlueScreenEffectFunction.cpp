#include "stdafx.h"
#include "XGlueScreenEffectFunction.h"

void XGlueScreenEffectFunction::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueScreenEffectFunction>(pLua->GetState(), "_shScreenEffect")
		.def("Add",			&XGlueScreenEffectFunction::Add)
		.def("SetPos",		&XGlueScreenEffectFunction::SetPos)
		.def("Register",	&XGlueScreenEffectFunction::Register)
		.def("SetRegPos",	&XGlueScreenEffectFunction::SetRegPos)
		.def("SetRegStr",	&XGlueScreenEffectFunction::SetRegStr)
		.def("SetChgAni",	&XGlueScreenEffectFunction::SetChgAni)
		.def("AniStop",		&XGlueScreenEffectFunction::AniStop)
		.def("Draw",		&XGlueScreenEffectFunction::Draw)
		;
}

void XGlueScreenEffectFunction::Add(const char* strListFileName, XSCREENEFFECT_CHARACTER_ATT charAtt, AnimationLoopType aniLoop)
{
	XScreenEffectManager * pScreenEffect = global.ui->GetScreenEffectMgr();

	pScreenEffect->AddScreenEffect(MLocale::ConvAnsiToUTF16(strListFileName), charAtt, aniLoop);
}

void XGlueScreenEffectFunction::Register(const char* strScreenEffectName, 
									const char* strListFileName,
									XSCREENEFFECT_CHARACTER_ATT charAtt,
									AnimationLoopType aniLoop)

{
	XScreenEffectManager * pScreenEffect = global.ui->GetScreenEffectMgr();

	pScreenEffect->RegisterScreenEffect(MLocale::ConvAnsiToUTF16(strScreenEffectName), MLocale::ConvAnsiToUTF16(strListFileName), charAtt, aniLoop);
}

void XGlueScreenEffectFunction::SetRegStr(const char* strScreenEffectName, const char* strContents)
{
	XScreenEffectManager * pScreenEffect = global.ui->GetScreenEffectMgr();

	pScreenEffect->SetString(MLocale::ConvAnsiToUTF16(strScreenEffectName), MLocale::ConvAnsiToUTF16(strContents));
}

void XGlueScreenEffectFunction::SetRegPos(const char* strScreenEffectName, float x, float y, float z, XSCREENEFFECT_LOCATION_TYPE scrBase)
{
	XScreenEffectManager * pScreenEffect = global.ui->GetScreenEffectMgr();

	pScreenEffect->SetPosition(MLocale::ConvAnsiToUTF16(strScreenEffectName), vec3(x, y, z), scrBase);
}

void XGlueScreenEffectFunction::SetPos(float x, float y, float z, XSCREENEFFECT_LOCATION_TYPE scrBase)
{
	XScreenEffectManager * pScreenEffect = global.ui->GetScreenEffectMgr();

	pScreenEffect->SetPosition(L"", vec3(x, y, z), scrBase);
}

void XGlueScreenEffectFunction::SetChgAni(const char* strScreenEffectName, const char* strAnimation, XSCREENEFFECT_ANIMATION_END_ATT aniDel)
{
	XScreenEffectManager * pScreenEffect = global.ui->GetScreenEffectMgr();

	pScreenEffect->ChangeAnimation(MLocale::ConvAnsiToUTF16(strScreenEffectName), MLocale::ConvAnsiToUTF16(strAnimation), aniDel);
}

void XGlueScreenEffectFunction::Draw(const char* strScreenEffectName)
{
	XScreenEffectManager * pScreenEffect = global.ui->GetScreenEffectMgr();

	pScreenEffect->SetDraw(MLocale::ConvAnsiToUTF16(strScreenEffectName), true);
}

void XGlueScreenEffectFunction::AniStop(const char* strScreenEffectName)
{
	XScreenEffectManager * pScreenEffect = global.ui->GetScreenEffectMgr();

	pScreenEffect->AnimationStop(MLocale::ConvAnsiToUTF16(strScreenEffectName));
}
