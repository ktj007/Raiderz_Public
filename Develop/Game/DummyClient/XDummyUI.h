#pragma once

#include "MPaint.h"

class XDummyData;

class XDummyUI
{
private:
	XDummyData*		m_pData;

	class XDummyAgentStatusPaint : public MPaint
	{
	protected:
		XDummyData*	m_pData;
		map<wstring, int>		m_mapY;
		virtual void OnDraw(MDrawContext* pDC);

		void DrawAgentStatus( MDrawContext* pDC );
		void DrawAgentName( MDrawContext* pDC );
	public:
		XDummyAgentStatusPaint(XDummyData* pData, const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	};

	class XDummyMainPaint : public MPaint
	{
	protected:
		XDummyData*	m_pData;
		FrameCounter	m_FrameCounter;
		virtual void OnDraw(MDrawContext* pDC);
	public:
		XDummyMainPaint(XDummyData* pData, const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	};

	class XDummyStatisticsPaint : public XDummyAgentStatusPaint
	{
	protected:
		virtual void OnDraw(MDrawContext* pDC);
		void DrawAgentInfo(MDrawContext* pDC);
	public:
		XDummyStatisticsPaint(XDummyData* pData, const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	};

	class MTabCtrlListener : public mint3::MListener
	{
	public:
		virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	};

	XDummyAgentStatusPaint*		m_pAgentStatusPaint;
	XDummyMainPaint*			m_pMainPaint;
	XDummyStatisticsPaint*		m_pStatisticsPaint;
	MTabCtrlListener*			m_pTabCtrlListener;

	void InitUI();
public:
	XDummyUI();
	~XDummyUI();
	void Create(XDummyData* pData);
	void Destroy();
	void Reload();
	void Render();
};