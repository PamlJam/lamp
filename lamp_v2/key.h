// ���״̬�������л�ģʽ
bit flag = 1;

void ExterInt0_Init(void)
{
	P3M1 &=~(1<<2);  	// ˫���ΪʲôҪ��ʼ������ô��ʼ���أ�
	P3M0 &=~(1<<2);		// P32 �˿ڳ�ʼ��

 	IT0=1;     			// ����Ϊ�½��ش���
	EX0=1;		 		// ���ⲿ�ж�
	EA=1;			 	// �����ж�
}


// �жϷ������ interrupt service routine
void ExterInt0_ISR(void) interrupt 0
{ 	
 	if(P3^2==0)		 		//����ж϶˿ڣ��ж��Ƿ�ȷʵ�����ж�
	{
		Delay_ms(2);	 	//�ʵ���ʱ
		if(P3^2==0)	 		//�ٴ��ж��Ƿ�ȷʵ�����ж�
		{
			flag = ~flag;	// �л�״̬ 
		}
	}  
}
