#include "adc.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"

extern int adc1,adc2,adc3,adc4;

void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	ADC_DeInit(ADC1);
	
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_InitStructure.ADC_NbrOfChannel=1;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_Cmd(ADC1,ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
		

}

u16 Get_Adc(u8 ch)
{
	ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_55Cycles5);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
		u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
//		delay_ms(5);
	}
	return temp_val/times;
}


void get_grayscale(void)
{
//		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==1) adc1 = 1;
//			else adc1 = 0;
//	
//	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)==1) adc2 = 1;
//			else adc2 = 0;
//	
//	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)==1) adc3 = 1;
//			else adc3 = 0;
//	
//	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)==1) adc4 = 1;
//			else adc4 = 0;

		adc1=Get_Adc_Average(ADC_Channel_10,10);
		adc2=Get_Adc_Average(ADC_Channel_11,10);
		adc3=Get_Adc_Average(ADC_Channel_12,10);
		adc4=Get_Adc_Average(ADC_Channel_13,10);
}


void show_adc(void)
{
	LCD_ShowIntNum(1,1,adc1,4,RED,WHITE,16);
	LCD_ShowIntNum(1,17,adc2,4,RED,WHITE,16);
	LCD_ShowIntNum(1,34,adc3,4,RED,WHITE,16);
	LCD_ShowIntNum(1,51,adc4,4,RED,WHITE,16);
	LED=~LED;
}
