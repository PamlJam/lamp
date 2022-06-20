// 延时微秒		   适用于 1T 的 15f2k60
void Delay_us(unsigned int us)
{
	while(us --)
	{
		_nop_();
		_nop_();
		_nop_();
	}
}


// 延时毫秒		   适用于 1T 的 15f2k60
void Delay_ms(unsigned int ms)
{
	unsigned char i, j;
	while(ms--)
	{
		_nop_();
		_nop_();
		_nop_();
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}