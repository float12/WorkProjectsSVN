using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace WindowsTest
{
    class Log
    {
        public static object locker = new object();
 
/// <summary>
/// 将异常打印到LOG文件
/// </summary>
/// <param name="ex">异常</param>
/// <param name="LogAddress">日志文件地址</param>
/// <param name="Tag">传入标签（这里用于标识函数由哪个线程调用）</param>
public static void WriteLog(object ob)
{
    lock (locker)
    {
        //如果日志文件为空，则默认在Debug目录下新建 YYYY-mm-dd_Log.log文件
        string LogAddress = Environment.CurrentDirectory + '\\' +
                DateTime.Now.Year + '-' +
                DateTime.Now.Month + '-' +
                DateTime.Now.Day + "_Log.log";
        
        //把信息输出到文件
        Users u = ob as Users;
        StreamWriter sw = new StreamWriter(LogAddress, true);
        sw.WriteLine(String.Concat('[', DateTime.Now.ToString(), "]") + " 地址:" + u.Ip+" 状态:"+u.Status);
        sw.WriteLine("数据：" + u.Data);
        sw.WriteLine();
        sw.Close();
    }
}
    }
}
