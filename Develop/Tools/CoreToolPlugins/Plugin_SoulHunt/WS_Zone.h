/// @file WS_Zone.h
#pragma once

#include "WS_WorkSpaceManager.h"

class CWorkSpaceZone : public CWorkSpaceImpl<CWorkSpaceZone>
{
public:
								CWorkSpaceZone(){}
	virtual						~CWorkSpaceZone(){}

	virtual const char*			GetWorkSpaceKeyword();												///< WorkSpace Keyword
	virtual void				InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction);	///< WorkSpace UI
	virtual void				HaltWorkSpace(CWorkSpaceHelpersAndActions& _rHelperAndAction);

	virtual const char*			GetFileExtension()	{ return "*.zone.xml"; }
	virtual const char*			GetFileExtensionDesc()	{ return "존 파일"; }

	void						LoadZoneCommonXmlAndNavigationMesh(const char* _pSzZoneName);

protected:

	virtual void				OnRollupExpandClicked(CListenableView* pRollup);		///< When Rollup Expand Clicked
	virtual void				OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message);	///< Workbench Mouse Edit Message

	virtual MAP_EDIT_TYPE		OnOpenFile(const char* _pSzFileName);					///< 파일 열기
	virtual std::string			OnSaveFile(const char* _pSzFileName, bool bSaveAs);		///< 파일 저장
	virtual void				OnCloseFile() {}
	virtual void				OnSceneSaved( const char* _filename );					///< 씬이 저장될때


	virtual void				OnBetweenCoreToolUpdateAndRS3Render(){}
	virtual void				OnResetResourceAfterSceneCleared(bool bOnDestroy){}

	virtual void				OnWBDevAction(){}
	virtual void				OnWBHelperRender(){}

	virtual void				OnFileModified( const string& strFileName ) override;		///< 파일이 변경되었을때 처리

	void SaveRestoreDirectionalLight(rs3::RDirectionalLightSceneNode* pLight, bool bSave);
};
