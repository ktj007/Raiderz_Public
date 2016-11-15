using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SqlClient;

namespace AsyncDatabase
{
    public class Database
    {
        private SqlConnection m_sqlConnection = new SqlConnection();
        private string m_strLastErrorMsg;
        private int m_nLastFetchedCount = 0;

        public string LastErrorMsg { get { return m_strLastErrorMsg; } }
        public int LastFetchedCount { get { return m_nLastFetchedCount; } }
        public bool IsOpen { get { return (m_sqlConnection.State != System.Data.ConnectionState.Closed); } }// && (m_sqlConnection.State != System.Data.ConnectionState.Broken); } }

        public bool Connect(string strDSN)
        {
            try
            {
                m_sqlConnection.ConnectionString = strDSN;
                m_sqlConnection.Open();
            }
            catch (SqlException e)
            {
                m_strLastErrorMsg = "[Database.Connect()] " + e.Number + " : " + e.Message;
                return false;
            }
            catch (SystemException e)
            {
                m_strLastErrorMsg = "[Database.Connect()] " + e.Message;
                return false;
            }
            return true;
        }
        public bool CheckDBReconnect()
        {
            if (IsOpen == false)
            {
                Close();
                if (Connect(m_sqlConnection.ConnectionString) == false)
                    return false;
            }
            return true;
        }
        public void Close()
        {
            try
            {
                m_sqlConnection.Close();
            }
            catch {}
        }

        public bool ExecuteSQL(string strSQL)
        {
            SqlCommand command = new SqlCommand(strSQL, m_sqlConnection);
            try
            {
                command.ExecuteNonQuery();
            }
            catch (SqlException e)
            {
                m_strLastErrorMsg = "[Database.ExecuteSQL()] " + e.Number + " : " + e.Message;
                return false;
            }
            catch (SystemException e)
            {
                m_strLastErrorMsg = "[Database.ExecuteSQL()] " + e.Message;
                return false;
            }
            finally
            {
                command.Dispose();
            }
            return true;
        }

        public bool ExecuteSQL(string strSQL, out SqlDataReader outDataReader)
        {
            outDataReader = null;

            SqlCommand command = new SqlCommand(strSQL, m_sqlConnection);
            try
            {
                outDataReader = command.ExecuteReader();
            }
            catch (SqlException e)
            {
                m_strLastErrorMsg = "[Database.ExecuteSQL(,)] " + e.Number + " : " + e.Message;
                return false;
            }
            catch (SystemException e)
            {
                m_strLastErrorMsg = "[Database.ExecuteSQL(,)] " + e.Message;
                return false;
            }
            finally
            {
                command.Dispose();
            }
            return true;
        }

    }
}
