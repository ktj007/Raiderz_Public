#pragma once

class XSkillInfo;
class rs3::RDevice;


class THitAreaRenderer
{
		THitAreaRenderer() : m_pDevice(NULL), m_pSkillInfo(NULL), m_dwColor(0xFFFF0000) {}

private:
		void				RenderPlayerMoveHitArea(MCapsule cap, bool bSelected);
		void				RenderHitCapsule(MCapsule& cap, bool bDrawSegment, bool opa, bool bSelected, bool bAllSelected = false);
public:	
		rs3::RDevice*		m_pDevice;
		XSkillInfo*			m_pSkillInfo;

		DWORD				m_dwColor;	///< 투명없이 진한색.

		// nDrawAxis = 0 : 없음 1 2 3 = x y z

		void				Render(CSTalentInfo* pSkillInfo, int nSegID, int nCapsuleID, bool bDrawSegment, bool opa);
		void				AllRender(CSTalentInfo* pSkillInfo, int nSelectedSegID, bool opa);
		static	THitAreaRenderer& GetInstance();
};