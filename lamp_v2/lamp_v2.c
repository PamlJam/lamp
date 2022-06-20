#include <stc15f2k60s2.h>
#include <intrins.h>			// 调用nop
#include <delay.h>				// 软件延时
#include <lcd1602.h>			// 液晶显示屏
#include <bh1750.h>				// 光传感器
#include <hc595.h>				// 数码管
#include <key.h>				// 独立按键、外部中断、扫描

unsigned int lux;			// 当前光照强度（单位勒克斯）
unsigned int Status = 0;	// 暂存 P4 状态，io 分时复用
unsigned int ratio = 1; 	// PWM 占空比

long int time = 2021400;	// 初始化时间（23 日 9点 30 分 0 秒）		
// 当前累计时间（单位秒）
// 长整型（-2147483648~ 2147483647）
// 全局变量可以在模式切换时保存时间

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;

unsigned char d=1;	// 方向（前进1、后退-1、暂停0）
unsigned char s=1;	// 步长（1、60、3600）

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

void Timer0_Init(void)	// 每隔 20ms 中断 1 次
{
	AUXR &= 0x7F;		// 定时器时钟12T模式
	TMOD &= 0xF0;		// 设置定时器模式
	TL0 = 0x00;			// 设置初值
	TH0 = 0xB8;
	TF0 = 0;			//清除TF0标记
	ET0=1;			  	//使能T0中断
	EA=1;			  	//使能总中断
	TR0=1;	  	  		//运行T0    
}


// 中断服务程序 interrupt service routine
void Timer0_ISR(void) interrupt 1
{
	static unsigned char cnt=0;		// 静态数据类型，用作计数器
	if(flag == 1)		// 模式1：光强检测、模拟调光
	{
		HC595_OE = 0;	// 打开	595
		Status = P4; 	// 存入状态
		AdjustLamp();
		P4 = Status;   	// 取出状态
		HC595_OE = 1;	// 关闭 595

		 
	}

	// 时间总是在流逝，不论当前是什么模式
	if(cnt++ == 50)		//定时 20ms*50 = 1s
	{
		cnt=0;
		time = time + d*s;
	}
}


void conversion(unsigned int t)  //  获取各位数据（个十百千万）
{ 
	// 1+0x30 = 0x31 = 49，在 ASCII 表中对应中字符 '1'
    ARR[4]=t/10000+0x30;
    t=t%10000;ARR[3]=t/1000+0x30;
    t=t%1000;ARR[2]=t/100+0x30;
    t=t%100;ARR[1]=t/10+0x30;
    t=t%10;ARR[0]=t+0x30;
}

void processTime()// 处理时间数据，例如100000秒 = 1天3小时46分40秒
{
	second = time%60;		// 得秒
	minute = time/60%60;	// 得分
	hour = time/3600%24;	// 得时
	day = time/3600/24%30;	// 得天
}



void main()
{  
	while(1)	// 大循环
	{
		unsigned char i;
		unsigned int num;
		unsigned char str[] = "light:";
	
		Delay_ms(100);
		Init_BH1750();
		LCD_Init();
		HC595_Init();
		Timer0_Init();				// 初始化定时器
		ExterInt0_Init(); 			// 初始化外部中断

		while(flag == 1)			// 模式1：光强检测、模拟调光
		{
			Single_Write_BH1750(0x01);   		// 上电命令 power on
			Single_Write_BH1750(0x10);  		// 连续高分辨测量 H- resolution mode，测量时间一般为120ms	
			  
			Delay_ms(180);						// 等待测量事件结束，适当地延时180ms，避免浪费时间
			
			Multiple_Read_BH1750();       		// 连续读出数据，存储在 BUF 中
	
			num=BUF[0];
			num=(num<<8) + BUF[1];				// 拼接数据
			
			lux = (float)num/1.2; 				// 按照公式换算
			conversion(lux);         			// 计算数据和显示
			
			for(i=0;i<=5;i++)		   			// 显示文本
			{
				LCD_Display(i,0,str[i]);	
			}
			for(i=6;i<=10;i++)				
			{
				LCD_Display(i,0,ARR[10-i]); 	// 显示数据	
			}
			LCD_Display(i+1,0,'L'); 			// 显示单位
			LCD_Display(i+2,0,'X');
		}

		HC595_Init();							// 重新初始化数码管
		LCD1602_Write_Cmd(0x01,1);				// 显示清屏

		while(flag != 1)						// 模式2：时钟
		{
			processTime();	   					// 更新秒、分、时、天数据
			Seg_Multiple_Display(day*100 + hour,minute*100 + second);
		}


	}
}
