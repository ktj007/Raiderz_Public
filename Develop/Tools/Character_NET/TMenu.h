#pragma once

enum SHOWBONESTATE
{
	SBS_NONE,
	SBS_SIMPLE,
	SBS_EXACT,

	SBS_MAX
};

class TMenu
{
private:
	int				m_nCurShowBoneState;

protected:
	bool			m_bShowAABB;
	bool			m_bShowInteractionAABB;
	bool			m_bVertexNormal;
	bool			m_bShowWire;
	RFILLMODE		m_FillMode;
	bool			m_bShowSpecular;

	RTextSceneNode*	m_pTextNode;
	RTextSceneNode* m_pTextNodeActorNodeInfo;
	RFont*			m_pTextFont;

	void Init();

public:
	TMenu(void);
	~TMenu(void);

	void InitFont();
	void Destroy();

	void OnShowNormal() { m_bVertexNormal = !m_bVertexNormal; }
	void OnShowBones();
	void OnShowWire()	{ m_bShowWire = !m_bShowWire; }
	void OnShowAABB();
	void OnShowInteractionAABB();
	void OnShowSpecular();

	void SetFillMode() { m_FillMode = (m_FillMode == RFILL_SOLID) ? RFILL_WIRE : RFILL_SOLID ; }
	void SetFillMode(int nFillmode);
	void SetShowBoneType(int nBoneTypeIndex);
	
	int GetShowBoneType() { return m_nCurShowBoneState; }

	void SetDeviceFillMode();

	void DrawWire();
	void DrawBone();
	void DrawNormal();
	void DrawAABBbox();
	void DrawInteractionAABBbox();
	void DrawPartsText();

	void DrawActorNodeInfo(RActorNode* pActorNode);

	bool CheckDrawBone(SHOWBONESTATE eState);

public:
	void Render();
};
