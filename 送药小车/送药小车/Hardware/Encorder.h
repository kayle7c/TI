#ifndef __ENCORDER_H_
#define __ENCORDER_H_

#include "sys.h"

#define ENCODER_TIM_PERIOD (u16)(65535)   //���ɴ���65535 ��ΪF103�Ķ�ʱ����16λ�ġ�


/***********************��������������**************************/

#define ACircleLeftEncode  585	//һȦ2350��2500ʵ��
#define ACircleRightEncode 585//һȦ
#define ACircleEncoder ACircleLeftEncode 


/*************************�ⲿ��������**************************/

extern int Encode_Left;			//�������ֱ�������ȡֵ���������Ļᵼ�´���
extern int Encode_Right;		//�������ֱ�������ȡֵ���������Ļᵼ�´���
extern int Encode_Left_ALL;		//�������ֱ������ۼ�ֵ
extern int Encode_Right_ALL;	//�������ֱ������ۼ�ֵ
extern int	Encode_Speed;		    //����ͨ����������ȡ��С��ǰ���ٶ�
extern int	Encode_Spin_Speed;	//ͨ����������ȡ��С����ת�ٶ�

/*************************�ⲿ��������**************************/
extern void Encoder_Init(void);		//��������ʼ������
extern int Read_Encoder(u8 TIMX);   //����������ֵ
void Encoder_Init_TIM3(void);
void Encoder_Init_TIM8(void);
void TIM1_Int_Init(u16 arr,u16 psc);
uint32_t read_cnt(u8 TIMX);

#endif





