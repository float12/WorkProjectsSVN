#ifndef TASK_SAMPLE_H
#define TASK_SAMPLE_H

// 主循环任务的采样点缓冲区大小
#define SAMP_BUFF_MAX_SIZE 		(CHMAX*10*sizeof(INT)*SAMPLE_NUM)          	//=(phase个通道*10周波*sizeof(int)*256采样率)
#define ONE_FRAME_LEN_R 		(CHMAX * sizeof(INT))						//每次读一帧
#define ONE_FRAME_LEN_W 		(SAMPLE_NUM * ONE_FRAME_LEN_R)				//每次写一个周波 24位计量模组
#define READ_LENGTH 			(CHMAX * sizeof(INT) * SAMPLE_NUM) 			//读取长度
#define ONE_FRAME_LEN 			(1+1+2+1+SAMPLE_NUM*CHMAX*3+4+1)  			// 每帧报文总长度，例: 1+1+2+1+256*6*3+4+1=4618
#pragma pack (4)
typedef struct
{
    // DWORD SPIRecBuf_wr_pos;
    // DWORD SPIRecBuf_rd_pos;
    // WORD SPIRecBuf_pass_tail;
    WORD len;
    WORD frame_num;
    WORD old_frame_num;
} SPIFRAMEINFO;
#pragma pack ()

#pragma pack (4)
typedef struct
{
    DWORD BufCoverError;
    DWORD HeadError;
    DWORD ModeError;
    DWORD LengthError;
    DWORD FrameNumError;
    DWORD CheckError;
    DWORD EndError;
    DWORD Success;
} SPICOUNT;
#pragma pack ()

void Init_Sample(void);
void DoSPIReceProc(BYTE *pBuf);
BOOL api_GetSmpPoints(void** new_data_ptr);
DWORD get_R_pos_Time(void);

extern int m_SampleNum;

#endif  // #ifndef TASK_SAMPLE_H
