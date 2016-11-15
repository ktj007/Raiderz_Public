#pragma once

class XLoadingChecker
{
protected:
	virtual void OnEvent(XEvent& evt) {}
public:
	XLoadingChecker() {}
	virtual ~XLoadingChecker() {}
	void Event(XEvent& evt)
	{
		OnEvent(evt);
	}
	virtual bool IsWorldLoaded() = 0;
	virtual bool IsCompleted() = 0;
};

