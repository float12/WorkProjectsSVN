using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.OleDb;

namespace WindowsTest
{
    class DB_AccessTool
    {
        public static object m_objSyncRoot = 0;
        private string m_strDataFileName = "";//数据库名
        public DB_AccessTool(string strDataFileName)
        {
            this.m_strDataFileName = strDataFileName;
        }

        /**/
        /// <summary>
        /// 获得连接对象
        /// </summary>
        /// <returns></returns>
        public OleDbConnection GetOleDbConnection()
        {
            string connstr;
            if (m_strDataFileName != "")
                connstr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + m_strDataFileName + ".accdb;Persist Security Info=True;Jet OLEDB:Database Password=\"1100\"";
            else
                connstr = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=WisdomDB.mdb;Persist Security Info=True;Jet OLEDB:Database Password=\"1100\"";
            return new OleDbConnection(connstr);
        }
        /// <summary>
        /// 执行语句
        /// </summary>
        /// <param name="cmdText">sql</param>
        /// <returns></returns>
        public int ExecuteNoneQuery(string cmdText)
        {
            int val = 0;
            lock (m_objSyncRoot)
            {
                using (OleDbConnection connection = GetOleDbConnection())
                {
                    OleDbCommand command = new OleDbCommand(cmdText, connection);
                    connection.Open();
                    try
                    {
                        val = command.ExecuteNonQuery();
                    }
                    catch (Exception e)
                    {
                        Console.Write(e.Message);
                        val = -1;
                    }
                    finally
                    {
                        connection.Close();
                    }
                }
            }
            return val;
        }
    }
}
