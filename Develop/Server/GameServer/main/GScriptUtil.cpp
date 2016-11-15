#include "StdAfx.h"
#include "GScriptUtil.h"
#include "GGlueNPC.h"
#include "GField.h"
#include "GNullGlueField.h"
#include "GNullGlueNPC.h"

bool GScriptUtil::MakeSpawnTable(MWLua::table* poutTable, int nSpawnID, GGlueNPC* pNPC, GGlueField* pField)
{
	VALID_RET(poutTable, false);
	VALID_RET(pNPC, false);
	VALID_RET(pField, false);

	poutTable->set("SpawnID", nSpawnID);
	poutTable->set("NPCID", pNPC->GetID());
	poutTable->set("NPC", pNPC);
	poutTable->set("Field", pField);
	poutTable->set("Pos", pNPC->GetPos());

	return true;
}

bool GScriptUtil::MakeDespawnTable(MWLua::table* poutTable, int nSpawnID, GGlueNPC* pNPC, GGlueField* pField)
{
	VALID_RET(poutTable, false);
	VALID_RET(pNPC, false);
	VALID_RET(pField, false);

	poutTable->set("SpawnID", nSpawnID);
	poutTable->set("NPCID", pNPC->GetID());
	poutTable->set("NPC", pNPC);
	poutTable->set("Field", pField);
	poutTable->set("Pos", pNPC->GetPos());

	return true;
}

