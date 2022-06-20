sbit HC595_DAT	=	P3^7;	   // ���������� - 14
sbit HC595_SCK	=	P4^2;	   // ��λʱ���� - 12���൱�ڻ���
// ע�� P42 �� LCD1602 ������ͻ
sbit HC595_RCK	=	P4^1;	   // ����ʱ���� - 11���൱��ƽ̨
sbit HC595_OE	=	P4^5;	   // ��������� - 13


// ��������ܶ��������ӳ���ϵ
unsigned char code DispCode[]={
0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,																			
// 0    1    2    3    4    5    6    7    8    9   
0x77,0x7C,0x39,0x5E,0x79,0x71,0x3D,0x76,0x74,0x30,	
// A	b    C	  d	   E	F    G	  H	   h	I   	
0x10,0x1E,0x38,0x54,0x5C,0x73,0x67,0x50,0x31,0x78,
// i    J    L	  n    o	P    q	  r    R	t     	
0x3E,0x1C,0x40,0x48,0x08,0x00};
// U    V   һ   ��	   _   ��

unsigned char DispBuf[8];	// ��ʾ������

void HC595_SendWord(unsigned char dat,unsigned char dig)	// dat ������룬dig ����λ��
{
	unsigned char n;	       
	unsigned int temp=0;
	temp=dat;
	temp=temp<<8  | dig; 				// 8bit + 8bit = 16bit

	for(n=0;n<16;n++)		 	
  	{										
		HC595_SCK = 0;	     			// �õ�ʱ���ߣ��൱�ڻ�����λ
		if((temp&0x8000)==0x8000)		// �Ӹߵ��ʹ��з���
		{
			HC595_DAT = 1;		   		// �ø��ź��ߣ�׼��һ�� 1
	 	}
		else
		{
			HC595_DAT = 0;		   		// �õ��ź��ߣ�׼��һ�� 0	
		}
		HC595_SCK = 1;		   			// �ø�ʱ���ߣ��൱�ڻ���ǰ��
	  	temp<<=1;			
	}
	
	HC595_RCK = 0;							 	
	HC595_RCK = 1;
	// �����ź��������������ݣ�����������˿�	
	// ��ƽ̨�ϵ�����һ�����Ƴ�
} 

void Seg_Single_Display(unsigned char Dat,unsigned char Dig )
{
	HC595_SendWord(DispCode[Dat],~Dig);
	// �� Dig = 0x80 = 1000 0000���� ~Dig = 0111 1111���� D7 ��
}

void Seg_Multiple_Display(unsigned int High_Dat,unsigned int Low_Dat)	// ѡ�� unsigned int ������ unsigned char��-128~127��
{
	unsigned char i;
	DispBuf[0]=High_Dat/1000;			//High_Dat��ǧλ
	DispBuf[1]=High_Dat%1000/100;		//High_Dat�İ�λ
	DispBuf[2]=High_Dat%100/10;			//High_Dat��ʮλ
	DispBuf[3]=High_Dat%10;			  	//High_Dat�ĸ�λ

	DispBuf[4]=Low_Dat/1000;			//Low_Dat��ǧλ
	DispBuf[5]=Low_Dat%1000/100;		//Low_Dat�İ�λ
	DispBuf[6]=Low_Dat%100/10;			//Low_Dat��ʮλ
	DispBuf[7]=Low_Dat%10;			  	//Low_Dat�ĸ�λ
				  		
	for(i=0;i<8;i++)
	{	
		HC595_SendWord(DispCode[DispBuf[i]],(~(0x01<<i))); 		
		HC595_OE = 0;				  	// ����ʾ
		Delay_ms(1);					// �ʵ���ʱ�������Ӿ�����     
		HC595_OE = 1;				  	// �ر���ʾ
	} 
}

void HC595_Clear(void)
{	
	unsigned char i;
	
	for(i=0;i<8;i++)	// �����ʾ����������
	{
		DispBuf[i]=35;	// DispCode[35] = 0x00����Ϩ��
	}
	HC595_SendWord(0x00,0xff);		  
	// λ��˵͵�ƽ����Ӧ������ܲ��ܵ���
}

void HC595_Init(void)
{	
	//�ȴ�HC595�ϵ��ȶ�
	Delay_ms(10);				
	
	//��ʼ��P41,P42,P45��Ϊ׼˫���
	P4M1 &=~( (1<<1) | (1<<2) | (1<<5));  
	P4M0 &=~( (1<<1) | (1<<2) | (1<<5)); 
	
	//��ʼ��P37��Ϊ׼˫���
	P3M1 &=~(1<<7);  
	P3M0 &=~(1<<7); 
	
	//���˿��õͣ�����˿ڹ̶���ƽ
	HC595_DAT = 0;
	HC595_SCK = 0;
	HC595_RCK = 0;

	//����ʾ
	HC595_OE = 0;		
	//�����ʾ����
	HC595_Clear();	
}