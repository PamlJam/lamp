sbit HC595_DAT	=	P3^7;	   // 串行数据线 - 14
sbit HC595_SCK	=	P4^2;	   // 移位时钟线 - 12，相当于活塞
// 注意 P42 与 LCD1602 发生冲突
sbit HC595_RCK	=	P4^1;	   // 锁存时钟线 - 11，相当于平台
sbit HC595_OE	=	P4^5;	   // 输出允许线 - 13


// 共阴数码管段码表，存入映射关系
unsigned char code DispCode[]={
0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,																			
// 0    1    2    3    4    5    6    7    8    9   
0x77,0x7C,0x39,0x5E,0x79,0x71,0x3D,0x76,0x74,0x30,	
// A	b    C	  d	   E	F    G	  H	   h	I   	
0x10,0x1E,0x38,0x54,0x5C,0x73,0x67,0x50,0x31,0x78,
// i    J    L	  n    o	P    q	  r    R	t     	
0x3E,0x1C,0x40,0x48,0x08,0x00};
// U    V   一   二	   _   灭

unsigned char DispBuf[8];	// 显示缓冲区

void HC595_SendWord(unsigned char dat,unsigned char dig)	// dat 传入段码，dig 传入位码
{
	unsigned char n;	       
	unsigned int temp=0;
	temp=dat;
	temp=temp<<8  | dig; 				// 8bit + 8bit = 16bit

	for(n=0;n<16;n++)		 	
  	{										
		HC595_SCK = 0;	     			// 置低时钟线，相当于活塞复位
		if((temp&0x8000)==0x8000)		// 从高到低串行发送
		{
			HC595_DAT = 1;		   		// 置高信号线，准备一个 1
	 	}
		else
		{
			HC595_DAT = 0;		   		// 置低信号线，准备一个 0	
		}
		HC595_SCK = 1;		   			// 置高时钟线，相当于活塞前推
	  	temp<<=1;			
	}
	
	HC595_RCK = 0;							 	
	HC595_RCK = 1;
	// 锁存信号上升沿锁存数据，数据输出到端口	
	// 将平台上的数据一次性推出
} 

void Seg_Single_Display(unsigned char Dat,unsigned char Dig )
{
	HC595_SendWord(DispCode[Dat],~Dig);
	// 若 Dig = 0x80 = 1000 0000，则 ~Dig = 0111 1111，故 D7 亮
}

void Seg_Multiple_Display(unsigned int High_Dat,unsigned int Low_Dat)	// 选择 unsigned int 而不是 unsigned char（-128~127）
{
	unsigned char i;
	DispBuf[0]=High_Dat/1000;			//High_Dat的千位
	DispBuf[1]=High_Dat%1000/100;		//High_Dat的百位
	DispBuf[2]=High_Dat%100/10;			//High_Dat的十位
	DispBuf[3]=High_Dat%10;			  	//High_Dat的个位

	DispBuf[4]=Low_Dat/1000;			//Low_Dat的千位
	DispBuf[5]=Low_Dat%1000/100;		//Low_Dat的百位
	DispBuf[6]=Low_Dat%100/10;			//Low_Dat的十位
	DispBuf[7]=Low_Dat%10;			  	//Low_Dat的个位
				  		
	for(i=0;i<8;i++)
	{	
		HC595_SendWord(DispCode[DispBuf[i]],(~(0x01<<i))); 		
		HC595_OE = 0;				  	// 打开显示
		Delay_ms(1);					// 适当延时，利用视觉残留     
		HC595_OE = 1;				  	// 关闭显示
	} 
}

void HC595_Clear(void)
{	
	unsigned char i;
	
	for(i=0;i<8;i++)	// 清除显示缓冲区内容
	{
		DispBuf[i]=35;	// DispCode[35] = 0x00，即熄灭
	}
	HC595_SendWord(0x00,0xff);		  
	// 位码端低电平，对应的数码管才能点亮
}

void HC595_Init(void)
{	
	//等待HC595上电稳定
	Delay_ms(10);				
	
	//初始化P41,P42,P45口为准双向口
	P4M1 &=~( (1<<1) | (1<<2) | (1<<5));  
	P4M0 &=~( (1<<1) | (1<<2) | (1<<5)); 
	
	//初始化P37口为准双向口
	P3M1 &=~(1<<7);  
	P3M0 &=~(1<<7); 
	
	//将端口置低，赋予端口固定电平
	HC595_DAT = 0;
	HC595_SCK = 0;
	HC595_RCK = 0;

	//打开显示
	HC595_OE = 0;		
	//清除显示内容
	HC595_Clear();	
}