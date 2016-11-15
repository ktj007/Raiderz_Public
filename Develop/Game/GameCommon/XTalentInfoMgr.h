#pragma once

#include "CSTalentInfoMgr.h"
class XTalentInfo;

class XTalentInfoMgr : public CSTalentInfoMgr
{
private:
	void BuildGroupInfo();

	XTalentInfoMgr(XTalentInfoMgr& other);
protected:
public:
	class IVisitor {
		friend class XTalentInfoMgr;
		virtual void OnVisit(const CSTalentInfo& talentInfo) = 0;
	};

public:
	XTalentInfoMgr();
	virtual ~XTalentInfoMgr();
	virtual void	Cooking();

	bool			Load(const TCHAR* szFileName, const TCHAR* szExtFileName, const TCHAR* szHitFileName);
	bool			Reload(const TCHAR* szFileName, const TCHAR* szExtFileName, const TCHAR* szHitFileName);
	void			Destroy();

	XTalentInfo*	Get(int nID, int nMode=0, bool bModeExactly=false);
	bool			IsPassiveTalent(int nID);

	XTalentInfo*	GetByLineHighRank(int nLine);
	XTalentInfo*	GetHighRankTalent(int nTalentID);

	XTalentInfo*	GetByLineAndRank(int nLine, int nRank);
	XTalentInfo*	GetNextRankTalent(int nTalentID);

	void			GetSocialTalentID(vector<int>& vecSocial);

	void			Visit(IVisitor& visitor);
};
