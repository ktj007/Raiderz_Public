/// @file WS_CutScene.h
#pragma once

#include "WS_WorkSpaceManager.h"

class CWorkSpaceCutScene : public CWorkSpaceImpl<CWorkSpaceCutScene>
{
public:
								CWorkSpaceCutScene(){}
	virtual						~CWorkSpaceCutScene(){}

protected:

	virtual const char*			GetWorkSpaceKeyword(){ return "cutscene.xml"; }
	virtual void				InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction);
	virtual void				HaltWorkSpace(CWorkSpaceHelpersAndActions& _rHelperAndAction);

	virtual const char*			GetFileExtension()	{ return "*.cutscene.xml"; }
	virtual const char*			GetFileExtensionDesc()	{ return "ÄÆ¾À ÆÄÀÏ"; }

	virtual MAP_EDIT_TYPE		OnOpenFile(const char* _pSzFileName);
	virtual std::string			OnSaveFile(const char* _pSzFileName, bool bSaveAs);
	virtual void				OnCloseFile() {}

	virtual void				OnBetweenCoreToolUpdateAndRS3Render();
	virtual void				OnResetResourceAfterSceneCleared(bool bOnDestroy);

	virtual void				OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message){}
	virtual bool				OnCheckAlwaysPickingWhenMoused();
	
	virtual void				OnWBDevAction();
	virtual void				OnWBHelperRender();
};
