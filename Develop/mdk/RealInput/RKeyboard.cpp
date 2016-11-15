#include "stdafx.h"
#include "RKeyboard.h"

#define SAMPLE_BUFFER_SIZE 16  // arbitrary number of buffer elements

RKeyboardImpl::RKeyboardImpl() : m_pDevice(NULL)
{
// 	for(int i=0; i<KEYNAMETABLE_COUNT; i++){
// 		m_szKeyNameTable[i] = NULL;
// 	}
}

RKeyboardImpl::~RKeyboardImpl()
{
	Destroy();
}

// enum keyboard buttons
BOOL CALLBACK EnumDeviceObjectsCB( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef )
{
    // Extract the passed pointer
	std::string* szKeyNameTable = (std::string*)pvRef;

	int nKey = lpddoi->dwOfs;
	if(nKey<0 || nKey>=KEYNAMETABLE_COUNT) return DIENUM_STOP;

//	size_t nLength = strlen(lpddoi->tszName)+2;
//	szKeyNameTable[nKey] = new char[nLength];
//	strcpy_s(szKeyNameTable[nKey], nLength, lpddoi->tszName);
	szKeyNameTable[nKey] = lpddoi->tszName;

	return DIENUM_CONTINUE;
}

bool RKeyboardImpl::Create(LPDIRECTINPUT8 pDirectInput, HWND hWnd)
{
	HRESULT hr;
	DWORD   dwCoopFlags;
	
// 	for(int i=0; i<KEYNAMETABLE_COUNT; i++){
// 		m_szKeyNameTable[i] = NULL;
// 	}
	
    dwCoopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;

#ifdef _PUBLISH
	dwCoopFlags |= DISCL_NOWINKEY ;
#endif

	if( FAILED( hr = pDirectInput->CreateDevice( GUID_SysKeyboard, &m_pDevice, NULL ) ) ) return false;
	if( FAILED( hr = m_pDevice->SetDataFormat( &c_dfDIKeyboard ) ) ) return false;

	hr = m_pDevice->SetCooperativeLevel( hWnd, dwCoopFlags );
	if( FAILED(hr) ) return false;


	// 버퍼 생성
	{
        DIPROPDWORD dipdw;

        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = SAMPLE_BUFFER_SIZE; // Arbitary buffer size

        if( FAILED( hr = m_pDevice->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) ) return false;
    }

    m_pDevice->Acquire();

	// Enum Buttons
    hr = m_pDevice->EnumObjects( EnumDeviceObjectsCB, m_szKeyNameTable, DIDFT_BUTTON );
    if( FAILED(hr)) return false;

	return true;
}

void RKeyboardImpl::Destroy()
{
    if( m_pDevice ) 
	{
        m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;

		for(int i=0; i<KEYNAMETABLE_COUNT; i++)
		{
			m_szKeyNameTable[i] = "";
		}
	}
}

bool RKeyboardImpl::ReadImmediateData(BYTE ScanCode[256])
{
	if (!IsEnabled()) return false;

    HRESULT hr;

    ZeroMemory( ScanCode, sizeof(ScanCode) );

	hr = m_pDevice->GetDeviceState( sizeof(ScanCode), ScanCode );
    if( FAILED(hr) ) {
        hr = m_pDevice->Acquire();
        while( hr == DIERR_INPUTLOST ) hr = m_pDevice->Acquire();
		return false;
    }
    
	return true;
}

DWORD RKeyboardImpl::GetBufferedData(RInputKeyBuffer* pBuffer)
{
	if( m_pDevice == NULL ) return 0;

    DIDEVICEOBJECTDATA didod[SAMPLE_BUFFER_SIZE];  // Receives buffered data 
    DWORD              dwElements;
    DWORD              i;
    HRESULT            hr;
    
	dwElements = SAMPLE_BUFFER_SIZE;

    hr = m_pDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0 );
    if( hr != DI_OK ) 
	{
        hr = m_pDevice->Acquire();
        while( hr == DIERR_INPUTLOST ) hr = m_pDevice->Acquire();
		return 0;
    }

    for( i = 0; i < dwElements; i++ ) {
		pBuffer[i].nScanCode = BYTE(didod[i].dwOfs & 0xFF);
		pBuffer[i].bPressed = (didod[i].dwData & 0x80)?true:false;
    }

    return dwElements;
}

const char* RKeyboardImpl::GetKeyName(unsigned long int nKey)
{
	if(nKey<0 || nKey>=KEYNAMETABLE_COUNT){
		static char* szUnknownKeyName = "N/A";
		return szUnknownKeyName;
	}
	return m_szKeyNameTable[nKey].c_str();
}

