//----------------------------------------------------------------------
// Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾��ѹ̨����Ʒ��
// ������	��Ȫ
// ��������
// ����
// �޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
   	eLogin = 1,         //��¼
   	eGetSize,    		//��ȡ�ļ�����
   	eDownload,        	//�����ļ�
   	eAllUpdate,      	//ȫ������
   	eWait,    			//�ȴ��ص�����
	eError,				//ִ��ʧ��						�˳���¼��erro ʱ��ͳһ ��������й������ı�־
	eTrueLogout,		//�����˳�
	eFalseLogout,		//�쳣�˳�
	eDbUpdate,			//�������
}eFtpUpdateStep;		//ftp�������̱�־
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static nwy_file_ftp_info_s g_fileftp;
char FtpupdateStep = eError;  
char UpdateMode = 1;//���������ȫ�������ж�
char FailCount = 0;
WORD Ftp_Timer;//ftp����������ʱ��
BYTE BaseMeterUpgradeProtect = 0;//ftp���ػ��������ļ�����
BYTE bResult = 0xFF;	//ftp�����ϱ��������
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
//-----------------------------------------------
//				��������
//-----------------------------------------------
void nwy_ftp_timer_cb(void *type)
{
	Ftp_Timer = 0;
}
//--------------------------------------------------
//��������:  app�������� ȫ������
//         
//����:      �ļ���
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  update_app( char* filename )
{
	int fd;
	ota_package_t ota_pack = {0};
	int ota_size = 0;
	int tmp_len = 0;
	int read_len = 0;
	char buff[NWY_APPIMG_FOTA_BLOCK_SIZE] = {0};
	char Tempfilepath[FILENAME_LEM] = "/nwy/";
	int ret = -1;

	strcat(Tempfilepath,filename);
	if(UpdateMode == 0)
	{
		nwy_sdk_file_unlink("/fota/fota.pack");//������� Ҫ����ɾ�����ļ� �����·��ļ��� ȥ�ж��ǲ�ֻ���ȫ����
	}
	else
	{
		
	}
	fd = nwy_sdk_fopen(Tempfilepath, NWY_RDONLY);

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
		//����ʧ�ܣ�
		return;
	}
	ota_size = nwy_sdk_fsize_fd(fd);
	nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
	
	while(ota_size > 0)
	{
		tmp_len = NWY_APPIMG_FOTA_BLOCK_SIZE;
		if(ota_size < tmp_len)
		{
			tmp_len = ota_size;
		}
	
		memset(buff,0,sizeof(buff));
		read_len = nwy_sdk_fread(fd, buff, tmp_len);
		if(read_len <= 0)
		{
			nwy_ext_echo("read file error:%d\r\n", read_len);
			free(ota_pack.data);
			nwy_sdk_fclose(fd);
			return;
		}
		memcpy(ota_pack.data, buff, read_len);
		ota_pack.len = read_len;
		if(UpdateMode == 0)
		{
			ret = nwy_fota_download_core(&ota_pack);
		}
		else
		{
			ret = nwy_fota_dm(&ota_pack);
		}
		if((ret < 0))//������� �� ret������0 ������
		{
			nwy_ext_echo("write ota error:%d\r\n", ret);
			free(ota_pack.data);
			nwy_sdk_fclose(fd);
			return;
		}
		ota_pack.offset += read_len;
		memset(ota_pack.data, 0, read_len);
		ota_size -= read_len;
	}
	free(ota_pack.data);
	nwy_sdk_fclose(fd);
	if(UpdateMode == 0)
	{
		ret = nwy_sdk_file_unlink(Tempfilepath);
		if(nwy_version_update(true))
		{
			nwy_ext_echo("\r\n nwy_version_update : updata fail");
		}
		else
		{ 
			nwy_ext_echo("\r\n nwy_version_update : updata suceess!"); 
		}
		return;
	}
	else
	{
		//�˶�
		ret = nwy_package_checksum();
		if(ret < 0)
		{
			nwy_ext_echo("checksum failed\r\n");
			ret = nwy_sdk_file_unlink(Tempfilepath);
			bResult = eFtpModuleUpgradeVerifyFail;
			return;
		}
		ret = nwy_sdk_file_unlink(Tempfilepath);
		//����
		ret = nwy_fota_ua();//�ɹ�
		if(ret < 0)
		{
			nwy_ext_echo("update failed\r\n");
			return;
		}
	}		
}
int nwy_test_ftp_write_file(nwy_file_ftp_info_s *pFileFtp, unsigned char *data, unsigned int len)
{
	char file_name[128 + 1] = "/nwy/";
	
	strcat(file_name, pFileFtp->filename);
	int fs = nwy_sdk_fopen(file_name, NWY_CREAT | NWY_RDWR);
	if(fs < 0)
	{
		OSI_LOGE(0, "nwy file open fail");
		return -1;
	}

	nwy_sdk_fseek(fs, pFileFtp->pos, 0);
	nwy_sdk_fwrite(fs, data, len);
	nwy_sdk_fclose(fs);
	pFileFtp->pos += len;
	OSI_LOGE(0, "nwy file write %d size success.", len);

	return 0;
}
//--------------------------------------------------
//��������:  ftp_result
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void nwy_ftp_result_cb(nwy_ftp_result_t *param)
{
	int *size;
	int ret;

	if(NULL == param)
	{
		nwy_ext_echo("event is NULL\r\n");
		FtpupdateStep = eFalseLogout;
	}

	OSI_LOGI(0, "event is %d", param->event);
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
        if ((tIap_645.dwIapFlag == DOWNLOAD_METERFILE) || (tIap_698.dwIapFlag == DOWNLOAD_METERFILE)) // ģ����������
		{
		}
		else
		{
			FtpupdateStep = eLogin;
			//����ͨ���رգ������µ�½һ��
		}
	}
	else if(NWY_FTP_EVENT_SIZE_ZERO == param->event)
	{
		nwy_ext_echo("\r\nFtp size is zero");
		FtpupdateStep = eFalseLogout;
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
		//ĳ���������ԣ����µ�¼���ϣ��������µ�½������ȴ�
	}
	else if(NWY_FTP_EVENT_DATA_GET == param->event)//  ��������ϱ�����
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
				ret = nwy_test_ftp_write_file(&g_fileftp, (unsigned char *)param->data, param->data_len);
				if(0 != ret)
				{
					nwy_ext_echo("\r\nwrite failed");
					FtpupdateStep = eFalseLogout;
				}
				else
				{
					//nwy_ext_echo("\r\n write success");
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
		g_fileftp.is_vaild = 0;
		nwy_ext_echo("\r\nfile %s download success.", g_fileftp.filename);
        if ((tIap_645.dwIapFlag == DOWNLOAD_METERFILE) || (tIap_698.dwIapFlag == DOWNLOAD_METERFILE)) // ģ����������
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
	else if(NWY_FTP_EVENT_DATA_OPEN_FAIL == param->event)
	{
		nwy_ext_echo("\r\nfile data open faile.");
		FtpupdateStep = eFalseLogout;
	}
	else if(NWY_FTP_EVENT_DATA_SETUP_ERR == param->event)
	{
		nwy_ext_echo("\r\nFtp data setup error");
		FtpupdateStep = eFalseLogout;
	}
	else
	{
		nwy_ext_echo("\r\n data_size is %d", param->data_len);
		//�������ػ����
	}
	return;
}
//--------------------------------------------------
//��������:  ����ftp������ ������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
static void  api_VersionUpgrades(FtpUpdatePara* ftppara)
{
	nwy_result_type result;
	nwy_ftp_login_t	ftp_login_param;
	char buf[FILENAME_LEM] = {0};
	nwy_osiEvent_t event;
	char ch = '/';
	char *ptr = NULL;
	char Tempfilepath[FILENAME_LEM] = "/nwy/";
	
	memset(&ftp_login_param, 0x00, sizeof(ftp_login_param));
	memset(buf,0,sizeof(buf));
	memset(&event, 0, sizeof(event));
	memcpy( (char*)&ftp_login_param, (char*)&ftppara->ftp_param, sizeof(ftp_login_param));
	ftp_login_param.channel = 1;
	ftp_login_param.timeout = 180;
	strcpy(buf,ftppara->filename);

	nwy_wait_thead_event(ftp_thread, &event, 200);

	switch (FtpupdateStep)
	{
		case eLogin:
			result = nwy_ftp_login(&ftp_login_param, nwy_ftp_result_cb);
			nwy_ext_echo("\r\nftp_login_param. mode is %d",ftp_login_param.mode);
			if (result != 0)
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
			if(nwy_ftp_filesize(buf, 30) != 0)
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
			memset(g_fileftp.filename,0,sizeof(g_fileftp.filename));
			ptr = strchr(buf,ch);//�����ַ���/��������Ϊ�������ļ�����֧���·������һ���ļ��� qingdaofile/app.bin
			if (ptr == NULL)
			{
				FtpupdateStep = eFalseLogout;
				nwy_ext_echo("\r\n can not find file ");
				return;
			}
			strcpy(g_fileftp.filename, ptr+1);
			if(nwy_ftp_get(buf,2,0,0) != 0)//�̶����������أ�
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
			nwy_put_msg_que(UpgradeResultMessageQueue, &bResult, 0xffffffff);	
			ptr = strchr(buf,ch);//�����ַ���/��������Ϊ�������ļ�
			if (ptr == NULL)
			{
				FtpupdateStep = eFalseLogout;
				return;
			}
			//�Լ���У�����ļ����ȶ�
			update_app(ptr+1);
			//���� ���� ����ģ��ʧ�ܣ����ص��κ��ļ�������ɾ����
			strcat(Tempfilepath,ptr+1);
			nwy_sdk_file_unlink(Tempfilepath);
			FtpupdateStep = eFalseLogout;
			bResult = eFtpModuleUpgradeFail;
			break;
		case eTrueLogout:
			if(nwy_ftp_logout(2000) !=0)
			{
				nwy_ext_echo("\r\n eTrueLogout fail");
			}
			FtpupdateStep = eError;
			break;
		case eFalseLogout:
			//ͳһ��������й������ı�־
			tIap_698.dwIapFlag = 0;
			tIap_645.dwIapFlag = 0;
			if(nwy_ftp_logout(2000) !=0)
			{
				nwy_ext_echo("\r\n eFalseLogout fail");
			}
			FtpupdateStep = eError;
			bResult = eFtpFailExit;
			break;
		case eDbUpdate:
			if (BaseMeterUpgradeProtect == eDbUpdate)//��֤�ļ������� ִ������
			{
				ptr = strchr(buf,ch);//�����ַ���/��������Ϊ�������ļ� �������Ҳ��� / ��������
				if (ptr == NULL)
				{
					FtpupdateStep = eFalseLogout;
					return;
				}
				strcat(Tempfilepath,ptr+1);
				if (tIap_645.dwIapFlag == DOWNLOAD_METERFILE) // ģ����������
				{
					memcpy(upgrade_file_path_645,Tempfilepath,FILENAME_LEM);
					EnterUpgrade645();
				}
				else if (tIap_698.dwIapFlag == DOWNLOAD_METERFILE)
				{
					memcpy(upgrade_file_path_698, Tempfilepath, FILENAME_LEM);
					EnterUpgrade698();
				}
				FtpupdateStep = eTrueLogout;//������ У��ֵ���ԣ������·� ���µ�½ftp
				nwy_ext_echo("\r\n start upgrade");
			}
			break;
		case eWait://�ȴ��ص����������¼�
			break;
		default:
			break;
	}
	if (bResult != 0xFF)
	{
		nwy_put_msg_que(UpgradeResultMessageQueue, &bResult, 0xffffffff);
		bResult = 0xFF;
	}
}
//--------------------------------------------------
//��������:  ��ʼ�����fota����
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  InitFtpUpGradePara( void )
{
	FailCount = 0;
	BaseMeterUpgradeProtect = 0;
	Ftp_Timer = 0xAA;
	FtpupdateStep = eLogin;
}
//--------------------------------------------------
//��������:  ���/nwy/�ļ����µ��ļ�,�����ļ���ɾ��
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void CheckFtpDir(void )
{
	char TempDirpath[FILENAME_LEM/2] = "/nwy";
	char Tempfilepath[FILENAME_LEM/2] = "/nwy/";
	char Tempfile[256+(FILENAME_LEM/2)]={0};
	nwy_dir *Dir;
	nwy_dirent *Dirent;
	BYTE i=1;
	
	Dir = nwy_sdk_vfs_opendir(TempDirpath);//���ļ���
	Dirent = nwy_sdk_vfs_readdir(Dir);//��ȡ�ļ��У������ļ���ɾ��
	while (Dirent != NULL)
	{
		strcat(Tempfile,Tempfilepath);
		strcat(Tempfile,Dirent->d_name);
		nwy_ext_echo("\r\n Dirent file%d %s",i,Tempfile);
		nwy_sdk_file_unlink(Tempfile);
		i++;
		Dirent = nwy_sdk_vfs_readdir(Dir);
		memset(Tempfile,0x00,sizeof(Tempfile));
		if (i > 10)
		{
			break;
		}
	}
	nwy_sdk_vfs_closedir(Dir);
}
//--------------------------------------------------
// ��������:
//
// ����: ����ftp�ļ���Ϣ����ַ���ļ�����
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void Fota_UpdateTask(void *parameter)
{
	FtpUpdatePara ftpupdatepara;

	memset(&g_fileftp, 0x00, sizeof(g_fileftp));
	memset((BYTE*)&ftpupdatepara,0,sizeof(FtpUpdatePara));
	nwy_sleep(5000);
	CheckFtpDir();

	while (1)
	{
		//�ȴ���Ϣ���� �ɹ���
		if(nwy_get_msg_que(FtpUpdateMessageQueue,&ftpupdatepara,0xffffffff))//���п�����������
		{	
			InitFtpUpGradePara();
			//ͨ���ж��ļ��� ��������ȫ�������Ͳ������
			nwy_ext_echo("\r\n get message success");
			//��ʱ����������ֵ��
			nwy_start_timer(ftp_timer,1200000);//20���ӵ��ζ�ʱ
			while (1)
			{
				if(0 != nwy_ext_check_data_connect())//��ʱ����
				{
					api_VersionUpgrades(&ftpupdatepara);
				}
				nwy_sleep(1000);
			
				if((Ftp_Timer == 0)
				||(FtpupdateStep == eError))//
				{
					FtpupdateStep = eError;
					if (Ftp_Timer == 0)
					{
						tIap_698.dwIapFlag = 0;
						tIap_645.dwIapFlag = 0;
					}
					nwy_ext_echo("\r\n break update");
					nwy_stop_timer(ftp_timer);
					break;
				}
			} 
		} 
		nwy_sleep(1000);	
	}	
}
//--------------------------------------------------
//��������:  ��ȡ��ǰģ������״̬
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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