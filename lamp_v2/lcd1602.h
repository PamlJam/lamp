#define DataPort P0	 // LCD1602 ���ݶ˿�

sbit RS=P4^2;	
// ע�� P42 �� hc595 ������ͻ
sbit RW=P4^1; 
sbit EN=P3^7;

unsigned char BUF[8];        	// ����һ�����飬��Ϊ������
unsigned char ARR[5];			// ��� ASCII ����

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
	if(Attribc)WaitForEnable();				// ��һ����Ŀ����ʲô............
		
	RS=0;			// �Ĵ���ѡ�񣬵͵�ƽ˵������дָ��
	RW=0;			// ʼ�ձ��ֵ͵�ƽ
	_nop_();	   	// ΢�뼶�ӳ�
	DataPort=cmd;	// д��һ���ֽ�
	_nop_();	
	EN=1;			// ʹ���ź�
	_nop_();
	_nop_();
	EN=0;
}
					
void LCD1602_Write_Dat(unsigned char dat)
{					
	WaitForEnable();		
	RS=1;			// �ߵ�ƽ˵������д���ݣ�����������ȫ��ͬ
	RW=0;
	_nop_();
	DataPort=dat;
	_nop_();	
	EN=1;
	_nop_();
	_nop_();
	EN=0;
}		

void Lcd_Init()	// ��ʼ��1602				
{			
	LCD1602_Write_Cmd(0x38,1);	// ������ʾģʽ	
	LCD1602_Write_Cmd(0x08,1);	// ��ʾ�ر�
	LCD1602_Write_Cmd(0x01,1);	// ��ʾ����
	LCD1602_Write_Cmd(0x06,1);	// ��ʾ����ƶ�
	LCD1602_Write_Cmd(0x0c,1);  // ��ʾ�����������
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