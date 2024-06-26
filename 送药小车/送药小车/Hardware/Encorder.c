#include "stm32f10x.h"
#include "usart.h"
#include  "encorder.h"

extern int Encoder_A_EXTI;


int Encode_Left=0;			//�������ֱ�������ȡֵ���������Ļᵼ�´���
int Encode_Right=0;			//�������ֱ�������ȡֵ���������Ļᵼ�´���
int Encode_Left_ALL=0;		//�������ֱ������ۼ�ֵ
int Encode_Right_ALL=0;		//�������ֱ������ۼ�ֵ
int	Encode_Speed=0;			//����ͨ����������ȡ��С��ǰ���ٶ�,����0ǰ���ٶȣ�С��0�����ٶ�
int	Encode_Spin_Speed=0;	//ͨ����������ȡ��С����ת�ٶȣ�������ת�ٶȣ�С��0��ת�ٶ�


/*@brief: ��������ʼ������
 * @param: None
 * @return: None
 */
void Encoder_Init(void)
{
	Encoder_Init_TIM3();//RIGHT�����ʼ������
	Encoder_Init_TIM8();//�����ʼ������
	//TIM1_Int_Init(50000-1,72-1);//��ʼ����ʱ1����ʱʱ��200ms�����ж��ж�ȡ����������
}

/*@brief: ��ʱ��1���ó��жϸ���ģʽ
 * @param:
 *        [in] arr �Զ���װֵ,��Χ0-65535
		  [in] psc Ԥ��Ƶϵ��,��Χ0-65535
 * @return: None
 */
void TIM1_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;//�ظ�������Ϊ0
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM1�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx					 
}
void TIM1_UP_IRQHandler(void)
{	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //���TIM1�����жϷ������
	{
		
		Encode_Left=Read_Encoder(8);//��ȡ���ֱ���������
   	Encode_Right=Read_Encoder(3);//��ȡ���ֱ���������
		Encode_Left_ALL+=Encode_Left;
		Encode_Right_ALL+=Encode_Right;
		// printf("���������%d %d\r\n",Encode_Left_ALL,Encode_Right_ALL);
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //���TIM1�����жϱ�־
		TIM_SetCounter(TIM3,0);
		TIM_SetCounter(TIM8,0);
	}
}


/* @brief: ��λʱ���ȡ����������
 * @param: 
 *			[in] TIMX ��ȡ����������ֵ��λ����ţ���Χ2-4
 * @return: ��ȡ�ı���������ֵ���붨ʱ����ȡʱ���й�
 */
int Read_Encoder(u8 TIMX)
{
	int Encoder_TIM=0;   
	switch(TIMX)
	{
		case 3:  Encoder_TIM= (u16)TIM3 -> CNT ;TIM3 -> CNT=0; break;
	
		case 8:  Encoder_TIM= (u16)TIM8 -> CNT;  TIM8 -> CNT=0;break;	

		default:  Encoder_TIM=0;
	}
	
	return Encoder_TIM;
}


/*@brief: ��ʱ��3���óɱ�����ģʽ,��ȡ���ֱ�����
 * @param:
 *        [in] arr �Զ���װֵ,��Χ0-65535
		  [in] psc Ԥ��Ƶϵ��,��Χ0-65535
 * @return: None
 */
void Encoder_Init_TIM3(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ�ܶ�ʱ��3��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PA�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOA

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���  
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;

	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//���TIM�ĸ��±�־λ
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
	TIM_SetCounter(TIM3,0);
//	TIM5->CNT = 0;
	TIM_Cmd(TIM3, ENABLE); 
}


/*@brief: ��ʱ��8���óɱ�����ģʽ,��ȡ���ֱ�����
 * @param: None
 * @return: None
 */
void Encoder_Init_TIM8(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);//ʹ�ܶ�ʱ��8��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��PC�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//�˿�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //����
	GPIO_Init(GPIOC, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOB

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���  
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;
	TIM_ICInit(TIM8, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2;
	TIM_ICInit(TIM8, &TIM_ICInitStructure);
	
	TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
	

	//Reset counter
	TIM_SetCounter(TIM8,0);
	TIM_Cmd(TIM8, ENABLE); 
}

uint32_t read_cnt(u8 TIMX) //��ʱ������ֵ��ȡ
{
	uint32_t encoder_cnt;;
	switch (TIMX)
	{
		case 3:
			encoder_cnt = TIM3->CNT; 
			TIM_SetCounter(TIM3, 0); 
		case 8:
			encoder_cnt = TIM8->CNT; 
			TIM_SetCounter(TIM8, 0); 
	}
	
	return encoder_cnt; 
}
