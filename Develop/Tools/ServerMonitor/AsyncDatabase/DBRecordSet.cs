using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SqlClient;

namespace AsyncDatabase
{
    public class DBRecordSet
    {
        private Database m_refDB = null;
        private SqlDataReader m_sqlDataReader = null;

        public Database refDB               { get { return m_refDB; } }

        public DBRecordSet(Database refDB)  { m_refDB = refDB; }
        public bool HasRows { get { return m_sqlDataReader.HasRows; } }

        public bool Open(string strSQL)
        {
            if (m_refDB.CheckDBReconnect() == false) return false;
            if (m_refDB.ExecuteSQL(strSQL, out m_sqlDataReader) == false) return false;
            if (m_sqlDataReader.HasRows)
                return m_sqlDataReader.Read();
            return true;
        }
        public void Close()                 { if (m_sqlDataReader != null) m_sqlDataReader.Close(); }

        public bool NextRecord()            { return m_sqlDataReader.Read(); }
	    public bool NextTable()		        { return m_sqlDataReader.NextResult(); }

        #region Getter - 일반적으로 많이 사용하는 몇 가지만 만듬. 필요한건 그때 그때 추가.
        public bool GetBool(string strColName, out bool outValue)
        {
            return GetBool(GetColumnIndex(strColName), out outValue);
        }
        public bool GetBool(int nColIndex, out bool outValue)
        {
            outValue = false;
            if (nColIndex < 0) return false;
            try {   outValue = m_sqlDataReader.GetSqlBoolean(nColIndex).Value; }
            catch { return false; }
            return true;
        }

        public bool GetByte(string strColName, out byte outValue)
        {
            return GetByte(GetColumnIndex(strColName), out outValue);
        }
        public bool GetByte(int nColIndex, out byte outValue)
        {
            outValue = 0;
            if (nColIndex < 0) return false;
            try { outValue = m_sqlDataReader.GetSqlByte(nColIndex).Value; }
            catch { return false; }
            return true;
        }

        public bool GetString(string strColName, out string outValue)
        {
            return GetString(GetColumnIndex(strColName), out outValue);
        }
        public bool GetString(int nColIndex, out string outValue)
        {
            outValue = "";
            if (nColIndex < 0) return false;
            try { outValue = m_sqlDataReader.GetSqlString(nColIndex).Value; }
            catch { return false; }
            return true;
        }

        public bool GetInt16(string strColName, out short outValue)
        {
            return GetInt16(GetColumnIndex(strColName), out outValue);
        }
        public bool GetInt16(int nColIndex, out short outValue)
        {
            outValue = -1;
            if (nColIndex < 0) return false;
            try { outValue = m_sqlDataReader.GetSqlInt16(nColIndex).Value; }
            catch { return false; }
            return true;
        }

        public bool GetInt32(string strColName, out int outValue)
        {
            return GetInt32(GetColumnIndex(strColName), out outValue);
        }
        public bool GetInt32(int nColIndex, out int outValue)
        {
            outValue = -1;
            if (nColIndex < 0) return false;
            try { outValue = m_sqlDataReader.GetSqlInt32(nColIndex).Value; }
            catch { return false; }
            return true;
        }

        public bool GetInt64(string strColName, out Int64 outValue)
        {
            return GetInt64(GetColumnIndex(strColName), out outValue);
        }
        public bool GetInt64(int nColIndex, out Int64 outValue)
        {
            outValue = -1;
            if (nColIndex < 0) return false;
            try { outValue = m_sqlDataReader.GetSqlInt64(nColIndex).Value; }
            catch { return false; }
            return true;
        }

        public bool GetFloat(string strColName, out float outValue)
        {
            return GetFloat(GetColumnIndex(strColName), out outValue);
        }
        public bool GetFloat(int nColIndex, out float outValue)
        {
            outValue = 0;
            if (nColIndex < 0) return false;
            try { outValue = m_sqlDataReader.GetSqlSingle(nColIndex).Value; }
            catch { return false; }
            return true;
        }

        public bool GetDateTime(string strColName, out DateTime outValue)
        {
            return GetDateTime(GetColumnIndex(strColName), out outValue);
        }
        public bool GetDateTime(int nColIndex, out DateTime outValue)
        {
            outValue = DateTime.MinValue;
            if (nColIndex < 0) return false;
            try { outValue = m_sqlDataReader.GetDateTime(nColIndex); }
            catch { return false; }
            return true;
        }
        #endregion

        private int GetColumnIndex(string strColName)
        {
	        int nIndex;
	        try { nIndex = m_sqlDataReader.GetOrdinal(strColName); }
	        catch { nIndex = -1; }
	        return nIndex;
        }

    }

}
