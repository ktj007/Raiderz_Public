#include "StdAfx.h"
#include "GScriptGroup_QuestPVP.h"
#include "GGlueQuestPVP.h"
#include "MWLua.h"
#include "GScriptManager.h"

GScriptGroup_QuestPVP::GScriptGroup_QuestPVP(void)
{
}

GScriptGroup_QuestPVP::~GScriptGroup_QuestPVP(void)
{
}

void GScriptGroup_QuestPVP::RegisterGlues()
{
	MWLua::class_<GGlueQuestPVP>(WLUA->GetState(), "GQuestPVP")
		.def(MWLua::constructor<GQuestPVP*>())
		.def("IsNull",			&GGlueQuestPVP::IsNull)
		.def("GetTeamCount",	&GGlueQuestPVP::GetTeamCount)
		.def("BeginEvent",		&GGlueQuestPVP::BeginEvent)
		.def("EndEvent",		&GGlueQuestPVP::EndEvent)
		.def("EnableMarkPos",	&GGlueQuestPVP::EnableMarkPos)
		.def("DisableMarkPos",	&GGlueQuestPVP::DisableMarkPos)
		;
}
