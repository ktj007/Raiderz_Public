#pragma once

#include "CSItemData.h"
#include "SaveNotifier.h"

namespace Character_NET {
	ref class Form1;

	//using namespace System;

	public enum ModelType
	{
		CT_PLAYER=0,
		CT_MOB,
		CT_NPC,
		CT_MAPOBJ,
	};

	ref class Global
	{
	public:
		Global(void) {}

		static public String^ GetModelTypeStr(ModelType nType)
		{
			switch (nType)
			{
			case Character_NET::CT_PLAYER	:	return "Player";
			case Character_NET::CT_MOB		:	return "Monster";
			case Character_NET::CT_NPC		:	return "NPC";
			case Character_NET::CT_MAPOBJ	:	return "MapObj";
			}

			return "none";
		}

		static public String^ GetSequenceName(int i)
		{
			switch(i)
			{
			case 0:	return "Default";
			case 1:	return "Forward";
			case 2:	return "Backward";
			case 3:	return "Left";
			case 4:	return "Right";
			}

			return "Default";
		}

		ColorDialog			m_colorDialog;	///< Parts Color 선택을 위한 다이얼로그
	};

}//namespace Character_NET

using namespace Character_NET;

public ref class SelInfo
{
public:
	int m_nMeshHitGroupID;
	int m_nMeshHitCapsuleID;
	int m_nMeshColID;
	int m_nMeshHitEffectCapsuleID;
	SelInfo() : m_nMeshHitGroupID(-1), m_nMeshHitCapsuleID(-1), m_nMeshColID(-1), m_nMeshHitEffectCapsuleID(-1) {}
};

ref class GlobalObjects
{
public:
	static SaveNotifier	g_SaveNotifier;
	static Form1^		g_pMainForm;
	static Global^		g_pGlobal;
	static SelInfo		g_SelInfo;
};
