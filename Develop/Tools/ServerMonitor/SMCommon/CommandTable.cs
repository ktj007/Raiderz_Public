using System;
using System.Collections.Generic;
using System.Text;

namespace SMCommon
{
    public class CMD
    {
        public const int COMMAND_VERSION = 20100219;

        public const ushort CHECK_CMD_VERSION_REQ = 10; ///< 커맨드 버젼 체크 요청
        //  4   int     Command Version

        public const ushort CHECK_CMD_VERSION_RES = 11; ///< 커맨드 버젼 체크 응답 
        //  1   bool    Version Check Success

        public const ushort START_SERVER_REQ = 12;      ///< 서버 시작 요청
        //  *   string  서버 이름

        public const ushort TERMINATE_SERVER_REQ = 13;  ///< 서버 정지 요청
        //  *   string  서버 이름
        
        public const ushort SET_REVIVABLE_ATT_REQ = 14; ///< 서버 되살리기 설정 요청
        //  *   string  서버 이름
        //  1   bool    되살리기 옵션

        public const ushort GET_SERVERLIST_REQ = 15;    ///< 서버 목록 요청

        public const ushort GET_SERVERLIST_RES = 16;    ///< 서버 목록 응답
        //  1   byte    서버 리스트 노드 개수
        //      *   string  서버 이름       ///< TRANS_SERVERLISTNODE
        //      1   byte    프로세스 상태
        //      1   byte    서버블 상태
        //      1   bool    되살리기 옵션
        //      4   int     World ID
        //      4   int     Server ID
        
        public const ushort GET_SERVERINFO_REQ = 17;    ///< 서버 정보 요청
        //  1   byte    서버 이름 개수
        //      *   string  서버 이름

        public const ushort GET_SERVERINFO_RES = 18;    ///< 서버 정보 응답
        //  1   byte    서버 정보 노드 개수
        //      *   string  서버 이름       ///< TRANS_SERVERINFONODE
        //      1   byte    서버 상태
        //      1   bool    되살리기 옵션
     

        public const ushort MONITORING_LOG = 19;        ///< 모니터링 로그
        //  *   string  서버 이름
        //  4   int     World ID
        //  4   int     Server ID
        //  *   string  로그 문자열

        public const ushort START_ALL_SERVER_REQ = 20;      ///< 모든 서버 시작 요청

        public const ushort TERMINATE_ALL_SERVER_REQ = 21;  ///< 모든 서버 정지 요청

    }
}
