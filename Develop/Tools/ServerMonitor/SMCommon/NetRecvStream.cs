using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.IO;

namespace SMCommon
{
    public class NetRecvStream
    {
        private const int MAX_BUFFER_SIZE = 2048;//8192;
        private const int MAX_BUFFER_MARGIN_SIZE = 512;//2048;
        private readonly int SIZE_CMD_HEADER = 4;

        private UInt64 m_sessionID = 0;
        private BinaryReader m_reader = null;
        private byte[] m_recvBuffer = new byte[MAX_BUFFER_SIZE + MAX_BUFFER_MARGIN_SIZE];
        private int m_nReceivedPacketSize = 0;
        private int m_nCursorWriteToBuffer = 0;
        private int m_nCursorReadFromBuffer = 0;

        public NetRecvStream(UInt64 sessionID, NetworkStream stream)
        {
            m_sessionID = sessionID;
            m_reader = new BinaryReader(stream);
        }
        public void Close()
        {
            m_reader.Close();
        }

        public bool Recv()
        {
            try
            {
                int nJustReceivedSize = m_reader.Read(m_recvBuffer, m_nCursorWriteToBuffer, m_recvBuffer.Length - m_nCursorWriteToBuffer);
                if (nJustReceivedSize == 0)
                    return false;

                m_nReceivedPacketSize += nJustReceivedSize;
                m_nCursorWriteToBuffer += nJustReceivedSize;
            }
            catch (System.ArgumentOutOfRangeException e)
            {
                FileLog.Instance.Write("버퍼의 인덱스가 잘못되었습니다. 받은 패킷 크기 : {0}, 버퍼를 쓸 위치 : {1},  예외 메시지 : {2}", m_nReceivedPacketSize, m_nCursorWriteToBuffer, e.Message);
                return false;
            }
            catch (System.ObjectDisposedException e)
            {
                FileLog.Instance.Write("NetworkStream이 닫혔거나 네트워크에서 읽는 동안 오류가 발생했습니다. 예외 메시지 : {0}", e.Message);
                return false;
            }
            catch (System.IO.IOException e)
            {
                // Socket Close
                return false;
            }
            return true;
        }

        public bool MakeCommand(out NetCommand command)
        {
            int nCommandID = 0;
            byte[] arrCommandData = null;
            command = null;

            if (m_nReceivedPacketSize < SIZE_CMD_HEADER) return false;

            byte[] arrPacketHeader = new byte[4];
            Array.Copy(m_recvBuffer, m_nCursorReadFromBuffer, arrPacketHeader, 0, 4);
            int nPacketSize = BitConverter.ToUInt16(arrPacketHeader, 0);
            nCommandID = BitConverter.ToUInt16(arrPacketHeader, 2);

            if (m_nReceivedPacketSize < nPacketSize) return false;

            int nCommandDataSize = nPacketSize - SIZE_CMD_HEADER;

            arrCommandData = new byte[nCommandDataSize];
            Array.Copy(m_recvBuffer, m_nCursorReadFromBuffer + SIZE_CMD_HEADER, arrCommandData, 0, nCommandDataSize);

            m_nReceivedPacketSize -= nPacketSize;
            m_nCursorReadFromBuffer += nPacketSize;

            command = new NetCommand(m_sessionID, nCommandID, arrCommandData, nCommandDataSize);

            if (m_nCursorWriteToBuffer >= MAX_BUFFER_SIZE)
            {
                Array.Copy(m_recvBuffer, m_nCursorReadFromBuffer, m_recvBuffer, 0, m_nCursorWriteToBuffer - m_nCursorReadFromBuffer);
                m_nCursorWriteToBuffer = 0;
                m_nCursorReadFromBuffer = 0;
            }
            return true;
        }

    }
}
