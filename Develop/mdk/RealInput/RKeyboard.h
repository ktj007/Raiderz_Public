#ifndef _RKEYBOARD_H
#define _RKEYBOARD_H

#include "RealInput.h"
#include <string>

/// Keyboard Device
class RKeyboardImpl : public RKeyboard
{
private:

protected:
	friend class RealInput;
	RealInput*					m_pRealInput;
	LPDIRECTINPUTDEVICE8		m_pDevice;

#define KEYNAMETABLE_COUNT	256
	std::string					m_szKeyNameTable[KEYNAMETABLE_COUNT];
public:
								RKeyboardImpl();
	virtual						~RKeyboardImpl();

	bool						Create(LPDIRECTINPUT8 pDirectInput, HWND hWnd);
	void						Destroy();
	bool						IsEnabled()				{ return (m_pDevice != NULL); }

								/// 256개의 스캔코드가 눌려있는지(&0x80) 검사 
	bool						ReadImmediateData(BYTE ScanCode[256]);	
								/// 키보드 버퍼로 읽어오기
								/// 스캔코드 및 Down/Up 플래그 포함, 읽은 버퍼 갯수만큼 리턴한다.
	DWORD						GetBufferedData(RInputKeyBuffer* pBuffer) override;

	const char* GetKeyName(unsigned long int nKey) override;
	
};


struct RAction
{

};

#endif // _RKEYBOARD_H