#pragma once

class XLoadingStarter
{
public:
	XLoadingStarter() {}
	virtual ~XLoadingStarter() {}
	virtual void Start() = 0;
};

class XGame;
class XLoadingFrame;

class XLoadingStarter_Game : public XLoadingStarter
{
private:
	XGame*			m_pGame;
	XLoadingFrame*	m_pLoadingFrame;
public:
	XLoadingStarter_Game(XGame* pGame, XLoadingFrame* pLoadingFrame);
	virtual ~XLoadingStarter_Game();
	virtual void Start();
};