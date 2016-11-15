#pragma once

class XLoadingCompleter
{
private:

public:
	XLoadingCompleter() {}
	virtual ~XLoadingCompleter() {}
	virtual void WorldLoadingFinish() = 0;			///< World 로딩 완료
	virtual void Complete() = 0;				///< 서버까지 준비 완료
};

class XGame;
class XLoadingFrame;

class XLoadingCompleter_Game : public XLoadingCompleter
{
private:
	XGame*			m_pGame;
	XLoadingFrame*	m_pLoadingFrame;
public:
	XLoadingCompleter_Game(XGame* pGame, XLoadingFrame* pLoadingFrame);
	virtual ~XLoadingCompleter_Game();
	virtual void WorldLoadingFinish();
	virtual void Complete();
};