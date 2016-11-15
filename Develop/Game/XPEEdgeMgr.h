#ifndef _XPEEDGE_MGR_H
#define _XPEEDGE_MGR_H

#pragma once

struct stPEEdgeInfo 
{
	MUID				uid;

	vec4				vColor;
	float				fTickNess;
	float				fBlurFactor;
	bool				bProjection;

	stPEEdgeInfo()
	{
		uid.SetZero();

		fTickNess		= 0.35f;
		fBlurFactor		= 2.0f;
		bProjection		= false;
	}
};

class XPEEdgeMgr
{
private:
	vector<stPEEdgeInfo>	m_vecEnableList;
	vector<MUID>			m_vecDisableList;

private:
	void				CheckDiablePEEdge(MUID& uid);

	bool				CheckDuplicationEnablePEEdeg(MUID& uid);
	bool				CheckDuplicationDiablePEEdeg(MUID& uid);

public:
	XPEEdgeMgr();
	virtual ~XPEEdgeMgr();

	void				Update();

	void				EnablePEEdge(MUID& uid, const RVector4& _color, float _fTickNess, float _fBlurFactor, bool _bProjection);
	void				DiablePEEdge(MUID& uid);
};



#endif