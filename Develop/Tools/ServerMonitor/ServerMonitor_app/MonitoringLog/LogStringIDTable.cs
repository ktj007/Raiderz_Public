using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerMonitor
{
    public class LogID
    {
        public const ushort STARTING_SERVER = 1;                                // 서버 구동 중... 잠시만 기다려 주세요.

        public const ushort SUCCESS_START_SERVER = 2;                           // 서버 시작 성공
        public const ushort FAILED_START_SERVER_ERROR = 3;                      // 서버 시작 실패 - 내부 처리 오류
        public const ushort FAILED_START_SERVER_ALREADY_START = 4;              // 서버 시작 실패 - 이미 시작됐거나 시작 중인 서버
        public const ushort FAILED_START_SERVER_DEPENDENCY_CHECK = 5;           // 서버 시작 실패 - 실행 종속된 서버의 구동 정보 체크 실패 ({0}, {1})
        public const ushort FAILED_START_SERVER_DEPENDENCY_ARG = 6;             // 서버 시작 실패 - 실행 종속된 서버가 켜져있지 않습니다 ({0})
        public const ushort FAILED_START_SERVER_PROCESS = 7;                    // 서버 시작 실패 - 프로세스 시작 실패

        public const ushort TERMINATING_SERVER = 10;                            // 서버 정지 중

        public const ushort SUCCESS_TERMINATE_SERVER = 11;                      // 서버 정지 성공
        public const ushort FAILED_TERMINATE_SERVER_ERROR = 12;                 // 서버 정지 실패 - 내부 처리 오류
        public const ushort FAILED_TERMINATE_SERVER_ALREADY_TERMINATED = 13;    // 서버 정지 실패 - 이미 정지됐거나 정지 중인 서버
        public const ushort FAILED_TERMINATE_SERVER_SEND_DESTROY_MESSAGE = 14;  // 서버 정지 실패 - 해당 서버로 정지 메시지를 보내지 못했습니다
        public const ushort WARN_TERMINATE_SERVER_NO_UPDATE = 15;               // 서버 정지 경고 - 업데이트가 멈춘 서버. 정상 종료 되지 않을 수 있습니다.
        public const ushort FAILED_TERMINATE_INVALID_STATE = 16;                // 서버 정지 실패 - 정지 가능한 상태가 아닙니다

        public const ushort SERVER_DIED = 20;                                   // 서버 죽음
        public const ushort SERVER_NOUPDATE = 21;                               // 서버 업데이트 멈춤
        public const ushort SERVER_RECOVERY_NOUPDATE = 22;                      // 서버 업데이트 재개
        public const ushort SERVER_INACTIVE = 23;                               // 서버 비활성화 됨
        public const ushort SERVER_ACTIVE = 24;                                 // 서버 활성화 됨
    }
}
