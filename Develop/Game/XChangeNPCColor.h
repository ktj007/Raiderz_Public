#pragma once

struct CHANGE_NPC_COLOR 
{
	MCOLOR						colorTexSource;
	MCOLOR						colorTexTarget;

	MCOLOR						colorSkinSource;
	MCOLOR						colorSkinTarget;

	MCOLOR						colorHairSource;
	MCOLOR						colorHairTarget;

	CHANGE_NPC_COLOR()
	{
		colorTexSource			= 0xffffffff;
		colorTexTarget			= 0xffffffff;

		colorSkinSource			= 0xffffffff;
		colorSkinTarget			= 0xffffffff;

		colorHairSource			= 0xffffffff;
		colorHairTarget			= 0xffffffff;

	}
};

class XChangeNPCColor
{
protected:
	bool						m_bUse;

	XCharacter*					m_pActor;

	CHANGE_NPC_COLOR			m_NPCColor;

	MCOLOR						m_colorTexChange;
	MCOLOR						m_colorSkinChange;
	MCOLOR						m_colorHairChange;

	float						m_fElapsedTime;

protected:
	MCOLOR						GetColorInterpolation(const MCOLOR& colorSource, const MCOLOR& colorTarget, float t);

public:
	XChangeNPCColor();
	virtual ~XChangeNPCColor();

	void						Init(XCharacter* pActor);
	void						Start(CHANGE_NPC_COLOR npcColor);
	void						Update(float fDelta);

	bool						IsUse() { return m_bUse; }

	static void					SetSkin(XCharacter* pActor, unsigned int nColor);
	static void					SetHair(XCharacter* pActor, unsigned int nColor);
	static void					SetEye(XCharacter* pActor, unsigned int nColor);
};

