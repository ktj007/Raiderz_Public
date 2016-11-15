#pragma once

class XTalentHitParam;

class XMyHitController
{
private:
	MF_STATE			nMFState;
	int					nMFWeight;
	int					nFlags;

private:
	void				InitHitData();

public:
	XMyHitController();
	virtual ~XMyHitController() {}

	void				SetHitData(XTalentHitParam* pHitInfo);
	void				DelHitData();

	bool				CheckSendMovePost();
};