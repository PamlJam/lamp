#include <stc15f2k60s2.h>
#include <intrins.h>			// ����nop
#include <delay.h>				// �����ʱ
#include <lcd1602.h>			// Һ����ʾ��
#include <bh1750.h>				// �⴫����
#include <hc595.h>				// �����
#include <key.h>				// �����������ⲿ�жϡ�ɨ��

unsigned int lux;			// ��ǰ����ǿ�ȣ���λ�տ�˹��
unsigned int Status = 0;	// �ݴ� P4 ״̬��io ��ʱ����
unsigned int ratio = 1; 	// PWM ռ�ձ�

long int time = 2021400;	// ��ʼ��ʱ�䣨23 �� 9�� 30 �� 0 �룩		
// ��ǰ�ۼ�ʱ�䣨��λ�룩
// �����ͣ�-2147483648~ 2147483647��
// ȫ�ֱ���������ģʽ�л�ʱ����ʱ��

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;

unsigned char d=1;	// ����ǰ��1������-1����ͣ0��
unsigned char s=1;	// ������1��60��3600��

void AdjustLamp()
{
	if (lux > 4000)return;
	Seg_Single_Display(0,0x80);
	if (lux > 2200)
	{
		ratio = 1;
	}
	else if (lux > 1200)
	{
		ratio = 3;
	}
	else if (lux > 500)
	{
		ratio = 4;
	}
	else if (lux > 300)
	{
		ratio = 6;
	}
	else if (lux > 150)
	{
		ratio = 8;
	}
	else
	{
		ratio = 10;
	}
	Delay_us(500 * ratio);
}

void Timer0_Init(void)	// ÿ�� 20ms �ж� 1 ��
{
	AUXR &= 0x7F;		// ��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		// ���ö�ʱ��ģʽ
	TL0 = 0x00;			// ���ó�ֵ
	TH0 = 0xB8;
	TF0 = 0;			//���TF0���
	ET0=1;			  	//ʹ��T0�ж�
	EA=1;			  	//ʹ�����ж�
	TR0=1;	  	  		//����T0    
}


// �жϷ������ interrupt service routine
void Timer0_ISR(void) interrupt 1
{
	static unsigned char cnt=0;		// ��̬�������ͣ�����������
	if(flag == 1)		// ģʽ1����ǿ��⡢ģ�����
	{
		HC595_OE = 0;	// ��	595
		Status = P4; 	// ����״̬
		AdjustLamp();
		P4 = Status;   	// ȡ��״̬
		HC595_OE = 1;	// �ر� 595

		 
	}

	// ʱ�����������ţ����۵�ǰ��ʲôģʽ
	if(cnt++ == 50)		//��ʱ 20ms*50 = 1s
	{
		cnt=0;
		time = time + d*s;
	}
}


void conversion(unsigned int t)  //  ��ȡ��λ���ݣ���ʮ��ǧ��
{ 
	// 1+0x30 = 0x31 = 49���� ASCII ���ж�Ӧ���ַ� '1'
    ARR[4]=t/10000+0x30;
    t=t%10000;ARR[3]=t/1000+0x30;
    t=t%1000;ARR[2]=t/100+0x30;
    t=t%100;ARR[1]=t/10+0x30;
    t=t%10;ARR[0]=t+0x30;
}

void processTime()// ����ʱ�����ݣ�����100000�� = 1��3Сʱ46��40��
{
	second = time%60;		// ����
	minute = time/60%60;	// �÷�
	hour = time/3600%24;	// ��ʱ
	day = time/3600/24%30;	// ����
}



void main()
{  
	while(1)	// ��ѭ��
	{
		unsigned char i;
		unsigned int num;
		unsigned char str[] = "light:";
	
		Delay_ms(100);
		Init_BH1750();
		LCD_Init();
		HC595_Init();
		Timer0_Init();				// ��ʼ����ʱ��
		ExterInt0_Init(); 			// ��ʼ���ⲿ�ж�

		while(flag == 1)			// ģʽ1����ǿ��⡢ģ�����
		{
			Single_Write_BH1750(0x01);   		// �ϵ����� power on
			Single_Write_BH1750(0x10);  		// �����߷ֱ���� H- resolution mode������ʱ��һ��Ϊ120ms	
			  
			Delay_ms(180);						// �ȴ������¼��������ʵ�����ʱ180ms�������˷�ʱ��
			
			Multiple_Read_BH1750();       		// �����������ݣ��洢�� BUF ��
	
			num=BUF[0];
			num=(num<<8) + BUF[1];				// ƴ������
			
			lux = (float)num/1.2; 				// ���չ�ʽ����
			conversion(lux);         			// �������ݺ���ʾ
			
			for(i=0;i<=5;i++)		   			// ��ʾ�ı�
			{
				LCD_Display(i,0,str[i]);	
			}
			for(i=6;i<=10;i++)				
			{
				LCD_Display(i,0,ARR[10-i]); 	// ��ʾ����	
			}
			LCD_Display(i+1,0,'L'); 			// ��ʾ��λ
			LCD_Display(i+2,0,'X');
		}

		HC595_Init();							// ���³�ʼ�������
		LCD1602_Write_Cmd(0x01,1);				// ��ʾ����

		while(flag != 1)						// ģʽ2��ʱ��
		{
			processTime();	   					// �����롢�֡�ʱ��������
			Seg_Multiple_Display(day*100 + hour,minute*100 + second);
		}


	}
}
