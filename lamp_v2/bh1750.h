sbit SCL=P1^5;      // IIC 时钟引脚
sbit SDA=P1^6;      // IIC 数据引脚

#define SlaveAddress   0x46
/*
定义器件在IIC总线中的从地址，ADD 引脚接地时地址为 0x46，接电源时地址为 0xB8
从设备地址用来区分总线上不同的从设备，因为因为有时候同一根时钟线和数据线可能会连接多个从设备
一般发送从设备地址的时候会在最低位加上读/写信号
比如设备地址为 0x50，则读数据就会发送 0x50，写数据就会发送 0x51
*/


// 	起始信号
void BH1750_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay_us(5);
    SDA = 0;                    //数据线由高到低，产生下降沿
    Delay_us(5);
    SCL = 0;                    //拉低时钟线，开始传输数据
}

//	停止信号
void BH1750_Stop()
{
    SDA = 0;
    SCL = 1;
    Delay_us(5);
    SDA = 1;                    //数据线由低到高，产生上升沿
    Delay_us(5);
}


// 想要接收数据，需要向发送方发送应答
void BH1750_SendACK(bit ack)
{
	// ack = 0: ACK
	// ack = 1: NAK
    SDA = ack;                  // 写应答信号
    SCL = 1;
    Delay_us(5);               
    SCL = 0;
    Delay_us(5);               
}


// 想要发送数据，需要等待接收方的应答
void BH1750_RecvACK()
{		
    SCL = 1;                 
    Delay_us(5);
   	// CY = SDA;                   // 读应答信号 ，可有可无
	// CY 溢出位，在此处作为存储应答信号的全局变量
    SCL = 0;
    Delay_us(5);
}



// 向 IIC 总线发送一个字节数据
void BH1750_SendByte(unsigned char dat)
{
    unsigned char i;
    for (i=0; i<8; i++)        
    {
        dat <<= 1;              // 移出数据的最高位，会影响溢出位 CY
        SDA = CY;               // 把 CY（即 dat 最高位）送数据口
        SCL = 1;                // 拉高时钟线，此时数据线状态必须保持稳定
        Delay_us(5);             
        SCL = 0;                // 只有在时钟线为低电平期间，数据线状态才允许变化		
        Delay_us(5);             
    }
    BH1750_RecvACK();			// 等待接收方的应答
}


// 从 IIC 总线接收一个字节数据
unsigned char BH1750_RecvByte()
{
    unsigned char i;
    unsigned char dat = 0;
    SDA = 1;			// 必须上拉，为什么.................
    for (i=0; i<8; i++)
    {
        dat <<= 1;
        SCL = 1;                // 拉高时钟线，此时数据线状态必须保持稳定
        Delay_us(5);         
        dat |= SDA;             // 读数据               
        SCL = 0;                // 只有在时钟线为低电平期间，数据线状态才允许变化
        Delay_us(5);           
    }
    return dat;
}



void Single_Write_BH1750(unsigned char REG_Address)
{
    BH1750_Start();                  // 发送起始信号
    BH1750_SendByte(SlaveAddress);   // 主设备发送设备地址 + 写信号 0
	BH1750_SendByte(REG_Address);    // 内部寄存器地址... 难道不是指令吗？ 比如上电 0x01
    BH1750_Stop();                   //	发送停止信号
}



// 连续读出 BH1750 的内部数据（ 2 个字节）
void Multiple_read_BH1750(void)
{   
	unsigned char i;	
    BH1750_Start();                          	//	发送起始信号
    BH1750_SendByte(SlaveAddress + 1);         //	发送设备地址 + 读信号 1
	
	 for (i=0; i<2; i++)                      // 连续读取 2 个地址数据，存储在 BUF 中
    {
        BUF[i] = BH1750_RecvByte();          // BUF[0]存储 0x32 地址中的数据，为什么是 0x32...... 不是很懂

        if (i == 1)	
        {
           	BH1750_SendACK(1);                // 最后一个数据需要回 NOACK，接收的数据只有2个字节，结束通讯
        }
        else
        {		
          	BH1750_SendACK(0);                // 回应一个 ACK
      	}
   }
    BH1750_Stop();                          //	发送停止信号
    Delay_ms(5);
}


//初始化BH1750
void Init_BH1750()
{
   Single_Write_BH1750(0x01);	// 上电命令  
}