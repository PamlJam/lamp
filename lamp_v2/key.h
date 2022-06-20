// 标记状态，用于切换模式
bit flag = 1;

void ExterInt0_Init(void)
{
	P3M1 &=~(1<<2);  	// 双向口为什么要初始化？怎么初始化呢？
	P3M0 &=~(1<<2);		// P32 端口初始化

 	IT0=1;     			// 设置为下降沿触发
	EX0=1;		 		// 开外部中断
	EA=1;			 	// 开总中断
}


// 中断服务程序 interrupt service routine
void ExterInt0_ISR(void) interrupt 0
{ 	
 	if(P3^2==0)		 		//检测中断端口，判断是否确实产生中断
	{
		Delay_ms(2);	 	//适当延时
		if(P3^2==0)	 		//再次判断是否确实产生中断
		{
			flag = ~flag;	// 切换状态 
		}
	}  
}
