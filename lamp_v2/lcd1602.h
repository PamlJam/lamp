#define DataPort P0	 // LCD1602 数据端口

sbit RS=P4^2;	
// 注意 P42 与 hc595 发生冲突
sbit RW=P4^1; 
sbit EN=P3^7;

unsigned char BUF[8];        	// 定义一个数组，作为缓存区
unsigned char ARR[5];			// 存放 ASCII 编码

void WaitForEnable(void)	
{					
	DataPort=0xff;		
	RS=0;
	RW=1;
	_nop_();
	EN=1;
	_nop_();
	_nop_();
	while(DataPort&0x80);	
	EN=0;				
}					

void LCD1602_Write_Cmd(unsigned char cmd,unsigned char Attribc)
{					
	if(Attribc)WaitForEnable();				// 这一步的目的是什么............
		
	RS=0;			// 寄存器选择，低电平说明正在写指令
	RW=0;			// 始终保持低电平
	_nop_();	   	// 微秒级延迟
	DataPort=cmd;	// 写入一个字节
	_nop_();	
	EN=1;			// 使能信号
	_nop_();
	_nop_();
	EN=0;
}
					
void LCD1602_Write_Dat(unsigned char dat)
{					
	WaitForEnable();		
	RS=1;			// 高电平说明正在写数据，后续步骤完全相同
	RW=0;
	_nop_();
	DataPort=dat;
	_nop_();	
	EN=1;
	_nop_();
	_nop_();
	EN=0;
}		

void Lcd_Init()	// 初始化1602				
{			
	LCD1602_Write_Cmd(0x38,1);	// 设置显示模式	
	LCD1602_Write_Cmd(0x08,1);	// 显示关闭
	LCD1602_Write_Cmd(0x01,1);	// 显示清屏
	LCD1602_Write_Cmd(0x06,1);	// 显示光标移动
	LCD1602_Write_Cmd(0x0c,1);  // 显示开及光标设置
}			

void LCD_Display(unsigned char X,unsigned char Y,unsigned char DData)
{						
	Y&=1;						
	X&=15;						
	if(Y)
		X|=0x40;					
	X|=0x80;			
	LCD1602_Write_Cmd(X,0);		
	LCD1602_Write_Dat(DData);		
}