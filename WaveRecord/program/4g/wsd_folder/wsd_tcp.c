//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
#include <sys/time.h>
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#ifndef FD_SETSIZE
#define FD_SETSIZE 256
#endif
#define IPORURL "8.135.10.183"
#define MAX_LINE 800
#define PORT 34418
#define LISTENEQ 6000
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
/*标志*/
enum Flag
{
    YES_C, /*代表被禁言*/
    NO_C   /*代表没有被禁言*/
};

/*定义服务器--客户端 消息传送类型*/
enum MessageType
{
    REGISTER = 1,   /*注册请求*/
    LOGIN,          /*登陆请求*/
    HELP,           /*帮助请求*/
    EXIT,           /*退出请求*/
    VIEW_USER_LIST, /*查看在线列表*/
    GROUP_CHAT,     /*群聊请求*/
    PERSONAL_CHAT,  /*私聊请求*/
    VIEW_RECORDS,   /*查看聊天记录请求*/
    RESULT,         /*结果消息类型*/
    UNKONWN         /*未知请求类型*/
};

/*定义操作结果 */
enum StateRet
{
    Ret_EXCEED,         // 已达服务器链接上限
    Ret_SUCCESS,        // 成功
    Ret_FAILED,         // 失败
    Ret_DUPLICATEID,    // 重复的用户名
    Ret_INVALID,        // 不合法的用户名
    Ret_ID_NOT_EXIST,   // 账号不存在
    Ret_WRONGPWD,       // 密码错误
    Ret_ALREADY_ONLINE, // 已经在线
    Ret_ID_NOT_ONLINE,  // 账号不在线
    Ret_ALL_NOT_ONLINE, // 无人在线
    Ret_MESSAGE_SELF    // 消息对象不能选择自己
};

// 用户信息结构体
typedef struct _User
{
    char userName[20]; // 用户名
    char password[20];
    struct sockaddr_in userAddr; // 用户IP地址，选择IPV4
    int sockfd;                  // 当前用户套接字描述符
    int speak;                   // 是否禁言标志
    char registerTime[20];       // 记录用户注册时间
} User;

/*定义用户链表结构体*/
typedef struct _ListNode
{
    User user;
    struct _ListNode *next;
} ListNode;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
int tcp_connect_flag = 0;
int socket = 0;
int uart_hd = 1;
char CallName[20];
char CallmsgTime[20];
char url_or_ip[256] = IPORURL;
nwy_osiThread_t *tcp_recv_thread = NULL;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
int HeartSeconds = 0;
TSerial Serial_698;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
extern int nwy_data_get_flowcalc_info(nwy_data_flowcalc_info_t *flowcalc_info);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
void Tcp_send(int Isocket, char *sendbuff, int sendlen)
{
    int s = Isocket;
    int send_len = 0;
    if (!tcp_connect_flag)
    {
        nwy_ext_echo("\r\ntcp not setup");
        return;
    }
    send_len = nwy_socket_send(s, sendbuff, sendlen, 0);
    if (send_len != sendlen)
        nwy_ext_echo("\r\nsend len=%d, return len=%d", strlen(sendbuff), send_len);
    else
        nwy_ext_echo("\r\nsend ok");
    return;
}
static void nwy_tcp_recv_func(void *param)
{
    char recv_buff[NWY_UART_RECV_SINGLE_MAX + 1] = {0};
    int recv_len = 0, result = 0;
    int s = *(int *)param;
    fd_set rd_fd;
    fd_set ex_fd;
    FD_ZERO(&rd_fd);
    FD_ZERO(&ex_fd);
    FD_SET(s, &rd_fd);
    FD_SET(s, &ex_fd);
    struct timeval tv = {0};
    tv.tv_sec = 20;
    tv.tv_usec = 0;
    tTranData tcptouart;

    while (1)
    {
        FD_ZERO(&rd_fd);
        FD_ZERO(&ex_fd);
        FD_SET(s, &rd_fd);
        FD_SET(s, &ex_fd);
        result = nwy_socket_select(s + 1, &rd_fd, NULL, &ex_fd, &tv);
        nwy_ext_echo("\r\nlistien is ing\r\n");
        if (result < 0)
        {
            nwy_ext_echo("\r\nstarttask2\r\n");
            nwy_socket_close(s);
            *(int *)param = 0;
            nwy_sleep(60000); //等待60s
            tcp_connect_flag = 0;
            nwy_exit_thread();
        }
        else if (result > 0)
        {
            nwy_ext_echo("\r\nstarttask2\r\n");
            if (FD_ISSET(s, &rd_fd))
            {
                memset(recv_buff, 0, NWY_UART_RECV_SINGLE_MAX + 1);
                recv_len = nwy_socket_recv(s, recv_buff, NWY_UART_RECV_SINGLE_MAX, 0);
                if (recv_len > 0)
                {
                    HeartSeconds = 0;
                    memset(&tcptouart, 0, sizeof(tcptouart));
                    memcpy(&tcptouart, recv_buff, sizeof(recv_buff));
                    tcptouart.len = recv_len;
                    tcptouart.TranType = SERVER_TRAN;
                    nwy_ext_echo("\r\n tcp send messg length is:%d\r\n", recv_len);
#if (TRANS_SWITCH == TCP_TRANS)
                    if (nwy_put_msg_que(TranDataToUartMessageQueue, &tcptouart, 0xffffffff))
                    {
                        nwy_ext_echo("\r\n tcp send messg success\r\n");
                    }
#endif
                }
                else if (recv_len == 0)
                {
                    nwy_ext_echo("\r\ntcp close by server\r\n");
                    nwy_socket_close(s);
                    *(int *)param = 0;
                    nwy_sleep(60000); //等待60s
                    tcp_connect_flag = 0;
                    nwy_exit_thread();
                }
                else
                {
                    nwy_ext_echo("\r\ntcp connection error\r\n");
                    nwy_socket_close(s);
                    *(int *)param = 0;
                    nwy_sleep(60000); //等待60s
                    tcp_connect_flag = 0;
                    nwy_exit_thread();
                }
            }
            if (FD_ISSET(s, &ex_fd))
            {
                nwy_ext_echo("\r\ntcp select ex_fd:\r\n");
                nwy_socket_close(s);
                *(int *)param = 0;
                nwy_sleep(60000); //等待60s
                tcp_connect_flag = 0;
                nwy_exit_thread();
            }
        }
        else
            OSI_LOGI(0, "\r\ntcp select timeout:\r\n");

        nwy_sleep(1000);
    }
}

static int nwy_hostname_check(char *hostname)
{
    int a, b, c, d;
    char temp[32] = {0};
    if (strlen(hostname) > 15)
        return NWY_GEN_E_UNKNOWN;

    if ((sscanf(hostname, "%d.%d.%d.%d", &a, &b, &c, &d)) != 4)
        return NWY_GEN_E_UNKNOWN;

    if (!((a <= 255 && a >= 0) && (b <= 255 && b >= 0) && (c <= 255 && c >= 0)))
        return NWY_GEN_E_UNKNOWN;

    sprintf(temp, "%d.%d.%d.%d", a, b, c, d);
    // memset(hostname, 0, sizeof(hostname));
    strcpy(hostname, temp);
    return NWY_SUCESS;
}
void Tcp_connet(tTranData *transdata)
{
    char ip_buf[256] = {0};
    int af_inet_flag = AF_INET;
    int on = 1;
    int opt = 1, ret = 0;
    uint64_t start;
    ip_addr_t addr = {0};

    struct sockaddr_in sa_v4;

    if (tcp_connect_flag)
    {
        nwy_ext_echo("\r\ntcp alreay connect");
        return;
    }
    memset(ip_buf, 0, sizeof(ip_buf));
    // 此处只使用了IP  若为Url需进行dns
    memcpy(ip_buf, Serverpara[0].ServerIP, sizeof(Serverpara[0].ServerIP));

    ret = nwy_hostname_check(ip_buf);
    if (ret != NWY_SUCESS)
    {
        nwy_ext_echo("\r\ninput ip or url is invalid");
        return;
    }
    if (ip4addr_aton(ip_buf, &addr.u_addr.ip4) == 0)
    {
        nwy_ext_echo("\r\ninput ip error:\r\n");
        return;
    }
    inet_addr_from_ip4addr(&sa_v4.sin_addr, ip_2_ip4(&addr));
    sa_v4.sin_len = sizeof(struct sockaddr_in);
    sa_v4.sin_family = AF_INET;
    sa_v4.sin_port = htons(Serverpara[0].ServerPort);
    af_inet_flag = AF_INET;

    if (socket == 0)
    {
        socket = nwy_socket_open(af_inet_flag, SOCK_STREAM, IPPROTO_TCP);
        nwy_ext_echo("\r\ntcp open socket is%d\r\n", socket);
    }
    nwy_socket_setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));

    nwy_socket_setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
    if (0 != nwy_socket_set_nonblock(socket))
    {
        nwy_socket_close(socket);
        socket = 0;
        nwy_ext_echo("\r\nsocket set err\r\n");
        return;
    }
    start = nwy_get_ms();
    do
    {
        ret = nwy_socket_connect(socket, (struct sockaddr *)&sa_v4, sizeof(sa_v4));

        if (ret)
        {
            nwy_ext_echo("\r\nsocket first connect ok\r\n");
        }
        if ((nwy_get_ms() - start) >= 9000)
        {
            nwy_ext_echo("\r\nsocket connect timeout\r\n");
            nwy_socket_shutdown(socket, SHUT_RD);
            nwy_socket_close(socket);
            socket = 0;
            break;
        }
        if (ret == -1)
        {
            // nwy_ext_echo("errno=%d\r\n", nwy_socket_errno());
            if (EISCONN == nwy_socket_errno())
            {
                nwy_ext_echo("\r\nnwy_net_connect_tcp connect ok..");
                tcp_connect_flag = 1;
                Send_Heartbeatframe_698(transdata, 0); //连接成功后 发送一条心跳帧
                break;
            }
            if (EINPROGRESS != nwy_socket_errno() && EALREADY != nwy_socket_errno())
            {
                nwy_ext_echo("\r\nconnect errno = %d", nwy_socket_errno());
                nwy_ext_echo("\r\n error socket is %d", socket);
                nwy_socket_close(socket);
                socket = 0;
                tcp_connect_flag = 0;
                break;
            }
        }
        nwy_sleep(100);
    } while (1);
    if (tcp_connect_flag)
    {
        {
            tcp_recv_thread = nwy_create_thread("tcp_recv_thread", nwy_tcp_recv_func, (void *)&socket, NWY_OSI_PRIORITY_BELOW_NORMAL, 1024 * 15, 8);
            if (tcp_recv_thread == NULL)
            {
                nwy_ext_echo("\r\ncreate tcp recv thread failed, close connect");
                nwy_socket_close(socket);
                socket = 0;
                tcp_connect_flag = 0;
            }
        }
    }
    return;
}

void TCP_Task(void *param)
{
    nwy_sleep(600);
    tTranData UploadTranData;
    DWORD Connectfre = 0;

    while (1)
    {
        if (0 != nwy_ext_check_data_connect())
        {
            if (tcp_connect_flag == 0)
            {
                nwy_gpio_set_value(REMOTE_LIGHT, 0);
                Tcp_connet(&UploadTranData);
                if (tcp_connect_flag == 0) //连接失败时，每分钟执行一次
                {
                    Connectfre++;
                    nwy_ext_echo("\r\n%d\r\n", Connectfre);
                    nwy_sleep(60000); //等待60s
                }

                if (Connectfre > (60 * 12)) //连续12~14小时连接失败 模块复位一次 连接失败一次等待60s~70s
                {
                    nwy_power_off(2);
                }
            }
            else
            {
                Connectfre = 0;
                nwy_gpio_set_value(REMOTE_LIGHT, 1);
                HeartSeconds++;
                if (HeartSeconds == 60 * 5)
                {
                    HeartSeconds = 0;
                    Send_Heartbeatframe_698(&UploadTranData, 1);
                    {
                        char rsp[128] = {0};
                        nwy_data_flowcalc_info_t flowcalc_info = {0};
                        nwy_data_get_flowcalc_info(&flowcalc_info);
                        snprintf(rsp, sizeof(rsp), "tx_bytes:%llu,rx_bytes:%llu\r\ntx_packets:%lu,rx_packets:%lu",
                                 flowcalc_info.tx_bytes, flowcalc_info.rx_bytes,
                                 flowcalc_info.tx_packets, flowcalc_info.rx_packets);
                        nwy_ext_echo("\r\nGet data traffic statistics success\r\n%s\r\n", rsp);
                    }
                }
                if (nwy_get_msg_que(TranDataToServerMessageQueue, &UploadTranData, 0xffffffff))
                {
                    HeartSeconds = 0;
                    Tcp_send(socket, (char *)&UploadTranData.buf[0], UploadTranData.len);
                }
                if (nwy_get_msg_que(CollectMessageQueue, &InstantData698Frame[0], 100))
                {
                    Tcp_send(socket, (char *)&InstantData698Frame[0], sizeof(InstantData698Frame));
                }
            }
        }
        nwy_sleep(1000);
    }
}
