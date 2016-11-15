#ifndef _MINET_H
#define _MINET_H

#include "MiNetLib.h"

namespace minet {

	/// MiNet 라이브러리를 총괄하는 클래스
	/// MiNet 라이브러리에서 필요한 것들을 초기화한다.
MINET_API bool Init(const int nCommandVersion);
/// MiNet에서 사용한 것을 모두 해제한다.
MINET_API void Fini();
MINET_API const int GetCommandVersion();
MINET_API void ClearCommandDescs();

} // namespace minet

#define MCOMMAND_VERSION		(minet::GetCommandVersion())

#endif

