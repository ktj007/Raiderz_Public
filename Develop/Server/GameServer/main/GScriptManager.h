#pragma once

#include "MWLua.h"
#include "GScriptMacro.h"
#include <fstream>

class GScriptGroup_Field;
class GScriptGroup_Quest;
class GScriptGroup_NPC;
class GScriptGroup_QuestPVP;
//class GLuaRuntimeErrorReporter;

/// 루아 스크립트를 관리
class GScriptManager : public MTestMemPool<GScriptManager>
{
public:
			GScriptManager();
	virtual	~GScriptManager();
	bool	Init(bool bUseCallbackLogger=false);
	
	void	Fini();
	bool	IsRunning()		{ return m_bRunning; }

	inline MWLua*			GetLua();
	GScriptGroup_Field*		GetScriptField();
	GScriptGroup_Quest*		GetScriptQuest();
	GScriptGroup_NPC*		GetScriptNPC();
	GScriptGroup_QuestPVP*	GetScriptQuestPVP();

private:
	bool	RegisterGlues();	
	void	RegisterGlue_Const();
	void	RegisterGlue_Foundation();
	void	RegisterGlue_Customdata();
	void	RegisterGlue_Debug();
	void	RegisterGlue_Util();
	void	RegisterGlue_EntityActor();
	void	RegisterGlue_EntityPlayer();	
	void	RegisterGlue_NullObject();	
	bool	RegisterGlue_Handlers();
	void	RegisterGlue_NPCSession();
	
	void	InitHandler();
	void	FinitHandler();
	
	void	InitNullObject();
	void	FiniNullObject();
	
private:
	bool	m_bRunning;
	MWLua	m_Lua;
	GScriptGroup_Field*		m_pHandlerField;
	GScriptGroup_Quest*		m_pHandlerQuest;
	GScriptGroup_NPC*		m_pHandlerNPC;
	GScriptGroup_QuestPVP*	m_pHandlerQuestPVP;
};




inline 
MWLua* 
GScriptManager::GetLua()					
{ 
	return &m_Lua; 
}

GScriptManager*			GetScriptManager();
GScriptGroup_Field*		GetScriptField();
GScriptGroup_Quest*		GetScriptQuest();
GScriptGroup_NPC*		GetScriptNPC();
GScriptGroup_QuestPVP*	GetScriptQuestPVP();
#define WLUA GetScriptManager()->GetLua()

