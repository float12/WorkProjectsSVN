//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾��ѹ̨����Ʒ��
//������	lishicai
//��������	2025.01.18
//����		�����ܲ����ݵ�VI�켣����,��ʱ���������ݹ�һ������
//�޸ļ�¼��������ֵ��itc������ȡ�ar�����lpaѭ�����򡢹켣���ȡ�yd����ؾࡢ
//sh�м��б�ʣ����������׼��֮�ͣ���D˲ʱ���ɱ仯��SD���/��С����롢��ֵ��ѹ���ֵ������ŷ����þ���
//--------------------------------------------------------------------------
#include "AllHead.h"
#include <math.h> 
#include "api_VI.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define vl	0.001f	//��ѹ��ֵ

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
//--------------------------------------------------
//��������:  ƽ��ֵ����
//         
//����:		float *array   ��������
//			BYTE length      ���鳤��
//         
//����ֵ:    �����ƽ��ֵ
//         
//��ע:  
//--------------------------------------------------
float calculate_mean(float *array, BYTE length) 
{
	float sum;
	BYTE i;
	
	sum= 0.0;
	for(i = 0; i < length; i++) 
	{
		sum += array[i];
	}

	return sum / length;
}
//--------------------------------------------------
//��������:  ��׼�����
//         
//����:		float *array   ��������
//			BYTE length      ���鳤��       
//����ֵ:   ���ݱ�׼�� 
//         
//��ע:  
//--------------------------------------------------
float calculate_std_dev(float *array, BYTE length) 
{
	float mean, sum;
	BYTE i;

	if(length < 2) 
	{
		return 0.0; // ���������
	}

	mean = calculate_mean(array, length);
	sum = 0.0;
	for(i = 0; i < length; i++) 
	{
		sum += (array[i] - mean) * (array[i] - mean);
	}

	return sqrt(sum / length);
}
//--------------------------------------------------
//��������:  �ȽϺ��������򣬱Ƚ�����ValueIndex�����ֵ��С
//         
//����:	 @param a ָ���һ��ValueIndex�����ָ��
//		@param b ָ��ڶ���ValueIndex�����ָ��
//         
//����ֵ:    
//         
//��ע:	�Ƚ�����ValueIndex�����ֵ��С�������һ�������ֵС�ڵڶ��������ֵ���򷵻�-1��
//		�����һ�������ֵ���ڵڶ��������ֵ���򷵻�1��������������ֵ��ȣ��򷵻�0�� 
//--------------------------------------------------
int compare( void const *a,  void const *b) 
{
	TValueIndex *va = (TValueIndex *)a;
	TValueIndex *vb = (TValueIndex *)b;
	
	if (va->value < vb->value) 
		return -1;
	if (va->value > vb->value) 
		return 1;

	return 0;
}
//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: V_I�켣����---������ֵ��itc�������
//
//����:     float* input_V  �ܲ���ѹ����
//         float* input_I   �ܲ���������
//          Len[in]         ���ݳ���
//          float max_I��itc      ������ֵ��itc�������
//
//����ֵ: �������1��2---
//��ע��������ֵ���������----�������ݲ���Ҫ��һ������
//-----------------------------------------------
TIResult GetVI_IMAX_ITC(float* input_I,BYTE len)
{  
	float max_I,min_I,itc;	
	BYTE i;
	TIResult result;	

	if (len == 0 || input_I == NULL)
	{
		result.max_I = 0;	//������ֵ
		result.itc = 0;		//itc�������
		// ������Ч�����ش���ֵ
		return result;
	}

	max_I = input_I[0];	//��ʼ��������ֵ
	min_I = input_I[0];	//��ʼ����С����ֵ

	//�������飬�ҳ������С����ֵ
	for( i=0; i < len; i++ )
	{
		if( input_I[i] > max_I )
		{
			max_I = input_I[i];
		}
		if( input_I[i] < min_I )
		{
			min_I = input_I[i];
		}
	}	
	itc = max_I - min_I;

	result.max_I = max_I;	//������ֵ
	result.itc = itc;	//itc�������

	return result;
}
//-----------------------------------------------
//��������: V_I�켣����---ar���
//
//����:     float* input_V  �ܲ���ѹ����
//         float* input_I   �ܲ���������
//          Len[in]         ���ݳ���
// 
//
//����ֵ: �������3��4
//��ע��ar���--��VI�켣Χ�ɵĽ������������lpaѭ������
//-----------------------------------------------
TVIarlpa GetVI_AR_LPA(float* input_V,float* input_I,BYTE len)
{
	float ar,lpa,sum1,sum2,temp; 	// ar���
	BYTE i,j;
	TVIarlpa result;	

	sum1 = 0.0f;	
	sum2 = 0.0f;	

	if (len == 0 || input_V == NULL || input_I == NULL)
	{
		result.ar = 0.0f;
		result.lpa = 3.0f;		//ѭ������
		// ������Ч�����ش���ֵ
		return result;
	}

	// ʹ��Ь��(Shoelace formula)��ʽ�������
	for (i = 0; i < len; i++)
	{
		j = (i + 1) % len;
		//�����ĵ�ѹ*��һ����ĵ���-��һ����ĵ�ѹ*�����ĵ���;
		//ar���
		sum1 += fabs(input_V[i] * input_I[j] - input_V[j] * input_I[i]);
		//lpaѭ������
		sum2 += input_V[i] * input_I[j] - input_V[j] * input_I[i];
	}

	ar = (0.5f) * fabs(sum1);
	temp  = (0.5f) * (sum2);

	if( temp > 0 )
	{
		lpa = 1.0f;		//˳ʱ��,������ǰ��ѹ
	}
	else if(temp < 0)
	{
		lpa = -1.0f;	//��ʱ��,�����ͺ��ѹ
	}
	else
	{
		lpa = 0.0f;		//�������ѹͬ��λ
	}

	result.ar = ar;		//���
	result.lpa = lpa;	//ѭ������

	return result;
}
//-----------------------------------------------
//��������: V_I�켣����---Loop_Length�켣����
//
//����:     float* input_V  �ܲ���ѹ����
//         float* input_I   �ܲ���������
//          Len[in]         ���ݳ���
//          float loop_length     �켣����
//
//����ֵ: �������5
//��ע:Loop_Length�켣����
//-----------------------------------------------
float GetVI_Loop_Length(float* input_V,float* input_I,BYTE len)
{
	float loop_length,dx,dy,distance;	// �켣����
	BYTE i,j;	

	loop_length = 0.0;
	for (i = 0; i < len - 1; i++) 
	{
		dx = input_V[i + 1] - input_V[i];
		dy = input_I[i + 1] - input_I[i];
		distance = sqrt(dx * dx + dy * dy);
		loop_length += distance;
	}
	// Add distance from last to first point
	dx = input_V[0] - input_V[len - 1];
	dy = input_I[0] - input_I[len - 1];
	distance = sqrt(dx * dx + dy * dy);

	loop_length += distance;
	
	return loop_length;
}
//-----------------------------------------------
//��������: V_I�켣����---sh����������ݱ�׼��
//
//����:     float* input_V  �ܲ���ѹ����
//         float* input_I   �ܲ���������
//          Len[in]         ���ݳ���
//          float sh      	�������ݱ�׼��
//
//����ֵ: �������6
//��ע:sh = std(inputI) ---����������ݱ�׼��
//-----------------------------------------------
float GetVI_SH(float* input_V,float* input_I,BYTE len)
{  
	float sh;

	sh = calculate_std_dev(input_I, len);	// ��������������ݵı�׼��

	return sh;
}
//-----------------------------------------------
//��������: V_I�켣����---yd����ؾ�
//
//����:     float* input_V  �ܲ���ѹ����
//         float* input_I   �ܲ���������
//          Len[in]         ���ݳ���
//          float yd      yd=A�ߡ�B������������������֮��ľ���
//
//����ֵ: �������7
//��ע:Euclidean distance between peak voltage and peak current.
//-----------------------------------------------
float GetVI_YD(float* input_V,float* input_I,BYTE len)
{  
	float yd,value1,value2;	
	BYTE i, middle1, middle2,originalIndex1,originalIndex2;		


	if (len % 2 != 0) 
	{
		return	0;
	}

	//�洢ֵ������
	TValueIndex vi[SAMPLE_NUM];
	for ( i = 0; i < len; i++ ) 
	{
		vi[i].value = input_V[i];
		vi[i].index = i;
	}

	qsort(vi, len, sizeof(TValueIndex), compare);

	middle1 = (len / 2) - 1;
	middle2 = len / 2;

	//�ҵ����������м���������ԭʼ����λ��
	originalIndex1 = vi[middle1].index;
	originalIndex2 = vi[middle2].index;

	//�ҵ����������Ӧ�ĵ���ֵ
	value1 = input_I[originalIndex1];
	value2 = input_I[originalIndex2];
	//����ydֵ
	yd = fabs(value1 - value2);

	return yd;
}
//-----------------------------------------------
//��������: V_I�켣����----D˲ʱ���ɱ仯
//
//����:     float* input_V  �ܲ���ѹ����
//         float* input_I   �ܲ���������
//          Len[in]         ���ݳ���
//          float D        �������
//
//����ֵ: �������8
//-----------------------------------------------
float GetVI_D(float* input_V,float* input_I,BYTE len)
{    

	float sum_s;  				// ���ɵ��ܺ�
	float sum_s2; 				// ����ƽ�����ܺ�
	BYTE valid_count; 		// ��Ч���ݵ������
	BYTE i;
	float s_i;
	float mean_s; 				// ���ɵ�ƽ��ֵ
	float variance;				// ���ɵķ���
	float D;	// ���ɵı�׼��


	if (len == 0 || input_V == NULL || input_I == NULL)
	{
		// ������Ч�����ش���ֵ
		return -1.0f;
	}

	// ��ʼ����ͱ���
	sum_s = 0.0f; 
	sum_s2 = 0.0f; 
	valid_count = 0;

	// �������ݣ�����˲ʱ����
	for (i = 0; i < len; i++)
	{
		// ����ѹ�Ƿ������ֵ
		if (fabs(input_V[i]) >= vl)
		{
			// ����˲ʱ����
			s_i = input_I[i] / input_V[i];
			// �ۼӵ��ɺ�,����ƽ��
			sum_s += s_i;
			sum_s2 += s_i * s_i;
			valid_count++;
		}
	}
	// ���û����Ч���ݵ㣬���ش���ֵ
	if (valid_count == 0)
	{
		return -1.0f;
	}

	// ���㵼�ɵ�ƽ��ֵ�ͱ�׼��
	mean_s = sum_s / valid_count;
	//����
	variance = (sum_s2 / valid_count) - (mean_s * mean_s);
	//���ɵı�׼��
	D = sqrt(variance);

	return D;

}
//-----------------------------------------------
//��������: V_I�켣����----SD���/��С����롢PED
//����:     float* input_V  �ܲ���ѹ����
//         float* input_I   �ܲ���������
//          Len[in]         ���ݳ���
//          float vi_SD      ������/��С����롢��ֵ��ѹ���ֵ�������ڵ��ŷ����þ���
//
//����ֵ: �������9��10
//-----------------------------------------------
TVISdPed GetVI_SD_PED(float* input_V,float* input_I,BYTE len)
{
	float maxV,minV,maxI,minI,dx,dy;
	float x1,x2,y1,y2;
	float vi_SD,ped,temp_V,temp_I;
	BYTE i;
	TVISdPed result;

	
	if (len == 0 || input_V == NULL || input_I == NULL) 
	{
		result.vi_SD = -0.1f;
		result.ped = -0.1f;
		// ������Ч����
		return result;
	}

	// ��ʼ�����ֵ����Сֵ
	maxV = input_V[0];  // input_V �����ֵ
	minV = input_V[0];  // input_V ����Сֵ
	maxI = input_I[0];  // input_I �����ֵ
	minI = input_I[0];  // input_I ����Сֵ

	// �������飬�ҵ�ÿ����������ֵ����Сֵ
	for ( i = 1; i < len; i++ ) 
	{
		// ���� input_V
		if (input_V[i] > maxV) 
		{
			maxV = input_V[i];
			x1 = maxV;			//��¼��ֵ��ѹ���ĺ�����
			y1 = input_I[i];	//��¼��ֵ��ѹ����������

		}
		if (input_V[i] < minV) 
		{
			minV = input_V[i];	
		}

		// ���� input_I
		if (input_I[i] > maxI) 
		{
			maxI = input_I[i];
			x2 = input_V[i];	//��¼��ֵ�������ĺ�����
			y2 = maxI;			//��¼��ֵ��������������
		}
		if (input_I[i] < minI) 
		{
			minI = input_I[i];
		}
	}

	temp_V = (maxV - minV)*(maxV - minV);
	temp_I = (maxI - minI)*(maxI - minI);
	vi_SD = sqrt(temp_V + temp_I);

	dx = fabs(x1 - x2);
	dy = fabs(y1 - y2);
	ped = sqrt(dx*dx + dy*dy);

	result.vi_SD = vi_SD;
	result.ped = ped;

	return result;
}
