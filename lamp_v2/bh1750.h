sbit SCL=P1^5;      // IIC ʱ������
sbit SDA=P1^6;      // IIC ��������

#define SlaveAddress   0x46
/*
����������IIC�����еĴӵ�ַ��ADD ���Žӵ�ʱ��ַΪ 0x46���ӵ�Դʱ��ַΪ 0xB8
���豸��ַ�������������ϲ�ͬ�Ĵ��豸����Ϊ��Ϊ��ʱ��ͬһ��ʱ���ߺ������߿��ܻ����Ӷ�����豸
һ�㷢�ʹ��豸��ַ��ʱ��������λ���϶�/д�ź�
�����豸��ַΪ 0x50��������ݾͻᷢ�� 0x50��д���ݾͻᷢ�� 0x51
*/


// 	��ʼ�ź�
void BH1750_Start()
{
    SDA = 1;                    //����������
    SCL = 1;                    //����ʱ����
    Delay_us(5);
    SDA = 0;                    //�������ɸߵ��ͣ������½���
    Delay_us(5);
    SCL = 0;                    //����ʱ���ߣ���ʼ��������
}

//	ֹͣ�ź�
void BH1750_Stop()
{
    SDA = 0;
    SCL = 1;
    Delay_us(5);
    SDA = 1;                    //�������ɵ͵��ߣ�����������
    Delay_us(5);
}


// ��Ҫ�������ݣ���Ҫ���ͷ�����Ӧ��
void BH1750_SendACK(bit ack)
{
	// ack = 0: ACK
	// ack = 1: NAK
    SDA = ack;                  // дӦ���ź�
    SCL = 1;
    Delay_us(5);               
    SCL = 0;
    Delay_us(5);               
}


// ��Ҫ�������ݣ���Ҫ�ȴ����շ���Ӧ��
void BH1750_RecvACK()
{		
    SCL = 1;                 
    Delay_us(5);
   	// CY = SDA;                   // ��Ӧ���ź� �����п���
	// CY ���λ���ڴ˴���Ϊ�洢Ӧ���źŵ�ȫ�ֱ���
    SCL = 0;
    Delay_us(5);
}



// �� IIC ���߷���һ���ֽ�����
void BH1750_SendByte(unsigned char dat)
{
    unsigned char i;
    for (i=0; i<8; i++)        
    {
        dat <<= 1;              // �Ƴ����ݵ����λ����Ӱ�����λ CY
        SDA = CY;               // �� CY���� dat ���λ�������ݿ�
        SCL = 1;                // ����ʱ���ߣ���ʱ������״̬���뱣���ȶ�
        Delay_us(5);             
        SCL = 0;                // ֻ����ʱ����Ϊ�͵�ƽ�ڼ䣬������״̬������仯		
        Delay_us(5);             
    }
    BH1750_RecvACK();			// �ȴ����շ���Ӧ��
}


// �� IIC ���߽���һ���ֽ�����
unsigned char BH1750_RecvByte()
{
    unsigned char i;
    unsigned char dat = 0;
    SDA = 1;			// ����������Ϊʲô.................
    for (i=0; i<8; i++)
    {
        dat <<= 1;
        SCL = 1;                // ����ʱ���ߣ���ʱ������״̬���뱣���ȶ�
        Delay_us(5);         
        dat |= SDA;             // ������               
        SCL = 0;                // ֻ����ʱ����Ϊ�͵�ƽ�ڼ䣬������״̬������仯
        Delay_us(5);           
    }
    return dat;
}



void Single_Write_BH1750(unsigned char REG_Address)
{
    BH1750_Start();                  // ������ʼ�ź�
    BH1750_SendByte(SlaveAddress);   // ���豸�����豸��ַ + д�ź� 0
	BH1750_SendByte(REG_Address);    // �ڲ��Ĵ�����ַ... �ѵ�����ָ���� �����ϵ� 0x01
    BH1750_Stop();                   //	����ֹͣ�ź�
}



// �������� BH1750 ���ڲ����ݣ� 2 ���ֽڣ�
void Multiple_read_BH1750(void)
{   
	unsigned char i;	
    BH1750_Start();                          	//	������ʼ�ź�
    BH1750_SendByte(SlaveAddress + 1);         //	�����豸��ַ + ���ź� 1
	
	 for (i=0; i<2; i++)                      // ������ȡ 2 ����ַ���ݣ��洢�� BUF ��
    {
        BUF[i] = BH1750_RecvByte();          // BUF[0]�洢 0x32 ��ַ�е����ݣ�Ϊʲô�� 0x32...... ���Ǻܶ�

        if (i == 1)	
        {
           	BH1750_SendACK(1);                // ���һ��������Ҫ�� NOACK�����յ�����ֻ��2���ֽڣ�����ͨѶ
        }
        else
        {		
          	BH1750_SendACK(0);                // ��Ӧһ�� ACK
      	}
   }
    BH1750_Stop();                          //	����ֹͣ�ź�
    Delay_ms(5);
}


//��ʼ��BH1750
void Init_BH1750()
{
   Single_Write_BH1750(0x01);	// �ϵ�����  
}