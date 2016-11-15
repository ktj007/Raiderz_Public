#pragma once

#include "XGlueFunction.h"


class XGlueOption : public XGlueFunction
{
public:
	virtual void DeclGlueFunc( MWLua* pLua);


	// Option
	void ReloadOption();
	void SaveOption();


	// Game
	void RefreshGameOption();


	// Graphic
	void RefreshGraphicOption();
	void SetScreenResolution( int i);


	// Sound
	void RefreshSoundOption();
	void SetMute(bool bMute);


	// Control
	void RefreshControlOption();
};
