#include "StdAfx.h"
#include "ZScript.h"
#include "ZGlue.h"
#include "ZApplication.h"


ZScript::ZScript()
{
}


ZScript::~ZScript()
{
}


bool ZScript::Create()
{
	// Create Lua
	m_Lua.Open();


	// Binding Lua script
	MWLua::class_<ZGlue>( m_Lua.GetState(), "_GameCommandData")
		.def( "Log",					&ZGlue::Log)
		.def( "Reload",					&ZGlue::Reload)
		.def( "ChangeResolution",		&ZGlue::ChangeResolution)
		.def( "Profile",				&ZGlue::Profile)
		.def( "Test",					&ZGlue::Test)
	;


	m_Lua.SetVar("ZGlue", ZGlue::GetInstance());

	return true;
}


void ZScript::Destroy()
{
	// Destroy Lua
	m_Lua.Close();
}
