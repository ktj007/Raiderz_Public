using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace SMCommon
{
    public class NetCommand
    {
        private readonly int MAX_BUFFER_SIZE = 8192;
        private readonly int HEADER_SIZE = 4;

        private UInt64 m_nSessionID = 0;
        private int m_nCommandID = 0;
        private byte[] m_arrCommandBuffer = null;
        private int m_nBufferReadPosition = 0;
        private int m_nBufferWritePosition = 0;

        public int CommandID { get { return m_nCommandID; } }
        public byte[] Buffer { get { return m_arrCommandBuffer; } }
        public int BufferSize { get { return m_nBufferWritePosition; } }
        public UInt64 SessionID { get { return m_nSessionID; } }

        public NetCommand(UInt64 nSessionID, int nCommandID)
        {
            m_nSessionID = nSessionID;
            m_nCommandID = nCommandID;
            m_arrCommandBuffer = new byte[MAX_BUFFER_SIZE];

            m_nBufferWritePosition = HEADER_SIZE;
            m_nBufferReadPosition = HEADER_SIZE;
        }

        public NetCommand(UInt64 nSessionID, int nCommandID, byte[] bufferRecvData, int nDataLength)
        {
            m_nSessionID = nSessionID;
            m_nCommandID = nCommandID;
            m_arrCommandBuffer = new byte[MAX_BUFFER_SIZE];

            m_nBufferWritePosition = HEADER_SIZE;
            m_nBufferReadPosition = HEADER_SIZE;

            WriteByteArray(bufferRecvData, nDataLength);
            MakingComplete();
        }

        public bool MakingComplete()
        {
            try
            {
                MemoryStream stream = new MemoryStream(m_arrCommandBuffer);

                byte[] bCommandLength = BitConverter.GetBytes((ushort)(m_nBufferWritePosition));
                stream.Write(bCommandLength, 0, sizeof(ushort));

                byte[] bCommandID = BitConverter.GetBytes((ushort)m_nCommandID);
                stream.Write(bCommandID, 0, sizeof(ushort));
            }
            catch { return false; }
            return true;
        }

        public bool ReadBoolean(out bool bRet)
        {
            bRet = false;
            try
            {
                bRet = BitConverter.ToBoolean(m_arrCommandBuffer, m_nBufferReadPosition++);
            } catch { return false; }
            return true;
        }
        public bool ReadByte(out byte byRet)
        {
            byRet = 0;
            try
            {
                byRet = m_arrCommandBuffer[m_nBufferReadPosition++];
            } catch { return false; }
            return true;
        }
        public bool ReadInt32(out Int32 nRet)
        {
            nRet = 0;
            try
            {
                nRet = BitConverter.ToInt32(m_arrCommandBuffer, m_nBufferReadPosition);
                m_nBufferReadPosition += sizeof(Int32);
            }
            catch { return false; }
            return true;
        }
        public bool ReadUInt64(out UInt64 nRet)
        {
            nRet = 0;
            try
            {
                nRet = BitConverter.ToUInt64(m_arrCommandBuffer, m_nBufferReadPosition);
                m_nBufferReadPosition += sizeof(UInt64);
            }
            catch { return false; }
            return true;
        }
        public bool ReadString(out string strRet)
        {
            strRet = "";
            try
            {
                ushort nStringLength = BitConverter.ToUInt16(m_arrCommandBuffer, m_nBufferReadPosition);
                m_nBufferReadPosition += sizeof(UInt16);

                byte[] tmpString = new byte[nStringLength];
                Array.Copy(m_arrCommandBuffer, m_nBufferReadPosition, tmpString, 0, nStringLength);
                strRet = Encoding.UTF8.GetString(tmpString);

                m_nBufferReadPosition += nStringLength;
            } catch { return false; }
            return true;
        }

        public bool WriteBoolean(bool bData)
        {
            try
            {
                MemoryStream stream = new MemoryStream(m_arrCommandBuffer, m_nBufferWritePosition, MAX_BUFFER_SIZE - m_nBufferWritePosition);
                byte bTemp = (byte)(bData ? 1 : 0);
                stream.WriteByte(bTemp);

                m_nBufferWritePosition += sizeof(byte);
            }
            catch { return false; }
            return true;
        }
        public bool WriteByte(byte bData)
        {
            try
            {
                MemoryStream stream = new MemoryStream(m_arrCommandBuffer, m_nBufferWritePosition, MAX_BUFFER_SIZE - m_nBufferWritePosition);
                stream.WriteByte(bData);

                m_nBufferWritePosition += sizeof(byte);
            } catch { return false; }
            return true;
        }
        public bool WriteInt32(Int32 nData)
        {
            try
            {
                MemoryStream stream = new MemoryStream(m_arrCommandBuffer, m_nBufferWritePosition, MAX_BUFFER_SIZE - m_nBufferWritePosition);
                byte[] arrParamData = BitConverter.GetBytes(nData);
                stream.Write(arrParamData, 0, arrParamData.Length);

                m_nBufferWritePosition += sizeof(Int32);
            }
            catch { return false; }
            return true;
        }
        public bool WriteUInt64(UInt64 nData)
        {
            try
            {
                MemoryStream stream = new MemoryStream(m_arrCommandBuffer, m_nBufferWritePosition, MAX_BUFFER_SIZE - m_nBufferWritePosition);
                byte[] arrParamData = BitConverter.GetBytes(nData);
                stream.Write(arrParamData, 0, arrParamData.Length);

                m_nBufferWritePosition += sizeof(UInt64);
            }
            catch { return false; }
            return true;
        }
        public bool WriteString(string strData)
        {
            try
            {
                MemoryStream stream = new MemoryStream(m_arrCommandBuffer, m_nBufferWritePosition, MAX_BUFFER_SIZE - m_nBufferWritePosition);
                byte[] arrParamData = Encoding.UTF8.GetBytes(strData.ToCharArray());

                byte[] nStringLength = BitConverter.GetBytes((ushort)arrParamData.Length);
                stream.Write(nStringLength, 0, 2);
                m_nBufferWritePosition += 2;

                stream.Write(arrParamData, 0, arrParamData.Length);
                m_nBufferWritePosition += arrParamData.Length;
            }
            catch { return false; }
            return true;
        }
        public bool WriteByteArray(byte[] arrData, int nLength)
        {
            try
            {
                MemoryStream stream = new MemoryStream(m_arrCommandBuffer, m_nBufferWritePosition, MAX_BUFFER_SIZE - m_nBufferWritePosition);
                stream.Write(arrData, 0, nLength);

                m_nBufferWritePosition += nLength;
            }
            catch { return false; }
            return true;
        }

    }
}
