#ifndef _XSCREEN_DEBUGGER_H
#define _XSCREEN_DEBUGGER_H

#pragma push_macro("_SECURE_SCL")
#pragma push_macro("_HAS_ITERATOR_DEBUGGING")
#pragma warning ( disable : 4005)

#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0


#include "XGlobal.h"
#include <map>
#include <string>
using namespace std;
using namespace mint3;

/// 화면에 디버깅 정보를 출력하기위한 클래스
/// 4개의 레이어에 각각 정보를 출력할 수 있다.
class XScreenDebugger
{
public:
	enum _Layer {
		LAYER1 = 0,
		LAYER2,
		LAYER3,
		LAYER4,
		LAYER5,
		LAYER6,
		LAYER7,
		LAYER8,			// MovableDebugger 전용
		LAYER9,			// HDR Monitor

		LAYER_MAX,
		LAYER_ALL	= LAYER_MAX
	};
private:
	static const int		LINE_PIXEL = 20;				// 한라인을 20픽셀로 정함.

	list< pair<int, wstring>>	m_BufferMap[LAYER_ALL+1];		// 각 레이어당..
	bool					m_bVisible;
	int						m_nLayer;
	int						m_nEndLine[LAYER_ALL+1];
	bool					m_bReleaseTextOnDraw[LAYER_ALL+1];
	MPOINT					GetPosFromKey(int key);
	int						MakeKey(int x, int y);
	void					DrawLayer(MDrawContext *pDC, _Layer layer);
	void					InitEndLine();
public:
							/// 생성자
							XScreenDebugger();
							/// 소멸자
	virtual					~XScreenDebugger();
							/// 버퍼에 모아놓은 정보를 출력
	void					Draw(MDrawContext *pDC);
							/// 레이어 선택
	void					SelLayer(_Layer layer);
							/// 지금 보고있는 레이어
	_Layer					GetSelLayer();
							/// 화면에 보여줄지를 설정
	void					Show(bool b=true, _Layer layer=LAYER1) { InitEndLine(); m_bVisible = b; m_nLayer=layer; }
							/// 디버깅 정보를 출력하고 있는지 여부
	bool					IsVisible()	{ return m_bVisible; }
							/// 텍스트 출력. 텍스트를 버퍼에 모아놓은 다음 Draw함수에서 실제로 화면에 출력한다.
	void					Print(int x, int y, const wchar_t* szText, _Layer layer=LAYER_ALL);
							/// 텍스트 출력. y값에 상관없이 한줄씩 출력한다.
	void					PrintLine(int x, const wchar_t* szText, _Layer layer=LAYER_ALL);
	void					NextLayer();
	void					PrevLayer();
	void					SetLine(int y, _Layer layer=LAYER_ALL);
	void					SetReleaseTextOnDraw(bool b, _Layer layer);
	void					ReleaseText(_Layer layer);

							/// 인스턴스 반환
	static XScreenDebugger*	GetInstance();
};

// 편의를 위한 함수

#define _MAKE_TEXT_SCRTEXT_		wchar_t text[1024];					\
								va_list args;						\
								va_start(args, pFormat);			\
								vswprintf(text, pFormat, args);		\
								va_end(args);						\
								
inline void _ScrText(int x, int y, const wchar_t *pFormat, ...) 
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->Print(x, y, text, XScreenDebugger::LAYER_ALL);
}
inline void _ScrText1(int x, int y, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->Print(x, y, text, XScreenDebugger::LAYER1);
}
inline void _ScrText2(int x, int y, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->Print(x, y, text, XScreenDebugger::LAYER2);
}
inline void _ScrText3(int x, int y, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;	
	XScreenDebugger::GetInstance()->Print(x, y, text, XScreenDebugger::LAYER3);
}
inline void _ScrText4(int x, int y, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->Print(x, y, text, XScreenDebugger::LAYER4);
}
inline void _ScrText5(int x, int y, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->Print(x, y, text, XScreenDebugger::LAYER5);
}
inline void _ScrText6(int x, int y, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->Print(x, y, text, XScreenDebugger::LAYER6);
}
inline void _ScrText7(int x, int y, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->Print(x, y, text, XScreenDebugger::LAYER7);
}
inline void _ScrLine(int x, const wchar_t *pFormat, ...) 
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER_ALL);
}
inline void _ScrLine1(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER1);
}
inline void _ScrLine2(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER2);
}
inline void _ScrLine3(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;	
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER3);
}
inline void _ScrLine4(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER4);
}
inline void _ScrLine5(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER5);
}
inline void _ScrLine6(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER6);
}
inline void _ScrLine7(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER7);
}
inline void _ScrLine8(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER8);
}
inline void _ScrLine9(int x, const wchar_t *pFormat, ...)
{
	_MAKE_TEXT_SCRTEXT_;
	XScreenDebugger::GetInstance()->PrintLine(x, text, XScreenDebugger::LAYER9);
}
inline XScreenDebugger::_Layer XScreenDebugger::GetSelLayer()		{ return XScreenDebugger::_Layer(m_nLayer); }

#pragma pop_macro("_SECURE_SCL")
#pragma pop_macro("_HAS_ITERATOR_DEBUGGING")

#endif // _XSCREEN_DEBUGGER_H