#include "stdafx.h"
#include "XGlueOption.h"
#include "XGameInterfaceOption.h"



void XGlueOption::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueOption>(pLua->GetState(), "_gameoption")

		// Option
		.def( "ReloadOption",						&XGlueOption::ReloadOption)
		.def( "SaveOption",							&XGlueOption::SaveOption)


		// Game
		.def( "RefreshGameOption",					&XGlueOption::RefreshGameOption)


		// Graphic
		.def( "RefreshGraphicOption",				&XGlueOption::RefreshGraphicOption)
		.def( "SetScreenResolution",				&XGlueOption::SetScreenResolution)


		// Sound
		.def( "RefreshSoundOption",					&XGlueOption::RefreshSoundOption)
		.def( "SetMute",							&XGlueOption::SetMute)

		// Control
		.def( "RefreshControlOption",				&XGlueOption::RefreshControlOption)

		;
}


void XGlueOption::ReloadOption()
{
	XGameInterfaceOption().LoadOption();
}


void XGlueOption::SaveOption()
{
	XGameInterfaceOption().SaveOption();
}



// Game
void XGlueOption::RefreshGameOption()
{
//	XGameInterfaceOption().RefreshOptionGameUI();
}


// Graphic
void XGlueOption::RefreshGraphicOption()
{
//	XGameInterfaceOption().RefreshOptionVideoUI();
}

void XGlueOption::SetScreenResolution( int i)
{
//	XGameInterfaceOption().SetScreenResolution( i);
}


// Sound
void XGlueOption::RefreshSoundOption()
{
//	XGameInterfaceOption().RefreshOptionSoundUI();
}

void XGlueOption::SetMute(bool bMute)
{
//	XGameInterfaceOption().SetMute( bMute);
}

// Control
void XGlueOption::RefreshControlOption()
{
//	XGameInterfaceOption().RefreshOptionControlKeyUI();
}