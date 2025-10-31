//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
   	eLogin = 1,         //登录
   	eGetSize,    		//获取文件长度
   	eDownload,        	//下载文件
   	eAllUpdate,      	//全包升级
   	eWait,    			//等待回调函数
	eError,				//执行失败						退出登录和erro 时需统一 清除所有有关升级的标志
	eTrueLogout,		//正常退出
	eFalseLogout,		//异常退出,8
	eDbUpdate,			//电表升级
}eFtpUpdateStep;		//ftp升级流程标志
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
nwy_ftp_handle_t g_ftp_fota_handl = NULL;
static nwy_file_ftp_info_s g_fileftp;
char FtpupdateStep = eError;  
char UpdateMode = 1;//差分升级或全包升级判断
char FailCount = 0;
WORD Ftp_Timer;//ftp升级单独定时器
BYTE BaseMeterUpgradeProtect = 0;//ftp下载基表升级文件防护
BYTE bResult = 0xFF;	//ftp升级上报相关流程
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
void nwy_ftp_timer_cb(void *type)
{
	Ftp_Timer = 0;
}
//--------------------------------------------------
//功能描述:  app升级函数 全包或差分
//         
//参数:      文件名
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  update_app( char* filename )
{
	int fd;
	nwy_ota_package_t ota_pack = {0};
	// nwy_ota_download_param_t ota_download_param;
	int ota_size = 0;
	int tmp_len = 0;
	int read_len = 0;
	char buff[NWY_APPIMG_FOTA_BLOCK_SIZE] = {0};
	char Tempfilepath[FILENAME_LEM] = "/nwy/";
	int ret = -1;

	// memset(&ota_download_param, 0, sizeof(nwy_ota_download_param_t));
	// ota_download_param.dl_mode = NWY_FOTA_DOWNLOAD_FTP; //ftp下载
	strcat(Tempfilepath,filename);
	fd = nwy_file_open(Tempfilepath, NWY_RDONLY);
	nwy_ext_echo("\r\ntmp filepath is %s",Tempfilepath);
	if(fd < 0)
	{
		nwy_ext_echo("\r\nopen appimg fail\r\n");
		return;
	}

	ota_pack.data = (unsigned char *)malloc(NWY_APPIMG_FOTA_BLOCK_SIZE);
	ota_pack.len = 0;
	ota_pack.offset = 0;

	if(ota_pack.data == NULL)
	{
		nwy_ext_echo("\r\nmalloc fail\r\n");
		//升级失败？
		return;
	}
	ota_size = nwy_file_fd_size(fd);
	nwy_file_seek(fd, 0, NWY_SEEK_SET);
	
	while(ota_size > 0)
	{
		tmp_len = NWY_APPIMG_FOTA_BLOCK_SIZE;
		if(ota_size < tmp_len)
		{
			tmp_len = ota_size;
		}
	
		memset(buff,0,sizeof(buff));
		read_len = nwy_file_read(fd, buff, tmp_len);
		if(read_len != tmp_len)
		{
			nwy_ext_echo("read file error:%d\r\n", read_len);
			free(ota_pack.data);
			nwy_file_close(fd);
			nwy_file_remove(Tempfilepath);
			return;
		}
		memcpy(ota_pack.data, buff, read_len);
		ota_pack.len = read_len;
		ret = nwy_fota_write(&ota_pack);
		if (ret == NWY_FOTA_CHECK_FAILED)
		{
			nwy_ext_echo("\r\n The fota package does not matche the current version\r\n");
			free(ota_pack.data);
			nwy_file_close(fd);
			nwy_file_remove(Tempfilepath);
			return;
		}
		else if (ret == NWY_FOTA_WRITE_FAILED)
		{
			nwy_ext_echo("\r\n fota write fail\r\n");
			free(ota_pack.data);
			nwy_file_close(fd);
			nwy_file_remove(Tempfilepath);
			return;
		}
		else if (ret == NWY_FS_SPACE_NOT_ENOUGH)
		{
			nwy_ext_echo("\r\n file system space not enough\r\n");
			free(ota_pack.data);
			nwy_file_close(fd);
			nwy_file_remove(Tempfilepath);
			return;
		}
		else if (ret == NWY_GEN_E_INVALID_PARA)
		{
			nwy_ext_echo("\r\n function input parameters error\r\n");
			free(ota_pack.data);
			nwy_file_close(fd);
			nwy_file_remove(Tempfilepath);
			return;
		}
		else if (ret == NWY_GEN_E_MALLOC_FAIL)
		{
			nwy_ext_echo("\r\n failed to apply for memory.\r\n");
			free(ota_pack.data);
			nwy_file_close(fd);
			nwy_file_remove(Tempfilepath);
			return;
		}
		else
		{
			nwy_ext_echo("\r\n write fota file success");
		}
		if(ret != 0)
		{
			nwy_ext_echo("write ota error: written len %d,len:%d\r\n", ret,ota_pack.len);
			free(ota_pack.data);
			nwy_file_close(fd);
			nwy_file_remove(Tempfilepath);
			return;
		}
		ota_pack.offset += read_len;
		memset(ota_pack.data, 0, read_len);
		ota_size -= read_len;
	}
	free(ota_pack.data);
	nwy_file_close(fd);
	nwy_file_remove(Tempfilepath);
	nwy_ext_echo("\r\n update app suc,reboot");
	ret = nwy_fota_update(1);
    if(ret == NWY_FOTA_CHECK_FAILED)
    {
        nwy_ext_echo("\r\n fota package check fail\r\n");
    }	
}
int ftp_write_file(nwy_file_ftp_info_s *pFileFtp, unsigned char *data, unsigned int len)
{
	char file_name[128 + 1] = "/nwy/";
	int writeLen = 0;
	int ret = 0;
	unsigned long long size= 0;
	int fs = 0;
	
	ret = nwy_vfs_free_size_get(NULL,&size);
	if (ret != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\nnwy vfs free size get fail");
		return -1;
	}
	strcat(file_name, pFileFtp->filename);
	if (nwy_file_exist(file_name) == FALSE)
	{
		fs = nwy_file_open(file_name, NWY_WB_PLUS_MODE);
		if(fs < 0)
		{
			nwy_ext_echo("\r\n nwy file open fail,%d", fs);
			return -1;
		}
		else
		{
			nwy_file_close(fs);
			nwy_ext_echo("\r\n nwy file open success");
		}
	}
	fs = nwy_file_open(file_name, NWY_RB_PLUS_MODE);
	if (fs < 0)
	{
		nwy_ext_echo("\r\n nwy file open fail,%d", fs);
		return -1;
	}
	nwy_file_seek(fs, pFileFtp->pos, NWY_SEEK_SET);
	
	writeLen = nwy_file_write(fs, data, len);
	if (writeLen != len)
	{
		nwy_ext_echo("\r\n nwy file write fail,written len=%d, len:%d", writeLen,len);
		ret = nwy_file_close(fs);
		if(ret != NWY_SUCCESS)
		{
			nwy_ext_echo("\r\n nwy file close fail");
		}
		return -1;
	}
	ret  =nwy_file_close(fs);
	if(ret != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n nwy file close fail");
	}
	pFileFtp->pos += len;
	return 0;
}
//--------------------------------------------------
//功能描述:  ftp_result
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void nwy_ftp_result_cb(nwy_ftp_result_t *param)
{
	int *size = NULL;
	int ret = 0;

	if(NULL == param)
	{
		nwy_ext_echo("event is NULL\r\n");
		FtpupdateStep = eFalseLogout;
	}

	if(NWY_FTP_EVENT_LOGIN == param->event)
	{
		nwy_ext_echo("\r\nFtp login success");
		FtpupdateStep = eGetSize;
	}
	else if(NWY_FTP_EVENT_PASS_ERROR == param->event)
	{
		nwy_ext_echo("\r\nFtp passwd error");
		FtpupdateStep = eFalseLogout;
	} 
	else if(NWY_FTP_EVENT_FILE_NOT_FOUND == param->event)
	{
		nwy_ext_echo("\r\nFtp file not found");
		FtpupdateStep = eFalseLogout;
	}
	else if(NWY_FTP_EVENT_FILE_SIZE_ERROR == param->event)
	{
		nwy_ext_echo("\r\nFtp file size error");
		FtpupdateStep = eFalseLogout;
	}
	else if(NWY_FTP_EVENT_SIZE == param->event)
	{
		size = (int*)param->data;
		tIap_645.dwFileSize = *size;
		nwy_ext_echo("\r\nFtp size is %d", *size);
		FtpupdateStep = eDownload;
	}
	else if(NWY_FTP_EVENT_LOGOUT == param->event)
	{
		nwy_ext_echo("\r\nFtp logout");
		// FtpupdateStep = eError;
	}
	else if(NWY_FTP_EVENT_CLOSED == param->event)
	{
		nwy_ext_echo("\r\nFtp connection closed");
        if ((tIap_645.dwIapFlag == DOWNLOAD_METERFILE) || (tIap_698.dwIapFlag == DOWNLOAD_METERFILE)) // 模块给电表升级
		{
		}
		else
		{
			FtpupdateStep = eLogin;
			//连接通道关闭，需重新登陆一下
		}
	}
	else if(NWY_FTP_EVENT_FILE_DELE_SUCCESS == param->event)
	{
		nwy_ext_echo("\r\nFtp file del success");
	}
	else if(NWY_FTP_EVENT_DATA_PUT_FINISHED == param->event)
	{
		nwy_ext_echo("\r\nFtp put file success");
	}
	else if(NWY_FTP_EVENT_DNS_ERR == param->event || NWY_FTP_EVENT_OPEN_FAIL == param->event)
	{
		nwy_ext_echo("\r\nFtp login fail");
		FtpupdateStep = eFalseLogout;
		//某个参数不对，导致登录不上，无需重新登陆，无需等待
	}
	else if(NWY_FTP_EVENT_DATA_GET == param->event)//  在这加上上报操作
	{
		//nwy_ext_echo("\r\n recv data");
		if(NULL == param->data)
		{
			nwy_ext_echo("\r\nrecved data is NULL");
			FtpupdateStep = eFalseLogout;
			return;
		}

		if(1 == g_fileftp.is_vaild)
		{
			if(param->data_len != 0)
			{
				ret = ftp_write_file(&g_fileftp, (unsigned char *)param->data, param->data_len);
				if(0 != ret)
				{
					nwy_ext_echo("\r\nwrite failed");
					FtpupdateStep = eFalseLogout;
				}
			}
		}
		else
		{
			//nwy_ext_echo("\r\ndata_size is %d", param->data_len);
			//nwy_ext_echo("\r\nparam->data is %s", param->data);
		}
	}
	else if(NWY_FTP_EVENT_DATA_CLOSED == param->event)
	{
		nwy_ext_echo("\r\ndata closed,file %s download success.", g_fileftp.filename);
		g_fileftp.is_vaild = 0;
		 if ((tIap_645.dwIapFlag == DOWNLOAD_METERFILE) || (tIap_698.dwIapFlag == DOWNLOAD_METERFILE)) // 模块给电表升级
		{
			FtpupdateStep = eDbUpdate;
			BaseMeterUpgradeProtect = eDbUpdate;
		}
		else
		{
			FtpupdateStep = eAllUpdate;
		}
		bResult = eFtpDownLoadSuccess;
	}
	else if(NWY_FTP_EVENT_DATA_GET_FINISHED == param->event)//！！！
	{
		nwy_ext_echo("\r\nFtp data get finished.");
		// FtpupdateStep = eFalseLogout;
	}
	else
	{
		nwy_ext_echo("\r\n other ftp cb event:%d", param->event);
		//正常下载会进入
	}
	return;
}

//--------------------------------------------------
//功能描述:  连接ftp服务器 并升级
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
static void  api_VersionUpgrades(FtpUpdatePara* ftppara)
{
	nwy_ftp_param_t	ftp_login_param;
	nwy_ftp_get_param_t ftp_get_param;
	char buf[FILENAME_LEM] = {0};
	nwy_event_msg_t event;
	char ch = '/';
	char *ptr = NULL;
	char Tempfilepath[FILENAME_LEM] ="/nwy/";
	
	memset(&ftp_get_param,0x00,sizeof(ftp_get_param));
	memset(&ftp_login_param, 0x00, sizeof(ftp_login_param));
	memset(buf,0,sizeof(buf));
	memset(&event, 0, sizeof(event));
	memcpy( (char*)&ftp_login_param, (char*)&ftppara->ftp_param, sizeof(ftp_login_param));
	ftp_login_param.cid = 1;
	ftp_login_param.timeout_s = 180;
	ftp_login_param.cb = nwy_ftp_result_cb;
	ftp_login_param.mode = 0;
	strcpy(buf,ftppara->filename);
	nwy_thread_event_wait(ftp_thread, &event, 200);

	switch (FtpupdateStep)
	{
		case eLogin:
		// nwy_ext_echo("\r\n ftp_login_param. host is %s",ftp_login_param.host);
		// nwy_ext_echo("\r\n ftp_login_param. port is %d",ftp_login_param.port);
		// nwy_ext_echo("\r\n ftp_login_param. username is %s",ftp_login_param.username);
		// nwy_ext_echo("\r\n ftp_login_param. password is %s",ftp_login_param.password);
		// nwy_ext_echo("\r\n ftppara. password is %s",ftppara->ftp_param.password);
		// nwy_ext_echo("\r\nftp_login_param. mode is %d",ftp_login_param.mode);
			g_ftp_fota_handl = nwy_ftp_login(&ftp_login_param, NULL);
			if (g_ftp_fota_handl == NULL)
			{
				nwy_ext_echo("\r\n nwy_ftp_login fail");
				FtpupdateStep = eLogin;
				FailCount++;
				if (FailCount>5)
				{
					FailCount = 0;
					FtpupdateStep = eFalseLogout;
				}
			}
            else
            {
                nwy_ext_echo("\r\n nwy_ftp_login success");
				bResult = eFtpLoginSuccess;
				FtpupdateStep = eWait;
				FailCount = 0;
            }
			break;
		case eGetSize:
			if(nwy_ftp_get_filesize(g_ftp_fota_handl, buf) != NWY_SUCCESS)
			{
				FtpupdateStep = eGetSize;
				FailCount++;
				if (FailCount>3)
				{
					FtpupdateStep = eFalseLogout;
				}
				nwy_ext_echo("\r\n nwy_ftp_filesize fail");
			}
            else
            {
                FtpupdateStep = eWait;
				FailCount = 0;
            }
			break;
		case eDownload:
			g_fileftp.is_vaild = 1;
            g_fileftp.pos = 0;
			nwy_ext_echo("\r\ng_fileftp.pos set to 0");
			memset(g_fileftp.filename,0,sizeof(g_fileftp.filename));
			ptr = strchr(buf,ch);//查找字符‘/’后面认为是升级文件，仅支持下发命令带一层文件夹 qingdaofile/app.bin
			if (ptr == NULL)
			{
				FtpupdateStep = eFalseLogout;
				nwy_ext_echo("\r\n can not find file ");
				return;
			}
			strcpy(g_fileftp.filename, ptr+1);
			ftp_get_param.type = 2;//binary
			ftp_get_param.len = 0;
			ftp_get_param.offset = 0;
			ftp_get_param.remotefile = buf;
			nwy_ext_echo("\r\n remotefile is %s",ftp_get_param.remotefile);
			if(nwy_ftp_get(g_ftp_fota_handl,&ftp_get_param) != NWY_SUCCESS)//固定二进制下载？
			{
				FtpupdateStep = eDownload;
				FailCount++;
				if (FailCount>3)
				{
					FtpupdateStep = eFalseLogout;
				}
				nwy_ext_echo("\r\n get file fail");
                return;
			}
            else
            {
                FtpupdateStep = eWait;
				FailCount = 0;
            }
			break;
		case eAllUpdate:
			nwy_msg_queue_send(UpgradeResultMessageQueue,1 , &bResult,NWY_OSA_NO_SUSPEND);	
			ptr = strchr(buf,ch);//查找字符‘/’后面认为是升级文件
			if (ptr == NULL)
			{
				FtpupdateStep = eFalseLogout;
				return;
			}
			//自己算校验与文件名比对
			update_app(ptr+1);
			//到这 算是 升级模块失败，下载的任何文件都必须删除掉
			strcat(Tempfilepath,ptr+1);
			nwy_file_remove(Tempfilepath);
			FtpupdateStep = eFalseLogout;
			bResult = eFtpModuleUpgradeFail;
			break;
		case eTrueLogout:
			if(nwy_ftp_logout(g_ftp_fota_handl) !=0)//！！！
			{
				nwy_ext_echo("\r\n eTrueLogout fail");
			}
			FtpupdateStep = eError;
			break;
		case eFalseLogout:
			//统一清除所有有关升级的标志
			tIap_698.dwIapFlag = 0;
			tIap_645.dwIapFlag = 0;
			if(nwy_ftp_logout(g_ftp_fota_handl) !=0)//！！！
			{
				nwy_ext_echo("\r\n eFalseLogout fail");
			}
			FtpupdateStep = eError;
			bResult = eFtpFailExit;
			break;
		case eDbUpdate:
			if (BaseMeterUpgradeProtect == eDbUpdate)//保证文件下载完 执行升级
			{
				ptr = strchr(buf,ch);//查找字符‘/’后面认为是升级文件 ？？？找不到 / 防护待加
				if (ptr == NULL)
				{
					FtpupdateStep = eFalseLogout;
					return;
				}
				strcpy(Tempfilepath,ptr+1);
				if (tIap_645.dwIapFlag == DOWNLOAD_METERFILE) // 模块给电表升级
				{
					memcpy(upgrade_file_path_645,Tempfilepath,FILENAME_LEM);
					EnterUpgrade645();
				}
				else if (tIap_698.dwIapFlag == DOWNLOAD_METERFILE)
				{
					memcpy(upgrade_file_path_698, Tempfilepath, FILENAME_LEM);
					EnterUpgrade698();
				}
				FtpupdateStep = eTrueLogout;//待测试 校验值不对，重新下发 重新登陆ftp
				nwy_ext_echo("\r\n start upgrade");
			}
			break;
		case eWait://等待回调函数触发事件
			break;
		default:
			break;
	}
	if (bResult != 0xFF)
	{
		nwy_msg_queue_send(UpgradeResultMessageQueue,1,&bResult,NWY_OSA_NO_SUSPEND);
		bResult = 0xFF;
	}
}
//--------------------------------------------------
//功能描述:  初始化相关fota参数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  InitFtpUpGradePara( void )
{
	FailCount = 0;
	BaseMeterUpgradeProtect = 0;
	Ftp_Timer = 0xAA;
	FtpupdateStep = eLogin;
}
//--------------------------------------------------
//功能描述:  检查/nwy/文件夹下的文件,如有文件则删除
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void CheckFtpDir(void)
{
	char TempDirpath[FILENAME_LEM / 2] = "/nwy";
	char Tempfilepath[FILENAME_LEM / 2] = "/nwy/";
	char Tempfile[256 + (FILENAME_LEM / 2)] = {0};
	nwy_dirent_t Dirent = {0};
	BYTE i = 1;
	int ret = 0;
	nwy_dir_info_t nwy_fs_dir = {0};
	int dir_exist = 1;
    // Remove the directory and its contents
	nwy_dir_rewind(&nwy_fs_dir);
	ret = nwy_dir_open(TempDirpath, &nwy_fs_dir); // 打开文件夹
    if (ret != NWY_SUCCESS)
	{
		nwy_dir_close(&nwy_fs_dir);
		nwy_ext_echo("\r\n Failed to open dir: %s, ret:%d", TempDirpath, ret);
		dir_exist = 0;
	}
	else
	{
		ret = nwy_dir_read(&nwy_fs_dir, &Dirent); // 读取文件夹
		if (ret != NWY_SUCCESS)
		{
			nwy_dir_close(&nwy_fs_dir);
		}
		else
		{
			while (1)
			{
				if ((strcmp(Dirent.d_name, ".") != 0) && (strcmp(Dirent.d_name, "..") != 0))
				{
					strcat(Tempfile, Tempfilepath);
					strcat(Tempfile, Dirent.d_name);
					nwy_ext_echo("\r\n Dirent file%d %s", i, Tempfile);
					ret = nwy_file_remove(Tempfile);
					if (ret != NWY_SUCCESS)
					{
						nwy_ext_echo("\r\n Failed to remove file: %s, ret:%d", Tempfile, ret);
					}
					else
					{
						nwy_ext_echo("\r\n Remove file: %s success", Tempfile);
	}
					i++;
				}
				ret = nwy_dir_read(&nwy_fs_dir, &Dirent);
				if (ret != NWY_SUCCESS)
				{
					break;
				}
				memset(Tempfile, 0x00, sizeof(Tempfile));
				if (i > 10)
				{
					break;
				}
			}
			nwy_dir_close(&nwy_fs_dir);
		}
	}
    // Create the directory again
	if (!dir_exist)
	{
		ret = nwy_dir_mk(TempDirpath);
    if (ret != NWY_SUCCESS)
    {
			nwy_ext_echo("\r\n Failed to create dir: %s, ret:%d", TempDirpath, ret);
    }
	else
	{
			nwy_ext_echo("\r\n Create dir: %s success", TempDirpath);
		}
	}
}

//--------------------------------------------------
// 功能描述:
//
// 参数: 升级ftp文件信息（地址、文件名）
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void Fota_UpdateTask(void *parameter)
{
	FtpUpdatePara* pFtpupdatepara = NULL;
	FtpUpdatePara ftpupdatepara;
	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_ftp_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};

	memset(&g_fileftp, 0x00, sizeof(g_fileftp));
	memset((BYTE*)&ftpupdatepara,0,sizeof(FtpUpdatePara));
	nwy_thread_sleep(3000);
	CheckFtpDir();
   
	while (1)
	{
		//等待消息队列 成功后
		if(nwy_msg_queue_recv(FtpUpdateMessageQueue,sizeof(char*),&pFtpupdatepara,NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)//队列空则立即返回
		{	
			InitFtpUpGradePara();
			memcpy((char*)&ftpupdatepara,(char*)pFtpupdatepara,sizeof(FtpUpdatePara));
			// memcpy((char*)&ftpupdatepara,(char*)pFtpupdatepara,sizeof(FtpUpdatePara));
			// memcpy(ftpupdatepara.filename,"test/fota202509090906.pkt",sizeof(ftpupdatepara.filename));
			// memcpy(ftpupdatepara.host,"218.201.129.20",sizeof(ftpupdatepara.host));
			// memcpy(ftpupdatepara.username,"wisdom",sizeof(ftpupdatepara.username));
			// memcpy(ftpupdatepara.PassWord,"wisdom@normal",sizeof(ftpupdatepara.PassWord));
			// ftpupdatepara.ftp_param.port=7202;
			//重新赋值ftp_param的指针
			ftpupdatepara.ftp_param.host = ftpupdatepara.host;
			ftpupdatepara.ftp_param.username = ftpupdatepara.username;
			ftpupdatepara.ftp_param.password = ftpupdatepara.PassWord;
// dump_hex(&ftpupdatepara, sizeof(FtpUpdatePara));
			nwy_ext_echo("\r\n ftp_login_param. host is %s",ftpupdatepara.ftp_param.host);
			nwy_ext_echo("\r\n ftp_login_param. username is %s",ftpupdatepara.ftp_param.username);
			nwy_ext_echo("\r\n ftp_login_param. port is %d",ftpupdatepara.ftp_param.port);
			nwy_ext_echo("\r\n get queue ftp password is %s",ftpupdatepara.ftp_param.password);
			// nwy_ext_echo("\r\n get queue ftp password is %p",ftpupdatepara.ftp_param.password);
			// nwy_ext_echo("\r\n get queue ftpupdatepara password is %s",ftpupdatepara.PassWord);
			// nwy_ext_echo("\r\n get queue ftpupdatepara password is %p",ftpupdatepara.PassWord);
			free(pFtpupdatepara);
			// 通过判断文件名 来区分是全包升级和差分升级
			nwy_ext_echo("\r\n get message success");
			// 定时器计数器赋值零
			OneTimeTImerPara.expired_time = 20 * 60 * 1000;
			OneTimeTImerPara.cb_para = NULL;
			nwy_sdk_timer_start(ftp_timer, &OneTimeTImerPara); //20分钟单次定时
			while (1)
			{
				if(0 != nwy_ext_check_data_connect())//超时处理
				{
					api_VersionUpgrades(&ftpupdatepara);
				}
				nwy_thread_sleep(500);
			
				if ((Ftp_Timer == 0) || (FtpupdateStep == eError)) //
				{
					FtpupdateStep = eError;
					if (Ftp_Timer == 0)
					{
						tIap_698.dwIapFlag = 0;
						tIap_645.dwIapFlag = 0;
					}
					nwy_ext_echo("\r\n break update");
					nwy_sdk_timer_stop(ftp_timer);
					break;
				}
			} 
		} 
		nwy_thread_sleep(100);	
	}	
}
//--------------------------------------------------
//功能描述:  获取当前模块升级状态
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_GetModuleUpGradeStatus( void )
{
	if (FtpupdateStep == eError)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}