#include "stdafx.h"
#include "XGlueFunctions.h"


void XGlueFunctions::Init( MWLua* pLua)
{
	if ( !pLua)		return;

	DeclGlueFunc( pLua);
	SetVars( pLua);
}


void XGlueFunctions::DeclGlueFunc( MWLua* pLua )
{
	m_glueGame.DeclGlueFunc( pLua);
	m_glueGameOption.DeclGlueFunc( pLua);
	m_glueDebug.DeclGlueFunc( pLua);
}


void XGlueFunctions::SetVars( MWLua* pLua )
{
	pLua->SetVar( "gamefunc",	&m_glueGame);
	pLua->SetVar( "gameoption",	&m_glueGameOption);
	pLua->SetVar( "gamedebug",	&m_glueDebug);
}
