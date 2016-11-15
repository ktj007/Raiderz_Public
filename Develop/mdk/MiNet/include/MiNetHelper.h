#pragma once

#include "MiNetLib.h"

namespace minet {

/// MiNet Helper 클래스
class MINET_API MiNetHelper
{
public:
	/// . 있는 IP 문자열을 . 없는 IP 문자열(12바이트)로 변환
	static void ConvertCompactIP(char* szOut, const char* szInputDottedIP);

	static unsigned long ConvertIPAddress(const char* szIP);
	static char* ConvertIPAddress(unsigned long nIP);

	static string GetHostIPString();
};

} // namespace minet