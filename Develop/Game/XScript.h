#ifndef _XSCRIPT_H
#define _XSCRIPT_H

#include "MWLua.h"
#include "XGlueFunctions.h"
#include "XGlueGameEvent.h"
#include "XGlueCharEvent.h"
#include "XGlueFieldEvent.h"
#include "XGlueCutSceneEvent.h"

class XScript
{
protected:
	MWLua*					m_pLua;
	XGlueFunctions			m_GlueFunctions;

	XGlueGameEvent			m_GlueGameEvent;
	XGlueDebugEvent			m_GlueDebugEvent;
	XGlueCharEvent			m_GlueCharEvent;
	XGlueFieldEvent			m_GlueFieldEvent;
	XGlueCutSceneEvent		m_GlueCutSceneEvent;

	static void				ErrorHandler(const wchar_t* szErrMessage);

	void					SetDebugScript();
public:
	XScript();
	virtual ~XScript();

	virtual void			Init();
	virtual bool			Create();
	virtual void			Destroy();
	void					RunString(const wchar_t* szString);
	bool					RunFile(const wchar_t* szFileName);

	// 게임 이벤트
	void 					Event(const wchar_t* szEvent);
	void 					Event(const wchar_t* szEvent, const wchar_t* str);
	void 					Event(const wchar_t* szEvent, const wchar_t* str1, const wchar_t* str2);
	void 					Event(const wchar_t* szEvent, const wchar_t* str1, const wchar_t* str2, const wchar_t* str3);
	void 					Event(const wchar_t* szEvent, double fArg1);
	void 					Event(const wchar_t* szEvent, double fArg1, double fArg2);
	void 					Event(const wchar_t* szEvent, double fArg1, double fArg2, double fArg3);

	MWLua*					GetLua()				{ return m_pLua; }
	XGlueCharEvent&			GetGlueCharEvent()		{ return m_GlueCharEvent; }
	XGlueGameEvent&			GetGlueGameEvent()		{ return m_GlueGameEvent; }
	XGlueDebugEvent&		GetGlueDebugEvent()		{ return m_GlueDebugEvent; }
	XGlueFieldEvent&		GetGlueFieldEvent()		{ return m_GlueFieldEvent; }
	XGlueCutSceneEvent&		GetGlueCutSceneEvent()	{ return m_GlueCutSceneEvent; }
};



#endif