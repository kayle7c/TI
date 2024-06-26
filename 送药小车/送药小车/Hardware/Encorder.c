#include "stm32f10x.h"
#include "usart.h"
#include  "encorder.h"

extern int Encoder_A_EXTI;


int Encode_Left=0;			//定义左轮编码器读取值变量，更改会导致错误
int Encode_Right=0;			//定义右轮编码器读取值变量，更改会导致错误
int Encode_Left_ALL=0;		//定义左轮编码器累加值
int Encode_Right_ALL=0;		//定义右轮编码器累加值
int	Encode_Speed=0;			//定义通过编码器获取的小车前进速度,大于0前进速度，小于0后退速度
int	Encode_Spin_Speed=0;	//通过编码器获取的小车旋转速度，大于右转速度，小于0左转速度


/*@brief: 编码器初始化函数
 * @param: None
 * @return: None
 */
void Encoder_Init(void)
{
	Encoder_Init_TIM3();//RIGHT电机初始化函数
	Encoder_Init_TIM8();//电机初始化函数
	//TIM1_Int_Init(50000-1,72-1);//初始化定时1，定时时间200ms，在中断中读取编码器数据
}

/*@brief: 定时器1配置成中断更新模式
 * @param:
 *        [in] arr 自动重装值,范围0-65535
		  [in] psc 预分频系数,范围0-65535
 * @return: None
 */
void TIM1_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;//重复计数器为0
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM1中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1更新中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM1, ENABLE);  //使能TIMx					 
}
void TIM1_UP_IRQHandler(void)
{	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //检查TIM1更新中断发生与否
	{
		
		Encode_Left=Read_Encoder(8);//读取左轮编码器数据
   	Encode_Right=Read_Encoder(3);//读取右轮编码器数据
		Encode_Left_ALL+=Encode_Left;
		Encode_Right_ALL+=Encode_Right;
		// printf("编码输出：%d %d\r\n",Encode_Left_ALL,Encode_Right_ALL);
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //清除TIM1更新中断标志
		TIM_SetCounter(TIM3,0);
		TIM_SetCounter(TIM8,0);
	}
}


/* @brief: 单位时间读取编码器计数
 * @param: 
 *			[in] TIMX 读取编码器计数值定位器编号，范围2-4
 * @return: 读取的编码器计数值，与定时器读取时间有关
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


/*@brief: 定时器3配置成编码器模式,读取右轮编码器
 * @param:
 *        [in] arr 自动重装值,范围0-65535
		  [in] psc 预分频系数,范围0-65535
 * @return: None
 */
void Encoder_Init_TIM3(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器3的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PA端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;

	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIM的更新标志位
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
	TIM_SetCounter(TIM3,0);
//	TIM5->CNT = 0;
	TIM_Cmd(TIM3, ENABLE); 
}


/*@brief: 定时器8配置成编码器模式,读取左轮编码器
 * @param: None
 * @return: None
 */
void Encoder_Init_TIM8(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);//使能定时器8的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能PC端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//端口配置
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;
	TIM_ICInit(TIM8, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2;
	TIM_ICInit(TIM8, &TIM_ICInitStructure);
	
	TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
	

	//Reset counter
	TIM_SetCounter(TIM8,0);
	TIM_Cmd(TIM8, ENABLE); 
}

uint32_t read_cnt(u8 TIMX) //定时器计数值读取
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
