#ifndef MINET_ERRORTABLE_H
#define MINET_ERRORTABLE_H

namespace minet {

enum MErrorTable
{
	MOK				= 1,	///< 에러 없음
	MERR_UNKNOWN	= -1,	///< 알지 못하는 에러

	MERR_COMMUNICATOR_HAS_NOT_UID						= 100,		///< 커뮤니케이터가 UID를 가지지 않은 경우, 아직 마스터나 혹은 UID를 발급받을 수 있는 커뮤니케이터와 접속하지 않았을 경우다.
	MERR_COMMUNICATOR_HAS_NOT_UID_SPACE					= 101,		///< 커뮤니케이터가 발급할 UID 가지고 있지 않다. UID를 예약중일 가능성이 높다.
	MERR_COMMUNICATOR_INVALID_DIRECT_CONNECTION_POINTER	= 110,		///< 하나의 프로세스내에 여러 커뮤니케이터가 동시에 떳을때, DirectConnectionPointer가 유효하지 않은 경우
	MERR_COMMAND_HAS_INVALID_RECEIVER_UID				= 200,		///< 커맨드에 설정된 Receiver UID가 유효하지 않을때

	MERR_ZONESERVER_NOT_CONNECTED_TO_MASTER				= 300,		///< Zone-Server가 Master-Controller와 연결되지 않았을때
	MERR_ZONESERVER_TRY_TO_INVALID_MASTER				= 301,		///< Zone-Server가 Master-Controller가 아니 커뮤니케이터에게 Master-Controller로 오인하고 연결을 시도할때

};


} // namespace minet

#endif