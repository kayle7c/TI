#include "stm32f10x.h"
#include "delay.h"
#include "oled.h"
#include "key.h"
#incldue "string.h"

void oled_show1(int mode);
void oled_show2(int mode);
void run1(int mode);
void mode1();
void mode2();
void mode3();
void mode4();
void mode5();

int main(void)
{
	int mode = 0;  // mode = 0���˳�
	int MODE_CONT = 5;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	OLED_Init();
	OLED_Clear();
	KEY_Init();
	
	
	while (1)
	{
		//��ⰴ��������Ϣ
		key = KEY_Scan(0);  
		switch(key)
		{
			case 1:  //����
				mode ++;
				if (mode > MODE_CONT) mode = 0;
				break;
			case 2:  //ȷ��
				if (mode != 0)
					run1(mode);
				break;
		}
		oled_show1(mode);	// oled��ʾѡ������
	}
}

// 1��ҳ����ʾ
void oled_show1(int mode)
{
	char oledbuf[30];
	sprintf(oled_buf, "select mode:%d", mode);
	OLED_ShowString(0,20,(uint8_t *)oled_buf, 12);
	OLED_Refresh();	
}

// 2��ҳ����ʾ
void oled_show2(int mode)
{
	char oledbuf[30];
	switch(mode)
	{
		case 1:
			sprintf(oled_buf, "run");
			OLED_ShowString(0,12,(uint8_t *)oled_buf, 12);
			OLED_Refresh();	
			break;
		case 0:
			sprintf(oled_buf, "exit", mode);
			OLED_ShowString(0,20,(uint8_t *)oled_buf, 12);
			OLED_Refresh();	
			break;
	}
}

void run1(int mode)
{
	char oledbuf[30];
	int select = 1;	// ���л����˳���Ĭ��������
	int state = 1;	// ��ʾ��ǰ��״̬���ĸ���ִ��ʱ��Ҫ��������ж�
	int SELECT_CNT = 1;
	
	OLED_Clear();
	
	// ��ʾ��ǰ�����
	sprintf(oled_buf, "mode:%d", mode);
	OLED_ShowString(0,0,(uint8_t *)oled_buf, 12);
	OLED_Refresh();	
	
	// ��ʼ������һ��Ĭ��״̬
	
	while (1)
	{
		//��ⰴ��������Ϣ
		key = KEY_Scan(0);  
		switch(key)
		{
			case 1:  //����
				select ++;
				if (select > SELECT_CNT) select = 0;
				break;
			case 2:  //ȷ��
				state = select;	// �޸�״̬
				switch(select)
				{
					case 1:  // ����
					{
						sprintf(oled_buf, "running", mode);
						OLED_ShowString(80,0,(uint8_t *)oled_buf, 12);
						OLED_Refresh();	
						
						switch (mode)
						{
							case 1:
								mode1();
								break;
							case 2:
								mose2();
								break;
							case 3:
								mode3();
								break;
							case 4:
								mode4();
								break;
							case 5:
								mode5();
								break;
						}
						break;
					}
					case 0:	// �˳�
						return;
				}
				break;
		}
		oled_show2(select);	// oled��ʾѡ������
	}
}

void mode1()
{
	char oledbuf[30];
	OLED_Clear();
	
	sprintf(oled_buf, "1");
	OLED_ShowString(0,20,(uint8_t *)oled_buf, 12);
	OLED_Refresh();
}

void mode2()
{
	char oledbuf[30];
	OLED_Clear();
	
	sprintf(oled_buf, "2");
	OLED_ShowString(0,20,(uint8_t *)oled_buf, 12);
	OLED_Refresh();
}

void mode3()
{
	char oledbuf[30];
	OLED_Clear();
	
	sprintf(oled_buf, "3");
	OLED_ShowString(0,20,(uint8_t *)oled_buf, 12);
	OLED_Refresh();
}

void mode4()
{
	char oledbuf[30];
	OLED_Clear();
	
	sprintf(oled_buf, "4");
	OLED_ShowString(0,20,(uint8_t *)oled_buf, 12);
	OLED_Refresh();
}

void mode5()
{
	char oledbuf[30];
	OLED_Clear();
	
	sprintf(oled_buf, "5");
	OLED_ShowString(0,20,(uint8_t *)oled_buf, 12);
	OLED_Refresh();
}

