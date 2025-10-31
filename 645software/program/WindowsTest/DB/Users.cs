using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsTest
{
    class Users
    {
        private string ip;
        public string Ip
        {
            get
            {
                return ip;
            }
            set
            {
                ip = value;
            }
        }
        private string status;
        public string Status
        {
            get
            {
                return status;
            }
            set
            {
                status = value;
            }
        }
        private string data;
        public string Data
        {
            get
            {
                return data;
            }
            set
            {
                data = value;
            }
        }
    }
}
