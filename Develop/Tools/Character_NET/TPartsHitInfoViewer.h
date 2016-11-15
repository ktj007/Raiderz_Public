#ifndef _TPARTS_HIT_INFO_VIEWER_H
#define _TPARTS_HIT_INFO_VIEWER_H

#include <Windows.h>
#include <MMSystem.h>
#include <shlwapi.h>
#include <math.h>
#include <algorithm>

#include "MainApp.h"
#include "TMeshInfo.h"

using namespace System;
using namespace System::Windows::Forms;

class TPartsHitInfoViewer
{
private:
	string				m_strModelName;

private:
	void				InitPartsHitInfoViewer();

	TreeView^			GetPartsHitInfoViewerTreeNode();
	PropertyGrid^		GetPartsHitInfoViewerPropertGrid();

	void				RenderEditionCapsule(MCapsule & capsuleInfo, DWORD dwColor);
	void				RenderCapsule(MCapsule & capsuleInfo, DWORD dwColor = 0x200000ff);

	void				SetPartsHitCapsuleDataToProperty(CSHitCapsule * pPartsHitCapsule);
	void				SetPropertyToPartsHitCapsuleData(CSHitCapsule * pPartsHitCapsule);

public:
	TPartsHitInfoViewer() {}
	virtual ~TPartsHitInfoViewer() {}

	void				ChangePartsHitInfoViewer(System::String^ ModelName);
	void				ChangePartsHitCapsuleInfo(int nGroupIndex, int nCapsuleIndex);

	void				CreatePartsHitInfoViewer(int nGroupIndex, int nCapsuleIndex = -1);
	void				Reload();
	void				RenderGroup(int nGroupIndex, int nSelectCapsuleID = -1);

	void				SetPartsHitCapsuleData(int nGroupIndex, int nCapsuleIndex);

	void NotifyChanging();

	MCapsule *			GetCurrentSelectCapsule();
	CSHitCapsule *		GetPartsHitCapsuleInfo(int nGroupIndex, int nCapsuleIndex);
	CSHitGroup *		GetPartsHitGroupInfo(int nGroupIndex);

};


#endif