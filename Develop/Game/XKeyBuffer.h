#ifndef _XKEY_BUFFER_H
#define _XKEY_BUFFER_H

#include "XVirtualKeyDef.h"

/// @brief 키 버퍼에서 사용하는 키노드
/// @author 남기룡
/// @date 2007-01-05
struct KEYNODE
{
	enum KeyState
	{
		PRESSONCE = 1,	///< 처음 누를때 발생
		PRESSED = 2,	///< 키를 눌려져 있다
		RELEASED = 3	///< 키를 떼었는지
	};

	XVirtualKey		nKeycode;		///< 키값
	unsigned int	nTime;			///< 시간
	KeyState		nState;			///< 키누름 상태

	bool			Pressed() const { return ((nState == PRESSONCE) || (nState == PRESSED)); }

	/// 생성자
	KEYNODE() : nKeycode(VKEY_NA), nTime(0), nState(PRESSED) {}
	/// 생성자
	KEYNODE(XVirtualKey key, DWORD time, KeyState state) : nKeycode(key), nTime(time), nState(state) {}
};

/// @class XKeyBuffer
/// @brief 콤보 처리, 연속기 등에 사용하는 입력 키 버퍼. 원형 큐로 되어 있음.
/// @author 남기룡
/// @date 2007-01-05
/// @see
class XKeyBuffer
{
private:
	static const int	MAX_KEY_BUFFER = 32;		///< 키버퍼 사이즈(32)
	vector<KEYNODE>		m_Buffer;					///< 버퍼
	int					m_nBegin;					///< 원형버퍼 시작점
	int					m_nEnd;						///< 원형버퍼 끝
	set<XVirtualKey>	m_Filter;					///< 키 필터. 여기 있는 값만 버퍼에 쌓이도록 한다.
public:
	XKeyBuffer();									///< 생성자
	~XKeyBuffer();									///< 소멸자
	void AddFilter(XVirtualKey nKey);				///< 필터에 추가. 여기에 추가된 키값만 버퍼에 넣는다.
	bool CheckFilter(XVirtualKey nKey);				///< 필터 체크
	void Clear();									///< 버퍼에 쌓인 모든 값을 지운다.
	void Push(KEYNODE& key_node);					///< 버퍼에 추가
	void Push2(KEYNODE& key_node);					///< 버퍼에 추가 < 뒤에서 2번째에 추가-_-;
	void Pop();
	void Remove(XVirtualKey nKey);
	int GetCapacity()	{ return MAX_KEY_BUFFER; }	///< 총 사이즈 반환
	int GetSize();									///< 현재 버퍼에 쌓인 키들의 사이즈
	bool IsEmpty();									///< 버퍼가 비었는지 체크
	const KEYNODE& Get(int index) const;			///< 키 값 반환. index가 작을수록 이전에 입력한 키이다.
	void GetDebugString(wchar_t* pszOut, int nOutLen);
	bool IsExistPressedKeyNode(XVirtualKey nVirtualKey);
};

#endif // _XKEY_BUFFER_H